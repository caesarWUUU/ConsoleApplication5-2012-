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
        std::cout << name << " �w�F��̰�����!" << std::endl;
        return false;
    }
    level++;
    std::cout << name << " �ɯŦ� " << level << " ��!" << std::endl;
    return true;
}

float CSkill::getPower() const {
    return base_power * (1.0f + (level - 1) * damage_scaling);
}

bool CSkill::use(CLifeEntity* caster, CLifeEntity* target) {
    if (!caster || !target) return false;

    // �ˬdSP�O�_����
    if (caster->getSP() < sp_cost) {
        std::cout << "SP����,�L�k�ϥΧޯ�!" << std::endl;
        return false;
    }

    // ����SP
    caster->delSP(sp_cost);

    // �ھڧޯ��������椣�P�ĪG
    switch (type) {
    case SkillType::PHYSICAL:
    case SkillType::MAGICAL: {
        int damage = static_cast<int>(getPower());
        target->gethurt(damage);
        std::cout << caster->getname() << " �ϥ� " << name
            << " �� " << target->getname()
            << " �y�� " << damage << " �I�ˮ`!" << std::endl;
        break;
    }
    case SkillType::HEAL: {
        int heal = static_cast<int>(getPower());
        target->addHP(heal);
        std::cout << caster->getname() << " �ϥ� " << name
            << " ��_�F " << heal << " �I�ͩR��!" << std::endl;
        break;
    }
    case SkillType::BUFF: {
        std::cout << caster->getname() << " �ϥ� " << name
            << " ��o�F�W�q�ĪG!" << std::endl;
        break;
    }
    }

    return true;
}