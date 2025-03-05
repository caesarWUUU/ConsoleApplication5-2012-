#ifndef DEF_H
#define DEF_H

// 實體類型
enum EntityType {
    efighter = 0,   // 玩家角色
    emonster,       // 怪物
    epet,          // 寵物
    eweapon,       // 武器
    efood,         // 食物
    earmor,        // 防具
    eskill         // 技能
};

// 在 def.h 中新增：
enum ItemType {
    ITEM_NONE = 0,  // 無類型
    ITEM_WEAPON = eweapon,  // 武器
    ITEM_FOOD = efood,      // 食物
    ITEM_ARMOR = earmor     // 防具
};

// 寵物狀態
enum class PetState {
    IDLE,     // 待機中
    ACTIVE,   // 出戰中
    TIRED     // 疲勞
};


// 戰鬥相關常數
#define FIGHTER_MAXHP 1000
#define FIGHTER_MAXSP 1000
#define FIGHTER_MAXLUCKY 20
#define FIGHTER_BASE_ATTACK 10
#define FIGHTER_BASE_DEFENSE 3
#define FIGHTER_MAX_LEVEL 50
#define FIGHTER_BASE_EXP 10      
#define FIGHTER_EXP_MULTIPLIER 1.5

// 寵物系統常數
#define PET_MAX_COUNT 6
#define PET_BASE_EXP 50
#define PET_EXP_MULTIPLIER 1.2
#define PET_MAX_LEVEL 50
#define PET_CATCH_BASE_RATE 30
#define PET_STAT_GROWTH 1.1
#define PET_MAX_ACTIVE 1

// 技能相關常數
#define SKILL_MAX_LEVEL 5
#define INITIAL_SKILL_POINTS 3
#define JOB_CHANGE_LEVEL 15

// 職業相關常數
#define WARRIOR_HP_BONUS 200
#define WARRIOR_ATTACK_BONUS 30
#define WARRIOR_DEFENSE_BONUS 20
#define MAGE_SP_BONUS 150
#define MAGE_INT_BONUS 40

// 技能傷害計算相關常數
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

// 快捷鍵位置
enum SkillSlot {
    SKILL_SLOT_Z = 0,
    SKILL_SLOT_X = 1,
    SKILL_SLOT_C = 2,
    SKILL_SLOT_MAX = 3
};

// 寵物指令
enum PetCommand {
    CMD_FOLLOW = 0,
    CMD_ATTACK,
    CMD_DEFEND,
    CMD_RETURN
};

// 背包相關常數
#define MAX_BAG_SIZE 50          // 背包最大容量
#define MAX_STACK_SIZE 99        // 物品堆疊上限

// 物品耐久度相關
#define MAX_DURABILITY 100       // 最大耐久度
#define DURABILITY_LOSS_RATE 1   // 每次使用耐久度損失

#endif