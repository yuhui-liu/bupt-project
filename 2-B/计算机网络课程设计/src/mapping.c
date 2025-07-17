#include "../include/mapping.h"
#include "../include/logs.h"

Map map[UINT16_MAX]; // 用于映射id

int cnt = 0; // 计数器

#define inc(x) x = (x + 1) % UINT16_MAX

/**
 * @brief 将(id, addr)映射到cnt
 * @param h 消息头
 * @param cli_addr 客户端地址
 * @return 映射后的id
 */
uint16_t convert_id_to_cnt(Header *head, struct sockaddr_in cli_addr) {
    map[cnt].id = head->id;
    map[cnt].addr = cli_addr;
    map[cnt].is_arrived = false;
    head->id = cnt;
    inc(cnt);
    return (cnt ? cnt - 1 : UINT16_MAX);
}

/**
 * @brief 将cnt映射回(id, addr)
 * @param head 消息头
 * @param local_cli_addr 客户端地址
 * @return 是否成功，超时未收到dns服务器的回复返回0
 */
bool convert_cnt_to_id(Header *head, struct sockaddr_in *local_cli_addr) {
    const uint16_t index = head->id;
    if (map[index].is_arrived == deleted) {
        log_detailed("Response timeout, drop it");
        return false;
    }
    head->id = map[index].id;
    map[index].is_arrived = true;
    *local_cli_addr = map[index].addr;
    return true;
}