#ifndef MONSTERDATA_H
#define MONSTERDATA_H

#include <string>
#include <vector>

using namespace std;

class CMonsterType {
    friend class CMonsterData;
    friend class CMonster;
public:
    CMonsterType(int in_id, string in_name, string in_eng, int in_hp, int in_sp, int in_rough, int in_level);
private:
    int id;
    string name;
    string prefix_eng_name;
    int max_hp;
    int max_sp;
    int max_rough;
    int level;  // 新增等級屬性
};

class CMonsterData {

public:
    CMonsterData();
    ~CMonsterData();
    int rand_get_monster_id();
    CMonsterType* get_monstertype_by_id(int);

private:
    vector<CMonsterType*> monsterdata;
};

#endif