#ifndef MAPDATA_H
#define MAPDATA_H

#include <map>
#include <string>
#include <vector>
#include "Place.h"

#define MAX_MONSTER_PER_PLACE 5

// 地圖位置結構體
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
    void increment_kill_count();  // 新增：增加擊殺計數
    bool check_and_respawn_monsters();  // 新增：檢查並重生怪物
    int get_kill_count() const { return killed_monster_count; }  // 新增：獲取當前擊殺數
    void clear_and_respawn();  // 新增：清除並重生怪物

  
    // 新增：獲取地點名稱的函數
    std::string get_place_name(int id) {
        CPlace* place = get_place_by_id(id);
        if (place) {
            return place->getname();
        }
        return "未知地點";
    }

private:
    std::map<int, CPlace*> mapdata;
    int num_cities;
    int killed_monster_count;  // 新增：記錄擊殺數量
    static const int RESPAWN_THRESHOLD = 1;  // 新增：重生閾值
};

#endif