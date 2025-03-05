#include "Pet.h"
#include "Fighter.h" // �ݭn���㪺 Fighter ���w�q
#include <iostream>
#include <iomanip>
#include <cmath>
#include "def.h"
#include "Monster.h"

CPet::CPet(const CMonsterType* type, CFighter* owner)
    : CMonster(type), owner(owner), current_exp(0), state(PetState::ACTIVE), loyalty(50) {
    initializePet();
}

CPet::CPet(const CMonster* monster, CFighter* owner)
    : CMonster(*monster), owner(owner), current_exp(0), state(PetState::ACTIVE), loyalty(50) {
    initializePet();
}

CPet::~CPet() {
    // �S��M�z�u�@(�p�G�ݭn)
}

void CPet::initializePet() {
    // �]�w�d����l�ʺ�(�ϥέ쥻���Ǫ��W��)
    nickname = getname();

    // �վ��¦�ݩ�
    calculateBaseStats();

    // ��l�Ƶ��Ŭ����ݩ�
    level = 1;
    current_exp = 0;

    cout << owner->getname() << " ��o�F�s���d�� " << nickname << "!" << endl;
    showStatus();
}
void CPet::setLoyalty(int value) {
    loyalty = std::max(0, std::min(100, value));  // �T�O���۫צb 0-100 ����
    cout << nickname << " �����۫��ܧ� " << loyalty << endl;
}
void CPet::calculateBaseStats() {
    // �ھڵ��ŭp���¦�ݩ�
    int base_modifier = (getLevel() - 1) * PET_STAT_GROWTH;  // �ϥ� getLevel() �Ӥ��O�����X�� level

    // �վ�HP�MSP
    setMaxHP(getMAXHP() + base_modifier);
    setMaxSP(getMAXSP() + base_modifier);
    setHP(getMAXHP());
    setSP(getMAXSP());

    // �վ��L�ݩ�
    RoughDegree = static_cast<int>(getRough() * (1 + base_modifier * 0.1));  // �ϥ� getRough() �����e��
}
void CPet::showBattleStatus() const {
    cout << "\n=== �d���԰����A ===" << endl;
    cout << "�W��: " << nickname << endl;
    cout << "����: " << getLevel() << endl;
    cout << "HP: " << getHP() << "/" << getMAXHP() << endl;
    cout << "SP: " << getSP() << "/" << getMAXSP() << endl;
    cout << "�����O: " << getRough() << endl;
    cout << "���۫�: " << loyalty << "/100" << endl;
}
void CPet::setPetState(PetState newState) {
    state = newState;

    // �ھڪ��A�ܤƶi������B�z
    switch (newState) {
    case PetState::ACTIVE:
        cout << nickname << " �w�g�i�J�X�Ԫ��A�I" << endl;
        break;
    case PetState::IDLE:
        cout << nickname << " �i�J�𮧪��A�C" << endl;
        break;
    case PetState::TIRED:
        cout << nickname << " �w�g�h�ҡA�ݭn�𮧡C" << endl;
        break;
    }
}
void CPet::showStatus() const {
    cout << "\n=== �d�����A ===" << endl;
    cout << "�W��: " << nickname << " (�����: " << getname() << ")" << endl;
    cout << "����: " << getLevel() << endl;  // �ϥ� getLevel()

    if (getLevel() < PET_MAX_LEVEL) {  // �ϥ� getLevel()
        cout << "�g���: " << current_exp << "/" << getNextLevelExp()
            << " (" << fixed << setprecision(2) << getExpProgress() << "%)" << endl;
    }
    else {
        cout << "�g���: �w�F�̰�����" << endl;
    }

    cout << "HP: " << getHP() << "/" << getMAXHP() << endl;
    cout << "SP: " << getSP() << "/" << getMAXSP() << endl;
    cout << "�����O: " << getRough() << endl;  // �ϥ� getRough()
    cout << "���۫�: " << loyalty << "/100" << endl;

   
}

void CPet::addExp(int exp) {
    if (level >= PET_MAX_LEVEL) {
        cout << nickname << " �w�F��̰�����!" << endl;
        return;
    }

    cout << nickname << " ��o " << exp << " �I�g��ȡI" << endl;
    current_exp += exp;

    // ��ܷ�e�g��Ȫ��A
    cout << "��e�g��ȡG" << current_exp << "/" << getNextLevelExp()
        << " (" << fixed << setprecision(2) << getExpProgress() << "%)" << endl;

    while (canLevelUp()) {
        levelUp();
    }
}

bool CPet::canLevelUp() const {
    if (level >= PET_MAX_LEVEL) return false;
    return current_exp >= getNextLevelExp();
}

void CPet::levelUp() {
    if (level >= PET_MAX_LEVEL) return;

    current_exp -= getNextLevelExp();
    level++;

    // �����ݩ�
    calculateBaseStats();

    cout << "\n" << nickname << " �ɯŤF!" << endl;
    cout << "���Ŵ��ɦ� " << level << endl;
    showStatus();
}

unsigned int CPet::getNextLevelExp() const {
    if (level >= PET_MAX_LEVEL) return 0;
    return static_cast<unsigned int>(PET_BASE_EXP * pow(PET_EXP_MULTIPLIER, level - 1));
}

double CPet::getExpProgress() const {
    if (level >= PET_MAX_LEVEL) return 100.0;
    return (static_cast<double>(current_exp) / getNextLevelExp()) * 100;
}

int CPet::physicaldamage() {
    // ��¦�ˮ`�p��
    int base_damage = CMonster::physicaldamage();

    // �ھک��۫׽վ�ˮ`
    double loyalty_bonus = 1.0 + (loyalty / 100.0) * 0.5;  // �̰��i��o50%�[��

    return static_cast<int>(base_damage * loyalty_bonus);
}

int CPet::attack(CLifeEntity* target) {
    // �d�������޿�
    int damage = physicaldamage() - target->defense(target);
    if (damage > target->getHP()) {
        damage = target->getHP();
    }

    target->gethurt(damage);

    if (damage > 0) {
        cout << nickname << " �� " << target->getname()
            << " �y���F " << damage << " �I�ˮ`!" << endl;
    }
    else {
        cout << nickname << " �������Q " << target->getname()
            << " �������m�F!" << endl;
    }

    return damage;
}

int CPet::defense(CLifeEntity* target) {
    // ��¦���m��
    int base_defense = CMonster::defense(target);

    // �ھک��۫׼W�[���m�O
    double loyalty_bonus = 1.0 + (loyalty / 100.0) * 0.3;  // �̰��i��o30%�[��

    return static_cast<int>(base_defense * loyalty_bonus);
}