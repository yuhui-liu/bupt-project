/**
 * @file secondwindow.cpp
 * @brief 次窗口类的实现。
 * @details 实现了次窗口的构造函数、析构函数、登出、查看所有在线用户、查看所有用户的精灵、升级赛、决斗赛、查看我的精灵、查看胜率、添加消息、重载关闭事件、回调函数、设置显示的徽章图片。
 */

#include "../include/secondwindow.h"
#include "../include/ui/ui_secondwindow.h"
#include "../include/viewpokemonwindow.h"
#include "../include/battlewindow.h"
#include "../include/viewmypokemonswindow.h"
#include "../include/wpwindow.h"
#include <QScrollBar>
#include <QTime>
#include <utility>
#include <QMessageBox>

/**
 * @brief 构造函数。
 * @param s 用户名。
 * @param client 客户端指针。
 * @param fn 回到Mainwindow的回调函数。
 * @param parent 父窗口指针。
 * @details 设置图标，设置显示的用户名，设置徽章，获取我的精灵并添加到下拉框中，连接了注销按钮、查看所有在线用户按钮、查看所有用户的精灵按钮、升级赛按钮、决斗赛按钮、查看我的精灵按钮、查看胜率按钮的信号与槽。
 */
SecondWindow::SecondWindow(const QString &s, std::unique_ptr<Client> client,
                           std::function<void(std::unique_ptr<Client>)> fn, QWidget *parent) : QWidget(parent),
                                                                                               ui(new Ui::SecondWindow),
                                                                                               cli(std::move(client)),
                                                                                               back_to_caller(
                                                                                                       std::move(fn)) {
    ui->setupUi(this);
    setWindowIcon(QIcon("../img/client.ico"));
    ui->label->setText(s);
    setBadge();
    auto pokemons = cli->getMyPokemons();
    for (const auto &pokemon: pokemons) {
        ui->comboBox->addItem(QString::fromStdString(pokemon));
    }
    // 连接登出按钮的clicked信号与cli_logout槽
    connect(ui->logoutButton, &QPushButton::clicked, this, &SecondWindow::cli_logout);
    // 连接查看所有在线用户按钮的clicked信号与cli_viewAllOnlineUsers槽
    connect(ui->onlineuserButton, &QPushButton::clicked, this, &SecondWindow::cli_viewAllOnlineUsers);
    // 连接查看所有用户的精灵按钮的clicked信号与cli_viewAllUsersPokemons槽
    connect(ui->alluserpokemonButton, &QPushButton::clicked, this, &SecondWindow::cli_viewAllUsersPokemons);
    // 连接升级赛按钮的clicked信号与cli_normalBattle槽
    connect(ui->normalBattleButton, &QPushButton::clicked, this, &SecondWindow::cli_normalBattle);
    // 连接决斗赛按钮的clicked信号与cli_duelBattle槽
    connect(ui->duelBattleButton, &QPushButton::clicked, this, &SecondWindow::cli_duelBattle);
    // 连接查看我的精灵按钮的clicked信号与cli_viewMyPokemons槽
    connect(ui->viewMyPokemonButton, &QPushButton::clicked, this, &SecondWindow::cli_viewMyPokemons);
    // 连接查看胜率按钮的clicked信号与cli_viewWP槽
    connect(ui->WPButton, &QPushButton::clicked, this, &SecondWindow::cli_viewWP);
}

/**
 * @brief 析构函数。
 * @details 删除界面指针。
 */
SecondWindow::~SecondWindow() {
    delete ui;
}

/**
 * @brief 登出。
 * @details 按下登出按钮后窗口将关闭，并回到Mainwindow。
 */
void SecondWindow::cli_logout() {
    this->close(); // 关闭本窗口
}

/**
 * @brief 查看所有在线用户。
 * @details 调用Client的viewAllOnlineUsers函数查看所有在线用户，将人数和时间以及具体用户名显示在文本框中。
 */
void SecondWindow::cli_viewAllOnlineUsers() const {
    std::vector<std::string> users;
    cli->viewAllOnlineUsers(users);
    appendMessage(QTime::currentTime().toString().toStdString() + "\n当前在线" + std::to_string(users.size()) + "人");
    for (const auto &user: users)
        appendMessage(user);
}

