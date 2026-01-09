/**
 * @file main.cpp
 * @brief 定义了主函数。
 * @details 创建了QApplication对象，创建了主窗口对象，显示主窗口，连接服务器。
 */
#include <QApplication>
#include "../include/mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv); // 创建QApplication对象
    MainWindow mainwind; // 创建主窗口对象
    mainwind.show(); // 显示主窗口
    mainwind.cli_connectToServer(); // 连接服务器
    return QApplication::exec(); // 调用QApplication的exec函数
}
