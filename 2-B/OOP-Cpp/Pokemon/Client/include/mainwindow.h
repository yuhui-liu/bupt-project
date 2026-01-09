/**
 * @file mainwindow.h
 * @brief 主窗口类的声明。
 * @details 声明了主窗口类，包括构造函数、析构函数、注册账户、连接服务器、登录、添加消息、关闭事件、获得客户端指针、回调函数。
 */

#ifndef CLIENT_MAINWINDOW_H
#define CLIENT_MAINWINDOW_H

#include "Client.h"
#include <QMainWindow>
#include <string>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @class MainWindow mainwindow.h "include/mainwindow.h"
 * @brief 主窗口类
 * @details 主窗口类，用于显示主窗口，实现了注册账户、连接服务器、登录、添加消息、关闭事件、获得客户端指针、回调函数。
 */
class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr); // 构造函数
    ~MainWindow() override; // 析构函数
    void cli_registerAccount(); // 注册账户
    void cli_connectToServer(); // 连接服务器
    void cli_login(); // 登录
    void appendMessage(const std::string &s) const; // 向消息框添加消息
    void closeEvent(QCloseEvent *event) override; // 重载关闭事件
    void setCli(std::unique_ptr<Client> &&cli2); // 获得客户端指针
    void callback(std::unique_ptr<Client> &&cli2); // 回调函数

private:
    Ui::MainWindow *ui; ///< 界面指针
    std::unique_ptr<Client> cli; ///< 指向客户端的unique_ptr
};


#endif //CLIENT_MAINWINDOW_H
