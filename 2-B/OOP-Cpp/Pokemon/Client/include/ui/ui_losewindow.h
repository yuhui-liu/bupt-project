/********************************************************************************
** Form generated from reading UI file 'losewindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

// 这是由Qt UIC自动生成的代码，用于生成失败窗口的UI。
// 包含控件有：文本框、按钮、垂直布局、标签等。
// 通过调用setupUi函数，可以将这些控件添加到窗口中。

#ifndef UI_LOSEWINDOW_H
#define UI_LOSEWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoseWindow
{
public:
    QWidget *verticalLayoutWidget_2; // 垂直布局
    QVBoxLayout *verticalLayout_2; // 垂直布局
    QLabel *label; // 标签
    QVBoxLayout *verticalLayout; // 垂直布局
    QRadioButton *op1; // 单选按钮
    QRadioButton *op2; // 单选按钮
    QPushButton *pushButton; // 确认按钮

    void setupUi(QDialog *LoseWindow)
    {
        if (LoseWindow->objectName().isEmpty())
            LoseWindow->setObjectName(QStringLiteral("LoseWindow"));
        LoseWindow->setWindowModality(Qt::ApplicationModal);
        LoseWindow->resize(304, 259);
        verticalLayoutWidget_2 = new QWidget(LoseWindow);
        verticalLayoutWidget_2->setObjectName(QStringLiteral("verticalLayoutWidget_2"));
        verticalLayoutWidget_2->setGeometry(QRect(10, 10, 269, 211));
        verticalLayout_2 = new QVBoxLayout(verticalLayoutWidget_2);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(verticalLayoutWidget_2);
        label->setObjectName(QStringLiteral("label"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);

        verticalLayout_2->addWidget(label);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        op1 = new QRadioButton(verticalLayoutWidget_2);
        op1->setObjectName(QStringLiteral("op1"));

        verticalLayout->addWidget(op1);

        op2 = new QRadioButton(verticalLayoutWidget_2);
        op2->setObjectName(QStringLiteral("op2"));

        verticalLayout->addWidget(op2);


        verticalLayout_2->addLayout(verticalLayout);

        pushButton = new QPushButton(verticalLayoutWidget_2);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(pushButton->sizePolicy().hasHeightForWidth());
        pushButton->setSizePolicy(sizePolicy1);
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        pushButton->setFont(font);

        verticalLayout_2->addWidget(pushButton);


        retranslateUi(LoseWindow);

        QMetaObject::connectSlotsByName(LoseWindow);
    } // setupUi

    void retranslateUi(QDialog *LoseWindow)
    {
        LoseWindow->setWindowTitle(QApplication::translate("LoseWindow", "BattleSystem", Q_NULLPTR));
        label->setText(QApplication::translate("LoseWindow", "/(\343\204\222o\343\204\222)/~~\344\275\240\350\276\223\344\272\206\357\274\214\351\200\211\344\270\200\344\270\252\351\200\201\345\207\272\345\216\273\345\220\247", Q_NULLPTR));
        op1->setText(QString());
        op2->setText(QString());
        pushButton->setText(QApplication::translate("LoseWindow", "\347\241\256\345\256\232", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class LoseWindow: public Ui_LoseWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOSEWINDOW_H
