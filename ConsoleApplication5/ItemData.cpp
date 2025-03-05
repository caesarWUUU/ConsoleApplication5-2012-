#include <fstream>
#include <iostream>
#include "Item.h"
#include "food.h"
#include "weapon.h"
#include "ItemData.h"
#include "GlobalInfo.h"
#include "armor.h"
using namespace std;

// 添加析構函數實現
CItemData::~CItemData() {
    for (auto* food : food_array) {
        delete food;
    }
    food_array.clear();

    for (auto* weapon : weapon_array) {
        delete weapon;
    }
    weapon_array.clear();

    for (auto* armor : armor_array) {
        delete armor;
    }
    armor_array.clear();
}

void CItemData::Initialize() {
    LoadFoodData();
    LoadWeaponData();
    LoadArmorData();
}

int CItemData::totalsize() {
    return food_array.size() + weapon_array.size() + armor_array.size();
}

CItem* CItemData::getRand() {
    if (totalsize() == 0) {
        return NULL;
    }

    unsigned int randnum = rand() % totalsize();
    if (randnum < food_array.size()) {
        return food_array[randnum];
    }
    randnum -= food_array.size();
    if (randnum < weapon_array.size()) {
        return weapon_array[randnum];
    }
    randnum -= weapon_array.size();
    if (randnum < armor_array.size()) {
        return armor_array[randnum];
    }
    return NULL;
}
void CItemData::LoadArmorData() {
    ifstream fin("armor.txt");
    if (!fin) {
        cout << "無法開啟檔案: armor.txt" << endl;
        return;
    }

    string name;
    int defense_bonus;
    int inID;
    int armor_type;

    while (fin >> inID >> name >> defense_bonus >> armor_type) {
        CArmor* armor = new CArmor(name, 0, 0, inID, defense_bonus,
            static_cast<ArmorType>(armor_type));
        armor_array.push_back(armor);
    }
    fin.close();
}
void CItemData::LoadFoodData() {
    ifstream fin("food.txt");
    if (!fin) {
        cout << "無法開啟檔案: food.txt" << endl;
        return;
    }

    string name;
    int hp_bonus;
    int inID;

    while (fin >> inID >> name >> hp_bonus) {
        CFood* food = new CFood(name, 0, 0, inID, hp_bonus);
        food_array.push_back(food);
    }
    fin.close();
}

void CItemData::LoadWeaponData() {
    ifstream fin("weapon.txt");
    if (!fin) {
        cout << "無法開啟檔案: weapon.txt" << endl;
        return;
    }

    string name;
    int attack_bonus;
    int inID;

    while (fin >> inID >> name >> attack_bonus) {
        CWeapon* weapon = new CWeapon(name, 0, 0, inID, attack_bonus);
        weapon_array.push_back(weapon);
    }
    fin.close();
}