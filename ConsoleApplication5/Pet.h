#ifndef PET_H
#define PET_H

#include "Monster.h"
#include "def.h"    // �]�t def.h �H�ϥΪT�|�w�q
#include <string>

// �e�V�ŧi
class CFighter;  // ��Ϋe�V�ŧi�Ӥ��O�]�t��� Fighter.h
enum class PetState;  // �N PetState �T�|���� def.h ��

class CPet : public CMonster {
public:
    // �غc�禡�M�Ѻc�禡
    CPet(const CMonsterType* type, CFighter* owner);
    CPet(const CMonster* monster, CFighter* owner);
    virtual ~CPet();

    // ���ŻP�g��Ȩt��
    void addExp(int exp);
    bool canLevelUp() const;
    void levelUp();
    unsigned int getNextLevelExp() const;
    double getExpProgress() const;

    // �԰�������k�мg
    virtual int physicaldamage() override;
    virtual int attack(CLifeEntity* target) override;
    virtual int defense(CLifeEntity* target) override;
    virtual int isA() const override { return epet; }

    // ���A�P��T
    void setPetState(PetState state);
    PetState getPetState() const { return state; }
    void setLoyalty(int value);
    int getLoyalty() const { return loyalty; }
    CFighter* getOwner() const { return owner; }
    const std::string& getNickname() const { return nickname; }
    void setNickname(const std::string& name) { nickname = name; }

    // �d�����A���
    void showStatus() const;
    void showBattleStatus() const;

private:
    unsigned int current_exp;     // ��e�g���
    PetState state;              // �d�����A
    int loyalty;                 // ���۫�(�v�T�԰���{)
    CFighter* owner;             // �d���D�H
    std::string nickname;        // �d���ʺ�

    // ��l�Ƭ���
    void initializePet();
    void calculateBaseStats();
};

#endif