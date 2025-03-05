#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cassert>
#include <windows.h>
#include <vector>
#include "MapData.h"
#include "MonsterData.h"
#include "GlobalInfo.h"
#include "User.h"
#include "Place.h"
#include <algorithm>    

using namespace std;

CMapData::CMapData() {  // 移除 isFirstDisplay 初始化
    killed_monster_count = 0;
    ifstream fin("mapdata.txt");
    if (!fin) {
        cout << "無法開啟檔案: mapdata.txt" << endl;
        return;
    }
    fin >> num_cities;
    int id, num_exits, dirs, place_id;
    map<int, CPlace*>::iterator it;
    string map_name;
    CPlace* tmp_place;
    for (int i = 0; i < num_cities; i++) {
        fin >> id >> map_name;
        tmp_place = new CPlace(map_name);
        it = mapdata.find(id);
        if (it == mapdata.end()) {
            mapdata.insert(pair<int, CPlace*>(id, tmp_place));
            fin >> num_exits;
            for (int j = 0; j < num_exits; j++) {
                fin >> dirs >> place_id;
                tmp_place->exits[dirs] = place_id;
            }
        }
        else {
            cout << map_name << " with " << id << " has been added before" << endl;
            delete tmp_place;
            exit(0);
        }
    }
    fin.close();
}

CMapData::~CMapData() {
    for (map<int, CPlace*>::iterator it = mapdata.begin(); it != mapdata.end(); it++) {
        delete it->second;
    }
}
int CMapData::get_place_id_by_name(const string& name) {
    for (const auto& place : mapdata) {
        if (place.second->getname() == name) {
            return place.first;
        }
    }
    return -1;  // 找不到回傳-1
}
void CMapData::display_map(int current_location) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    WORD defaultColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    WORD highlightColor = FOREGROUND_RED | FOREGROUND_INTENSITY;

    vector<string> mapLines = {
        "                          沙漠 ---- 轉職所    ",
        "                            |                    ",
        "深山神廟 ---- 黑森林      郊外草原 ---- 洞窟 ---- 廢棄礦坑",
        "                 |          |                    ",
        "              技術場 ---- 城鎮中心 ---- 市集        ",
        "                            |                     ",
        "                          城堡                     "
    };

    struct MapLocation {
        string name;
        int id;
        int row;
        size_t startPos;
    };

    cout << "\n遊戲地圖：\n" << endl;

    // 對每一行分別處理
    for (int i = 0; i < mapLines.size(); i++) {
        string line = mapLines[i];
        vector<MapLocation> locations;

        // 檢查每個可能的位置
        if (i == 0) {  // 第一行
            locations.push_back({ "沙漠", 4, 0, line.find("沙漠") });
            locations.push_back({ "轉職NPC", 11, 0, line.find("轉職NPC") });
        }
        else if (i == 2) {  // 第三行
            locations.push_back({ "深山神廟", 7, 2, line.find("深山神廟") });
            locations.push_back({ "黑森林", 3, 2, line.find("黑森林") });
            locations.push_back({ "郊外草原", 9, 2, line.find("郊外草原") });
            locations.push_back({ "洞窟", 5, 2, line.find("洞窟") });
            locations.push_back({ "廢棄礦坑", 10, 2, line.find("廢棄礦坑") });
        }
        else if (i == 4) {  // 第五行
            locations.push_back({ "技術場", 8, 4, line.find("技術場") });
            locations.push_back({ "城鎮中心", 1, 4, line.find("城鎮中心") });
            locations.push_back({ "市集", 2, 4, line.find("市集") });
        }
        else if (i == 6) {  // 第七行
            locations.push_back({ "城堡", 6, 6, line.find("城堡") });
        }

        size_t currentPos = 0;
        // 處理這一行的每個位置
        for (const auto& loc : locations) {
            if (loc.startPos != string::npos) {
                // 輸出位置前的連接線
                cout << line.substr(currentPos, loc.startPos - currentPos);

                // 輸出位置名稱（根據是否是當前位置決定顏色）
                if (loc.id == current_location) {
                    SetConsoleTextAttribute(hConsole, highlightColor);
                    cout << loc.name;
                    SetConsoleTextAttribute(hConsole, defaultColor);
                }
                else {
                    cout << loc.name;
                }

                currentPos = loc.startPos + loc.name.length();
            }
        }

        // 輸出該行剩餘部分
        if (currentPos < line.length()) {
            cout << line.substr(currentPos);
        }
        cout << endl;
    }
}

