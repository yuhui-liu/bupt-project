#ifndef FILE_READER_H
#define FILE_READER_H

#include <stdint.h>
#define MAX_ENTRY_LEN 10000
#define MAX_ADDR_LEN 15

typedef struct {
    uint32_t addr;
    char *name;
} pair; // （地址，域名）结构体

extern pair pairs[MAX_ENTRY_LEN]; // （地址，域名）数组
extern uint32_t entries_cnt; // 文件中的记录数
extern void load_file(); // 加载文件，将文件中的数据读入内存
extern uint32_t find_entry(char *name); // 在内存中查找name对应的地址

#endif
