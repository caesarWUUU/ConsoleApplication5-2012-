#include "Skill.h"
#include "LifeEntity.h"
#include <iostream>

CSkill::CSkill(const std::string& in_name, SkillType in_type, int in_sp_cost, int in_base_power, float in_scaling, const std::string& in_desc)
    : name(in_name), type(in_type), level(1), sp_cost(in_sp_cost),
    base_power(in_base_power), damage_scaling(in_scaling), description(in_desc) {
}

CSkill::~CSkill() {
}

bool CSkill::levelUp() {
    if (level >= MAX_SKILL_LEVEL) {
        std::cout << name << " 已達到最高等級!" << std::endl;
        return false;
    }
    level++;
    std::cout << name << " 升級至 " << level << " 級!" << std::endl;
    return true;
}

float CSkill::getPower() const {
    return base_power * (1.0f + (level - 1) * damage_scaling);
}

bool CSkill::use(CLifeEntity* caster, CLifeEntity* target) {
    if (!caster || !target) return false;

    // 檢查SP是否足夠
    if (caster->getSP() < sp_cost) {
        std::cout << "SP不足,無法使用技能!" << std::endl;
        return false;
    }

    // 消耗SP
    caster->delSP(sp_cost);

    // 根據技能類型執行不同效果
    switch (type) {
    case SkillType::PHYSICAL:
    case SkillType::MAGICAL: {
        int damage = static_cast<int>(getPower());
        target->gethurt(damage);
        std::cout << caster->getname() << " 使用 " << name
            << " 對 " << target->getname()
            << " 造成 " << damage << " 點傷害!" << std::endl;
        break;
    }
    case SkillType::HEAL: {
        int heal = static_cast<int>(getPower());
        target->addHP(heal);
        std::cout << caster->getname() << " 使用 " << name
            << " 恢復了 " << heal << " 點生命值!" << std::endl;
        break;
    }
    case SkillType::BUFF: {
        std::cout << caster->getname() << " 使用 " << name
            << " 獲得了增益效果!" << std::endl;
        break;
    }
    }

    return true;
}