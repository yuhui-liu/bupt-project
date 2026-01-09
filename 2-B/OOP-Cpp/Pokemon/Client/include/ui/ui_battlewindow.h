/********************************************************************************
** Form generated from reading UI file 'battlewindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

// 这是由Qt UIC自动生成的代码，用于生成战斗窗口的UI。
// 包含控件有：文本框、按钮、水平布局、标签等。
// 通过调用setupUi函数，可以将这些控件添加到窗口中。

#ifndef UI_BATTLEWINDOW_H
#define UI_BATTLEWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BattleWindow
{
public:
    QPlainTextEdit *plainTextEdit; // 文本框
    QPushButton *startButton; // 开始按钮
    QWidget *horizontalLayoutWidget; // 水平布局
    QHBoxLayout *horizontalLayout; // 水平布局
    QLabel *myPoke; // 我的精灵（图片）
    QLabel *hisPoke; // 对方精灵（图片）
    QWidget *horizontalLayoutWidget_2; // 水平布局
    QHBoxLayout *horizontalLayout_2; // 水平布局
    QLabel *myPokeLabel; // 我的精灵（标签）
    QLabel *hisPokeLabel; // 对方精灵（标签）

    void setupUi(QWidget *BattleWindow)
    {
        if (BattleWindow->objectName().isEmpty())
            BattleWindow->setObjectName(QStringLiteral("BattleWindow"));
        BattleWindow->resize(732, 572);
        plainTextEdit = new QPlainTextEdit(BattleWindow);
        plainTextEdit->setObjectName(QStringLiteral("plainTextEdit"));
        plainTextEdit->setGeometry(QRect(0, 370, 731, 201));
        plainTextEdit->setReadOnly(true);
        startButton = new QPushButton(BattleWindow);
        startButton->setObjectName(QStringLiteral("startButton"));
        startButton->setGeometry(QRect(330, 330, 71, 41));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\215\216\346\226\207\347\220\245\347\217\200"));
        font.setPointSize(12);
        startButton->setFont(font);
        horizontalLayoutWidget = new QWidget(BattleWindow);
        horizontalLayoutWidget->setObjectName(QStringLiteral("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(0, 0, 731, 301));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        myPoke = new QLabel(horizontalLayoutWidget);
        myPoke->setObjectName(QStringLiteral("myPoke"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(myPoke->sizePolicy().hasHeightForWidth());
        myPoke->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(myPoke);

        hisPoke = new QLabel(horizontalLayoutWidget);
        hisPoke->setObjectName(QStringLiteral("hisPoke"));

        horizontalLayout->addWidget(hisPoke);

        horizontalLayoutWidget_2 = new QWidget(BattleWindow);
        horizontalLayoutWidget_2->setObjectName(QStringLiteral("horizontalLayoutWidget_2"));
        horizontalLayoutWidget_2->setGeometry(QRect(0, 300, 731, 31));
        horizontalLayout_2 = new QHBoxLayout(horizontalLayoutWidget_2);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        myPokeLabel = new QLabel(horizontalLayoutWidget_2);
        myPokeLabel->setObjectName(QStringLiteral("myPokeLabel"));
        sizePolicy.setHeightForWidth(myPokeLabel->sizePolicy().hasHeightForWidth());
        myPokeLabel->setSizePolicy(sizePolicy);
        QFont font1;
        font1.setFamily(QString::fromUtf8("\346\226\271\346\255\243\350\210\222\344\275\223"));
        font1.setPointSize(11);
        myPokeLabel->setFont(font1);
        myPokeLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(myPokeLabel);

        hisPokeLabel = new QLabel(horizontalLayoutWidget_2);
        hisPokeLabel->setObjectName(QStringLiteral("hisPokeLabel"));
        hisPokeLabel->setFont(font1);
        hisPokeLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(hisPokeLabel);


        retranslateUi(BattleWindow);

        QMetaObject::connectSlotsByName(BattleWindow);
    } // setupUi

    void retranslateUi(QWidget *BattleWindow)
    {
        BattleWindow->setWindowTitle(QApplication::translate("BattleWindow", "BattleWindow", Q_NULLPTR));
        startButton->setText(QApplication::translate("BattleWindow", "\345\274\200\345\247\213", Q_NULLPTR));
        myPoke->setText(QString());
        hisPoke->setText(QString());
        myPokeLabel->setText(QString());
        hisPokeLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class BattleWindow: public Ui_BattleWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BATTLEWINDOW_H
