/**
 * @file secondwindow.h
 * @brief 次窗口类的声明。
 * @details 声明了次窗口类，包括构造函数、析构函数、注销、查看所有在线用户、查看所有用户的精灵、升级赛、决斗赛、查看我的精灵、查看胜率、添加消息、重载关闭事件、回调函数、设置显示的徽章图片。
 */

#ifndef CLIENT_SECONDWINDOW_H
#define CLIENT_SECONDWINDOW_H

#include "Client.h"
#include <QWidget>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class SecondWindow; }
QT_END_NAMESPACE

/**
 * @class SecondWindow secondwindow.h "include/secondwindow.h"
 * @brief 次窗口类
 * @details 次窗口类，用于显示次窗口，实现了注销、查看所有在线用户、查看所有用户的精灵、升级赛、决斗赛、查看我的精灵、查看胜率、添加消息、重载关闭事件、回调函数、设置显示的徽章图片。
 */
class SecondWindow : public QWidget {
Q_OBJECT

public:
    // 构造函数
    explicit SecondWindow(const QString &s, std::unique_ptr<Client> client,
                          std::function<void(std::unique_ptr<Client>)> fn, QWidget *parent = nullptr);

    // 析构函数
    ~SecondWindow() override;

    // 注销
    void cli_logout();

    // 查看所有在线用户
    void cli_viewAllOnlineUsers() const;

    // 查看所有用户的精灵
    void cli_viewAllUsersPokemons() const;

    // 升级赛
    void cli_normalBattle();

    // 决斗赛
    void cli_duelBattle();

    // 查看我的精灵
    void cli_viewMyPokemons() const;

    // 查看胜率
    void cli_viewWP() const;

    // 添加消息
    void appendMessage(const std::string &s) const;

    // 重载关闭事件
    void closeEvent(QCloseEvent *event) override;

    // 回调函数
    void callback(std::unique_ptr<Client> &&cli2, const std::string &s, bool r, bool isDuel);

    // 设置显示的徽章图片
    void setBadge() const;

private:
    Ui::SecondWindow *ui; ///< 界面指针
    std::unique_ptr<Client> cli; ///< 客户端指针
    std::function<void(std::unique_ptr<Client>)> back_to_caller; ///< 回到Mainwindow的回调函数
};


#endif //CLIENT_SECONDWINDOW_H
