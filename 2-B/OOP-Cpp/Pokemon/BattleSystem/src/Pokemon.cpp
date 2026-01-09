/**
 * @file Pokemon.cpp
 * @brief Pokemon类的实现文件。
 * @details 包含Pokemon类的成员函数的实现。
 */
#include "../include/Pokemon.h"
#include "../include/Tools.h"
#include <algorithm>
#include <iostream>
#include <utility>

/**
 * @details 构造函数，初始化精灵属性。通过随机数在一定范围内生成攻击力、防御力、生命值、攻击间隔，并且防止总和超过或者低于一定值引起不平衡。
 */
Pokemon::Pokemon(std::string s, std::string ski) : name(std::move(s)), skill(std::move(ski)) {
    experience = 0;
    attackPower = random::randDouble(9, 11);
    defensePower = random::randDouble(4, 6);
    healthPoint = random::randDouble(28, 32);
    attackInterval = random::randInt(55, 65);
    if (attackPower + defensePower + healthPoint + attackInterval < 98)
        attackPower++, defensePower++, healthPoint++, attackInterval++;
    else if (attackPower + defensePower + healthPoint + attackInterval > 112)
        attackPower--, defensePower--, healthPoint--, attackInterval--;
}

/**
 * @details 构造函数，根据传入的@c json 对象初始化精灵各项属性。
 */
Pokemon::Pokemon(json j) : name(j["name"]), level(j["level"]), experience(j["exp"]), healthPoint(j["hp"]),
                           attackPower(j["att_point"]), defensePower(j["defensive"]), attackInterval(j["att_interval"]),
                           skill(j["skill"]) {
    std::string t = j["type"];
    switch (t[0]) {
        case 'P':
            type = Type::Power;
            break;
        case 'T':
            type = Type::Tank;
            break;
        case 'D':
            type = Type::Defensive;
            break;
        case 'A':
            type = Type::Agile;
            break;
    }
}

/**
 * @details 检查是否满足升级条件，满足则升级。
 */
void Pokemon::checkIfLevelUp() {
    if (auto i = std::lower_bound(levelExp.begin(), levelExp.end(), experience) - levelExp.begin() + 1 - level; i > 0)
        while (i--)
            levelUp();
}

/**
 * @details 升级函数，根据精灵种类不同，升级时属性增长不同。主属性增长较多，次属性增长较少。
 */
void Pokemon::levelUp() {
    level++;
    switch (type) {
        case Type::Power:
            attackPower *= random::randDouble(1.3, 1.5);
            defensePower *= random::randDouble(1.05, 1.1);
            healthPoint *= random::randDouble(1.1, 1.2);
            attackInterval = static_cast<int>(attackInterval * random::randDouble(0.83, 0.91));
            break;
        case Type::Tank:
            attackPower *= random::randDouble(1.1, 1.2);
            defensePower *= random::randDouble(1.05, 1.1);
            healthPoint *= random::randDouble(1.3, 1.5);
            attackInterval = static_cast<int>(attackInterval * random::randDouble(0.83, 0.91));
            break;
        case Type::Defensive:
            attackPower *= random::randDouble(1.1, 1.2);
            defensePower *= random::randDouble(1.1, 1.2);
            healthPoint *= random::randDouble(1.1, 1.2);
            attackInterval = static_cast<int>(attackInterval * random::randDouble(0.83, 0.91));
            break;
        case Type::Agile:
            attackPower *= random::randDouble(1.1, 1.2);
            defensePower *= random::randDouble(1.05, 1.1);
            healthPoint *= random::randDouble(1.1, 1.2);
            attackInterval = static_cast<int>(attackInterval * random::randDouble(0.67, 0.77));
            break;
    }
}

/**
 * @details 攻击函数，输出攻击信息，返回攻击力。
 */
double Pokemon::attack() {
    std::cout << name << " 发动了 " << skill << "!\n";
    std::cout << name << " 造成了 " << attackPower << " 点伤害！\n";
    return attackPower;
}

/**
 * @details 测试函数，模拟精灵升级和攻击过程。
 */
void Pokemon::test() {
    attack();
    for (int i = 0; i < 10; ++i) {
        addExperience(15);
        checkIfLevelUp();
        attack();
    }
}

