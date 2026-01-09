/**
 * @file Server.h
 * @brief 服务器类的头文件。
 * @details 定义了服务器类，包括服务器主循环、事件处理循环、注册账户、登录、登出、查看所有用户Pokemon、查看所有在线用户、对战、查看我的Pokemon、查看胜率等方法。
 */
#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <vector>
#include <string>
#include <map>
#include "../include/nlohmann/json.hpp"
#include "../include/Pokemon.h"

using json = nlohmann::json;

/**
 * @class Server Server.h "include/Server.h"
 * @brief 服务器类
 * @details 定义了服务器端的响应请求方法。
 */
class Server {
private:
    /// 选项字符串，用于日志信息的打印
    inline static const std::string options[]{"Disconnect", "Register", "Login", "Logout", "View Pokemons",
                                              "View Online Users", "Normal Battle", "Duel Battle", "View My Pokemons",
                                              "View Winning Percentage"};
    /// 字符常量，用于通信双方确认请求类型
    static constexpr char _disconnect = '0', _register = '1', _login = '2', _logout = '3', _view_pokemons = '4', _view_users = '5', _normal_battle = '6', _duel_battle = '7', _view_my_pokemons = '8', _view_winning_percentage = '9';
    /// 当前在线用户
    std::vector<std::string> onlineUsers;
    /** 用户数据json对象
     * @details 包含@c name 用户名，@c normal 升级赛场次，@c normal_wins 升级赛胜场次，@c duel 决斗赛场次，@c duel_wins 决斗赛胜场次。
     */
    json userData;
    /// 将文件描述符（套接字）映射到用户名
    std::map<int, std::string> fd2username;
    /// 将用户名映射到所拥有的Pokemon
    std::map<std::string, std::vector<std::unique_ptr<Pokemon>>> username2pokemon;
    /// 从default_pokemons.json中读取预设Pokemon信息，并保存到带对象中，方便从预设Pokemon生成
    json defaultPokemons;

    /// 从预设Pokemon生成具体Pokemon
    std::unique_ptr<Pokemon> generatePokemon(int index);

public:
    /// 构造函数
    Server();

    /// 析构函数
    ~Server() = default;

    /// 服务器主循环
    void serverMain();

    /**
     * @brief 事件处理循环
     * @param client_fd 客户端套接字
     */
    void clientHandler(int client_fd);

    /**
     * @brief 注册账户
     * @param client_fd 客户端套接字
     */
    void registerAccount(int client_fd);

    /**
     * @brief 登录账户
     * @param client_fd 客户端套接字
     */
    void login(int client_fd);

    /**
     * @brief 登出账户
     * @param client_fd 客户端套接字
     */
    void logout(int client_fd);

    /// 写用户数据到文件
    void writeToFile() const;

    /**
     * @brief 查看所有用户Pokemon
     * @param client_fd 客户端套接字
     */
    void viewAllUsersPokemons(int client_fd);

    /**
     * @brief 查看所有在线用户
     * @param client_fd 客户端套接字
     */
    void viewAllOnlineUsers(int client_fd);

    /**
     * @brief 对战
     * @param client_fd 客户端套接字
     * @param kind 战斗种类
     */
    void battle(int client_fd, char kind);

    /**
     * @brief 查看我的Pokemon
     * @param client_fd 客户端套接字
     */
    void viewMyPokemons(int client_fd);

    /**
     * @brief 查看胜率
     * @param client_fd 客户端套接字
     */
    void viewWP(int client_fd);
};

#endif //SERVER_SERVER_H
