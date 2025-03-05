#include <iostream>
#include <fstream>
#include <cassert>
#include "Place.h"
#include "MapData.h"
#include "MonsterData.h"
#include "GlobalInfo.h"

using namespace std;

CPlace::CPlace(string filename) {
	ifstream fin(filename);
	if (!fin) {
		cout << "Ū�ɥ���: " << filename << endl;
		return;
	}
	fin >> name >> description;
	fin.close();
	for (int i = 0; i < MAX_DIRECTIONS; i++) {
		exits[i] = 0;
	}
}

string CPlace::get_exits (){
	string output;
	int next_city;
	bool comma = false;
	for (int i = 0; i < MAX_DIRECTIONS; i++){
		next_city = exits[i];
		if (next_city > 0){
			if (comma)
				output += string("�A");				
			switch (i){
			case eNorth:
				output += string("���_��(north)�O ") + CGlobalInfo::map_data->mapdata [next_city]->getname ();
				break;
			case eEastNorth:
				output += string("���F�_��(eastnorth)�O ") + CGlobalInfo::map_data->mapdata [next_city]->getname ();
				break;
			case eEast:
				output += string("���F��(east)�O ") + CGlobalInfo::map_data->mapdata [next_city]->getname ();
				break;
			case eEastSouth:
				output += string("���F�n��(eastsouth)�O ") + CGlobalInfo::map_data->mapdata [next_city]->getname ();
				break;
			case eSouth:
				output += string("���n��(south)�O ") + CGlobalInfo::map_data->mapdata [next_city]->getname ();
				break;
			case eWestSouth:
				output += string("����n��(westsouth)�O ") + CGlobalInfo::map_data->mapdata [next_city]->getname ();
				break;
			case eWest:
				output += string("�����(west)�O ") + CGlobalInfo::map_data->mapdata [next_city]->getname ();
				break;
			case eWestNorth:
				output += string("����_��(westnorth)�O ") + CGlobalInfo::map_data->mapdata [next_city]->getname ();
				break;
			default:
				cerr << "Error in calling CPlace::show_exits" << endl;		
				exit (0);
			}			
			comma = true;
		}
	}
	if (comma)
		output += string("�C");	
	return output;
}

void CPlace::gen_monster_by_id (int id){
	CMonsterType *type = CGlobalInfo::monster_data->get_monstertype_by_id (id);
	assert (type);
	CMonster *tmp = new CMonster (type);
	monsters.push_back (tmp);
}

CMonster *CPlace::get_monster_by_engname (string engname){
	for (vector <CMonster *>::iterator it = monsters.begin (); it != monsters.end (); it++){
		if ((*it)->get_engname () == engname)
			return (*it);
	}
	return NULL;
}

bool CPlace::remove_moster_by_engname (string engname){	
	for (vector <CMonster *>::iterator it = monsters.begin (); it != monsters.end (); it++){
		if ((*it)->get_engname () == engname){			
			delete (*it);
			monsters.erase (it);			
			return true;
		}
	}	
	return false;
}



void CPlace::show_mosters() {
	vector<CMonster*>::iterator it = monsters.begin();
	if (it == monsters.end()) {
		cout << name << "�G�S������Ǫ�" << endl;
		return;
	}

	// �Ĥ@���Ǫ��P�a�I�P�@�����
	cout << name << "�G" << (*it)->get_basic_data() << " (" << (*it)->get_engname() << ")" << endl;
	++it;

	// ��ܳѾl���Ǫ��]�ϥ��Y�ơ^
	for (; it != monsters.end(); it++) {
		cout << "          " << (*it)->get_basic_data() << " (" << (*it)->get_engname() << ")" << endl;
	}
}
void CPlace::debug_print_exits() {
	cout << "\n��m [" << name << "] ���X�f�H���G" << endl;
	for (int i = 0; i < MAX_DIRECTIONS; i++) {
		string direction;
		switch (i) {
		case eNorth: direction = "�_��"; break;
		case eEastNorth: direction = "�F�_��"; break;
		case eEast: direction = "�F��"; break;
		case eEastSouth: direction = "�F�n��"; break;
		case eSouth: direction = "�n��"; break;
		case eWestSouth: direction = "��n��"; break;
		case eWest: direction = "���"; break;
		case eWestNorth: direction = "��_��"; break;
		default: direction = "������V"; break;
		}
		cout << direction << "(" << i << ") -> ";
		if (exits[i] > 0) {
			// �ϥ� CGlobalInfo ����ؼЦ�m�W��
			CPlace* target = CGlobalInfo::map_data->get_place_by_id(exits[i]);
			if (target) {
				cout << "�q�� [" << target->getname() << "] (ID: " << exits[i] << ")" << endl;
			}
			else {
				cout << "ID: " << exits[i] << " (�L�k����ؼЦ�m�H��)" << endl;
			}
		}
		else {
			cout << "�L�X�f" << endl;
		}
	}
}