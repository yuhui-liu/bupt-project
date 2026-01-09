/**
 * @file Client.cpp
 * @brief 客户端类的实现。
 * @details 客户端类，用于与服务器进行通信。
 */

#include "../include/Client.h"
#include <cstring>
#include <stdexcept>
#include <winsock2.h>
#include <windows.h>
#include <fstream>

const std::string Client::_disconnect("0");
const std::string Client::_register("1");
const std::string Client::_login("2");
const std::string Client::_logout("3");
const std::string Client::_view_pokemons("4");
const std::string Client::_view_users("5");
const std::string Client::_normal_battle("6");
const std::string Client::_duel_battle("7");
const std::string Client::_view_my_pokemons("8");
const std::string Client::_view_winning_percentage("9");

// 注册账户
void Client::registerAccount(const std::string &userName) const {
    sendToServer(_register); // 发送请求
    Sleep(50); // 防止阻塞
    sendToServer(userName); // 发送userName
    // 接收结果并检查
    auto res = recvFromServer();
    if (res[0] == 'N')
        throw std::runtime_error("不能重名！");
}

/**
 * @details 登录，包含检查是否已注册，是否已登录，以及接收精灵信息。
 */
void Client::login(const std::string &userName) {
    sendToServer(_login); // 发送请求
    Sleep(50); // 防止阻塞
    sendToServer(userName); // 发送userName
    // 接收结果并检查
    auto res = recvFromServer();
    if (res[0] == 'N')
        throw std::runtime_error("未注册！");
    if (res[0] == 'A')
        throw std::runtime_error("已登录！");
    std::string s;
    myPokemons.clear();
    while (true) {
        s = recvFromServer();
        if (s[0] == 'D') break;
        myPokemons.emplace_back(s);
    }
    highLevelNum = std::stoi(recvFromServer());
}

// 登出
void Client::logout() const {
    sendToServer(_logout);
}

/**
 * @details 查看所有用户的精灵，用引用返回结果。
 */
void
Client::viewAllUsersPokemons(std::vector<std::string> &username, std::vector<std::vector<std::string>> &pokes) const {
    sendToServer(_view_pokemons);
    auto n = std::stoi(recvFromServer()); // 接收用户数
    pokes.resize(n); // 重设pokes大小
    for (int i = 0; i < n; ++i) {
        auto user = recvFromServer(); // 接收用户名
        username.emplace_back(user);
        auto n2 = std::stoi(recvFromServer()); // 接收精灵数
        for (int j = 0; j < n2; ++j) {
            auto poke = recvFromServer(); // 接收精灵
            pokes[i].emplace_back(poke);
        }
    }
}

// 查看所有在线用户
void Client::viewAllOnlineUsers(std::vector<std::string> &users) const {
    sendToServer(_view_users);
    auto n = std::stoi(recvFromServer());
    for (int i = 0; i < n; ++i) {
        auto username = recvFromServer();
        users.emplace_back(username);
    }
}

/**
 * @details 连接到服务器。包含初始化Winsock，创建套接字，设置服务器地址和端口，连接服务器。
 */
void Client::connectToServer() {
    // Windows下，初始化 Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        throw std::runtime_error("未能初始化Winsock.");

    // 创建套接字
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        WSACleanup(); //终止使用 Winsock 2 DLL
        throw std::runtime_error("创建套接字失败.");
    }

    // 设置服务器地址和端口
    struct sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(12315);

    // 连接服务器
    if (connect(client_fd, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr)) < 0) {
        closesocket(client_fd); //关闭套接字
        WSACleanup();
        throw std::runtime_error("连接服务器失败.");
    }
    isConnectedFlag = true;
}

/**
 * @details 发送 @p s 到服务器，并检查是否发送成功。
 */
void Client::sendToServer(const std::string &s) const {
    int send_len = send(client_fd, s.c_str(), s.length(), 0);
    if (send_len < 0) {
        closesocket(client_fd);
        WSACleanup();
        throw std::runtime_error("发送数据失败.");
    }
}

