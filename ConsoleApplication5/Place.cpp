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
		cout << "讀檔失敗: " << filename << endl;
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
				output += string("，");				
			switch (i){
			case eNorth:
				output += string("往北方(north)是 ") + CGlobalInfo::map_data->mapdata [next_city]->getname ();
				break;
			case eEastNorth:
				output += string("往東北方(eastnorth)是 ") + CGlobalInfo::map_data->mapdata [next_city]->getname ();
				break;
			case eEast:
				output += string("往東方(east)是 ") + CGlobalInfo::map_data->mapdata [next_city]->getname ();
				break;
			case eEastSouth:
				output += string("往東南方(eastsouth)是 ") + CGlobalInfo::map_data->mapdata [next_city]->getname ();
				break;
			case eSouth:
				output += string("往南方(south)是 ") + CGlobalInfo::map_data->mapdata [next_city]->getname ();
				break;
			case eWestSouth:
				output += string("往西南方(westsouth)是 ") + CGlobalInfo::map_data->mapdata [next_city]->getname ();
				break;
			case eWest:
				output += string("往西方(west)是 ") + CGlobalInfo::map_data->mapdata [next_city]->getname ();
				break;
			case eWestNorth:
				output += string("往西北方(westnorth)是 ") + CGlobalInfo::map_data->mapdata [next_city]->getname ();
				break;
			default:
				cerr << "Error in calling CPlace::show_exits" << endl;		
				exit (0);
			}			
			comma = true;
		}
	}
	if (comma)
		output += string("。");	
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
		cout << name << "：沒有任何怪物" << endl;
		return;
	}

	// 第一隻怪物與地點同一行顯示
	cout << name << "：" << (*it)->get_basic_data() << " (" << (*it)->get_engname() << ")" << endl;
	++it;

	// 顯示剩餘的怪物（使用縮排）
	for (; it != monsters.end(); it++) {
		cout << "          " << (*it)->get_basic_data() << " (" << (*it)->get_engname() << ")" << endl;
	}
}
void CPlace::debug_print_exits() {
	cout << "\n位置 [" << name << "] 的出口信息：" << endl;
	for (int i = 0; i < MAX_DIRECTIONS; i++) {
		string direction;
		switch (i) {
		case eNorth: direction = "北方"; break;
		case eEastNorth: direction = "東北方"; break;
		case eEast: direction = "東方"; break;
		case eEastSouth: direction = "東南方"; break;
		case eSouth: direction = "南方"; break;
		case eWestSouth: direction = "西南方"; break;
		case eWest: direction = "西方"; break;
		case eWestNorth: direction = "西北方"; break;
		default: direction = "未知方向"; break;
		}
		cout << direction << "(" << i << ") -> ";
		if (exits[i] > 0) {
			// 使用 CGlobalInfo 獲取目標位置名稱
			CPlace* target = CGlobalInfo::map_data->get_place_by_id(exits[i]);
			if (target) {
				cout << "通往 [" << target->getname() << "] (ID: " << exits[i] << ")" << endl;
			}
			else {
				cout << "ID: " << exits[i] << " (無法獲取目標位置信息)" << endl;
			}
		}
		else {
			cout << "無出口" << endl;
		}
	}
}