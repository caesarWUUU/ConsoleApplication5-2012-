#ifndef ARMOR_H
#define ARMOR_H

#include "Item.h"
#include <string>

enum class ArmorType {
    eHead = 0,  // �Y���˳�
    eBody,      // ����˳�
    eHand,      // �ⳡ�˳�
    eLeg        // �L���˳�
};

enum class ArmorSet {
    eNoSet,     // �L�M��
    eLeather,   // �֭��M��
    eSteel,     // ���K�M��
    eHoly       // ���t�M��
};

class CArmor : public CItem {
public:
    CArmor(std::string inname = "", int initSize = 0, int initWeight = 0,
        int initID = 0, int init_defense = 0,
        ArmorType type = ArmorType::eHead,
        ArmorSet set = ArmorSet::eNoSet);

    // �s�W��k
    void setdefensebonus(int value) { bonus_defense = value; }
    void setArmorType(ArmorType type) { armor_type = type; }

    // �{����k�[�W const
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