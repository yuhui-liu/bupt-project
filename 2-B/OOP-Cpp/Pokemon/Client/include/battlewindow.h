/**
 * @file battlewindow.h
 * @brief 战斗窗口类的声明。
 * @details 声明了战斗窗口类。
 */

#ifndef CLIENT_BATTLEWINDOW_H
#define CLIENT_BATTLEWINDOW_H

#include "../include/Client.h"
#include <QWidget>
#include <functional>
#include <memory>


QT_BEGIN_NAMESPACE
namespace Ui { class BattleWindow; }
QT_END_NAMESPACE

/**
 * @class BattleWindow battlewindow.h "include/battlewindow.h"
 * @brief 战斗窗口类。
 * @details 用于显示战斗的窗口。
 */
class BattleWindow : public QWidget {
Q_OBJECT

public:
    // 构造函数
    explicit BattleWindow(std::string _me, std::unique_ptr<Client> client,
                          std::function<void(std::unique_ptr<Client>, std::string, bool, bool)> fn, bool _isDuel,
                          QWidget *parent = nullptr);

    // 析构函数
    ~BattleWindow() override;

    // 显示战斗日志
    void showBattleLog();

private:
    Ui::BattleWindow *ui; ///< 界面指针
    std::string msg; ///< 战斗信息
    bool isDuel; ///< 是否是决斗赛
    std::string me, enemy; ///< 我方精灵和敌方精灵
    std::function<void(std::unique_ptr<Client>, std::string, bool, bool)> back_to_caller; ///< 回到caller的回调函数
    std::unique_ptr<Client> cli; ///< 客户端的unique_ptr
    void closeEvent(QCloseEvent *event) override; // 重载关闭事件
    bool result; ///< 战斗结果
};


#endif //CLIENT_BATTLEWINDOW_H
