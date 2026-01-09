#ifndef ARGS_HANDLER_H
#define ARGS_HANDLER_H
#include <stdint.h>

#define DEFAULT_DEBUG_LEVEL 0
#define DEFAULT_DNS_SERVER_ADDR "10.3.9.4"
#define DEFAULT_LOCAL_FILE_ADDR "dnsrelay.txt"

typedef struct {
    // 参数结构体，包含调试等级、dns服务器地址、本地文件地址
    int debug_level;
    uint32_t dns_server_addr;
    char *local_file_addr;
} arguments;

extern arguments args; // 全局变量，供其他文件使用

// 根据命令行参数初始化调试等级、dns服务器地址、本地文件地址
extern void args_init(int argc, char *argv[]);

#endif
