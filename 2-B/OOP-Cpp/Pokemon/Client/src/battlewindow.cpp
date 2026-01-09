/**
 * @file battlewindow.cpp
 * @brief 战斗窗口类的实现。
 * @details 实现了战斗窗口的构造函数、析构函数、显示战斗日志、关闭事件。
 */

#include "../include/battlewindow.h"
#include "../include/ui/ui_battlewindow.h"
#include "../include/losewindow.h"
#include <windows.h>
#include <QMessageBox>
#include <utility>

/**
 * @brief 构造函数。
 * @param _me 我的精灵。
 * @param client 指向客户端的unique_ptr。
 * @param fn 回调函数，接受四个参数：客户端指针、对方精灵、是否胜利、是否是决斗赛。
 * @param _isDuel 是否是决斗赛。
 * @param parent 父窗口指针。
 * @details 设置图标，初始化界面，连接了开始按钮的clicked信号与showBattleLog槽。用fn初始化back_to_caller，用_isDuel初始化isDuel。
 */
BattleWindow::BattleWindow(std::string _me, std::unique_ptr<Client> client,
                           std::function<void(std::unique_ptr<Client>, std::string, bool, bool)> fn, bool _isDuel,
                           QWidget *parent) : me(std::move(_me)), cli(std::move(client)), back_to_caller(std::move(fn)),
                                              isDuel(_isDuel), QWidget(parent), ui(new Ui::BattleWindow) {
    ui->setupUi(this);
    setWindowIcon(QIcon("../img/client.ico"));
    connect(ui->startButton, &QPushButton::clicked, this, &BattleWindow::showBattleLog);
}

/**
 * @brief 析构函数。
 * @details 删除界面指针。
 */
BattleWindow::~BattleWindow() {
    delete ui;
}

/**
 * @brief 显示战斗日志。
 * @details 调用客户端的battle函数，显示战斗日志，根据战斗结果弹出对话框，更新高级精灵数量。
 */
void BattleWindow::showBattleLog() {
    ui->startButton->hide(); // 隐藏开始按钮
    const auto tup = cli->battle(me, isDuel); // 调用Client的battle函数，返回值为三元组
    // 设置我方和对方图片
    QImage img1("../tmp/1.png");
    QImage img2("../tmp/2.png");
    img1 = img1.scaled(300, 300, Qt::KeepAspectRatio);
    img2 = img2.scaled(300, 300, Qt::KeepAspectRatio);
    ui->myPoke->setPixmap(QPixmap::fromImage(img1));
    ui->hisPoke->setPixmap(QPixmap::fromImage(img2));
    // 获取三元组的三个元素
    enemy = std::get<0>(tup);
    msg = std::get<1>(tup);
    result = std::get<2>(tup);
    // 设置标签
    ui->myPokeLabel->setText(QString::fromStdString(me));
    ui->hisPokeLabel->setText(QString::fromStdString(enemy));
    int pos = 0; // 行开头位置
    // 遍历战斗消息，模拟战斗过程
    for (int i = 0; i < msg.length(); ++i) {
        if (msg[i] == '\n') { // 遇到换行符
            ui->plainTextEdit->appendPlainText(QString::fromStdString(msg.substr(pos, i - pos))); // 截取上一行并添加到文本框
            pos = i + 1;
            Sleep(175); // 等待一段时间
            QApplication::processEvents(); // 处理事件，保证文本框能够及时显示
        }
    }
    // 如果是决斗赛，根据战斗结果弹出对话框
    if (isDuel) {
        if (result) {
            cli->addPokemon(enemy);
            auto *message = new QMessageBox(this);
            message->setText(QString::fromStdString("(*^_^*)恭喜你获得了 " + enemy + "！"));
            message->exec();
            delete message;
        } else {
            if (cli->getPokemonNum() == 1) { // 失败且只剩一个精灵，直接失去
                auto *message = new QMessageBox(this);
                message->setText(QString::fromStdString("/(ㄒoㄒ)/~~你失去了 " + me + "！"));
                message->exec();
                delete message;
                cli->removePokemon(0);
                cli->losePokemon(me);

            } else { // 失败且有多个精灵，弹出选择窗口
                auto *lw = new LoseWindow(cli->getMyPokemons());
                auto r = lw->exec() - 10;
                cli->losePokemon(r);
                cli->removePokemon(r);
            }
        }
    }
    cli->updateHighLevelNum(); // 更新高级精灵数量
}

/**
 * @brief 重载关闭事件。
 * @param event 关闭事件。
 * @details 调用回调函数，关闭窗口。
 */
void BattleWindow::closeEvent(QCloseEvent *event) {
    back_to_caller(std::move(cli), enemy, result, isDuel);
    event->accept();
}

