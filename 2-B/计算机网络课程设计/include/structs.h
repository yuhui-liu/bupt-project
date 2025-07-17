/**
 * @file structs.h
 * @brief 定义了一些结构体
 */
#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdint.h>
#include <netinet/in.h>

typedef struct Header {
    uint16_t id;
    uint8_t rd : 1;
    uint8_t tc : 1;
    uint8_t aa : 1;
    uint8_t opcode : 4;
    uint8_t qr : 1;

    uint8_t rcode : 4;
    uint8_t z : 3;
    uint8_t ra : 1;

    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
} Header;

typedef struct Question {
    unsigned char * qname;
    uint16_t qtype;
    uint16_t qclass;
} Question;

typedef struct RR {
    unsigned char * name; // 只是占位
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t rdlength;
    unsigned char *rdata;
} RR;

// 参数，包括缓冲区、长度、客户端地址、客户端地址长度
typedef struct arg_t {
    unsigned char *buf;
    int len;
    struct sockaddr_in cli_addr;
    socklen_t cli_addr_len;
} arg_t;


#endif