#include <iostream>
#include "Item.h"
#include "weapon.h"
#include "LifeEntity.h"
#include "def.h"

using namespace std;

CWeapon::CWeapon(string inname, int initSize, int initWeight, int initID, int init_attack)
    : CItem(inname, initSize, initWeight, initID) {
    bonus_attack = init_attack;
}

int CWeapon::getattackbonus() const {
    return bonus_attack;
}

void CWeapon::beUsed(CLifeEntity* le) {
    cout << "戰士使用武器" << getName() << ", ";
    cout << "攻擊力從原本 " << le->getSP();
    le->addSP(this->getattackbonus());
    cout << "上升到 " << le->getSP() << endl;
}

void CWeapon::UnUsed(CLifeEntity* le) {
    cout << "戰士 " << getName() << " ";
    cout << "攻擊力從原本 " << le->getSP();
    le->delSP(this->getattackbonus());
    cout << "下降到 " << le->getSP() << endl;
}

int CWeapon::isA() const {
    return eweapon;
}