/**
 * @brief 查看所有用户的精灵。
 * @details 调用Client的viewAllUsersPokemons函数查看所有用户的精灵，将用户名和精灵显示在新窗口中。
 */
void SecondWindow::cli_viewAllUsersPokemons() const {
    auto *vpw = new ViewPokemonWindow();
    std::vector<std::string> username;
    std::vector<std::vector<std::string>> pokes;
    cli->viewAllUsersPokemons(username, pokes);
    vpw->showPokemons(username, pokes);
    vpw->setAttribute(Qt::WA_DeleteOnClose, true); // 设置属性：关闭窗口时析构
    vpw->show();
}

/**
 * @brief 添加消息。
 * @param s 消息。
 * @details 将消息添加到文本框中，并将滚动条移动到最下方。
 */
void SecondWindow::appendMessage(const std::string &s) const {
    ui->plainTextEdit->appendPlainText(QString::fromStdString(s));
    ui->plainTextEdit->verticalScrollBar()->setValue(ui->plainTextEdit->verticalScrollBar()->maximum());
}

/**
 * @brief 重载关闭事件。
 * @param event 关闭事件。
 * @details 调用Client的logout函数注销，调用回调函数回到Mainwindow，然后关闭窗口。
 */
void SecondWindow::closeEvent(QCloseEvent *event) {
    cli->logout();
    back_to_caller(std::move(cli));
    event->accept();
}

/**
 * @brief 升级赛。
 * @details 调用Client的normalBattle函数进行升级赛，创建一个新的BattleWindow窗口，隐藏本窗口。
 */
void SecondWindow::cli_normalBattle() {
    // 创建一个新的BattleWindow窗口
    // 其中的回调函数是一个lambda表达式，捕获了this指针，声明了4个转发引用作为参数
    // std::forward<decltype(PH1)>(PH1)表示转发PH1的类型，保持原有的左值或右值特性
    auto *baw = new BattleWindow(ui->comboBox->currentText().toStdString(), std::move(cli),
                                 [this](auto &&PH1, auto &&PH2, auto &&PH3, auto &&PH4) {
                                     callback(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2),
                                              std::forward<decltype(PH3)>(PH3), std::forward<decltype(PH4)>(PH4));
                                 }, false);
    baw->setAttribute(Qt::WA_DeleteOnClose, true); // 设置属性：关闭窗口时析构
    baw->show();
    this->hide();
}

/**
 * @brief 决斗赛。
 * @details 调用Client的duelBattle函数进行决斗赛，创建一个新的BattleWindow窗口，隐藏本窗口。
 */
void SecondWindow::cli_duelBattle() {
    auto *baw = new BattleWindow(ui->comboBox->currentText().toStdString(), std::move(cli),
                                 [this](auto &&PH1, auto &&PH2, auto &&PH3, auto &&PH4) {
                                     callback(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2),
                                              std::forward<decltype(PH3)>(PH3), std::forward<decltype(PH4)>(PH4));
                                 }, true);
    baw->setAttribute(Qt::WA_DeleteOnClose, true); // 设置属性：关闭窗口时析构
    baw->show();
    this->hide();
}

/**
 * @brief 回调函数。
 * @param cli2 客户端指针。
 * @param s 对手名。
 * @param r 结果。
 * @param isDuel 是否是决斗赛。
 * @details 显示本窗口，获取客户端unique_ptr，根据战斗种类和结果进行操作，战斗会导致精灵升级，所以重新设置徽章。
 */
void SecondWindow::callback(std::unique_ptr<Client> &&cli2, const std::string &s, bool r, bool isDuel) {
    this->show();
    cli = std::move(cli2);
    if (!isDuel) { // 升级赛，直接设置徽章
        setBadge();
        return;
    }
    if (r) { // 决斗赛，胜利
        ui->comboBox->addItem(QString::fromStdString(s)); // 将对手精灵添加到下拉框
    } else { // 决斗赛，失败
        ui->comboBox->clear(); // 清空下拉框
        auto pokemons = cli->getMyPokemons(); // 重新获取我的精灵
        for (const auto &pokemon: pokemons) { // 添加到下拉框
            ui->comboBox->addItem(QString::fromStdString(pokemon));
        }
        if (ui->comboBox->count() == 0) { // 如果没有精灵了，从服务器获取一个新的精灵
            auto *message = new QMessageBox(this);
            QString newPoke = QString::fromStdString(cli->getNewPokemon());
            message->setText("由于你失去了所有的Pokemon，送你一个");
            message->setInformativeText(newPoke);
            message->setStandardButtons(QMessageBox::Yes);
            message->exec();
            delete message;
            ui->comboBox->addItem(newPoke);
        }
    }
    setBadge(); // 设置徽章
}

