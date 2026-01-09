/**
 * @file Server.cpp
 * @brief 服务器类的实现文件。
 * @details 实现了服务器类的各种方法。
 */
#include "../include/Pokemon.h"
#include "../include/Server.h"
#include "../include/Tools.h"
#include <iostream>
#include <cstring>
#include <stdexcept>
#include <thread>
#include <fstream>
#include <filesystem>
#include <string>
#include <set>

#ifdef __linux__
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define sleep_ms(ms) usleep(ms*1000)
#else // windows

#include <winsock2.h>
#include <windows.h>

#define sleep_ms(ms) Sleep(ms)

using socklen_t = int;
#endif

/**
 * @details 如果不存在UserData.json就创建之，已存在就将其读到@c userData 中。
 * 同时，将@c defaultPokemons 从文件中读出。
 */
Server::Server() {
    if (!std::filesystem::exists("../data/UserData.json")) {
        std::ofstream fout("../data/UserData.json");
        fout << "[]";
    } else {
        std::ifstream fin("../data/UserData.json");
        userData = json::parse(fin);
    }
    std::ifstream fin("../data/default_pokemons.json");
    defaultPokemons = json::parse(fin);
}

/**
 * @details 尝试创建套接字并监听客户端的连接请求，包含了出错时的应对措施。
 *
 * 为每一个连接请求创建单独线程，从而可以处理来自多个客户端的请求。
 */
void Server::serverMain() {
    try {
#ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw std::runtime_error("未能初始化Winsock.");
        }
#endif
        // 创建套接字
        int server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0) {
#ifdef _WIN32
            WSACleanup();
#endif
            throw std::runtime_error("创建套接字失败");
        }

        // 设置服务器地址和端口
        struct sockaddr_in server_addr = {};
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        server_addr.sin_port = htons(12315);

        //绑定套接字到地址和端口
        if (bind(server_fd, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr)) < 0) {
#ifdef _WIN32
            closesocket(server_fd);
            WSACleanup();
#else
            close(server_fd);
#endif
            throw std::runtime_error("绑定套接字失败");
        }

        //开始监听
        if (listen(server_fd, 5) < 0) {
#ifdef _WIN32
            closesocket(server_fd);
            WSACleanup();
#else
            close(server_fd);
#endif
            throw std::runtime_error("监听失败");
        }
        message::putMessage("服务器已启动，等待客户端连接...");

        while (true) {
            struct sockaddr_in client_addr;
            socklen_t client_addr_len = sizeof(client_addr);
            int client_fd = accept(server_fd, reinterpret_cast<struct sockaddr *>(&client_addr),
                                   &client_addr_len);
            // 创建线程对象，并将其所对应的执行实例与该线程对象分离，使之可以单独执行
            std::thread(&Server::clientHandler, this, client_fd).detach();
        }
    } catch (const std::runtime_error &e) {
        std::cerr << e.what();
    }
}

/**
 * @details 请求处理函数，针对客户端的请求分发给对应的函数执行。
 */
void Server::clientHandler(int client_fd) {
    try {
        if (client_fd < 0) {
            throw std::runtime_error("接受客户端连接失败");
        }
        message::putMessage("客户端连接成功, client_fd: " + std::to_string(client_fd));
        while (true) {
            auto buffer = network::recv_with_check(client_fd);
            message::putMessage("来自 " + std::to_string(client_fd) + ", 操作: " + options[buffer[0] - '0']);
            switch (buffer[0]) {
                case _disconnect:
                    message::putMessage(std::to_string(client_fd) + "已断开");
                    return;
                case _register:
                    registerAccount(client_fd);
                    break;
                case _login:
                    login(client_fd);
                    break;
                case _logout:
                    logout(client_fd);
                    break;
                case _view_pokemons:
                    viewAllUsersPokemons(client_fd);
                    break;
                case _view_users:
                    viewAllOnlineUsers(client_fd);
                    break;
                case _normal_battle:
                    battle(client_fd, _normal_battle);
                    break;
                case _duel_battle:
                    battle(client_fd, _duel_battle);
                    break;
                case _view_my_pokemons:
                    viewMyPokemons(client_fd);
                    break;
                case _view_winning_percentage:
                    viewWP(client_fd);
                    break;
            }
        }
    } catch (const std::runtime_error &e) {
        std::cerr << e.what();
    }
}

/**
 * @details 注册账户处理函数。先在@c userData 中查找请求的用户名是否已经注册，是则返回错误，
 * 否则初始化该user，为其分配3个随机Pokemon，并将Pokemon数据载入@c username2pokemon 。
 */
