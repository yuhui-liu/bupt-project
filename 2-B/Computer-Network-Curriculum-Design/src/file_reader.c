/**
 * @file file_reader.c
 * @brief 从文件中读取数据，从内存中查找name对应的地址
 */
#include "../include/logs.h"
#include "../include/file_reader.h"
#include "../include/args_handler.h"
#include "../include/consts.h"
#include <stdio.h>
#include <arpa/inet.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

pair pairs[MAX_ENTRY_LEN];
uint32_t entries_cnt = 0;

/**
 * @brief 比较函数，用于qsort和bsearch
 * @param a pair指针
 * @param b 另一个pair指针
 */
static int cmp(const void *a, const void *b) {
    return strcmp(((pair *) a)->name, ((pair *) b)->name);
}

/**
 * @brief 从文件中读取数据
 */
void load_file() {
    // 打开文件
    FILE *local_DNS_file = fopen(args.local_file_addr, "r");
    if (!local_DNS_file) {
        perror("fopen failed");
        exit(-1);
    }

    char addr_string[MAX_ADDR_LEN + 1]; // 多一个字节存放'\0'
    char name[MAX_NAME_LEN]; // '\0'已经含在MAX_NAME_LEN中

    memset(addr_string, 0, sizeof(addr_string));
    memset(name, 0, sizeof(name));
    log_always("Loading file...");
    while (fscanf(local_DNS_file, "%s %s\n", addr_string, name) != EOF) {
        int t = inet_addr(addr_string);
        if (t == -1) {
            if (strcmp(addr_string, "255.255.255.255") == 0)
                pairs[entries_cnt].addr = UINT32_MAX;
            else
                log_always("Invalid address: %s", addr_string);
        } else {
            pairs[entries_cnt].addr = t;
        }
        if (args.debug_level == 2)
            printf("%s \t: %s\n", addr_string, name);
        pairs[entries_cnt].name = (char *) calloc(strlen(name) + 1, sizeof(char));
        memcpy(pairs[entries_cnt].name, name, strlen(name));
        memset(addr_string, 0, sizeof(addr_string));
        memset(name, 0, sizeof(name));
        entries_cnt++;
    }
    log_always("Load file success");

    if (fclose(local_DNS_file) == EOF) {
        perror("fclose failed");
        exit(-1);
    }

    qsort(pairs, entries_cnt, sizeof(pair), cmp);
}

/**
 * @brief 在内存中查找name对应的地址
 */
uint32_t find_entry(char *name) {
    log_detailed("Finding in memory...");
    pair p;
    p.name = name;
    pair *res = bsearch(&p, pairs, entries_cnt, sizeof(pair), cmp);
    if (res != NULL)
        return res->addr;
    return UINT32_MAX;
}
