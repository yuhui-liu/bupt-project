/********************************************************************************
** Form generated from reading UI file 'viewmypokemonswindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

// 这是由Qt UIC自动生成的代码，用于生成查看我的精灵窗口的UI。
// 包含控件有：文本框等。
// 通过调用setupUi函数，可以将这些控件添加到窗口中。

#ifndef UI_VIEWMYPOKEMONSWINDOW_H
#define UI_VIEWMYPOKEMONSWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ViewMyPokemonsWindow
{
public:
    QPlainTextEdit *plainTextEdit; // 显示文本框

    void setupUi(QWidget *ViewMyPokemonsWindow)
    {
        if (ViewMyPokemonsWindow->objectName().isEmpty())
            ViewMyPokemonsWindow->setObjectName(QStringLiteral("ViewMyPokemonsWindow"));
        ViewMyPokemonsWindow->resize(672, 452);
        plainTextEdit = new QPlainTextEdit(ViewMyPokemonsWindow);
        plainTextEdit->setObjectName(QStringLiteral("plainTextEdit"));
        plainTextEdit->setGeometry(QRect(0, 0, 671, 451));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\271\274\345\234\206"));
        font.setPointSize(11);
        plainTextEdit->setFont(font);

        retranslateUi(ViewMyPokemonsWindow);

        QMetaObject::connectSlotsByName(ViewMyPokemonsWindow);
    } // setupUi

    void retranslateUi(QWidget *ViewMyPokemonsWindow)
    {
        ViewMyPokemonsWindow->setWindowTitle(QApplication::translate("ViewMyPokemonsWindow", "ViewMyPokemonsWindow", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ViewMyPokemonsWindow: public Ui_ViewMyPokemonsWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEWMYPOKEMONSWINDOW_H
