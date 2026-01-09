/**
 * @file Tools.cpp
 * @brief 辅助类的实现文件。
 * @details 实现了各种辅助函数。
 */
#include "../include/Tools.h"
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <random>

#ifdef __linux__
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define sleep_ms(ms) usleep(ms*1000)
#else
using socklen_t = unsigned int;

#include <winsock2.h>

#define sleep_ms(ms) Sleep(ms)
#endif


int random::randInt(int a, int b) {
    std::random_device rd; // 随机数引擎的种子源
    std::minstd_rand gen(rd()); // 以 rd() 播种的 std::linear_congruential_engine
    std::uniform_int_distribution<int> distribution(a, b); // 按照均匀分布生成
    return distribution(gen);
}

double random::randDouble(double a, double b) {
    std::random_device rd; //随机数引擎的种子源
    std::minstd_rand gen(rd()); // 以 rd() 播种的 std::linear_congruential_engine
    std::uniform_real_distribution<double> distribution(a, b); // 按照均匀分布生成
    return round(distribution(gen) * 10000) / 10000; //保留到4位小数
}

void network::send_with_check(int fd, const char *buf, int len) {
    int send_len = send(fd, buf, len, 0);
    sleep_ms(40); // TCP传输数据流，防止连续两次发送被同一个recv()接收
    if (send_len < 0) // 检查发送长度，小于零说明出错
        throw std::runtime_error("发送数据失败");
}

void network::send_with_check(int fd, const std::string &buf) {
    send_with_check(fd, buf.c_str(), static_cast<int>(buf.length()));
}

void network::send_file(int fd, const std::string &file, bool isBinary) {
    std::ifstream file_in;
    if (isBinary)
        file_in.open(file, std::ios::in | std::ios::ate | std::ios::binary); // std::ios::ate 定位到流结尾
    else
        file_in.open(file, std::ios::in | std::ios::ate);
    if (file_in.fail()) {
        throw std::runtime_error(file + "打开失败");
    }
    auto size = static_cast<int>(file_in.tellg()); // 当前指针位置，即文件长度
    file_in.seekg(0, std::ios::beg); // 定位到流开头
    auto buffer = new char[size];
    file_in.read(buffer, size);
    file_in.close();
    send_with_check(fd, buffer, size);
}

/// 接收消息
std::string network::recv_with_check(int fd) {
    char buffer[128]{};
    int recv_len = recv(fd, buffer, sizeof(buffer), 0);
    if (recv_len < 0) { // 检查接收长度，小于零说明出错
        throw std::runtime_error("接受数据失败");
    } else if (recv_len == 0) { // 检查接收长度，等于零说明断开
        throw std::runtime_error("连接已断开");
    } else {
        return {buffer};
    }
}


void message::putMessage(const std::string &s) {
    time_t now = time(nullptr);
    struct tm t = *localtime(&now); // 当前时间结构体
    char buf[80];
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &t);
    std::clog << buf << ' ' << s << std::endl;
}

// 静态成员需要在头文件外声明从而为其分配内存
std::ofstream file::fout;
std::streambuf *file::oldCout;

void file::writeCache() {
    fout.open("cache", std::ios::out | std::ios::trunc); // 打开cache文件
    oldCout = std::cout.rdbuf(); // 保存旧的std::cout
    std::streambuf *fileBuf = fout.rdbuf(); // 得到文件流的缓冲区
    std::cout.rdbuf(fileBuf); //重定向std::cout
}

void file::closeCache() {
    fout.flush();
    fout.close();
    std::cout.rdbuf(oldCout); // 重定向回std::cout
}
