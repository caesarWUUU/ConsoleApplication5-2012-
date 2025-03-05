#ifndef CMDPARSER_H
#define CMDPARSER_H

#include <string>
#include <vector>
#include <map>
#include "Monster.h"
#include "Fighter.h"
using namespace std;

// 命令處理函數的前向宣告
int function_exit(vector<string>& tokens);
int function_next_direction(vector<string>& tokens);
int function_list(vector<string>& tokens);
int function_kill(vector<string>& tokens);
int function_check_bag(vector<string>& tokens);
int function_status(vector<string>& tokens);
int function_shop_weapon(vector<string>& tokens);
int function_shop_armor(vector<string>& tokens);
int function_shop_food(vector<string>& tokens);
int function_move(vector<string>& tokens);
int function_job_change(vector<string>& tokens);
int function_skill_management(vector<string>& tokens);
int function_logout(vector<string>& tokens);

// 定義遊戲模式列舉
enum GameMode {
    MODE_NORMAL,    // 一般模式
    MODE_BATTLE     // 戰鬥模式
};

// CmdParser 類別定義
class CCmdParser {
public:
    CCmdParser();
    void splitstring(const string& s, vector<string>& v, const string& c);
    int query();
    int process_wasd(const string& direction);

    // 戰鬥相關函數
    void enter_battle_mode(CMonster* enemy);
    void exit_battle_mode();
    void show_battle_help();
    int process_battle_command(const string& cmd);
    CMonster* get_current_enemy() { return current_enemy; }
    bool executePetAttack(CMonster* enemy, CFighter* player);

private:
    map<string, void(*)> mappingfunc;
    map<string, void(*)> battle_commands;
    GameMode current_mode;
    CMonster* current_enemy;
    void show_help();
    
};

// 戰鬥相關函數宣告
void fightstatus(CMonster* m, CFighter* f);
int battle_attack(vector<string>& tokens);
int battle_skill(vector<string>& tokens);
int battle_ultimate(vector<string>& tokens);

// 遊戲結束處理函數
void handle_game_over();
void handle_battle_end(CFighter* player, CMonster* enemy);

#endif