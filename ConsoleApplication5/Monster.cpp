#include <iostream>
#include <string>
#include "Monster.h"
#include "MonsterData.h"
#include "def.h"

using namespace std;

int CMonster::counter_for_monster_id = 0;

CMonster::CMonster(int initHP, int initSP, int initrough,
    string init_name, string init_eng_name)
    : CLifeEntity(initHP, initSP, init_name),
    RoughDegree(initrough),
    eng_name(init_eng_name)
{
    // 加入參數檢查
    if (initHP <= 0 || initSP <= 0 || initrough < 0) {
        throw invalid_argument("Invalid monster parameters");
    }

    counter_for_monster_id++;
    level = 1;

    // 確保初始化完成
    cout << "Created monster: " << init_name
        << " (HP=" << initHP
        << ", SP=" << initSP
        << ", Rough=" << initrough
        << ", Level=" << level << ")" << endl;
}

CMonster::CMonster(const CMonsterType* type)
    : CLifeEntity(type->max_hp,
        type->max_sp,
        type->name),
    RoughDegree(type->max_rough) {
    counter_for_monster_id++;
    level = type->level;
    eng_name = type->prefix_eng_name + to_string(counter_for_monster_id);

    // 修改建構訊息格式，使用整數等級
    cout << "生成了 " << type->name << " Lv." << level
        << " (" << eng_name << ")" << endl;
}

int CMonster::getRough() const {
    return RoughDegree;
}

int CMonster::physicaldamage() {
    return (rand() % getSP() + getRough());
}

int CMonster::attack(CLifeEntity* l) {
    // 加入防護檢查
    if (!l || this->isdead()) {
        return 0;
    }

    int damage = physicaldamage() - l->defense(l);
    if (damage > l->getHP()) {
        damage = l->getHP();
    }

    l->gethurt(damage);

    if (damage > 0) {
        cout << this->getname() << " 攻擊命中，造成 " << l->getname()
            << " " << damage << " 點傷害" << endl;
    }
    else {
        cout << this->getname() << " 攻擊命中，但是 " << l->getname()
            << " 完全防禦，因此閃避了攻擊" << endl;
    }
    return damage;
}

int CMonster::defense(CLifeEntity* l) {
    return getRough();  // 使用怪物的 RoughDegree 作為防禦值
}

int CMonster::isA() const {
    return emonster;
}

string CMonster::get_basic_data() const {
    // 只顯示怪物名稱和整數等級
    string output = this->getname() + " Lv." + to_string((int)level);
    return output;
}
