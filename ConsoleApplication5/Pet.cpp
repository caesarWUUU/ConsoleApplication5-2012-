#include "Pet.h"
#include "Fighter.h" // 需要完整的 Fighter 類定義
#include <iostream>
#include <iomanip>
#include <cmath>
#include "def.h"
#include "Monster.h"

CPet::CPet(const CMonsterType* type, CFighter* owner)
    : CMonster(type), owner(owner), current_exp(0), state(PetState::ACTIVE), loyalty(50) {
    initializePet();
}

CPet::CPet(const CMonster* monster, CFighter* owner)
    : CMonster(*monster), owner(owner), current_exp(0), state(PetState::ACTIVE), loyalty(50) {
    initializePet();
}

CPet::~CPet() {
    // 特殊清理工作(如果需要)
}

void CPet::initializePet() {
    // 設定寵物初始暱稱(使用原本的怪物名稱)
    nickname = getname();

    // 調整基礎屬性
    calculateBaseStats();

    // 初始化等級相關屬性
    level = 1;
    current_exp = 0;

    cout << owner->getname() << " 獲得了新的寵物 " << nickname << "!" << endl;
    showStatus();
}
void CPet::setLoyalty(int value) {
    loyalty = std::max(0, std::min(100, value));  // 確保忠誠度在 0-100 之間
    cout << nickname << " 的忠誠度變更為 " << loyalty << endl;
}
void CPet::calculateBaseStats() {
    // 根據等級計算基礎屬性
    int base_modifier = (getLevel() - 1) * PET_STAT_GROWTH;  // 使用 getLevel() 而不是直接訪問 level

    // 調整HP和SP
    setMaxHP(getMAXHP() + base_modifier);
    setMaxSP(getMAXSP() + base_modifier);
    setHP(getMAXHP());
    setSP(getMAXSP());

    // 調整其他屬性
    RoughDegree = static_cast<int>(getRough() * (1 + base_modifier * 0.1));  // 使用 getRough() 獲取當前值
}
void CPet::showBattleStatus() const {
    cout << "\n=== 寵物戰鬥狀態 ===" << endl;
    cout << "名稱: " << nickname << endl;
    cout << "等級: " << getLevel() << endl;
    cout << "HP: " << getHP() << "/" << getMAXHP() << endl;
    cout << "SP: " << getSP() << "/" << getMAXSP() << endl;
    cout << "攻擊力: " << getRough() << endl;
    cout << "忠誠度: " << loyalty << "/100" << endl;
}
void CPet::setPetState(PetState newState) {
    state = newState;

    // 根據狀態變化進行相應處理
    switch (newState) {
    case PetState::ACTIVE:
        cout << nickname << " 已經進入出戰狀態！" << endl;
        break;
    case PetState::IDLE:
        cout << nickname << " 進入休息狀態。" << endl;
        break;
    case PetState::TIRED:
        cout << nickname << " 已經疲勞，需要休息。" << endl;
        break;
    }
}
void CPet::showStatus() const {
    cout << "\n=== 寵物狀態 ===" << endl;
    cout << "名稱: " << nickname << " (原種類: " << getname() << ")" << endl;
    cout << "等級: " << getLevel() << endl;  // 使用 getLevel()

    if (getLevel() < PET_MAX_LEVEL) {  // 使用 getLevel()
        cout << "經驗值: " << current_exp << "/" << getNextLevelExp()
            << " (" << fixed << setprecision(2) << getExpProgress() << "%)" << endl;
    }
    else {
        cout << "經驗值: 已達最高等級" << endl;
    }

    cout << "HP: " << getHP() << "/" << getMAXHP() << endl;
    cout << "SP: " << getSP() << "/" << getMAXSP() << endl;
    cout << "攻擊力: " << getRough() << endl;  // 使用 getRough()
    cout << "忠誠度: " << loyalty << "/100" << endl;

   
}

void CPet::addExp(int exp) {
    if (level >= PET_MAX_LEVEL) {
        cout << nickname << " 已達到最高等級!" << endl;
        return;
    }

    cout << nickname << " 獲得 " << exp << " 點經驗值！" << endl;
    current_exp += exp;

    // 顯示當前經驗值狀態
    cout << "當前經驗值：" << current_exp << "/" << getNextLevelExp()
        << " (" << fixed << setprecision(2) << getExpProgress() << "%)" << endl;

    while (canLevelUp()) {
        levelUp();
    }
}

bool CPet::canLevelUp() const {
    if (level >= PET_MAX_LEVEL) return false;
    return current_exp >= getNextLevelExp();
}

void CPet::levelUp() {
    if (level >= PET_MAX_LEVEL) return;

    current_exp -= getNextLevelExp();
    level++;

    // 提升屬性
    calculateBaseStats();

    cout << "\n" << nickname << " 升級了!" << endl;
    cout << "等級提升至 " << level << endl;
    showStatus();
}

unsigned int CPet::getNextLevelExp() const {
    if (level >= PET_MAX_LEVEL) return 0;
    return static_cast<unsigned int>(PET_BASE_EXP * pow(PET_EXP_MULTIPLIER, level - 1));
}

double CPet::getExpProgress() const {
    if (level >= PET_MAX_LEVEL) return 100.0;
    return (static_cast<double>(current_exp) / getNextLevelExp()) * 100;
}

int CPet::physicaldamage() {
    // 基礎傷害計算
    int base_damage = CMonster::physicaldamage();

    // 根據忠誠度調整傷害
    double loyalty_bonus = 1.0 + (loyalty / 100.0) * 0.5;  // 最高可獲得50%加成

    return static_cast<int>(base_damage * loyalty_bonus);
}

int CPet::attack(CLifeEntity* target) {
    // 寵物攻擊邏輯
    int damage = physicaldamage() - target->defense(target);
    if (damage > target->getHP()) {
        damage = target->getHP();
    }

    target->gethurt(damage);

    if (damage > 0) {
        cout << nickname << " 對 " << target->getname()
            << " 造成了 " << damage << " 點傷害!" << endl;
    }
    else {
        cout << nickname << " 的攻擊被 " << target->getname()
            << " 完全防禦了!" << endl;
    }

    return damage;
}

int CPet::defense(CLifeEntity* target) {
    // 基礎防禦值
    int base_defense = CMonster::defense(target);

    // 根據忠誠度增加防禦力
    double loyalty_bonus = 1.0 + (loyalty / 100.0) * 0.3;  // 最高可獲得30%加成

    return static_cast<int>(base_defense * loyalty_bonus);
}