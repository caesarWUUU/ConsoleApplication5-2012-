#ifndef PLACE_H
#define PLACE_H

#include <string>
#include <vector>
#include "Monster.h"
using namespace std;

#define MAX_DIRECTIONS 8
enum {
    eNorth = 0,
    eEastNorth ,
    eEast ,
    eEastSouth ,
    eSouth ,
    eWestSouth  ,
    eWest ,
    eWestNorth 
};
// type of exits
// 0: north
// 1: eastnorth
// 2: east
// 3: eastsouth
// 4: south 
// 5: westsouth
// 6: west
// 7: westnorth

class CMonster;

class CPlace {
    friend class CMapData;
public:
    CPlace(string filename);
    string getname() { return name; }
    string getdescription() { return description; }
    string get_exits();
    void gen_monster_by_id(int);
    void show_mosters();
    CMonster* get_monster_by_engname(string);
    bool remove_moster_by_engname(string);
    void debug_print_exits();
    // 新增：只添加一個必要的函數來解決編譯錯誤
    vector<int> get_available_directions() {
        vector<int> available_dirs;
        for (int i = 0; i < MAX_DIRECTIONS; i++) {
            if (exits[i] > 0) {
                available_dirs.push_back(i);
            }
        }
        return available_dirs;
    }

private:
    string name;
    string description;
    int exits[8]; // store next direction place ID
    vector<CMonster*> monsters;
};

#endif