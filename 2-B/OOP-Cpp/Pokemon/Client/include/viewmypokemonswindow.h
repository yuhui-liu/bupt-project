/**
 * @file viewmypokemonswindow.h
 * @brief 定义了查看精灵界面类。
 * @details 该界面显示了玩家的所有精灵。
 */

#ifndef CLIENT_VIEWMYPOKEMONSWINDOW_H
#define CLIENT_VIEWMYPOKEMONSWINDOW_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class ViewMyPokemonsWindow; }
QT_END_NAMESPACE

/**
 * @class ViewMyPokemonsWindow viewmypokemonswindow.h "include/viewmypokemonswindow.h"
 * @brief 查看精灵界面类。
 * @details 该界面显示了玩家的所有精灵。
 */
class ViewMyPokemonsWindow : public QWidget {
Q_OBJECT

public:
    // 构造函数
    explicit ViewMyPokemonsWindow(const std::string &s, QWidget *parent = nullptr);

    // 析构函数
    ~ViewMyPokemonsWindow() override;

private:
    Ui::ViewMyPokemonsWindow *ui; ///< 界面指针
};


#endif //CLIENT_VIEWMYPOKEMONSWINDOW_H
