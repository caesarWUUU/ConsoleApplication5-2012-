#include <iostream>
#include "Item.h"
#include "food.h"
#include "LifeEntity.h"
#include "def.h"

using namespace std;

CFood::CFood(string inname, int initSize, int initWeight, int initID, int init_hp)
    : CItem(inname, initSize, initWeight, initID) {
    bonus_hp = init_hp;
}

int CFood::gethpbonus() const {
    return bonus_hp;
}

void CFood::beUsed(CLifeEntity* le) {
    cout << "戰士使用食物" << getName() << ", ";
    le->addHP(this->gethpbonus());
    cout << "體力回復到 " << le->getHP() << endl;
}

void CFood::UnUsed(CLifeEntity* le) {
    // 食物使用後即消失，無需實現 UnUsed
}

int CFood::isA() const {
    return efood;
}