/**
 * @file mainwindow.cpp
 * @brief 主窗口类的实现。
 * @details 实现了主窗口的构造函数、析构函数、注册账户、连接服务器、登录、添加消息、关闭事件、获得客户端指针、回调函数。
 */
#include "../include/mainwindow.h"
#include "../include/ui/ui_mainwindow.h"
#include "../include/secondwindow.h"
#include <QMessageBox>
#include <QScrollBar>

/**
 * @brief 构造函数。
 * @param parent 父窗口指针。
 * @details 设置图标，连接了注册按钮和登录按钮的信号与槽。
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), cli(new Client) {
    ui->setupUi(this);
    setWindowIcon(QIcon("../img/client.ico"));
    connect(ui->registerButton, &QPushButton::clicked, this,
            &::MainWindow::cli_registerAccount); // 将注册按钮的clicked信号与cli_registerAccount槽连接
    connect(ui->loginButton, &QPushButton::clicked, this, &::MainWindow::cli_login); // 将登录按钮的clicked信号与cli_login槽连接
}

/**
 * @brief 析构函数。
 * @details 删除界面指针。
 */
MainWindow::~MainWindow() {
    delete ui;
}

/**
 * @brief 注册账户。
 * @details 如果输入框为空则不执行操作，否则尝试注册账户，如果已连接则直接注册，否则先连接再注册。
 * 注册操作通过调用Client的registerAccount函数实现。
 */
void MainWindow::cli_registerAccount() {
    if (ui->lineEdit->text().isEmpty())
        return;
    if (cli->isConnected()) {
        try {
            cli->registerAccount(ui->lineEdit->text().toStdString());
            appendMessage("注册成功");
        } catch (const std::runtime_error &e) {
            appendMessage("注册失败");
            QMessageBox::warning(this, "注册失败", QString::fromStdString(e.what()));
        }
    } else {
        cli_connectToServer();
        if (cli->isConnected()) {
            try {
                cli->registerAccount(ui->lineEdit->text().toStdString());
                appendMessage("注册成功");
            } catch (const std::runtime_error &e) {
                appendMessage("注册失败");
                QMessageBox::warning(this, "注册失败", QString::fromStdString(e.what()));
            }
        }
    }
}

/**
 * @brief 连接服务器。
 * @details 尝试连接服务器，如果连接成功则设置连接状态标签为“已连接”，连接失败则弹出警告框提示用户是否重试。
 * 连接操作通过调用Client的connectToServer函数实现。
 */
void MainWindow::cli_connectToServer() {
    try {
        appendMessage("尝试连接...");
        cli->connectToServer();
        if (cli->isConnected()) {
            ui->connectStatusLabel->setText(QString::fromStdString("已连接"));
            appendMessage("已连接");
        }
    } catch (const std::runtime_error &e) {
        auto *message = new QMessageBox(this);
        message->setText(QString::fromStdString(e.what()));
        message->setInformativeText("是否重试？");
        message->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int result = message->exec();
        delete message;
        if (result == QMessageBox::Yes) cli_connectToServer();
    }
}

/**
 * @brief 登录。
 * @details 如果输入框为空则不执行操作，否则尝试登录，如果已连接则直接登录，否则先连接再登录。
 * 登录操作通过调用Client的login函数实现。
 * 登陆成功后隐藏主窗口，显示次窗口。
 */
void MainWindow::cli_login() {
    if (ui->lineEdit->text().isEmpty())
        return;
    if (cli->isConnected()) {
        try {
            cli->login(ui->lineEdit->text().toStdString());
            appendMessage("登录成功");
            auto *secondwind = new SecondWindow(ui->lineEdit->text(), std::move(cli),
                                                [this](auto &&PH1) { callback(std::forward<decltype(PH1)>(PH1)); });
            secondwind->setAttribute(Qt::WA_DeleteOnClose, true); // 设置属性：关闭窗口时析构
            secondwind->show();
            this->hide();
        } catch (const std::runtime_error &e) {
            appendMessage("登录失败");
            QMessageBox::warning(this, "登录失败", QString::fromStdString(e.what()));
        }
    } else {
        cli_connectToServer();
        if (cli->isConnected()) {
            try {
                cli->login(ui->lineEdit->text().toStdString());
                appendMessage("登录成功");
                auto *secondwind = new SecondWindow(ui->lineEdit->text(), std::move(cli),
                                                    [this](auto &&PH1) { callback(std::forward<decltype(PH1)>(PH1)); });
                secondwind->setAttribute(Qt::WA_DeleteOnClose, true); // 设置属性：关闭窗口时析构
                secondwind->show();
                this->hide();
            } catch (const std::runtime_error &e) {
                appendMessage("登录失败");
                QMessageBox::warning(this, "登录失败", QString::fromStdString(e.what()));
            }
        }
    }
}

/**
 * @brief 向消息框添加消息。
 * @param s 消息字符串。
 */
void MainWindow::appendMessage(const std::string &s) const {
    ui->plainTextEdit->appendPlainText(QString::fromStdString(s));
    ui->plainTextEdit->verticalScrollBar()->setValue(ui->plainTextEdit->verticalScrollBar()->maximum());
}

/**
 * @brief 重载QMainWindow的关闭事件。
 * @param event 关闭事件。
 * @details 先断开连接再关闭窗口，断开连接操作通过调用Client的disconnect函数实现。
 */
void MainWindow::closeEvent(QCloseEvent *event) {
    cli->disconnect();
    event->accept();
}

/**
 * @brief 获得指向Client的unique_ptr的所有权。
 * @param cli2 指向Client的unique_ptr。
 */
void MainWindow::setCli(std::unique_ptr<Client> &&cli2) {
    cli = std::move(cli2);
}

/**
 * @brief 回调函数。
 * @param cli2 指向Client的unique_ptr。
 * @details 显示窗口，将unique_ptr的所有权转移给成员变量cli。
 */
void MainWindow::callback(std::unique_ptr<Client> &&cli2) {
    this->show();
    cli = std::move(cli2);
}
