/**
 * @file args_handler.c
 * @brief 处理命令行参数，使用GNU argp库
 */
#include "../include/args_handler.h"
#include "../include/logs.h"
#include <argp.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>

arguments args; // 全局变量，保存命令行参数，包括调试等级、dns服务器地址、本地文件地址

static struct argp_option argp_options[] = {
    // 命令行参数选项
    {0, 'd', 0, 0, "Use \"-d\" to display debugging info, or \"-dd\" to display more detailed info."}, // -d选项
    {0}
};

/**
 * @brief 命令行参数解析器
 * @param key 选项
 * @param arg 选项的值
 * @param state 当下解析状态
 */
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    arguments *arguments = state->input;
    switch (key) {
        // -d选项
        case 'd':
            if (arguments->debug_level < 2) {
                arguments->debug_level++;
            } else {
                argp_error(state, "debug level should be no more than 2");
                return ARGP_ERR_UNKNOWN;
            }
            break;
        // 其他参数
        case ARGP_KEY_ARG:
            if (state->arg_num == 0) {
                // 第一个参数是dns服务器地址
                uint32_t t = inet_addr(arg);
                if (t == UINT32_MAX) {
                    argp_error(state, "incorrect ip address");
                    return ARGP_ERR_UNKNOWN;
                } else {
                    arguments->dns_server_addr = t;
                }
            } else if (state->arg_num == 1) {
                // 第二个参数是本地文件地址
                arguments->local_file_addr = arg;
            } else {
                // 参数过多
                argp_error(state, "too many arguments");
                return ARGP_ERR_UNKNOWN;
            }
            break;
    }
    return 0;
}

// argp结构体，提供给argp_parse函数使用
static struct argp argp = {
    .options = argp_options,
    .parser = parse_opt,
    .args_doc = "[-d|-dd][dns-server-ipaddr][filename]",
    .doc = "A dns relay.",
};

/**
 * @brief 根据命令行参数初始化调试等级、dns服务器地址、本地文件地址
 * @param argc 参数个数
 * @param argv 参数列表
 */
void args_init(int argc, char *argv[]) {
    // 默认值
    args.debug_level = DEFAULT_DEBUG_LEVEL;
    args.dns_server_addr = inet_addr(DEFAULT_DNS_SERVER_ADDR);
    args.local_file_addr = DEFAULT_LOCAL_FILE_ADDR;
    // 解析命令行参数
    argp_parse(&argp, argc, argv, 0, 0, &args);
    // 打印信息
    printf("----------------------------------------------\n"
        "----------DNS RELAY  VERSION 0.1--------------\n"
        "----------Build: "__DATE__" "__TIME__"---------\n"
        "----------------------------------------------\n"
    );
    log_always("Debug level is %d", args.debug_level);
    struct in_addr addr;
    memcpy(&addr, &args.dns_server_addr, sizeof (args.dns_server_addr));
    log_always("DNS server is %s", inet_ntoa(addr));
    log_always("Local file in %s", args.local_file_addr);
}