void Server::registerAccount(int client_fd) {
    auto buffer = network::recv_with_check(client_fd);
    bool same_name = false;
    for (const auto &it: userData)
        if (it["name"] == buffer) {
            same_name = true;
            break;
        }
    if (same_name) {
        network::send_with_check(client_fd, "NO", 2);
        message::putMessage(std::to_string(client_fd) + "尝试注册" + buffer + ". 失败: 用户名已存在");
        return;
    }
    message::putMessage(std::to_string(client_fd) + "尝试注册" + buffer + ". 成功");
    userData.push_back({
        {"name", buffer},
        {"normal", 0},
        {"normal_wins", 0},
        {"duel", 0},
        {"duel_wins", 0}
    });
    // 存下已经给予的Pokemon的编号，保证不会给予相同的Pokemon
    std::set<int> alreadyHad;
    for (int i = 0, _n = static_cast<int>(defaultPokemons.size()); i < 3; ++i) {
        int j = random::randInt(0, _n - 1);
        if (alreadyHad.count(j)) {
            while (alreadyHad.count(j))
                j = random::randInt(0, _n - 1);
            alreadyHad.insert(j);
        } else
            alreadyHad.insert(j);
        username2pokemon[buffer].emplace_back(generatePokemon(j));
    }
    network::send_with_check(client_fd, "OK", 2);
}

/**
 * @details 登录处理函数。先查看@c onlineUsers ，判断请求的用户名是否已经登录。
 * 再遍历@c userData ，如果找到对应的账户，就将其数据载入@c username2pokemon 。
 */
void Server::login(int client_fd) {
    auto buffer = network::recv_with_check(client_fd);
    bool flag = false;
    if (std::find(onlineUsers.begin(), onlineUsers.end(), buffer) != onlineUsers.end()) {
        message::putMessage(std::to_string(client_fd) + "尝试登录, 用户名: " + buffer + ". 失败: 已登录");
        network::send_with_check(client_fd, "ALREADY", 7);
    }
    for (const auto &it: userData) {
        if (it["name"] == buffer) {
            // 找到则载入内存
            flag = true;
            if (username2pokemon.find(buffer) != username2pokemon.end()) break;
            std::ifstream fin("../data/users/" + buffer + ".json");
            json j = json::parse(fin);
            for (const auto &poke: j) {
                std::string t = poke["type"];
                switch (t[0]) {
                    case 'P':
                        username2pokemon[buffer].emplace_back(std::make_unique<PowerPokemon>(poke));
                        break;
                    case 'T':
                        username2pokemon[buffer].emplace_back(std::make_unique<TankPokemon>(poke));
                        break;
                    case 'D':
                        username2pokemon[buffer].emplace_back(std::make_unique<DefensivePokemon>(poke));
                        break;
                    case 'A':
                        username2pokemon[buffer].emplace_back(std::make_unique<AgilePokemon>(poke));
                        break;
                }
            }
            break;
        }
    }
    if (flag) {
        message::putMessage(std::to_string(client_fd) + "尝试登录, 用户名: " + buffer + ". 成功");
        network::send_with_check(client_fd, "OK", 2);
        onlineUsers.emplace_back(buffer);
        fd2username[client_fd] = buffer;
        int highLevelNum = 0;
        for (const auto &poke: username2pokemon[buffer]) {
            // 发送用户所有Pokemon，便于显示
            network::send_with_check(client_fd, poke->getName());
            if (poke->getLevel() == 15) highLevelNum++;
        }
        network::send_with_check(client_fd, "Done", 4);
        network::send_with_check(client_fd, std::to_string(highLevelNum)); // 发送高级Pokemon数量
    } else {
        message::putMessage(std::to_string(client_fd) + "尝试登录, 用户名: " + buffer + ". 失败: 用户名不存在");
        network::send_with_check(client_fd, "NO", 2);
    }
}

/**
 * @details 用户登出时，将其拥有的Pokemon数据保存到同名json文件中，并从内存中删除该用户。
 *
 * 同时保存@c userData 到文件，这样做停止程序后可以确保数据不会丢失，提高了健壮性。
 */
void Server::logout(int client_fd) {
    message::putMessage(std::to_string(client_fd) + "注销");
    onlineUsers.erase(std::remove(onlineUsers.begin(), onlineUsers.end(), fd2username[client_fd]), onlineUsers.end());
    json j;

    for (const auto &poke: username2pokemon[fd2username[client_fd]]) {
        j.push_back({
            {"name", poke->getName()},
            {"type", poke->getType()},
            {"skill", poke->getSkill()},
            {"exp", poke->getExperience()},
            {"level", poke->getLevel()},
            {"hp", poke->getHealthPoint()},
            {"att_point", poke->getAttackPower()},
            {"defensive", poke->getDefensePower()},
            {"att_interval", poke->getAttackInterval()}
        });
    }
    std::ofstream fout("../data/users/" + fd2username[client_fd] + ".json", std::ios::out | std::ios::trunc);
    fout << j;
    fout.close();
    username2pokemon.erase(fd2username[client_fd]);
    fd2username.erase(client_fd);
    writeToFile();
}