/**
 * @details 战斗函数，模拟精灵之间的战斗过程。根据攻击间隔，模拟两个精灵的攻击过程，直到一方血量小于0。胜利获得大量经验，失败获得较少经验。
 */
bool Pokemon::battle(Pokemon &other) {
    char timeIntervals[2000]{}; // 时间间隔，每个间隔是一个时间单位
    // 根据攻击间隔，初始化时间间隔数组，1代表我方，2代表对方，3代表同时攻击
    for (int i = 0; i < 2000; i += this->getAttackInterval()) {
        timeIntervals[i] = 1;
    }
    for (int i = 0; i < 2000; i += other.getAttackInterval()) {
        timeIntervals[i] += 2;
    }
    // 实时血量
    auto myHp = this->getHealthPoint(), hisHp = other.getHealthPoint();
    // 触发了特殊技能的标志
    bool myFlag = false, hisFlag = false;
    // 检查是否触发肉盾型的特殊技能的匿名函数，返回值为回复的血量
    auto tankCheck = [](const Pokemon &p) -> double {
        if (p.getType()[0] == 'T') {
            if (auto x = p.getSpecialAttack(); x > 0) {
                return x;
            } else return 0;
        } else return 0;
    };
    // 检查是否触发防御型的特殊技能的匿名函数，返回值为是否完全防御下一次攻击
    auto defensiveCheck = [](const Pokemon &p) -> bool {
        if (p.getType()[0] == 'D') {
            if (p.getSpecialAttack() > 0) {
                return true;
            } else return false;
        } else return false;
    };
    // 检查血量小于0的匿名函数，返回值为1代表我方血量小于0，2代表对方血量小于0，0代表都大于0
    auto checkHp = [&]() -> int {
        if (myHp < 0) {
            return 1;
        } else if (hisHp < 0) {
            return 2;
        }
        return 0;
    };
    // 我方回合的匿名函数，输出攻击信息，计算对方血量，检查是否触发特殊技能。伤害值是我方攻击力减去对方防御力，最低造成0.1点伤害，防止无限循环
    auto myRound = [&] {
        auto x = this->attack() - other.getDefensePower();
        if (!hisFlag) {
            hisHp -= (x > 0 ? x : 0.1);
            std::cout << other.getName() << " 防御了 " << other.getDefensePower() << " 点！\n";
        } else {
            std::cout << other.getName() << " 完全防御了！\n";
        }
        myHp += tankCheck(*this);
        myFlag = defensiveCheck(*this);
    };
    // 对方回合的匿名函数，输出攻击信息，计算我方血量，检查是否触发特殊技能。伤害值是对方攻击力减去我方防御力，最低造成0.1点伤害，防止无限循环
    auto hisRound = [&] {
        auto x = other.attack() - this->getDefensePower();
        if (!myFlag) {
            myHp -= (x > 0 ? x : 0.1);
            std::cout << this->getName() << " 防御了 " << this->getDefensePower() << " 点！\n";
        } else {
            std::cout << this->getName() << " 完全防御了！\n";
        }
        hisHp += tankCheck(other);
        hisFlag = defensiveCheck(other);
    };
    // 战斗结束标志
    int flag = 0;
    // 模拟战斗过程
    for (char timeInterval: timeIntervals) {
        if (timeInterval == 1) { // 我方回合
            myRound();
            flag = checkHp();
            if (flag)
                break;
        } else if (timeInterval == 2) { // 对方回合
            hisRound();
            flag = checkHp();
            if (flag)
                break;
        } else if (timeInterval == 3) { // 同时攻击则根据攻击间隔判断谁先攻击
            if (this->getAttackInterval() < other.getAttackInterval()) {
                myRound();
                flag = checkHp();
                if (flag)
                    break;
                hisRound();
                flag = checkHp();
                if (flag)
                    break;
            } else {
                hisRound();
                flag = checkHp();
                if (flag)
                    break;
                myRound();
                flag = checkHp();
                if (flag)
                    break;
            }
        }
    }
    // 输出战斗结果，获得经验，检查是否升级
    int exp;
    if (flag == 2) {
        std::cout << this->getName() << " 赢了！\n";
        exp = static_cast<int>(this->getExperience() * random::randDouble(0.3, 0.5)) + 10;
    } else {
        std::cout << this->getName() << " 输了！\n";
        exp = static_cast<int>(this->getExperience() * random::randDouble(0.1, 0.2)) + 3;
    }
    std::cout << this->getName() << " 获得了 " << exp << " 点经验。\n";
    this->addExperience(exp);
    this->checkIfLevelUp();
    return (flag == 2);
}

