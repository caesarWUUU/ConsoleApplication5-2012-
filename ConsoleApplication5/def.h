#ifndef DEF_H
#define DEF_H

// ��������
enum EntityType {
    efighter = 0,   // ���a����
    emonster,       // �Ǫ�
    epet,          // �d��
    eweapon,       // �Z��
    efood,         // ����
    earmor,        // ����
    eskill         // �ޯ�
};

// �b def.h ���s�W�G
enum ItemType {
    ITEM_NONE = 0,  // �L����
    ITEM_WEAPON = eweapon,  // �Z��
    ITEM_FOOD = efood,      // ����
    ITEM_ARMOR = earmor     // ����
};

// �d�����A
enum class PetState {
    IDLE,     // �ݾ���
    ACTIVE,   // �X�Ԥ�
    TIRED     // �h��
};


// �԰������`��
#define FIGHTER_MAXHP 1000
#define FIGHTER_MAXSP 1000
#define FIGHTER_MAXLUCKY 20
#define FIGHTER_BASE_ATTACK 10
#define FIGHTER_BASE_DEFENSE 3
#define FIGHTER_MAX_LEVEL 50
#define FIGHTER_BASE_EXP 10      
#define FIGHTER_EXP_MULTIPLIER 1.5

// �d���t�α`��
#define PET_MAX_COUNT 6
#define PET_BASE_EXP 50
#define PET_EXP_MULTIPLIER 1.2
#define PET_MAX_LEVEL 50
#define PET_CATCH_BASE_RATE 30
#define PET_STAT_GROWTH 1.1
#define PET_MAX_ACTIVE 1

// �ޯ�����`��
#define SKILL_MAX_LEVEL 5
#define INITIAL_SKILL_POINTS 3
#define JOB_CHANGE_LEVEL 15

// ¾�~�����`��
#define WARRIOR_HP_BONUS 200
#define WARRIOR_ATTACK_BONUS 30
#define WARRIOR_DEFENSE_BONUS 20
#define MAGE_SP_BONUS 150
#define MAGE_INT_BONUS 40

// �ޯ�ˮ`�p������`��
#define PHYSICAL_SKILL_BASE_POWER 50
#define MAGICAL_SKILL_BASE_POWER 60
#define HEAL_SKILL_BASE_POWER 40
#define BUFF_SKILL_BASE_POWER 20
#define SKILL_BASE_SP_COST 20
#define SKILL_ULTIMATE_SP_COST 50
#define PHYSICAL_SKILL_SCALING 1.2f
#define MAGICAL_SKILL_SCALING 1.3f
#define HEAL_SKILL_SCALING 1.2f
#define BUFF_SKILL_SCALING 1.1f

// �ֱ����m
enum SkillSlot {
    SKILL_SLOT_Z = 0,
    SKILL_SLOT_X = 1,
    SKILL_SLOT_C = 2,
    SKILL_SLOT_MAX = 3
};

// �d�����O
enum PetCommand {
    CMD_FOLLOW = 0,
    CMD_ATTACK,
    CMD_DEFEND,
    CMD_RETURN
};

// �I�]�����`��
#define MAX_BAG_SIZE 50          // �I�]�̤j�e�q
#define MAX_STACK_SIZE 99        // ���~���|�W��

// ���~�@�[�׬���
#define MAX_DURABILITY 100       // �̤j�@�[��
#define DURABILITY_LOSS_RATE 1   // �C���ϥέ@�[�׷l��

#endif