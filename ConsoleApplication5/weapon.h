#ifndef WEAPON_H
#define WEAPON_H

#include "Item.h"

class CWeapon : public CItem {
public:
    CWeapon(string inname = "", int initSize = 0, int initWeight = 0,
        int initID = 0, int init_attack = 0);
    int getattackbonus() const;
    virtual void beUsed(CLifeEntity*) override;
    virtual void UnUsed(CLifeEntity*) override;
    virtual int isA() const override;
private:
    int bonus_attack;
};

#endif