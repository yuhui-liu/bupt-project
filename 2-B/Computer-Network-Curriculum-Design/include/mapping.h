#ifndef MAPPING_H
#define MAPPING_H

#include <arpa/inet.h>
#include "../include/structs.h"

//  用于表示bool类型，但是增加了deleted状态
#define bool uint8_t
#define false 0
#define true 1
#define deleted 2

typedef struct {
    uint16_t id;
    struct sockaddr_in addr;
    bool is_arrived; // 用来表示计时器超时前是否收到dns服务器的回复，deleted表示超时但未收到，将这一映射删除
} Map; // 用于映射id

extern Map map[UINT16_MAX]; // 用于映射id

extern uint16_t convert_id_to_cnt(Header *h, struct sockaddr_in cli_addr);
extern bool convert_cnt_to_id(Header *head, struct sockaddr_in *local_cli_addr);

#endif