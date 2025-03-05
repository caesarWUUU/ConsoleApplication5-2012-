#include <iostream>
#include <Windows.h>
#include "GlobalInfo.h"
#include "LifeEntity.h"



void CLifeEntity::setInitSPHP (int initHP, int initSP){
	maxSP = SP = initSP;
	maxHP = HP = initHP;	
}

void CLifeEntity::setHP (int inHP){
	HP = inHP > maxHP ? maxHP : inHP;	
}

void CLifeEntity::addHP (int inHP){
	HP = (inHP+HP) > maxHP ? maxHP : (HP+inHP);	
}

void CLifeEntity::addSP (int inSP){
	SP = inSP+SP;	
}

void CLifeEntity::delSP (int inSP){
	SP -= inSP;	
}
bool CLifeEntity::isdead() const {
	return (HP <= 0);
}

int CLifeEntity::getHP() const {
	return HP;
}

int CLifeEntity::getMAXHP() const {
	return maxHP;
}

int CLifeEntity::getSP() const {
	return SP;
}

int CLifeEntity::getMAXSP() const {
	return maxSP;
}

string CLifeEntity::getname() const {
	return Name;
}

int  CLifeEntity::gethurt (int hurt){
	if (hurt > HP)
		setHP(0);
	else if (hurt > 0)
		setHP(HP - hurt);
	return HP;
}

void CLifeEntity::setname (string inname){
	Name = inname;	
}

bool CLifeEntity::kill(CLifeEntity* enemy) {
    if (!enemy) {  // �[�J�ˬd
        return false;
    }

    int f_damage = 0, s_damage = 0;
    CLifeEntity* first, * second;
    int whofirst;

    while (!this->isdead() && !enemy->isdead()) {
        whofirst = rand() % 2;
        if (whofirst == 0) {
            cout << "�����o�����A�v���X�����" << endl;
            first = enemy;
            second = this;
        }
        else {
            cout << "�A��o�����A�v���X�����" << endl;
            first = this;
            second = enemy;
        }

        s_damage = first->attack(second);

        // �[�J���A�ˬd
        if (!first || !second) {
            cout << "���~�G�԰����L��" << endl;
            return false;
        }

        fightstatus(enemy, this);
        if (second->isdead()) {
            Sleep(1000);
            break;
        }

        f_damage = second->attack(first);

        // �ˬd�Ĥ@�ӧ����̬O�_�w�g�L��
        if (first->isdead()) {
            fightstatus(enemy, this);
            Sleep(1000);
            break;
        }

        fightstatus(enemy, this);
        Sleep(2000);
    }

    return !this->isdead();  // �אּ��M�����^�ǭ�
}

void CLifeEntity::fightstatus(CLifeEntity* f, CLifeEntity* s) {
    if (!f || !s) {
        cout << "���~�G�L�Ī��԰����" << endl;
        return;
    }
	cout << endl << f->getname() << endl;
	bloodbarshow(string("HP"), f->getMAXHP(), f->getHP());
	bloodbarshow(string("SP"), f->getMAXSP(), f->getSP());

	cout << s->getname() << endl;
	bloodbarshow(string("HP"), s->getMAXHP(), s->getHP());
	bloodbarshow(string("SP"), s->getMAXSP(), s->getSP());
	cout << endl;
}

void CLifeEntity::bloodbarshow(string title, int maxvalue, int value) {
	cout << title << "     |";
	float slotlen = (float)maxvalue / MAXBLOODBARLEN;
	int numfill = (int)ceil(value / slotlen);

	// ��ܦ�����]�O��
	for (int i = 0; i < numfill; i++) {
		cout << "#";
	}
	int numempty = (int)floor((maxvalue - value) / slotlen);
	for (int i = 0; i < numempty; i++) {
		cout << " ";
	}
	cout << "|  " << value << "/" << maxvalue;
	cout << endl;
}

