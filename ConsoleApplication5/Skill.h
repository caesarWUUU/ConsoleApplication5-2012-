
#ifndef SKILL_H
#define SKILL_H

#include <string>
#include "def.h"

class CLifeEntity;

enum class SkillType {
    PHYSICAL,   // ���z�ޯ�
    MAGICAL,    // �]�k�ޯ�
    BUFF,       // �W�q�ĪG
    HEAL        // �v���ޯ�
};

class CSkill {
public:
    CSkill(const std::string& name, SkillType type, int sp_cost, int base_power, float scaling, const std::string& desc);
    ~CSkill();

    // ���ݩʦs��
    const std::string& getName() const { return name; }
    SkillType getType() const { return type; }
    int getLevel() const { return level; }
    int getSpCost() const { return sp_cost; }
    float getScaling() const { return damage_scaling; }
    const std::string& getDescription() const { return description; }

    // �ޯ�����ާ@
    bool levelUp();  // �ɯŧޯ�
    float getPower() const;  // �����e���Ū��ޯ�¤O
    bool use(CLifeEntity* caster, CLifeEntity* target);  // �ϥΧޯ�

private:
    std::string name;
    SkillType type;
    int level;              // ��e����(1-5)
    int sp_cost;           // SP����
    int base_power;        // ��¦�¤O
    float damage_scaling;  // �ˮ`�����Y��
    std::string description;

    static const int MAX_SKILL_LEVEL = 5;
};

#endif