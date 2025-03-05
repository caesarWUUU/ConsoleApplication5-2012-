#ifndef PET_H
#define PET_H

#include "Monster.h"
#include "def.h"    // 包含 def.h 以使用枚舉定義
#include <string>

// 前向宣告
class CFighter;  // 改用前向宣告而不是包含整個 Fighter.h
enum class PetState;  // 將 PetState 枚舉移到 def.h 中

class CPet : public CMonster {
public:
    // 建構函式和解構函式
    CPet(const CMonsterType* type, CFighter* owner);
    CPet(const CMonster* monster, CFighter* owner);
    virtual ~CPet();

    // 等級與經驗值系統
    void addExp(int exp);
    bool canLevelUp() const;
    void levelUp();
    unsigned int getNextLevelExp() const;
    double getExpProgress() const;

    // 戰鬥相關方法覆寫
    virtual int physicaldamage() override;
    virtual int attack(CLifeEntity* target) override;
    virtual int defense(CLifeEntity* target) override;
    virtual int isA() const override { return epet; }

    // 狀態與資訊
    void setPetState(PetState state);
    PetState getPetState() const { return state; }
    void setLoyalty(int value);
    int getLoyalty() const { return loyalty; }
    CFighter* getOwner() const { return owner; }
    const std::string& getNickname() const { return nickname; }
    void setNickname(const std::string& name) { nickname = name; }

    // 寵物狀態顯示
    void showStatus() const;
    void showBattleStatus() const;

private:
    unsigned int current_exp;     // 當前經驗值
    PetState state;              // 寵物狀態
    int loyalty;                 // 忠誠度(影響戰鬥表現)
    CFighter* owner;             // 寵物主人
    std::string nickname;        // 寵物暱稱

    // 初始化相關
    void initializePet();
    void calculateBaseStats();
};

#endif