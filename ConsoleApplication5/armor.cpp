#include <iostream>
#include "Item.h"
#include "armor.h"
#include "LifeEntity.h"
#include "def.h"

using namespace std;

CArmor::CArmor(string inname, int initSize, int initWeight, int initID,
    int init_defense, ArmorType type, ArmorSet set)
    : CItem(inname, initSize, initWeight, initID) {
    bonus_defense = init_defense;
    armor_type = type;
    armor_set = set;
    if (armor_set == ArmorSet::eNoSet) {
        determineArmorSet();
    }
}

int CArmor::getdefensebonus() const {
    return bonus_defense;
}

void CArmor::beUsed(CLifeEntity* le) {
    cout << "戰士裝備" << getName() << endl;
}

void CArmor::UnUsed(CLifeEntity* le) {
    cout << "戰士卸下" << getName() << endl;
}

void CArmor::determineArmorSet() {
    if (ID <= 0) {
        armor_set = ArmorSet::eNoSet;
        return;
    }

    switch (ID % 3) {
    case 1:
        armor_set = ArmorSet::eLeather;
        break;
    case 2:
        armor_set = ArmorSet::eSteel;
        break;
    case 0:
        armor_set = ArmorSet::eHoly;
        break;
    default:
        armor_set = ArmorSet::eNoSet;
    }
}

int CArmor::getSetBonus(ArmorSet set) {
    switch (set) {
    case ArmorSet::eLeather:
        return 15;
    case ArmorSet::eSteel:
        return 20;
    case ArmorSet::eHoly:
        return 30;
    default:
        return 0;
    }
}

int CArmor::isA() const {
    return earmor;
}