/**
 * @file viewpokemonwindow.cpp
 * @brief 定义了查看精灵界面类。
 * @details 该界面显示了所有玩家的所有精灵。
 */

#include "../include/viewpokemonwindow.h"
#include "../include/ui/ui_viewpokemonwindow.h"

/**
 * @brief 构造函数。
 * @param parent 父窗口指针。
 * @details 显示界面。
 */
ViewPokemonWindow::ViewPokemonWindow(QWidget *parent) : QWidget(parent), ui(new Ui::ViewPokemonWindow) {
    ui->setupUi(this);\
    setWindowIcon(QIcon("../img/client.ico"));
}

/**
 * @brief 析构函数。
 * @details 删除界面指针。
 */
ViewPokemonWindow::~ViewPokemonWindow() {
    delete ui;
}

/**
 * @brief 显示精灵。
 * @param username 用户名数组。
 * @param pokes 精灵数组。
 * @details 显示所有玩家的所有精灵。
 */
void ViewPokemonWindow::showPokemons(const std::vector<std::string> &username, std::vector<std::vector<std::string>> pokes) const {
    for (int i = 0; i < username.size(); ++i) {
        QString s = QString::fromStdString(username[i] + " :");
        for (const auto &poke: pokes[i]) {
            s += QString::fromStdString(" " + poke);
        }
        ui->plainTextEdit->appendPlainText(s);
    }
}


