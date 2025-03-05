#ifndef MAPDATA_H
#define MAPDATA_H

#include <map>
#include <string>
#include <vector>
#include "Place.h"

#define MAX_MONSTER_PER_PLACE 5

// �a�Ϧ�m���c��
struct MapLocation {
    std::string name;
    int id;
    int row;
    int col;
};

class CMapData {
    friend class CPlace;
public:
    CMapData();
    ~CMapData();
    void show_description(int in_ID);
    std::string get_exits(int in_ID);
    int next_city(int cur_city, int next_dir);
    void generate_monsters();
    CPlace* get_place_by_id(int id);
    void display_map(int current_location);
    int get_place_id_by_name(const std::string& name); 
    void increment_kill_count();  // �s�W�G�W�[�����p��
    bool check_and_respawn_monsters();  // �s�W�G�ˬd�í��ͩǪ�
    int get_kill_count() const { return killed_monster_count; }  // �s�W�G�����e������
    void clear_and_respawn();  // �s�W�G�M���í��ͩǪ�

  
    // �s�W�G����a�I�W�٪����
    std::string get_place_name(int id) {
        CPlace* place = get_place_by_id(id);
        if (place) {
            return place->getname();
        }
        return "�����a�I";
    }

private:
    std::map<int, CPlace*> mapdata;
    int num_cities;
    int killed_monster_count;  // �s�W�G�O�������ƶq
    static const int RESPAWN_THRESHOLD = 1;  // �s�W�G�����H��
};

#endif