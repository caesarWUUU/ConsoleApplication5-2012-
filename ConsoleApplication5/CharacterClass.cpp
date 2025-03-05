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
        // �L¾�~���A�A�Ҧ��ݩʥ[����0
        attributes = { 0, 0, 0, 0 };
        description = "�|�����¾�~";
        break;
    }
}

void CharacterClass::initializeWarrior() {
    // �]�w�Ԥh���ݩʥ[��
    attributes = {
        20,     // �O�q +20
        5,      // ���O +5
        15,     // ��O +15
        5       // ���B +5
    };

    description = "�Ԥh�ժ��񨭾԰��A�㦳�j�j�����z�����O�M���m�O";

    // �]�w�Ԥh�ޯ�
    skills = {
        {"�r�O�@��", 20, 50, 3, "��ĤH�y��150%���z�ˮ`"},
        {"���m���A", 15, 0, 4, "�W�[30%���m�O�A����3�^�X"},
        {"�ԧq", 25, 0, 5, "���ɦۨ�20%�����O�A����3�^�X"}
    };
}

void CharacterClass::initializeMage() {
    // �]�w�k�v���ݩʥ[��
    attributes = {
        5,      // �O�q +5
        25,     // ���O +25
        10,     // ��O +10
        5       // ���B +5
    };

    description = "�k�v��q�]�k�A�㦳�j�j���]�k�����O�M�״I���k�O��";

    // �]�w�k�v�ޯ�
    skills = {
        {"���y�N", 25, 60, 2, "��ĤH�y���]�k�ˮ`"},
        {"�B��N", 30, 45, 3, "�y���ˮ`�ô�w�ĤH�t��"},
        {"�]�O�@��", 35, 0, 4, "���ͤ@�ӯ�l���ˮ`���@��"}
    };
}

std::string CharacterClass::getClassName() const {
    switch (classType) {
    case Warrior:
        return "�Ԥh";
    case Mage:
        return "�k�v";
    default:
        return "�L¾�~";
    }
}
