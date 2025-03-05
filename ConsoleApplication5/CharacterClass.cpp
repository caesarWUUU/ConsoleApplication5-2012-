#include "CharacterClass.h"

CharacterClass::CharacterClass(CharacterClassType type) : classType(type) {
    switch (type) {
    case Warrior:
        initializeWarrior();
        break;
    case Mage:
        initializeMage();
        break;
    default:
        // 無職業狀態，所有屬性加成為0
        attributes = { 0, 0, 0, 0 };
        description = "尚未選擇職業";
        break;
    }
}

void CharacterClass::initializeWarrior() {
    // 設定戰士基本屬性加成
    attributes = {
        20,     // 力量 +20
        5,      // 智力 +5
        15,     // 體力 +15
        5       // 幸運 +5
    };

    description = "戰士擅長近身戰鬥，具有強大的物理攻擊力和防禦力";

    // 設定戰士技能
    skills = {
        {"猛力一擊", 20, 50, 3, "對敵人造成150%物理傷害"},
        {"防禦姿態", 15, 0, 4, "增加30%防禦力，持續3回合"},
        {"戰吼", 25, 0, 5, "提升自身20%攻擊力，持續3回合"}
    };
}

void CharacterClass::initializeMage() {
    // 設定法師基本屬性加成
    attributes = {
        5,      // 力量 +5
        25,     // 智力 +25
        10,     // 體力 +10
        5       // 幸運 +5
    };

    description = "法師精通魔法，具有強大的魔法攻擊力和豐富的法力值";

    // 設定法師技能
    skills = {
        {"火球術", 25, 60, 2, "對敵人造成魔法傷害"},
        {"冰凍術", 30, 45, 3, "造成傷害並減緩敵人速度"},
        {"魔力護盾", 35, 0, 4, "產生一個能吸收傷害的護盾"}
    };
}

std::string CharacterClass::getClassName() const {
    switch (classType) {
    case Warrior:
        return "戰士";
    case Mage:
        return "法師";
    default:
        return "無職業";
    }
}
