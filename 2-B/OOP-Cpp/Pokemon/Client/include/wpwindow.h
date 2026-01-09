/**
 * @file wpwindow.h
 * @brief 声明了查看胜率界面类。
 * @details 该界面显示了玩家的胜率。
 */

#ifndef CLIENT_WPWINDOW_H
#define CLIENT_WPWINDOW_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class WPWindow; }
QT_END_NAMESPACE

/**
 * @class WPWindow wpwindow.h "include/wpwindow.h"
 * @brief 查看胜率界面类。
 * @details 该界面显示了玩家的胜率。
 */
class WPWindow : public QWidget {
Q_OBJECT

public:
    // 构造函数
    explicit WPWindow(const std::string &s, QWidget *parent = nullptr);

    // 析构函数
    ~WPWindow() override;

private:
    Ui::WPWindow *ui; ///< 界面指针
};


#endif //CLIENT_WPWINDOW_H