/**
 * @details 将@c userData 存到@c UserData.json。
 */
void Server::writeToFile() const {
    std::ofstream fout("../data/UserData.json", std::ios::out | std::ios::trunc);
    if (!fout.is_open())
        throw std::runtime_error("未能打开文件");
    else
        fout << userData;
    fout.close();
}

/**
 * @details 先在内存中找用户名，没有找到再到文件中找。
 */
void Server::viewAllUsersPokemons(int client_fd) {
    std::string userNum = std::to_string(userData.size());
    network::send_with_check(client_fd, userNum);
    for (const auto &user: userData) {
        const std::string &username = user["name"];
        network::send_with_check(client_fd, username);
        if (username2pokemon.find(username) != username2pokemon.end()) {
            // 在内存中找用户名
            std::string pokeNum = std::to_string(username2pokemon[username].size());
            network::send_with_check(client_fd, pokeNum);
            for (const auto &poke: username2pokemon[username])
                network::send_with_check(client_fd, poke->getName());
        } else {
            // 到文件中找
            std::ifstream fin("../data/users/" + username + ".json");
            json j = json::parse(fin);
            network::send_with_check(client_fd, std::to_string(j.size()));
            for (const auto &poke: j)
                network::send_with_check(client_fd, poke["name"]);
        }
    }
}

void Server::viewAllOnlineUsers(int client_fd) {
    std::string num = std::to_string(onlineUsers.size());
    network::send_with_check(client_fd, num);
    for (const auto &user: onlineUsers) {
        network::send_with_check(client_fd, user);
    }
}

/**
 * @details 接收客户端文件描述符和战斗类型作为参数。
 * 接收客户端选择的Pokemon名称，并生成一个随机的对手。
 * 将对手的等级提升到等于用户选择的Pokemon的等级。
 * 开始战斗，并将战斗过程写入cache文件。
 * 将战斗结果和相关数据发送回客户端，更新高级Pokemon数量。
 * 决斗赛失败的用户会失去一只Pokemon，如果没有Pokemon了，会重新分配一只。
 */
void Server::battle(int client_fd, char kind) {
    const auto buffer = network::recv_with_check(client_fd);
    for (const auto &poke: username2pokemon[fd2username[client_fd]]) {
        if (poke->getName() == buffer) {
            // 生成对手
            const int j = random::randInt(0, static_cast<int>(defaultPokemons.size()) - 1);
            std::unique_ptr<Pokemon> other = std::move(generatePokemon(j));
            if (!other) {
                throw std::runtime_error("分配错误");
            }
            // 提升对手等级
            other->addExperience(poke->getExperience());
            // 开始战斗并写文件
            file::writeCache();
            bool flag = poke->battle(*other);
            file::closeCache();
            // 修改胜率
            if (flag) {
                message::putMessage("赢了");
                for (auto &user: userData) {
                    if (user["name"] == fd2username[client_fd]) {
                        if (kind == _normal_battle) {
                            int tmp = user["normal"];
                            user["normal"] = tmp + 1;
                            tmp = user["normal_wins"];
                            user["normal_wins"] = tmp + 1;
                        } else {
                            int tmp = user["duel"];
                            user["duel"] = tmp + 1;
                            tmp = user["duel_wins"];
                            user["duel_wins"] = tmp + 1;
                        }
                        break;
                    }
                }
            } else {
                message::putMessage("输了");
                for (auto &user: userData) {
                    if (user["name"] == fd2username[client_fd]) {
                        if (kind == _normal_battle) {
                            int tmp = user["normal"];
                            user["normal"] = tmp + 1;
                        } else {
                            int tmp = user["duel"];
                            user["duel"] = tmp + 1;
                        }
                        break;
                    }
                }
            }
            message::putMessage("胜率已更新");
            // 发送战斗结果: 对手名、战斗过程、双方图片、是否胜利、用户高级Pokemon数量
            network::send_with_check(client_fd, other->getName());
            network::send_file(client_fd, "cache", false);
            network::send_file(client_fd, "../img/" + poke->getName() + ".png", true);
            network::send_file(client_fd, "../img/" + other->getName() + ".png", true);
            network::send_with_check(client_fd, (flag ? "1" : "0"), 1);
            // 更新高级Pokemon数量
            int highLevelNum = 0;
            for (const auto &pp: username2pokemon[fd2username[client_fd]])
                if (pp->getLevel() == 15)
                    highLevelNum++;
            if (kind == _duel_battle) {
                if (flag) {
                    if (other->getLevel() == 15)
                        highLevelNum++;
                } else {
                    if (poke->getLevel() == 15)
                        highLevelNum--;
                }
            }
            network::send_with_check(client_fd, std::to_string(highLevelNum));
            // 决斗赛处理胜利和失败的结果
            if (kind == _duel_battle) {
                if (flag) {
                    username2pokemon[fd2username[client_fd]].emplace_back(std::move(other));
                } else {
                    // 失败
                    auto lose_poke = network::recv_with_check(client_fd); // 接受失去的Pokemon
                    for (const auto &pp: username2pokemon[fd2username[client_fd]]) {
                        if (pp->getName() == lose_poke) {
                            username2pokemon[fd2username[client_fd]].erase(
                                std::remove(username2pokemon[fd2username[client_fd]].begin(),
                                            username2pokemon[fd2username[client_fd]].end(), pp),
                                username2pokemon[fd2username[client_fd]].end());
                            break;
                        }
                    }
                    if (username2pokemon[fd2username[client_fd]].empty()) {
                        // 如果没有Pokemon了，重新分配
                        auto index = random::randInt(0, static_cast<int>(defaultPokemons.size()) - 1);
                        username2pokemon[fd2username[client_fd]].emplace_back(generatePokemon(index));
                        network::send_with_check(client_fd, defaultPokemons[index]["name"]);
                    }
                }
            }
            break;
        }
    }
}

