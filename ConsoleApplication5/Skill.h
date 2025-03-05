
#ifndef SKILL_H
#define SKILL_H

#include <string>
#include "def.h"

class CLifeEntity;

enum class SkillType {
    PHYSICAL,   // 物理技能
    MAGICAL,    // 魔法技能
    BUFF,       // 增益效果
    HEAL        // 治療技能
};

class CSkill {
public:
    CSkill(const std::string& name, SkillType type, int sp_cost, int base_power, float scaling, const std::string& desc);
    ~CSkill();

    // 基本屬性存取
    const std::string& getName() const { return name; }
    SkillType getType() const { return type; }
    int getLevel() const { return level; }
    int getSpCost() const { return sp_cost; }
    float getScaling() const { return damage_scaling; }
    const std::string& getDescription() const { return description; }

    // 技能相關操作
    bool levelUp();  // 升級技能
    float getPower() const;  // 獲取當前等級的技能威力
    bool use(CLifeEntity* caster, CLifeEntity* target);  // 使用技能

private:
    std::string name;
    SkillType type;
    int level;              // 當前等級(1-5)
    int sp_cost;           // SP消耗
    int base_power;        // 基礎威力
    float damage_scaling;  // 傷害成長係數
    std::string description;

    static const int MAX_SKILL_LEVEL = 5;
};

#endif