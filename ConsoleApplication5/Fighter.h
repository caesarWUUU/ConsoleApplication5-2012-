#ifndef FIGHTER_H
#define FIGHTER_H

#include "LifeEntity.h"
#include "armor.h"
#include "Skill.h"
#include <array>
#include <vector>
#include <iostream>
#include "Pet.h"
using namespace std;


#define FIGHTER_MAXHP 1000
#define FIGHTER_MAXSP 1000
#define FIGHTER_MAXLUCKY 20
#define FIGHTER_BASE_ATTACK 10
#define FIGHTER_BASE_DEFENSE 3
#define FIGHTER_MAX_LEVEL 50
#define FIGHTER_BASE_EXP 10      
#define FIGHTER_EXP_MULTIPLIER 1.5  

class CFood;
class CBag;
class CItem;
class CWeapon;
enum class JobClass {
    NOVICE,     // 初心者
    WARRIOR,    // 戰士
    MAGE        // 法師
};
class CFighter : public CLifeEntity {
public:

    CFighter(int initHP = 0, int initSP = 0, int initLucky = 0, int in_city = 1);
    ~CFighter();

   
    virtual int getAttack() const;    
    virtual int getDefense() const;   
    int getBaseAttack() const { return base_attack; }
    int getBaseDefense() const { return base_defense; }
    void setBaseAttack(int attack) { base_attack = attack; }
    void setBaseDefense(int defense) { base_defense = defense; }

    
    int getLevel() const { return level; }
    unsigned int getNextLevelExp() const;  
    void addExp(int exp);  
    bool canLevelUp() const;     
    void levelUp();             
    double getExpProgress() const;  
    unsigned int getCurrentExp() const {
        return current_exp;
    }

    int physicaldamage();
    virtual int attack(CLifeEntity*);
    virtual int defense(CLifeEntity*);
    virtual int isA() const override;

    
    void captureItem(CItem*, bool replace = false);
    int showAllBagItems();
    bool useBagItems(int no);

    
    int goto_next_city(int next_dir);
    int get_current_city() const;
    void set_current_city(int new_city);

    
    int getMoney() const { return money; }
    void addMoney(int amount);
    bool spendMoney(int amount);

    
    void showStatus() const;
    void showWeapon() const;
    void showLevelAndExp() const;  

    
    void setArmor(CArmor* armor);
    void removeArmor(ArmorType type);
    CArmor* getArmor(ArmorType type) const;
    int getTotalDefenseBonus() const;
    void showEquippedArmor() const;

    int getStrength() const { return strength; }    
    int getIntelligence() const { return intelligence; }  
    int getTenacity() const { return tenacity; }    
    virtual int getLucky() const override { return Lucky; }  

    void setStrength(int value) { strength = value; }
    void setIntelligence(int value) { intelligence = value; }
    void setTenacity(int value) { tenacity = value; }
    void setCurrentExp(unsigned int exp) {
        current_exp = exp;
        cout << "設置經驗值為：" << exp << endl;  // 加入除錯訊息
    }

    void setLevel(int newLevel) { level = newLevel; }
    void setMoney(int newMoney) { money = newMoney; }

    CBag* getBag() const { return bag; }
    
    int getAvailablePoints() const { return available_points; }
    void addAvailablePoints(int points) { available_points += points; }
    bool capturePet(CMonster* monster);     // 捕獲寵物
    bool switchPet(int pet_index);          // 切換出戰寵物
    void showPets() const;                  // 顯示所有寵物
    const std::vector<CPet*>& getPets() const; // 獲取寵物列表
    CPet* getActivePet() const;             // 獲取當前出戰寵物
    void addStrength(int points) {
        strength += points;
        base_attack += points * 2;  
    }

    void addIntelligence(int points) {
        intelligence += points;
        setMaxSP(getMAXSP() + points * 5);  
        setSP(getMAXSP());  
    }

    void addTenacity(int points) {
        tenacity += points;
        setMaxHP(getMAXHP() + points * 10);  
        setHP(getMAXHP());  
        base_defense += points;  
    }

    void addLucky(int points) {
        Lucky += points;  
    }
   
    int getArmorSetBonus() const; 
    bool hasFullSet() const;       
    ArmorSet getCurrentSet() const; 

    // 新增職業相關方法
    JobClass getJobClass() const { return job_class; }
    bool changeJob(JobClass newJob);  // 轉職
    void addSkillPoint(int points) { skill_points += points; }
    int getSkillPoints() const { return skill_points; }

    // 技能相關方法
    bool learnSkill(CSkill* skill);
    bool upgradeSkill(int skillIndex);
    void showSkills() const;
    const std::vector<CSkill*>& getSkills() const { return skills; }

    // 戰鬥相關方法更新
    bool useSkill(int skillIndex, CLifeEntity* target);
    CSkill* getEquippedSkill(int slot) const {
        if (slot >= 0 && slot < 3) {
            return equipped_skills[slot];
        }
        return nullptr;
    }

    void CFighter::setSkillHotkey(int slot, int skillIndex) {
        // Z鍵(slot 0)不允許更換
        if (slot == 0) {
            cout << "基本攻擊(Z鍵)不能被更換！" << endl;
            return;
        }

        // 檢查其他快捷鍵位置是否有效
        if (slot < 1 || slot > 2 || skillIndex < 0 || skillIndex >= skills.size()) {
            cout << "無效的快捷鍵位置或技能選擇！" << endl;
            return;
        }

        equipped_skills[slot] = skills[skillIndex];
        cout << "已將 " << skills[skillIndex]->getName() << " 設置到快捷鍵 "
            << (slot == 1 ? "X" : "C") << endl;
    }

private:
    int cur_city;         
    int Lucky;           
    CBag* bag;            
    std::array<CArmor*, 4> equipped_armors; 
    int money;           
    int base_attack;      
    int base_defense;    
    int strength;      
    int intelligence;  
    int tenacity;      
    int level;           
    unsigned int current_exp;  
    int available_points;
    JobClass job_class;
    int skill_points;
    std::vector<CSkill*> skills;
    std::array<CSkill*, 3> equipped_skills;  // Z、X、C 快捷鍵對應的技能
    std::vector<CPet*> pets;           // 寵物列表
    CPet* active_pet;
};

#endif  