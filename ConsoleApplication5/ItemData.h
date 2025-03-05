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
    std::vector<CArmor*> armor_array;  // �K�[����Ʋ�

private:
    void LoadFoodData();
    void LoadWeaponData();
    void LoadArmorData();  // �K�[Ū������ƾڪ����
};

#endif