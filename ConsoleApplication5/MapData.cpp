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

CMapData::CMapData() {  // ���� isFirstDisplay ��l��
    killed_monster_count = 0;
    ifstream fin("mapdata.txt");
    if (!fin) {
        cout << "�L�k�}���ɮ�: mapdata.txt" << endl;
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
    return -1;  // �䤣��^��-1
}
void CMapData::display_map(int current_location) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    WORD defaultColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    WORD highlightColor = FOREGROUND_RED | FOREGROUND_INTENSITY;

    vector<string> mapLines = {
        "                          �F�z ---- ��¾��    ",
        "                            |                    ",
        "�`�s���q ---- �´˪L      ���~��� ---- �}�] ---- �o���q�|",
        "                 |          |                    ",
        "              �޳N�� ---- ������ ---- ����        ",
        "                            |                     ",
        "                          ����                     "
    };

    struct MapLocation {
        string name;
        int id;
        int row;
        size_t startPos;
    };

    cout << "\n�C���a�ϡG\n" << endl;

    // ��C�@����O�B�z
    for (int i = 0; i < mapLines.size(); i++) {
        string line = mapLines[i];
        vector<MapLocation> locations;

        // �ˬd�C�ӥi�઺��m
        if (i == 0) {  // �Ĥ@��
            locations.push_back({ "�F�z", 4, 0, line.find("�F�z") });
            locations.push_back({ "��¾NPC", 11, 0, line.find("��¾NPC") });
        }
        else if (i == 2) {  // �ĤT��
            locations.push_back({ "�`�s���q", 7, 2, line.find("�`�s���q") });
            locations.push_back({ "�´˪L", 3, 2, line.find("�´˪L") });
            locations.push_back({ "���~���", 9, 2, line.find("���~���") });
            locations.push_back({ "�}�]", 5, 2, line.find("�}�]") });
            locations.push_back({ "�o���q�|", 10, 2, line.find("�o���q�|") });
        }
        else if (i == 4) {  // �Ĥ���
            locations.push_back({ "�޳N��", 8, 4, line.find("�޳N��") });
            locations.push_back({ "������", 1, 4, line.find("������") });
            locations.push_back({ "����", 2, 4, line.find("����") });
        }
        else if (i == 6) {  // �ĤC��
            locations.push_back({ "����", 6, 6, line.find("����") });
        }

        size_t currentPos = 0;
        // �B�z�o�@�檺�C�Ӧ�m
        for (const auto& loc : locations) {
            if (loc.startPos != string::npos) {
                // ��X��m�e���s���u
                cout << line.substr(currentPos, loc.startPos - currentPos);

                // ��X��m�W�١]�ھڬO�_�O��e��m�M�w�C��^
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

        // ��X�Ӧ�Ѿl����
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
    cout << "\n�z�ثe����m�G" << place->getname() << endl;
    cout << "��m�y�z�G" << place->getdescription() << endl;
    cout << "\n�i���ʤ�V�G" << endl;
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
    cout << "\n�ثe�w���� " << killed_monster_count << " ���Ǫ��C"
        << "�]�ٻݭn " << RESPAWN_THRESHOLD - killed_monster_count << " ���Ǫ��N�|���͡^" << endl;

    // �ˬd�O�_�F�쭫�ͱ���
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
    cout << "\n========== �Ǫ����ͨt�� ==========" << endl;
    Sleep(800);
    cout << "������w���� " << killed_monster_count << " ���Ǫ�..." << endl;
    Sleep(800);
    cout << "�}�l�i��Ǫ�����..." << endl;
    Sleep(1000);

    // �M���Ҧ��a�I���{���Ǫ�
    for (auto& place_pair : mapdata) {
        if (place_pair.second) {
            // �b Place �����M���Ǫ�
            vector<CMonster*>& monsters = place_pair.second->monsters;
            for (auto& monster : monsters) {
                delete monster;
            }
            monsters.clear();
        }
    }

    // ���s�ͦ��Ǫ�
    generate_monsters();

    // ���m�p�ƾ�
    killed_monster_count = 0;

    cout << "�Ǫ����ͧ����I�s���Ǫ��w�X�{�b�U�Ӧa�ϡC" << endl;
    cout << "3���۰ʸ���a��..." << endl;
    Sleep(1000);
    cout << "2..." << endl;
    Sleep(1000);
    cout << "1..." << endl;
    Sleep(1000);
}


