#include <iostream>
#include <fstream>
#include "MonsterData.h"

CMonsterType::CMonsterType(int in_id, string in_name, string in_eng, int in_hp, int in_sp, int in_rough, int in_level) {
    id = in_id;
    name = in_name;
    prefix_eng_name = in_eng;
    max_hp = in_hp;
    max_sp = in_sp;
    max_rough = in_rough;
    level = in_level;  // 儲存等級
}

CMonsterData::CMonsterData() {
    ifstream fin("monsterdata.txt");
    if (!fin) {
        cout << "無法開啟檔案: monsterdata.txt" << endl;
        exit(0);
    }
    int tmp_id;
    string tmp_name;
    string tmp_eng;
    int tmp_hp, tmp_sp, tmp_rough, tmp_level;
    CMonsterType* tmp_type;

    // 修改讀取格式，加入等級
    while (fin >> tmp_id >> tmp_name >> tmp_eng >> tmp_hp >> tmp_sp >> tmp_rough >> tmp_level) {
        tmp_type = new CMonsterType(tmp_id, tmp_name, tmp_eng, tmp_hp, tmp_sp, tmp_rough, tmp_level);
        monsterdata.push_back(tmp_type);
    }

    fin.close();
}

CMonsterData::~CMonsterData() {
    for (vector<CMonsterType*>::iterator it = monsterdata.begin(); it != monsterdata.end(); it++) {
        delete (*it);
    }
}

int CMonsterData::rand_get_monster_id() {
    int idx = rand() % monsterdata.size();
    return monsterdata[idx]->id;
}

CMonsterType* CMonsterData::get_monstertype_by_id(int id) {
    for (vector<CMonsterType*>::iterator it = monsterdata.begin(); it != monsterdata.end(); it++) {
        if ((*it)->id == id) {
            return (*it);
        }
    }
    return NULL;
}