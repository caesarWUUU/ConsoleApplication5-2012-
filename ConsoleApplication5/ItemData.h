#ifndef ITEMDATA_H
#define ITEMDATA_H

#include <vector>
#include "Item.h"

class CFood;
class CWeapon;
class CArmor;

class CItemData {
public:
    CItemData() {}
    ~CItemData();
    void Initialize();
    int totalsize();
    CItem* getRand();

    std::vector<CFood*> food_array;
    std::vector<CWeapon*> weapon_array;
    std::vector<CArmor*> armor_array;  // 添加防具數組

private:
    void LoadFoodData();
    void LoadWeaponData();
    void LoadArmorData();  // 添加讀取防具數據的函數
};

#endif