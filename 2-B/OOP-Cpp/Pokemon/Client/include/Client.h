/**
 * @file Client.h
 * @brief 客户端类的声明
 * @details 客户端类，用于与服务器进行通信。
 */

#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H

#include <string>
#include <vector>
#include <tuple>

/**
 * @class Client Client.h "include/Client.h"
 * @brief 客户端类
 * @details 客户端类，用于与服务器进行通信
 */
class Client {
private:
    int client_fd; ///< 客户端套接字
    bool isConnectedFlag = false; ///< 是否连接到服务器标志
    /**
     * @brief 发送 @p s 到服务器
     * @param s 要发送的字符串
     */
    void sendToServer(const std::string &s) const;

    /**
     * @brief 从服务器接收字符串
     * @return 从服务器接收到的字符串
     */
    std::string recvFromServer() const;

    char *buf; ///< 接收缓冲区，构造函数中初始化，析构函数中释放

    /// 协议字符串，用于客户端与服务器通信
    static const std::string _disconnect, _register, _login, _logout, _view_pokemons, _view_users, _normal_battle, _duel_battle, _view_my_pokemons, _view_winning_percentage;
    std::vector<std::string> myPokemons; ///< 我的精灵
    int highLevelNum = 0; ///< 高级精灵的数量


public:

    /// 构造函数
    Client();

    /// 析构函数
    ~Client();

    /// 返回是否连接到服务器
    bool isConnected() const;

    /**
     * @brief 注册账户
     * @param userName 用户名
     */
    void registerAccount(const std::string &userName) const;

    /**
     * @brief 登录
     * @param userName 用户名
     */
    void login(const std::string &userName);

    /// 登出
    void logout() const;

    /**
     * @brief 查看所有用户的精灵
     * @param username 用户名数组的引用
     * @param pokes 精灵数组的引用
     */
    void viewAllUsersPokemons(std::vector<std::string> &username, std::vector<std::vector<std::string>> &pokes) const;

    /**
     * @brief 查看所有在线用户
     * @param users 用户名数组的引用
     */
    void viewAllOnlineUsers(std::vector<std::string> &users) const;

    /// 连接到服务器
    void connectToServer();

    /// 断开连接
    void disconnect() const;

    /**
     * @brief 普通战斗
     * @param pokemon 我方精灵
     * @param isDuel 是否是决斗赛
     * @return 返回一个元组，包含对手精灵、对战过程、对战结果
     */
    std::tuple<std::string, std::string, bool> battle(const std::string &pokemon, bool isDuel) const;

    /**
     * @brief 查看我的精灵
     * @return 我的精灵的数组
     */
    const std::vector<std::string> &getMyPokemons() const;

    /**
     * @brief 向myPokemons中添加精灵
     * @param s 要添加的精灵
     */
    void addPokemon(const std::string & s);

    /**
     * @brief 从myPokemons中删除精灵
     * @param i 要删除的精灵的下标
     */
    void removePokemon(int i);

    /**
     * @brief 获取精灵的数量
     * @return 精灵的数量，即myPokemons的大小
     */
    int getPokemonNum() const;

    /**
     * @brief 从服务器获取精灵的图片
     * @param i 保存图片的文件名
     */
    void getPic(int i) const;

    /**
     * @brief 查看我的精灵
     */
    std::string viewMyPokemons() const;

    /**
     * @brief 查看胜率
     * @return 字符串，包含胜率信息
     */
    std::string viewWP() const; // winning percentage
    /**
     * @brief 从服务器获取新精灵并添加到myPokemons中
     * @return 新精灵的名字
     */
    std::string getNewPokemon();

    /**
     * @brief 向服务器发送要失去的精灵
     * @param s 要失去的精灵
     */
    void losePokemon(const std::string &) const;

    /**
     * @brief 向服务器发送要失去的精灵
     * @param i 要失去的精灵的下标
     */
    void losePokemon(int i) const;

    /**
     * @brief 获取高级精灵的数量
     * @return 高级精灵的数量
     */
    int getHighLevelNum() const;

    /**
     * @brief 更新高级精灵的数量
     */
    void updateHighLevelNum();

};


#endif //CLIENT_CLIENT_H
