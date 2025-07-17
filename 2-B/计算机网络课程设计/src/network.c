/**
 * @file network.c
 * @brief 网络相关操作
 */
#include "../include/network.h"
#include "../include/args_handler.h"
#include "../include/logs.h"
#include "../include/file_reader.h"
#include "../include/structs.h"
#include "../include/dns_parser.h"
#include "../include/consts.h"
#include "../include/mapping.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

int udpfd; // 数据报套接字
struct sockaddr_in dns_addr; // DNS服务器地址

/**
 * @brief 初始化用于通信的UDP套接字
 */
static void init_udp() {
    // 创建数据报套接字
    udpfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpfd < 0) {
        perror("socket failed");
        exit(-1);
    }
    // 创建描述地址的结构体
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(DNS_PORT),
        .sin_addr.s_addr = INADDR_ANY
    };
    // 地址复用
    int reuse = 1;
    if (setsockopt(udpfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse))) {
        perror("setsockopt failed");
        exit(-1);
    }
    // 将套接字与地址绑定
    if (bind(udpfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("bind failed");
        exit(-1);
    }
    log_always("Create udp socket success");
}

/**
 * @brief 初始化DNS服务器地址
 */
static void init_dns_server() {
    // 将DNS服务器地址填入结构体
    dns_addr.sin_family = AF_INET;
    dns_addr.sin_port = htons(DNS_PORT);
    dns_addr.sin_addr.s_addr = args.dns_server_addr;
    log_always("Init dns server address success");
}

/**
 * @brief 初始化网络相关部分
 */
void network_init() {
    init_udp();
    init_dns_server();
}

/**
 * @brief 处理来自客户端的消息
 * @param buf 消息缓冲区
 * @param len 消息长度
 * @param cli_addr 客户端地址
 * @param cli_addr_len 客户端地址长度
 */
static void handle_message(unsigned char *buf, const int len, struct sockaddr_in cli_addr, socklen_t cli_addr_len) {
    char *name = name_parse((char *) buf + sizeof(Header)); // 找到name
    log_brief("Request name: %s", name);
    log_brief("Request id: %d", htons(((Header *) buf)->id));
    const uint32_t result = find_entry(name); // 在文件中查找name对应的地址
    free(name); // 释放空间，防止内存泄漏
    // find_entry的不同结果
    if (result == UINT32_MAX) {
        // 如果找不到，转发给DNS服务器
        log_detailed("No local entry found, send to DNS server");
        // id变换，(id,addr)->cnt
        uint16_t this = convert_id_to_cnt((Header *) buf, cli_addr);
        if(sendto(udpfd, buf, len, 0, (struct sockaddr *) &dns_addr, sizeof(dns_addr)) < 0)
            perror("sendto failed");
        // 1s未收到DNS服务器的响应，超时
        sleep(TIMEOUT);
        if (map[this].is_arrived == false) {
            log_detailed("DNS server response timeout");
            map[this].is_arrived = deleted;
        }
    } else if (result == inet_addr("0.0.0.0")) {
        // 如果找到且为0.0.0.0，屏蔽
        log_detailed("Find local entry 0.0.0.0, sheild it");
        const Header *request_head = (Header *) buf;
        Header response_head;
        fill_header(&response_head, request_head, REJECT);
        if (sendto(udpfd, (char *) &response_head, sizeof(response_head), 0,
            (struct sockaddr *) &cli_addr, cli_addr_len) < 0)
            perror("sendto failed");
    } else {
        // 如果找到且不为0.0.0.0，返回给客户端
        if (get_question_type(buf + sizeof(Header)) == AAAA) {
            // 如果是AAAA请求，转发给DNS服务器
            log_detailed("AAAA request, send to DNS server");
            // id变换，(id,addr)->cnt
            uint16_t this = convert_id_to_cnt((Header *) buf, cli_addr);
            if(sendto(udpfd, buf, len, 0, (struct sockaddr *) &dns_addr, sizeof(dns_addr)) < 0)
                perror("sendto failed");
            // 1s未收到DNS服务器的响应，超时
            sleep(TIMEOUT);
            if (map[this].is_arrived == false) {
                log_detailed("DNS server response timeout");
                map[this].is_arrived = deleted;
            }
            return;
        }
        log_detailed("Find local entry, send to client");
        // 构造头
        const Header *request_head = (Header *) buf;
        Header response_head;
        fill_header(&response_head, request_head, ACCEPT);
        // 构造RR
        unsigned char rr[16];
        construct_RR(rr, result);
        // 构造response
        unsigned char response[MAX_MSG_LEN];
        construct_response(response, &response_head, buf, len, rr);
        if (sendto(udpfd, response, len + 16, 0, (struct sockaddr *) &cli_addr, cli_addr_len) < 0)
            perror("sendto failed");
    }
}

/**
 * @brief 对消息进行分类处理，两类：1）本地请求，2）DNS服务器答复
 * @param arg 参数
 */
static void *message_classify(void *arg) {
    pthread_detach(pthread_self()); // 分离线程，使其在结束时自动释放资源
    // 提取参数
    unsigned char *buf = ((arg_t *) arg)->buf;
    const int len = ((arg_t *) arg)->len;
    const struct sockaddr_in cli_addr = ((arg_t *) arg)->cli_addr;
    socklen_t cli_addr_len = ((arg_t *) arg)->cli_addr_len;
    // 分类处理
    Header *head = (Header *) buf;
    if (head->qr == 0) {
        // 本地请求，交给handle_message处理
        log_brief("Receive local request");
        handle_message(buf, len, cli_addr, cli_addr_len);
    } else {
        // DNS服务器答复，转换id后返回给客户端
        log_brief("Receive DNS server response, send to client");
        // id变换，cnt->(id,addr)
        struct sockaddr_in local_cli_addr;
        if (convert_cnt_to_id(head, &local_cli_addr))
            if (sendto(udpfd, buf, len, 0, (struct sockaddr *) &local_cli_addr, sizeof(local_cli_addr)) < 0)
                perror("sendto failed");
    }
    free(buf);
    free(arg);
}

/**
 * @brief 主处理函数
 */
void main_process() {
    log_always("DNS relay is running...");
    unsigned char *buf = calloc(MAX_MSG_LEN, sizeof(char));
    while (true) {
        memset(buf, 0, MAX_MSG_LEN);
        struct sockaddr_in cli_addr;
        socklen_t len = sizeof(struct sockaddr_in);
        const int recv_len = recvfrom(udpfd, buf, MAX_MSG_LEN, 0, (struct sockaddr *) &cli_addr, &len);
        if (recv_len < 0) {
            perror("recvfrom failed");
        } else {
            log_detailed("Receive message from %s:%d", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
            if (args.debug_level == 2) {
                log_detailed("Message details:");
                for (int i = 0; i < recv_len; i++) {
                    printf("%02x ", buf[i]);
                    if ((i + 1) % 16 == 0)
                        printf("\n");
                }
                printf("\n");
            }
            arg_t *arg = malloc(sizeof(arg_t));
            // 构造message_classify的参数
            arg->buf = (unsigned char *) calloc(recv_len, sizeof(char));
            memcpy(arg->buf, buf, recv_len);
            arg->len = recv_len;
            arg->cli_addr = cli_addr;
            arg->cli_addr_len = len;
            // 新建线程处理
            pthread_t tid;
            if (pthread_create(&tid, NULL, message_classify, (void *) arg))
                perror("pthread_create failed");
        }
    }
}