double Pokemon::getDefensePower() const {
    return defensePower;
}

double Pokemon::getAttackPower() const {
    return attackPower;
}

double Pokemon::getHealthPoint() const {
    return healthPoint;
}

int Pokemon::getAttackInterval() const {
    return attackInterval;
}

int Pokemon::getLevel() const {
    return level;
}

const std::string &Pokemon::getName() const {
    return name;
}

int Pokemon::getExperience() const {
    return experience;
}

const std::string &Pokemon::getType() const {
    static const std::string types[4]{"Power", "Tank", "Defensive", "Agile"};
    return types[static_cast<int>(type)];
}

const std::string &Pokemon::getSkill() const {
    return skill;
}

double Pokemon::getSpecialAttack() const {
    return specialAttack;
}

void Pokemon::addExperience(int exp) {
    experience += exp;
}

/**
 * @details 力量型精灵构造函数，初始攻击力在原有基础上增加4。
 */
PowerPokemon::PowerPokemon(std::string s, std::string ski) : Pokemon(std::move(s), std::move(ski)) {
    type = Type::Power;
    attackPower += 4;
}

/**
 * @details 力量型精灵攻击函数，有概率触发暴击，额外造成一定伤害。
 */
double PowerPokemon::attack() {
    double r = Pokemon::attack();
    if (random::randInt(1, 5) == 1) {
        double x = random::randDouble(0.67, 1) * r;
        std::cout << name << " 触发了 暴击！额外造成 " << x << " 点伤害！\n";
        r += x;
    }
    return r;
}

/**
 * @details 肉盾型精灵构造函数，初始生命值在原有基础上增加15。
 */
TankPokemon::TankPokemon(std::string s, std::string ski) : Pokemon(std::move(s), std::move(ski)) {
    type = Type::Tank;
    healthPoint += 15;
}

/**
 * @details 肉盾型精灵攻击函数，有概率触发治疗，回复一定血量。
 */
double TankPokemon::attack() {
    curePoint = -1;
    double r = Pokemon::attack();
    if (random::randInt(1, 5) == 1) {
        curePoint = random::randDouble(0.3, 0.5) * healthPoint;
        std::cout << name << " 触发了 治疗！回复 " << curePoint << " 点血量！\n";
    }
    specialAttack = curePoint;
    return r;
}

/**
 * @details 防御型精灵构造函数，初始防御力在原有基础上增加3。
 */
DefensivePokemon::DefensivePokemon(std::string s, std::string ski) : Pokemon(std::move(s), std::move(ski)) {
    type = Type::Defensive;
    defensePower += 3;
}

/**
 * @details 防御型精灵攻击函数，有概率触发结界，下次攻击无效。
 */
double DefensivePokemon::attack() {
    flag = false;
    double r = Pokemon::attack();
    if (random::randInt(1, 6) == 1) {
        std::cout << name << " 触发了 结界！对面下次攻击无效！\n";
        flag = true;
    }
    specialAttack = (flag ? 1 : -1);
    return r;
}

/**
 * @details 敏捷型精灵构造函数，初始攻击间隔减少25。
 */
AgilePokemon::AgilePokemon(std::string s, std::string ski) : Pokemon(std::move(s), std::move(ski)) {
    type = Type::Agile;
    attackInterval -= 25;
}

/**
 * @details 敏捷型精灵攻击函数，有概率触发双重打击，造成两次伤害。
 */
double AgilePokemon::attack() {
    double r = Pokemon::attack();
    if (random::randInt(1, 6) == 1) {
        std::cout << name << " 触发了 双重打击！造成了两次伤害！\n";
        r *= 2;
    }
    return r;
}
