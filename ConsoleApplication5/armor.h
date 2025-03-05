#ifndef ARMOR_H
#define ARMOR_H

#include "Item.h"
#include <string>

enum class ArmorType {
    eHead = 0,  // 頭部裝備
    eBody,      // 身體裝備
    eHand,      // 手部裝備
    eLeg        // 腿部裝備
};

enum class ArmorSet {
    eNoSet,     // 無套裝
    eLeather,   // 皮革套裝
    eSteel,     // 鋼鐵套裝
    eHoly       // 神聖套裝
};

class CArmor : public CItem {
public:
    CArmor(std::string inname = "", int initSize = 0, int initWeight = 0,
        int initID = 0, int init_defense = 0,
        ArmorType type = ArmorType::eHead,
        ArmorSet set = ArmorSet::eNoSet);

    // 新增方法
    void setdefensebonus(int value) { bonus_defense = value; }
    void setArmorType(ArmorType type) { armor_type = type; }

    // 現有方法加上 const
    int getdefensebonus() const;
    ArmorType getArmorType() const { return armor_type; }
    ArmorSet getArmorSet() const { return armor_set; }
    static int getSetBonus(ArmorSet set);

    virtual void beUsed(CLifeEntity*) override;
    virtual void UnUsed(CLifeEntity*) override;
    virtual int isA() const override;

private:
    int bonus_defense;
    ArmorType armor_type;
    ArmorSet armor_set;
    void determineArmorSet();
};

#endif