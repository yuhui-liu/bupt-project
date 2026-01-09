/**
 * @file losewindow.h
 * @brief 失败窗口类的声明。
 * @details 声明了失败窗口类，包括构造函数、析构函数和槽函数。
 */

#ifndef CLIENT_LOSEWINDOW_H
#define CLIENT_LOSEWINDOW_H

#include <QDialog>
#include <QRadioButton>

QT_BEGIN_NAMESPACE
namespace Ui { class LoseWindow; }
QT_END_NAMESPACE

/**
 * @class LoseWindow losewindow.h "include/losewindow.h"
 * @brief 失败窗口类。
 * @details 用于显示失败窗口，选择失去的精灵。
 */
class LoseWindow : public QDialog {
Q_OBJECT

public:
    // 构造函数
    explicit LoseWindow(std::vector<std::string> _pokes, QWidget *parent = nullptr);

    // 析构函数
    ~LoseWindow() override;

    // 槽函数，判断选择的精灵
    void handler();

private:
    Ui::LoseWindow *ui; ///< 界面指针
    std::vector<std::string> pokes; ///< 精灵列表
    QRadioButton *op3; ///< 第三个单选框
    int i = 0, j = 1, k = 2; ///< 所选择精灵的下标
};


#endif //CLIENT_LOSEWINDOW_H
