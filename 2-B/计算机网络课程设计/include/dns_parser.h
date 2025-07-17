#ifndef DNS_PARSER_H
#define DNS_PARSER_H

#include "../include/structs.h"

#define REJECT 0
#define ACCEPT 1

extern char *name_parse(const char *buf); // 解析域名
extern void fill_header(Header *to_fill, const Header *src, int type); // 填充头部
extern void construct_RR(unsigned char *rr, uint32_t result); // 构造资源记录
extern void construct_response(unsigned char *response, Header *response_head, const unsigned char *buf, int len,
                               const unsigned char *rr); // 构造响应报文

#define TYPE uint16_t
#define A 1
#define NS 2
#define MD 3
#define MF 4
#define CNAME 5
#define SOA 6
#define MB 7
#define MG 8
#define MR 9
#define NULL_R 10
#define WKS 11
#define PTR 12
#define HINFO 13
#define MINFO 14
#define MX 15
#define TXT 16
#define AAAA 28
extern TYPE get_question_type(const unsigned char *question); // 获取Question字段类型

#endif
