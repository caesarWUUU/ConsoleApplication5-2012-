#ifndef LIFEENTITY_H
#define LIFEENTITY_H

#include <string>
#include "weapon.h"

#define MAXBLOODBARLEN 40
using namespace std;

class CLifeEntity {
protected:  // 改為 protected 讓衍生類可以訪問
    int maxSP, maxHP;
    int SP, HP;
    string Name;
    CWeapon* weapon;

public:
    CLifeEntity() : maxSP(0), maxHP(0), SP(0), HP(0), Name(""), weapon(nullptr) {}
    CLifeEntity(int initHP, int initSP, string initname = "")
        : maxSP(initSP), maxHP(initHP), SP(initSP), HP(initHP), Name(initname), weapon(nullptr) {
    }

    void setInitSPHP(int initHP = 0, int initSP = 0);
    bool isdead() const;
    void setHP(int);
    void addHP(int);
    int getHP() const;
    int getMAXHP() const;
    int getSP() const;
    int getMAXSP() const;
    void addSP(int);
    void delSP(int);
    int gethurt(int hurt);
    string getname() const;
    void setname(string);
    CWeapon* getWeapon() const { return weapon; }
    void setWeapon(CWeapon* in_weapon) { weapon = in_weapon; }
    bool kill(CLifeEntity*);
    virtual int attack(CLifeEntity*) = 0;
    virtual int defense(CLifeEntity*) = 0;
    virtual int isA() const = 0;
    virtual int getLucky() const = 0;
    void setMaxHP(int maxhp) { maxHP = maxhp; }
    void setMaxSP(int maxsp) { maxSP = maxsp; }
    void setSP(int sp) { SP = sp > maxSP ? maxSP : sp; }

private:
    void fightstatus(CLifeEntity* f, CLifeEntity* s);
    void bloodbarshow(string title, int maxvalue, int value);
};

#endif