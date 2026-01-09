/**
 * @file Tools.h
 * @brief 辅助函数类的头文件。
 * @details 包含了一些辅助类，这些类提供静态方法作为辅助函数。
 */
#ifndef BATTLESYSTEM_TOOLS_H
#define BATTLESYSTEM_TOOLS_H

#include <fstream>

/**
 * @class random Tools.h "include/Tools.h"
 * @brief 随机数类
 * @details 封装了随机数引擎，产生符合均匀分布的随机数。
 */
class random {
public:
    /**
     * @brief 产生[a,b]之间的随机整数
     * @param a 下界
     * @param b 上界
     * @return 随机整数
     */
    static int randInt(int a, int b);

    /**
     * @brief 产生[a,b]之间的随机实数，保留小数点后四位
     * @param a 下界
     * @param b 上界
     * @return 随机实数，保留小数点后四位
     */
    static double randDouble(double a, double b);
};

/**
 * @class network Tools.h "include/Tools.h"
 * @brief 网络通信类
 * @details 用于网络通信的类，封装了安全的@c send 、@c recv 函数。
 */
class network {
public:
    /**
     * @brief 发送一串字节，并检查是否成功
     * @param fd 对方套接字
     * @param buf 待发送缓冲区
     * @param len 待发送内容长度
     */
    static void send_with_check(int fd, const char *buf, int len);

    /**
     * @brief 发送一个@c string ，并检查是否成功
     * @param fd 对方套接字
     * @param buf 待发送字符串
     */
    static void send_with_check(int fd, const std::string &buf);

    /**
     * @brief 发送文件（文本或二进制）
     * @param fd 对方方套接字
     * @param file 文件名
     * @param isBinary 是否是二进制文件
     */
    static void send_file(int fd, const std::string &file, bool isBinary);

    /**
     * @brief 接受数据，转化成字符串返回
     * @param fd 对方套接字
     * @return 收到的字符串
     */
    static std::string recv_with_check(int fd);
};

/**
 * @class message Tools.h "include/class.h"
 * @brief 消息类
 * @details 提供打印日志函数。
 */
class message {
public:
    /**
     * @brief 打印含有时间的字符串
     * @param s 待打印的字符串
     */
    static void putMessage(const std::string &s);
};

/**
 * @class file Tools.h "include/Tools.h"
 * @brief 将对战消息写到@c cache ，便于发送
 */
class file {
public:
    /// 重定向@c std::cout ，写对战消息到@c cache
    static void writeCache();

    /// 关闭文件，重定向@c std::cout 回控制台
    static void closeCache();

private:
    /// 指向cache的文件流
    static std::ofstream fout;
    /// 重定向前的@c std::cout ，用于重定向@c std::cout 回控制台
    static std::streambuf *oldCout;

};

#endif //BATTLESYSTEM_TOOLS_H
