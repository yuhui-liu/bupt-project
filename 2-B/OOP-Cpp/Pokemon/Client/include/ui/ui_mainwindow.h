/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

// 这是由Qt UIC自动生成的代码，用于生成主窗口的UI。
// 包含控件有：文本框、按钮、水平布局、标签等。
// 通过调用setupUi函数，可以将这些控件添加到窗口中。

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget; // 中心窗口
    QLabel *connectStatusLabel; // 连接状态标签
    QPlainTextEdit *plainTextEdit; // 显示文本框
    QWidget *horizontalLayoutWidget; // 水平布局
    QHBoxLayout *horizontalLayout; // 水平布局
    QPushButton *registerButton; // 注册按钮
    QPushButton *loginButton; // 登录按钮
    QWidget *horizontalLayoutWidget_2; // 水平布局
    QHBoxLayout *horizontalLayout_2; // 水平布局
    QLabel *userNameLabel; // 用户名标签
    QLineEdit *lineEdit; // 用户名输入框
    QMenuBar *menubar; // 菜单栏
    QStatusBar *statusbar; // 状态栏

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->setEnabled(true);
        MainWindow->resize(724, 456);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        connectStatusLabel = new QLabel(centralwidget);
        connectStatusLabel->setObjectName(QStringLiteral("connectStatusLabel"));
        connectStatusLabel->setGeometry(QRect(0, 0, 61, 16));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        connectStatusLabel->setFont(font);
        connectStatusLabel->setStyleSheet(QStringLiteral(""));
        plainTextEdit = new QPlainTextEdit(centralwidget);
        plainTextEdit->setObjectName(QStringLiteral("plainTextEdit"));
        plainTextEdit->setGeometry(QRect(0, 20, 121, 87));
        plainTextEdit->setReadOnly(true);
        horizontalLayoutWidget = new QWidget(centralwidget);
        horizontalLayoutWidget->setObjectName(QStringLiteral("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(240, 190, 241, 61));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        registerButton = new QPushButton(horizontalLayoutWidget);
        registerButton->setObjectName(QStringLiteral("registerButton"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(registerButton->sizePolicy().hasHeightForWidth());
        registerButton->setSizePolicy(sizePolicy);
        QFont font1;
        font1.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font1.setPointSize(11);
        registerButton->setFont(font1);
        registerButton->setAutoFillBackground(false);
        registerButton->setStyleSheet(QStringLiteral("color:rgb(0, 0, 255);"));
        registerButton->setFlat(false);

        horizontalLayout->addWidget(registerButton);

        loginButton = new QPushButton(horizontalLayoutWidget);
        loginButton->setObjectName(QStringLiteral("loginButton"));
        sizePolicy.setHeightForWidth(loginButton->sizePolicy().hasHeightForWidth());
        loginButton->setSizePolicy(sizePolicy);
        loginButton->setFont(font1);
        loginButton->setStyleSheet(QStringLiteral("color:rgb(0, 0, 255);"));

        horizontalLayout->addWidget(loginButton);

        horizontalLayoutWidget_2 = new QWidget(centralwidget);
        horizontalLayoutWidget_2->setObjectName(QStringLiteral("horizontalLayoutWidget_2"));
        horizontalLayoutWidget_2->setGeometry(QRect(240, 80, 241, 78));
        horizontalLayout_2 = new QHBoxLayout(horizontalLayoutWidget_2);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        userNameLabel = new QLabel(horizontalLayoutWidget_2);
        userNameLabel->setObjectName(QStringLiteral("userNameLabel"));
        QFont font2;
        font2.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font2.setPointSize(11);
        font2.setBold(true);
        font2.setWeight(75);
        userNameLabel->setFont(font2);

        horizontalLayout_2->addWidget(userNameLabel);

        lineEdit = new QLineEdit(horizontalLayoutWidget_2);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));

        horizontalLayout_2->addWidget(lineEdit);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 724, 26));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "BattleSystem", Q_NULLPTR));
        connectStatusLabel->setText(QApplication::translate("MainWindow", "\346\234\252\350\277\236\346\216\245", Q_NULLPTR));
        registerButton->setText(QApplication::translate("MainWindow", "\346\263\250\345\206\214", Q_NULLPTR));
        loginButton->setText(QApplication::translate("MainWindow", "\347\231\273\345\275\225", Q_NULLPTR));
        userNameLabel->setText(QApplication::translate("MainWindow", "\347\224\250\346\210\267\345\220\215", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