/**
 * @details 查看用户拥有的Pokemon的以下信息：名字、类型、等级、经验、技能、血量、攻击力、防御力、攻击间隔。
 */
void Server::viewMyPokemons(int client_fd) {
    static const std::string description[]{
        "名字：", "\n类型：", "\n等级：", "\n经验：", "\n技能：", "\n血量：", "\n攻击力：",
        "\n防御力：", "\n攻击间隔："
    };
    std::string toSend;
    for (const auto &poke: username2pokemon[fd2username[client_fd]]) {
        toSend += description[0] + poke->getName() + description[1] + poke->getType() + description[2] +
                std::to_string(poke->getLevel()) + description[3] + std::to_string(poke->getExperience()) +
                description[4] + poke->getSkill() + description[5] + std::to_string(poke->getHealthPoint()) +
                description[6] + std::to_string(poke->getAttackPower()) + description[7] +
                std::to_string(poke->getDefensePower()) + description[8] + std::to_string(poke->getAttackInterval()) +
                "\n\n";
    }
    network::send_with_check(client_fd, toSend);
}

/**
 * @details 查看所有用户的胜率信息。具体：用户名、总场次、总胜率、升级赛场次、升级赛胜率、决斗赛场次、决斗赛胜率。
 * 如果未进行过任何比赛，胜率显示为none。
 */
void Server::viewWP(int client_fd) {
    std::string toSend;
    static const std::string description[]{
        "用户名：", "\n总场次：", "\n总胜率：", "\n升级赛场次：", "\n升级赛胜率：",
        "\n决斗赛场次：", "\n决斗赛胜率："
    };
    for (const auto &user: userData) {
        const std::string &username = user["name"];
        const int &normal = user["normal"];
        const int &duel = user["duel"];
        const int &normal_wins = user["normal_wins"];
        const int &duel_wins = user["duel_wins"];
        toSend += description[0] + username + description[1] + std::to_string(normal + duel) + description[2] +
                ((normal + duel == 0)
                     ? "none"
                     : (
                         std::to_string(1.0 * (normal_wins + duel_wins) / (normal + duel) * 100) + "%")) +
                description[3] + std::to_string(normal) + description[4] +
                ((normal == 0) ? "none" : (std::to_string(1.0 * normal_wins / normal * 100) + "%")) + description[5] +
                std::to_string(duel) + description[6] +
                ((duel == 0) ? "none" : (std::to_string(1.0 * duel_wins / duel * 100) + "%")) + "\n\n";
    }
    network::send_with_check(client_fd, toSend);
}

/**
 * @details 从预设Pokemon生成具体Pokemon。调用不同的构造函数生成不同类型的Pokemon。
 */
std::unique_ptr<Pokemon> Server::generatePokemon(int index) {
    std::string t = defaultPokemons[index]["type"];
    std::string na = defaultPokemons[index]["name"];
    std::string sk = defaultPokemons[index]["skill"];
    std::unique_ptr<Pokemon> ptr;
    switch (t[0]) {
        case 'P':
            ptr = std::make_unique<PowerPokemon>(na, sk);
            break;
        case 'T':
            ptr = std::make_unique<TankPokemon>(na, sk);
            break;
        case 'D':
            ptr = std::make_unique<DefensivePokemon>(na, sk);
            break;
        case 'A':
            ptr = std::make_unique<AgilePokemon>(na, sk);
            break;
    }
    return ptr;
}
