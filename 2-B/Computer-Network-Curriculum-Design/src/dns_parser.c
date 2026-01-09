/**
 * @file dns_parser.c
 * @brief DNS报文解析
 */
#include "../include/dns_parser.h"
#include "../include/consts.h"
#include "../include/logs.h"
#include <string.h>
#include <stdlib.h>


/**
 * @brief 从原始以标签长度分隔的域名中解析出以点分隔的域名
 * @param buf 待解析的缓冲区
 * @return 域名，形如baidu.com
 */
char *name_parse(const char *buf) {
    int i = 0;
    char *name = (char *) calloc(MAX_NAME_LEN, sizeof(char));
    while (buf[i] != '\0') {
        strncat(name, buf + i + 1, (int) buf[i]);
        strcat(name, ".");
        i += (int) buf[i] + 1;
    }
    /* 5 b a i d u 3 c o m 0
       b a i d u . c o m .  */
    name[i - 1] = '\0';
    return name;
}

/**
 * @brief 根据请求报文头填充应答报文头
 * @param to_fill 待填充的报文头
 * @param src 请求报文头
 * @param type 类型，REFUSE表示拒绝，ACCEPT表示接受
 */
void fill_header(Header *to_fill, const Header *src, const int type) {
    memset(to_fill, 0, sizeof(Header));
    to_fill->id = src->id;
    to_fill->qr = 1; // qr=1，表示是响应报文
    to_fill->opcode = src->opcode;
    to_fill->aa = 0;
    to_fill->tc = 0;
    to_fill->rd = src->rd;
    to_fill->ra = 1;
    to_fill->z = 0;
    if (type == REJECT) {
        to_fill->rcode = 5; // 5表示拒绝
    } else {
        to_fill->rcode = 0;
        to_fill->qdcount = src->qdcount;
        to_fill->ancount = htons(1);
    }
}

/**
 * @brief 构造RR
 * @param rr 待填充的RR
 * @param result 查询结果
 */
void construct_RR(unsigned char *rr, const uint32_t result) {
    RR rr_without_name_and_rdata = {
        .type = htons(1), // A记录
        .class = htons(1), // IN，因特网
        .ttl = htonl(60), // 保留1分钟
        .rdlength = htons(4), // rdata长度为4
    };
    rr[0] = 0xc0;
    rr[1] = 0x0c; /* 0xc00c表示指针指向name，具体地，0b1100000000001100，11是标志，后面是偏移量 */
    memcpy(rr + 2, ((char *) &rr_without_name_and_rdata) + sizeof(unsigned char *), 10); /* 跳过name */
    memcpy(rr + 12, &result, 4);
}

/**
 * @brief 构造响应报文
 * @param response 待填充的响应报文
 * @param response_head 响应报文头
 * @param buf 请求报文
 * @param len 请求报文长度
 * @param rr 资源记录
 */
void construct_response(unsigned char *response, Header *response_head, const unsigned char *buf, const int len,
                        const unsigned char *rr) {
    memcpy(response, (char *) response_head, sizeof(Header));
    memcpy(response + sizeof(Header), buf + sizeof(Header), len - sizeof(Header));
    memcpy(response + len, rr, 16);
}

/**
 * @brief 获取Question字段类型
 * @param question Question字段
 */
TYPE get_question_type(const unsigned char *question) {
    int i = 0;
    while (question[i] != '\0') {
        i += question[i] + 1;
    }
    i++;
    TYPE type = (question[i] << 8) + question[i + 1];
    switch (type) {
        case A:
            log_detailed("Question type: A");
            break;
        case NS:
            log_detailed("Question type: NS");
            break;
        case MD:
            log_detailed("Question type: MD");
            break;
        case MF:
            log_detailed("Question type: MF");
            break;
        case CNAME:
            log_detailed("Question type: CNAME");
            break;
        case SOA:
            log_detailed("Question type: SOA");
            break;
        case MB:
            log_detailed("Question type: MB");
            break;
        case MG:
            log_detailed("Question type: MG");
            break;
        case MR:
            log_detailed("Question type: MR");
            break;
        case NULL_R:
            log_detailed("Question type: NULL");
            break;
        case WKS:
            log_detailed("Question type: WKS");
            break;
        case PTR:
            log_detailed("Question type: PTR");
            break;
        case HINFO:
            log_detailed("Question type: HINFO");
            break;
        case MINFO:
            log_detailed("Question type: MINFO");
            break;
        case MX:
            log_detailed("Question type: MX");
            break;
        case TXT:
            log_detailed("Question type: TXT");
            break;
        case AAAA:
            log_detailed("Question type: AAAA");
            break;
        default:
            log_detailed("Question type: Unknown");
            break;
    }
    return type;
}