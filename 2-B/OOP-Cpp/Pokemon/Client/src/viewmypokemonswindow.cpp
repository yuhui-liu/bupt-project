/**
 * @file viewmypokemonswindow.cpp
 * @brief 定义了查看精灵界面类。
 * @details 该界面显示了玩家的所有精灵。
 */

#include "../include/viewmypokemonswindow.h"
#include "../include/ui/ui_ViewMyPokemonsWindow.h"

/**
 * @brief 构造函数。
 * @param s 精灵列表。
 * @param parent 父窗口指针。
 * @details 设置界面，显示精灵列表。
 */
ViewMyPokemonsWindow::ViewMyPokemonsWindow(const std::string &s, QWidget *parent) : QWidget(parent),
                                                                                    ui(new Ui::ViewMyPokemonsWindow) {
    ui->setupUi(this);
    setWindowIcon(QIcon("../img/client.ico"));
    ui->plainTextEdit->appendPlainText(QString::fromStdString(s));
}

/**
 * @brief 析构函数。
 * @details 删除界面指针。
 */
ViewMyPokemonsWindow::~ViewMyPokemonsWindow() {
    delete ui;
}
