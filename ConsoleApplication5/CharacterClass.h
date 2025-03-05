#ifndef CHARACTER_CLASS_H
#define CHARACTER_CLASS_H

#include <string>
#include <vector>

// 職業類型列舉
enum CharacterClassType {
    NoClass = 0,    // 初始無職業
    Warrior = 1,    // 戰士
    Mage = 2        // 法師
};

// 職業技能結構
struct ClassSkill {
    std::string name;           // 技能名稱
    int spCost;                 // SP消耗
    int baseDamage;            // 基礎傷害
    int cooldown;              // 冷卻回合
    std::string description;    // 技能描述
};

// 職業基礎屬性加成
struct ClassAttributes {
    int strengthBonus;     // 力量加成
    int intelligenceBonus; // 智力加成
    int tenacityBonus;     // 體力加成
    int luckyBonus;        // 幸運加成
};

// 職業類別
class CharacterClass {
public:
    CharacterClass(CharacterClassType type = NoClass);

    // 獲取職業資訊
    CharacterClassType getType() const { return classType; }
    std::string getClassName() const;
    std::string getDescription() const { return description; }

    // 獲取職業屬性加成
    const ClassAttributes& getAttributes() const { return attributes; }

    // 獲取職業技能
    const std::vector<ClassSkill>& getSkills() const { return skills; }

private:
    CharacterClassType classType;
    std::string description;        // 職業描述
    ClassAttributes attributes;     // 職業屬性加成
    std::vector<ClassSkill> skills; // 職業技能列表

    // 初始化職業資訊
    void initializeWarrior();
    void initializeMage();
};
#endif