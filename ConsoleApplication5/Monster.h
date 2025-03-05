#ifndef MONSTER_H
#define MONSTER_H

#include "LifeEntity.h"
class CMonsterType;

class CMonster : public CLifeEntity {
public:
    CMonster(int initHP = 0, int initSP = 0, int initrough = 0,
        string init_name = string("怪物"), string init_eng_name = string("monster"));
    CMonster(const CMonsterType* type);
    int getRough() const;
    virtual int physicaldamage();
    virtual int attack(CLifeEntity*);
    virtual int defense(CLifeEntity*);
    virtual int isA() const override;
    string get_basic_data() const;
    string get_engname() const { return eng_name; }
    virtual int getLucky() const override { return RoughDegree; }
    int getLevel() const { return level; }

protected:  // 改為 protected，讓子類可以訪問
    int RoughDegree;
    int level;

private:
    static int counter_for_monster_id;
    string eng_name;
};

#endif