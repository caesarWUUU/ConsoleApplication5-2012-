#ifndef FOOD_H
#define FOOD_H

#include "Item.h"
#include <string>

class CLifeEntity;

class CFood : public CItem {
public:
    CFood(string inname = "", int initSize = 0, int initWeight = 0,
        int initID = 0, int init_hp = 0);

    int gethpbonus() const;  // ²K¥[ const
    virtual void beUsed(CLifeEntity*) override;
    virtual void UnUsed(CLifeEntity*) override;
    virtual int isA() const override;  // ²K¥[ const

private:
    int bonus_hp;
};

#endif