/**
 * @file logs.h
 * @brief 定义了输出调试信息的宏
 */
#ifndef LOGS_H
#define LOGS_H
#include "args_handler.h"
#include <time.h>
#include <stdio.h>

// 打印带时间的调试信息
// 定义成宏是为了方便参数传递
#define print_with_time(fmt, ...) do { \
                        time_t t = time(NULL); \
                        struct tm *info = localtime(&t); \
                        printf("%d/%d/%d %02d:%02d:%02d ", info->tm_year + 1900, info->tm_mon + 1, \
                                info->tm_mday, info->tm_hour, info->tm_min, info->tm_sec); \
                        printf(fmt "\n", ##__VA_ARGS__); \
                    } while (0)

#define log_detailed(fmt, ...) do {\
                                   if (args.debug_level == 2) \
                                   print_with_time(fmt, ##__VA_ARGS__); \
                               } while (0)

#define log_brief(fmt, ...) do {\
                                if (args.debug_level >= 1) \
                                print_with_time(fmt, ##__VA_ARGS__); \
                            } while (0)

#define log_always(fmt, ...) print_with_time(fmt, ##__VA_ARGS__)
#endif