/**
 * @details 接收服务器返回的结果，并检查是否接收成功。将结果储存在@c buf 中。
 */
std::string Client::recvFromServer() const {
    // 接收服务器返回的结果
    memset(buf, 0, 65536);
    int recv_len = recv(client_fd, buf, 65536, 0);
    if (recv_len < 0) {
        closesocket(client_fd);
        WSACleanup();
        throw std::runtime_error("接收数据失败");
    }
    return {buf};
}

/**
 * @details 断开连接，关闭套接字，终止使用Winsock 2 Dll，并释放相关资源。
 */
void Client::disconnect() const {
    sendToServer(_disconnect);
    closesocket(client_fd);
    WSACleanup();
}

/**
 * @details 构造函数，初始化缓冲区。
 */
Client::Client() {
    buf = new char[65536];
}

/**
 * @details 析构函数，释放缓冲区。
 */
Client::~Client() {
    delete[] buf;
}

/**
 * @details 返回是否连接到服务器。
 */
bool Client::isConnected() const {
    return isConnectedFlag;
}


// 返回我的精灵
const std::vector<std::string> &Client::getMyPokemons() const {
    return myPokemons;
}

/**
 * @details 从服务器接收图片，并保存到.\tmp文件夹，名称为i.png。
 * @param i 名称
 */
void Client::getPic(int i) const {
    char *buffer = new char[1048576];
    int recv_len = recv(client_fd, buffer, 1048576, 0);
    if (recv_len < 0) {
        closesocket(client_fd);
        WSACleanup();
        throw std::runtime_error("接收数据失败");
    }
    std::ofstream writePic("../tmp/" + std::to_string(i) + ".png", std::ios::out | std::ios::trunc | std::ios::binary);
    writePic.write(buffer, recv_len);
    writePic.close();
    delete[] buffer;
}

/**
 * @details 升级赛或决斗赛，返回敌人名字、战斗信息、是否胜利。
 */
std::tuple<std::string, std::string, bool> Client::battle(const std::string &pokemon, bool isDuel) const {
    sendToServer((isDuel ? _duel_battle : _normal_battle));
    sendToServer(pokemon);
    auto enemy = recvFromServer();
    auto msg = recvFromServer();
    getPic(1);
    getPic(2);
    auto result = (recvFromServer()[0] == '1');
    return std::make_tuple(enemy, msg, result);
}

/**
 * @details 查看我的精灵。
 * @return 字符串，表示我的精灵。
 */
std::string Client::viewMyPokemons() const {
    sendToServer(_view_my_pokemons);
    return recvFromServer();
}

/**
 * @details 查看胜率。
 * @return 字符串，表示胜率。
 */
std::string Client::viewWP() const {
    sendToServer(_view_winning_percentage);
    return recvFromServer();
}

// 从服务器获取新精灵并添加到myPokemons中
std::string Client::getNewPokemon() {
    auto s = recvFromServer();
    addPokemon(s);
    return s;
}

// 向myPokemons中添加精灵
void Client::addPokemon(const std::string &s) {
    myPokemons.emplace_back(s);
}

// 获取精灵数量
int Client::getPokemonNum() const {
    return static_cast<int>(myPokemons.size());
}

// 从myPokemons中删除精灵
void Client::removePokemon(int i) {
    myPokemons.erase(myPokemons.begin() + i);
}

// 向服务器发送要失去的精灵
void Client::losePokemon(const std::string &s) const {
    sendToServer(s);
}

// 向服务器发送要失去的精灵
void Client::losePokemon(int i) const {
    sendToServer(myPokemons[i]);
}

// 获取高级精灵的数量
int Client::getHighLevelNum() const {
    return highLevelNum;
}

// 更新高级精灵的数量
void Client::updateHighLevelNum() {
    highLevelNum = std::stoi(recvFromServer());
}

