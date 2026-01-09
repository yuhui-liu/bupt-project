/********************************************************************************
** Form generated from reading UI file 'wpwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

// 这是由Qt UIC自动生成的代码，用于生成胜率窗口的UI。
// 包含控件有：文本框。
// 通过调用setupUi函数，可以将这些控件添加到窗口中。

#ifndef UI_WPWINDOW_H
#define UI_WPWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_WPWindow
{
public:
    QPlainTextEdit *plainTextEdit; // 显示文本框

    void setupUi(QWidget *WPWindow)
    {
        if (WPWindow->objectName().isEmpty())
            WPWindow->setObjectName(QStringLiteral("WPWindow"));
        WPWindow->resize(600, 409);
        plainTextEdit = new QPlainTextEdit(WPWindow);
        plainTextEdit->setObjectName(QStringLiteral("plainTextEdit"));
        plainTextEdit->setGeometry(QRect(0, 0, 601, 411));

        retranslateUi(WPWindow);

        QMetaObject::connectSlotsByName(WPWindow);
    } // setupUi

    void retranslateUi(QWidget *WPWindow)
    {
        WPWindow->setWindowTitle(QApplication::translate("WPWindow", "WPWindow", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class WPWindow: public Ui_WPWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WPWINDOW_H
