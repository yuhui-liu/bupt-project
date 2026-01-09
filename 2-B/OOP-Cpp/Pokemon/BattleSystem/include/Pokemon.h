/**
 * @file Pokemon.h
 * @brief Pokemon类的头文件。
 * @details 定义了Pokemon类及其派生类的属性和行为。
 */

#ifndef BATTLESYSTEM_POKEMON_H
#define BATTLESYSTEM_POKEMON_H

#include "../include/nlohmann/json.hpp"
#include <string>
#include <array>
#include <utility>

using json = nlohmann::json;

/**
 * @class Pokemon Pokemon.h "include/Pokemon.h"
 * @brief Pokemon类
 * @details Pokemon类，定义了精灵的基本属性和行为。
 */
class Pokemon {
protected:
    std::string name; ///< 名字
    std::string skill; ///< 招式名称
    int level = 1; ///< 等级
    int experience; ///< 经验值
    /// 等级对应的经验值
    static constexpr std::array<int, 15> levelExp = {0, 10, 25, 41, 59, 80, 107, 137, 170, 206, 245, 287, 335, 386,
                                                     441};
    /**
     * @enum Type
     * @brief 精灵种类枚举
     * @details 定义了精灵的种类。
     */
    enum class Type {
        Power, ///< 力量型
        Tank, ///< 肉盾型
        Defensive, ///< 防御型
        Agile ///< 敏捷型
    };
    Type type; ///< 精灵种类
    double attackPower; ///< 攻击力
    double defensePower; ///< 防御力
    double healthPoint; ///< 生命值
    int attackInterval; ///< 攻击间隔
    double specialAttack; ///< 特殊攻击标志

public:
    /**
     * @brief 构造函数
     * @param s 名字
     * @param ski 技能名
     */
    explicit Pokemon(std::string s, std::string ski);

    /**
     * @brief 构造函数
     * @param j 用于构造的@c json 对象
     */
    explicit Pokemon(json j);

    virtual ~Pokemon() = default; ///< 析构函数
    void checkIfLevelUp(); ///< 检查是否满足升级条件
    void levelUp(); ///< 升级函数
    virtual double attack(); ///< 攻击方法
    void test(); ///< 测试函数
    bool battle(Pokemon &other); ///< 对战函数
    // getters
    [[nodiscard]] int getLevel() const; ///< 返回等级
    [[nodiscard]] const std::string &getName() const; ///< 返回名字
    [[nodiscard]] int getExperience() const; ///< 返回经验值
    [[nodiscard]] const std::string &getType() const; ///< 返回种类
    [[nodiscard]] double getDefensePower() const; ///< 返回防御力
    [[nodiscard]] double getAttackPower() const; ///< 返回攻击力
    [[nodiscard]] double getHealthPoint() const; ///< 返回生命值
    [[nodiscard]] int getAttackInterval() const; ///< 返回攻击间隔
    [[nodiscard]] const std::string &getSkill() const; ///< 返回技能名
    [[nodiscard]] double getSpecialAttack() const; ///< 返回特殊攻击标志

    void addExperience(int exp); ///< 增加经验值
};

/**
 * @class PowerPokemon Pokemon.h "include/Pokemon.h"
 * @brief 力量型Pokemon类
 * @details PowerPokemon类，继承自Pokemon类，定义了力量型精灵的属性和行为。
 */
class PowerPokemon : public Pokemon {
public:
    explicit PowerPokemon(std::string, std::string);

    explicit PowerPokemon(json j) : Pokemon(std::move(j)) {}

    double attack() override;
};

/**
 * @class TankPokemon Pokemon.h "include/Pokemon.h"
 * @brief 肉盾型Pokemon类
 * @details TankPokemon类，继承自Pokemon类，定义了肉盾型精灵的属性和行为。
 */
class TankPokemon : public Pokemon {
private:
    double curePoint = -1; ///< 治疗量

public:
    explicit TankPokemon(std::string, std::string);

    explicit TankPokemon(json j) : Pokemon(std::move(j)) {}

    double attack() override;
};

/**
 * @class DefensivePokemon Pokemon.h "include/Pokemon.h"
 * @brief 防御型Pokemon类
 * @details DefensivePokemon类，继承自Pokemon类，定义了防御型精灵的属性和行为。
 */
class DefensivePokemon : public Pokemon {
private:
    bool flag = false; ///< 技能触发标志
public:
    explicit DefensivePokemon(std::string, std::string);

    explicit DefensivePokemon(json j) : Pokemon(std::move(j)) {}

    double attack() override;
};

/**
 * @class AgilePokemon Pokemon.h "include/Pokemon.h"
 * @brief 敏捷型Pokemon类
 * @details AgilePokemon类，继承自Pokemon类，定义了敏捷型精灵的属性和行为。
 */
class AgilePokemon : public Pokemon {
public:
    explicit AgilePokemon(std::string, std::string);

    explicit AgilePokemon(json j) : Pokemon(std::move(j)) {}

    double attack() override;
};

#endif //BATTLESYSTEM_POKEMON_H
