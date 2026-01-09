/**
 * @file viewpokemonwindow.h
 * @brief 定义了查看精灵界面类。
 * @details 该界面显示了所有玩家的所有精灵。
 */

#ifndef CLIENT_VIEWPOKEMONWINDOW_H
#define CLIENT_VIEWPOKEMONWINDOW_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class ViewPokemonWindow; }
QT_END_NAMESPACE

/**
 * @class ViewPokemonWindow viewpokemonwindow.h "include/viewpokemonwindow.h"
 * @brief 查看精灵界面类。
 * @details 该界面显示了所有玩家的所有精灵。
 */
class ViewPokemonWindow : public QWidget {
Q_OBJECT

public:
    // 构造函数
    explicit ViewPokemonWindow(QWidget *parent = nullptr);

    // 析构函数
    ~ViewPokemonWindow() override;

    // 显示精灵
    void showPokemons(const std::vector<std::string> &username, std::vector<std::vector<std::string>> pokes) const;

private:
    Ui::ViewPokemonWindow *ui; ///< 界面指针
};


#endif //CLIENT_VIEWPOKEMONWINDOW_H
