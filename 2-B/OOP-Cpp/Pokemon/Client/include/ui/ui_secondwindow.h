/********************************************************************************
** Form generated from reading UI file 'secondwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

// 这是由Qt UIC自动生成的代码，用于生成次窗口的UI。
// 包含控件有：文本框、按钮、水平布局、标签等。
// 通过调用setupUi函数，可以将这些控件添加到窗口中。

#ifndef UI_SECONDWINDOW_H
#define UI_SECONDWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SecondWindow
{
public:
    QWidget *verticalLayoutWidget; // 垂直布局
    QVBoxLayout *verticalLayout; // 垂直布局
    QLabel *label; // 用户名标签
    QPushButton *logoutButton; // 登出按钮
    QPushButton *onlineuserButton; // 在线用户按钮
    QPushButton *alluserpokemonButton; // 所有用户精灵按钮
    QPushButton *viewMyPokemonButton; // 查看我的精灵按钮
    QPushButton *WPButton; // 胜率按钮
    QLabel *badge1; // 精灵数量徽章
    QLabel *badge2; // 高级精灵数量徽章
    QPlainTextEdit *plainTextEdit; // 文本框，显示在线用户
    QWidget *verticalLayoutWidget_2; // 垂直布局
    QVBoxLayout *verticalLayout_2; // 垂直布局
    QComboBox *comboBox; // 下拉框，选择精灵
    QHBoxLayout *horizontalLayout; // 水平布局
    QPushButton *normalBattleButton; // 升级赛按钮
    QPushButton *duelBattleButton; // 决斗赛按钮

    void setupUi(QWidget *SecondWindow)
    {
        if (SecondWindow->objectName().isEmpty())
            SecondWindow->setObjectName(QStringLiteral("SecondWindow"));
        SecondWindow->resize(955, 653);
        verticalLayoutWidget = new QWidget(SecondWindow);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(0, 0, 160, 651));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(verticalLayoutWidget);
        label->setObjectName(QStringLiteral("label"));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        label->setFont(font);

        verticalLayout->addWidget(label);

        logoutButton = new QPushButton(verticalLayoutWidget);
        logoutButton->setObjectName(QStringLiteral("logoutButton"));
        logoutButton->setFont(font);

        verticalLayout->addWidget(logoutButton);

        onlineuserButton = new QPushButton(verticalLayoutWidget);
        onlineuserButton->setObjectName(QStringLiteral("onlineuserButton"));
        onlineuserButton->setFont(font);

        verticalLayout->addWidget(onlineuserButton);

        alluserpokemonButton = new QPushButton(verticalLayoutWidget);
        alluserpokemonButton->setObjectName(QStringLiteral("alluserpokemonButton"));
        alluserpokemonButton->setFont(font);

        verticalLayout->addWidget(alluserpokemonButton);

        viewMyPokemonButton = new QPushButton(verticalLayoutWidget);
        viewMyPokemonButton->setObjectName(QStringLiteral("viewMyPokemonButton"));
        viewMyPokemonButton->setFont(font);

        verticalLayout->addWidget(viewMyPokemonButton);

        WPButton = new QPushButton(verticalLayoutWidget);
        WPButton->setObjectName(QStringLiteral("WPButton"));
        WPButton->setFont(font);

        verticalLayout->addWidget(WPButton);

        badge1 = new QLabel(verticalLayoutWidget);
        badge1->setObjectName(QStringLiteral("badge1"));
        badge1->setFont(font);
        badge1->setToolTipDuration(5000);

        verticalLayout->addWidget(badge1);

        badge2 = new QLabel(verticalLayoutWidget);
        badge2->setObjectName(QStringLiteral("badge2"));
        badge2->setFont(font);
        badge2->setToolTipDuration(5000);

        verticalLayout->addWidget(badge2);

        plainTextEdit = new QPlainTextEdit(verticalLayoutWidget);
        plainTextEdit->setObjectName(QStringLiteral("plainTextEdit"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(plainTextEdit->sizePolicy().hasHeightForWidth());
        plainTextEdit->setSizePolicy(sizePolicy);
        QFont font1;
        font1.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font1.setPointSize(10);
        plainTextEdit->setFont(font1);

        verticalLayout->addWidget(plainTextEdit);

        verticalLayoutWidget_2 = new QWidget(SecondWindow);
        verticalLayoutWidget_2->setObjectName(QStringLiteral("verticalLayoutWidget_2"));
        verticalLayoutWidget_2->setGeometry(QRect(330, 180, 411, 211));
        verticalLayout_2 = new QVBoxLayout(verticalLayoutWidget_2);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        comboBox = new QComboBox(verticalLayoutWidget_2);
        comboBox->setObjectName(QStringLiteral("comboBox"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(comboBox->sizePolicy().hasHeightForWidth());
        comboBox->setSizePolicy(sizePolicy1);
        QFont font2;
        font2.setFamily(QString::fromUtf8("\351\232\266\344\271\246"));
        font2.setPointSize(24);
        comboBox->setFont(font2);

        verticalLayout_2->addWidget(comboBox);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        normalBattleButton = new QPushButton(verticalLayoutWidget_2);
        normalBattleButton->setObjectName(QStringLiteral("normalBattleButton"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(normalBattleButton->sizePolicy().hasHeightForWidth());
        normalBattleButton->setSizePolicy(sizePolicy2);
        normalBattleButton->setFont(font2);

        horizontalLayout->addWidget(normalBattleButton);

        duelBattleButton = new QPushButton(verticalLayoutWidget_2);
        duelBattleButton->setObjectName(QStringLiteral("duelBattleButton"));
        sizePolicy2.setHeightForWidth(duelBattleButton->sizePolicy().hasHeightForWidth());
        duelBattleButton->setSizePolicy(sizePolicy2);
        duelBattleButton->setFont(font2);

        horizontalLayout->addWidget(duelBattleButton);


        verticalLayout_2->addLayout(horizontalLayout);

        verticalLayout_2->setStretch(0, 1);
        verticalLayout_2->setStretch(1, 2);

        retranslateUi(SecondWindow);

        QMetaObject::connectSlotsByName(SecondWindow);
    } // setupUi

    void retranslateUi(QWidget *SecondWindow)
    {
        SecondWindow->setWindowTitle(QApplication::translate("SecondWindow", "SecondWindow", Q_NULLPTR));
        label->setText(QApplication::translate("SecondWindow", "username", Q_NULLPTR));
        logoutButton->setText(QApplication::translate("SecondWindow", "\346\263\250\351\224\200", Q_NULLPTR));
        onlineuserButton->setText(QApplication::translate("SecondWindow", "\345\210\267\346\226\260\345\234\250\347\272\277\347\224\250\346\210\267", Q_NULLPTR));
        alluserpokemonButton->setText(QApplication::translate("SecondWindow", "\346\211\200\346\234\211\347\224\250\346\210\267\347\262\276\347\201\265", Q_NULLPTR));
        viewMyPokemonButton->setText(QApplication::translate("SecondWindow", "\346\237\245\347\234\213\346\210\221\347\232\204\347\262\276\347\201\265", Q_NULLPTR));
        WPButton->setText(QApplication::translate("SecondWindow", "\346\237\245\347\234\213\350\203\234\347\216\207", Q_NULLPTR));
        badge1->setText(QApplication::translate("SecondWindow", "\346\227\240\345\256\240\347\211\251\344\270\252\346\225\260\345\213\213\347\253\240", Q_NULLPTR));
        badge2->setText(QApplication::translate("SecondWindow", "\346\227\240\351\253\230\347\272\247\345\256\240\347\211\251\345\213\213\347\253\240", Q_NULLPTR));
        normalBattleButton->setText(QApplication::translate("SecondWindow", "\345\215\207\347\272\247\350\265\233", Q_NULLPTR));
        duelBattleButton->setText(QApplication::translate("SecondWindow", "\345\206\263\346\226\227\350\265\233", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class SecondWindow: public Ui_SecondWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SECONDWINDOW_H
