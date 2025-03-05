#include <iostream>
#include <string>
#include "Monster.h"
#include "MonsterData.h"
#include "def.h"

using namespace std;

int CMonster::counter_for_monster_id = 0;

CMonster::CMonster(int initHP, int initSP, int initrough,
    string init_name, string init_eng_name)
    : CLifeEntity(initHP, initSP, init_name),
    RoughDegree(initrough),
    eng_name(init_eng_name)
{
    // �[�J�Ѽ��ˬd
    if (initHP <= 0 || initSP <= 0 || initrough < 0) {
        throw invalid_argument("Invalid monster parameters");
    }

    counter_for_monster_id++;
    level = 1;

    // �T�O��l�Ƨ���
    cout << "Created monster: " << init_name
        << " (HP=" << initHP
        << ", SP=" << initSP
        << ", Rough=" << initrough
        << ", Level=" << level << ")" << endl;
}

CMonster::CMonster(const CMonsterType* type)
    : CLifeEntity(type->max_hp,
        type->max_sp,
        type->name),
    RoughDegree(type->max_rough) {
    counter_for_monster_id++;
    level = type->level;
    eng_name = type->prefix_eng_name + to_string(counter_for_monster_id);

    // �ק�غc�T���榡�A�ϥξ�Ƶ���
    cout << "�ͦ��F " << type->name << " Lv." << level
        << " (" << eng_name << ")" << endl;
}

int CMonster::getRough() const {
    return RoughDegree;
}

int CMonster::physicaldamage() {
    return (rand() % getSP() + getRough());
}

int CMonster::attack(CLifeEntity* l) {
    // �[�J���@�ˬd
    if (!l || this->isdead()) {
        return 0;
    }

    int damage = physicaldamage() - l->defense(l);
    if (damage > l->getHP()) {
        damage = l->getHP();
    }

    l->gethurt(damage);

    if (damage > 0) {
        cout << this->getname() << " �����R���A�y�� " << l->getname()
            << " " << damage << " �I�ˮ`" << endl;
    }
    else {
        cout << this->getname() << " �����R���A���O " << l->getname()
            << " �������m�A�]���{�פF����" << endl;
    }
    return damage;
}

int CMonster::defense(CLifeEntity* l) {
    return getRough();  // �ϥΩǪ��� RoughDegree �@�����m��
}

int CMonster::isA() const {
    return emonster;
}

string CMonster::get_basic_data() const {
    // �u��ܩǪ��W�٩M��Ƶ���
    string output = this->getname() + " Lv." + to_string((int)level);
    return output;
}
