/**
 * @file wpwindow.cpp
 * @brief 定义了查看精灵界面
 */

#include "../include/wpwindow.h"
#include "../include/ui/ui_WPWindow.h"

/**
 * @brief 构造函数
 * @param s 胜率字符串
 * @param parent 父窗口指针
 * @details 显示界面
 */
WPWindow::WPWindow(const std::string &s, QWidget *parent) : QWidget(parent), ui(new Ui::WPWindow) {
    ui->setupUi(this);
    setWindowIcon(QIcon("../img/client.ico"));
    ui->plainTextEdit->appendPlainText(QString::fromStdString(s));
}

/**
 * @brief 析构函数
 * @details 删除界面指针
 */
WPWindow::~WPWindow() {
    delete ui;
}
