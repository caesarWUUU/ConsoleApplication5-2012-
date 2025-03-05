#ifndef CHARACTER_CLASS_H
#define CHARACTER_CLASS_H

#include <string>
#include <vector>

// ¾�~�����C�|
enum CharacterClassType {
    NoClass = 0,    // ��l�L¾�~
    Warrior = 1,    // �Ԥh
    Mage = 2        // �k�v
};

// ¾�~�ޯ൲�c
struct ClassSkill {
    std::string name;           // �ޯ�W��
    int spCost;                 // SP����
    int baseDamage;            // ��¦�ˮ`
    int cooldown;              // �N�o�^�X
    std::string description;    // �ޯ�y�z
};

// ¾�~��¦�ݩʥ[��
struct ClassAttributes {
    int strengthBonus;     // �O�q�[��
    int intelligenceBonus; // ���O�[��
    int tenacityBonus;     // ��O�[��
    int luckyBonus;        // ���B�[��
};

// ¾�~���O
class CharacterClass {
public:
    CharacterClass(CharacterClassType type = NoClass);

    // ���¾�~��T
    CharacterClassType getType() const { return classType; }
    std::string getClassName() const;
    std::string getDescription() const { return description; }

    // ���¾�~�ݩʥ[��
    const ClassAttributes& getAttributes() const { return attributes; }

    // ���¾�~�ޯ�
    const std::vector<ClassSkill>& getSkills() const { return skills; }

private:
    CharacterClassType classType;
    std::string description;        // ¾�~�y�z
    ClassAttributes attributes;     // ¾�~�ݩʥ[��
    std::vector<ClassSkill> skills; // ¾�~�ޯ�C��

    // ��l��¾�~��T
    void initializeWarrior();
    void initializeMage();
};
#endif