/**
 * @brief 查看我的Pokemon
 * @details 调用Client的viewMyPokemons方法得到返回结果，用返回结果初始化ViewMyPokemonsWindow，显示新窗口。
 */
void SecondWindow::cli_viewMyPokemons() const {
    auto *vmpw = new ViewMyPokemonsWindow(cli->viewMyPokemons());
    vmpw->setAttribute(Qt::WA_DeleteOnClose, true); // 设置属性：关闭窗口时析构
    vmpw->show();
}

/**
 * @brief 查看胜率
 * @details 调用Client的viewWP方法得到返回结果，用返回结果初始化WPWindow，显示新窗口。
 */
void SecondWindow::cli_viewWP() const {
    auto *wpw = new WPWindow(cli->viewWP());
    wpw->setAttribute(Qt::WA_DeleteOnClose, true); // 设置属性：关闭窗口时析构
    wpw->show();
}

/**
 * @brief 设置显示的徽章图片。
 * @details 根据精灵数量和高级精灵数量设置徽章图片。
 */
void SecondWindow::setBadge() const {
    // 静态图片对象，确保只加载一次
    static QImage Cu("../img/Cu.jpg");
    static QImage Ag("../img/Ag.jpg");
    static QImage Au("../img/Au.jpg");
    static QImage No("../img/No.jpg");
    static QImage Cu2("../img/Cu2.jpg");
    static QImage Ag2("../img/Ag2.jpg");
    static QImage Au2("../img/Au2.jpg");
    static QImage No2("../img/No2.jpg");
    // 缩放图片
    Cu = Cu.scaled(50, 50, Qt::KeepAspectRatio);
    Ag = Ag.scaled(50, 50, Qt::KeepAspectRatio);
    Au = Au.scaled(50, 50, Qt::KeepAspectRatio);
    No = No.scaled(50, 50, Qt::KeepAspectRatio);
    Cu2 = Cu2.scaled(50, 50, Qt::KeepAspectRatio);
    Ag2 = Ag2.scaled(50, 50, Qt::KeepAspectRatio);
    Au2 = Au2.scaled(50, 50, Qt::KeepAspectRatio);
    No2 = No2.scaled(50, 50, Qt::KeepAspectRatio);
    // 根据精灵数量设置徽章
    int n = cli->getPokemonNum();
    if (n >= 10) {
        ui->badge1->setPixmap(QPixmap::fromImage(Au));
        ui->badge1->setToolTip("宠物个数勋章（金）");
    } else if (n >= 7) {
        ui->badge1->setPixmap(QPixmap::fromImage(Ag));
        ui->badge1->setToolTip("宠物个数勋章（银）");
    } else if (n >= 4) {
        ui->badge1->setPixmap(QPixmap::fromImage(Cu));
        ui->badge1->setToolTip("宠物个数勋章（铜）");
    } else {
        ui->badge1->setPixmap(QPixmap::fromImage(No));
        ui->badge1->setToolTip("宠物个数勋章（无）");
    }
    // 根据高级精灵数量设置徽章
    int n2 = cli->getHighLevelNum();
    if (n2 >= 5) {
        ui->badge2->setPixmap(QPixmap::fromImage(Au2));
        ui->badge2->setToolTip("高级宠物勋章（金）");
    } else if (n2 >= 3) {
        ui->badge2->setPixmap(QPixmap::fromImage(Ag2));
        ui->badge2->setToolTip("高级宠物勋章（银）");
    } else if (n2 >= 1) {
        ui->badge2->setPixmap(QPixmap::fromImage(Cu2));
        ui->badge2->setToolTip("高级宠物勋章（铜）");
    } else {
        ui->badge2->setPixmap(QPixmap::fromImage(No2));
        ui->badge2->setToolTip("高级宠物勋章（无）");
    }
}