void CMapData::show_description(int in_ID) {
    system("cls");
    display_map(in_ID);


    map<int, CPlace*>::iterator it = mapdata.find(in_ID);
    if (it == mapdata.end()) {
        cerr << "CMapData::show_description Error";
        exit(0);
    }
    CPlace* place = (*it).second;
    assert(place);
    cout << "\n您目前的位置：" << place->getname() << endl;
    cout << "位置描述：" << place->getdescription() << endl;
    cout << "\n可移動方向：" << endl;
    cout << get_exits(in_ID) << endl;
}

void CMapData::generate_monsters() {
    map<int, CPlace*>::iterator it;
    int num, monster_id;
    for (it = mapdata.begin(); it != mapdata.end(); it++) {
        num = rand() % MAX_MONSTER_PER_PLACE;
        for (int i = 0; i < num; i++) {
            monster_id = CGlobalInfo::monster_data->rand_get_monster_id();
            it->second->gen_monster_by_id(monster_id);
        }
    }
}

string CMapData::get_exits(int in_ID) {
    map<int, CPlace*>::iterator it = mapdata.find(in_ID);
    if (it == mapdata.end()) {
        cerr << "CMapData::get_exits Error";
        exit(0);
    }
    return (*it).second->get_exits();
}

CPlace* CMapData::get_place_by_id(int id) {
    map<int, CPlace*>::iterator it = mapdata.find(id);
    if (it == mapdata.end()) {
        cerr << id << " id is wrong";
        exit(0);
    }
    return (it->second);
}

int CMapData::next_city(int cur_city, int next_dir) {
    map<int, CPlace*>::iterator it = mapdata.find(cur_city);
    if (it == mapdata.end()) {
        cerr << "user is in a wrong city";
        exit(0);
    }
    assert(next_dir >= 0 && next_dir <= 7);
    return (it->second->exits[next_dir]);
}
void CMapData::increment_kill_count() {
    killed_monster_count++;
    cout << "\n目前已擊殺 " << killed_monster_count << " 隻怪物。"
        << "（還需要 " << RESPAWN_THRESHOLD - killed_monster_count << " 隻怪物就會重生）" << endl;

    // 檢查是否達到重生條件
    check_and_respawn_monsters();
}
bool CMapData::check_and_respawn_monsters() {
    if (killed_monster_count >= RESPAWN_THRESHOLD) {
        clear_and_respawn();
        return true;
    }
    return false;
}
void CMapData::clear_and_respawn() {
    cout << "\n========== 怪物重生系統 ==========" << endl;
    Sleep(800);
    cout << "偵測到已擊殺 " << killed_monster_count << " 隻怪物..." << endl;
    Sleep(800);
    cout << "開始進行怪物重生..." << endl;
    Sleep(1000);

    // 清除所有地點的現有怪物
    for (auto& place_pair : mapdata) {
        if (place_pair.second) {
            // 在 Place 類中清除怪物
            vector<CMonster*>& monsters = place_pair.second->monsters;
            for (auto& monster : monsters) {
                delete monster;
            }
            monsters.clear();
        }
    }

    // 重新生成怪物
    generate_monsters();

    // 重置計數器
    killed_monster_count = 0;

    cout << "怪物重生完成！新的怪物已出現在各個地區。" << endl;
    cout << "3秒後自動跳轉地圖..." << endl;
    Sleep(1000);
    cout << "2..." << endl;
    Sleep(1000);
    cout << "1..." << endl;
    Sleep(1000);
}


