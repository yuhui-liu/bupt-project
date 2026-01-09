/**
 * @file losewindow.cpp
 * @brief 定义了失败窗口类。
 * @details 定义了失败窗口类，包括构造函数、析构函数和槽函数。
 */

#include <utility>
#include <random>

#include "../include/losewindow.h"
#include "../include/ui/ui_LoseWindow.h"

#include <QMessageBox>

/**
 * @brief 构造函数。
 * @param _pokes 精灵列表。
 * @param parent 父窗口指针。
 * @details 设置界面，根据精灵数量初始化界面，连接了pushButton的clicked信号与handler槽。
 */
LoseWindow::LoseWindow(std::vector<std::string> _pokes, QWidget *parent) : pokes(std::move(_pokes)), QDialog(parent),
                                                                           ui(new Ui::LoseWindow) {
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint); // 不显示关闭按钮
    setWindowIcon(QIcon("../img/client.ico"));
    if (pokes.size() == 2) { // 如果精灵数量为2，不需要随机，直接显示这两个精灵
        ui->op1->setText(QString::fromStdString(pokes[i]));
        ui->op2->setText(QString::fromStdString(pokes[j]));

    } else if (pokes.size() == 3) { // 如果精灵数量为3，不需要随机，直接显示这三个精灵
        ui->op1->setText(QString::fromStdString(pokes[i]));
        ui->op2->setText(QString::fromStdString(pokes[j]));
        op3 = new QRadioButton(QString::fromStdString(pokes[k]));
        ui->verticalLayout->addWidget(op3);
    } else { // 如果精灵数量大于3，随机显示三个精灵
        int _n = static_cast<int>(pokes.size()) - 1;
        // 调用distribution(gen)生成随机数
        std::random_device rd;
        std::minstd_rand gen(rd());
        std::uniform_int_distribution<int> distribution(0, _n);
        i = distribution(gen), j = distribution(gen), k = distribution(gen);
        // 确保两两不等
        while (j == i)
            j = distribution(gen);
        while (k == i || k == j)
            k = distribution(gen);
        ui->op1->setText(QString::fromStdString(pokes[i]));
        ui->op2->setText(QString::fromStdString(pokes[j]));
        op3 = new QRadioButton(QString::fromStdString(pokes[k]));
        ui->verticalLayout->addWidget(op3);
    }
    // 连接确认按钮的clicked信号与handler槽
    connect(ui->pushButton, &QPushButton::clicked, this, &LoseWindow::handler);
}

/**
 * @brief 析构函数。
 * @details 删除界面指针。
 */
LoseWindow::~LoseWindow() {
    delete ui;
}

/**
 * @brief 判断选择的精灵。
 * @details 根据被选中的复选框判断选择的精灵，返回对应的下标。调用QDialog的done函数结束窗口。
 */
void LoseWindow::handler() {
    // 之所以加10是为了和QDialog的原有返回值区分开。
    if (ui->op1->isChecked()) {
        this->done(i + 10);
    } else if (ui->op2->isChecked()) {
        this->done(j + 10);
    } else if (op3->isChecked()) {
        this->done(k + 10);
    } else {
        QMessageBox::warning(this, "", "请选择一个"); // 如果没有选择，弹出警告框
    }
}
