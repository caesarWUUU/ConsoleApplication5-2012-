#ifndef ITEM_H
#define ITEM_H

#include <string>

class CLifeEntity;
using namespace std;

class CItem {
public:
    CItem(string inname = "", int initSize = 0, int initWeight = 0, int initID = 0);
    virtual ~CItem() {}

    void setSize(int);
    void setWeight(int);
    void setName(string);

    int getSize() const;
    int getWeight() const;
    string getName() const;
    int getID() const { return ID; }

    virtual void beUsed(CLifeEntity*) = 0;
    virtual void UnUsed(CLifeEntity*) = 0;
    virtual int isA() const = 0;

protected:
    int ID;
    int Size, Weight;
    string Name;
};

#endif