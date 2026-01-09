/********************************************************************************
** Form generated from reading UI file 'viewpokemonwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

// 这是由Qt UIC自动生成的代码，用于生成查看精灵窗口的UI。
// 包含控件有：文本框。
// 通过调用setupUi函数，可以将这些控件添加到窗口中。

#ifndef UI_VIEWPOKEMONWINDOW_H
#define UI_VIEWPOKEMONWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ViewPokemonWindow
{
public:
    QPlainTextEdit *plainTextEdit; // 显示文本框

    void setupUi(QWidget *ViewPokemonWindow)
    {
        if (ViewPokemonWindow->objectName().isEmpty())
            ViewPokemonWindow->setObjectName(QStringLiteral("ViewPokemonWindow"));
        ViewPokemonWindow->resize(789, 579);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ViewPokemonWindow->sizePolicy().hasHeightForWidth());
        ViewPokemonWindow->setSizePolicy(sizePolicy);
        plainTextEdit = new QPlainTextEdit(ViewPokemonWindow);
        plainTextEdit->setObjectName(QStringLiteral("plainTextEdit"));
        plainTextEdit->setGeometry(QRect(0, 0, 791, 581));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(plainTextEdit->sizePolicy().hasHeightForWidth());
        plainTextEdit->setSizePolicy(sizePolicy1);
        QFont font;
        font.setFamily(QString::fromUtf8("\345\271\274\345\234\206"));
        font.setPointSize(20);
        plainTextEdit->setFont(font);
        plainTextEdit->setReadOnly(true);

        retranslateUi(ViewPokemonWindow);

        QMetaObject::connectSlotsByName(ViewPokemonWindow);
    } // setupUi

    void retranslateUi(QWidget *ViewPokemonWindow)
    {
        ViewPokemonWindow->setWindowTitle(QApplication::translate("ViewPokemonWindow", "ViewPokemonWindow", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ViewPokemonWindow: public Ui_ViewPokemonWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEWPOKEMONWINDOW_H
