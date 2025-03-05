#ifndef CMDPARSER_H
#define CMDPARSER_H

#include <string>
#include <vector>
#include <map>
#include "Monster.h"
#include "Fighter.h"
using namespace std;

// �R�O�B�z��ƪ��e�V�ŧi
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

// �w�q�C���Ҧ��C�|
enum GameMode {
    MODE_NORMAL,    // �@��Ҧ�
    MODE_BATTLE     // �԰��Ҧ�
};

// CmdParser ���O�w�q
class CCmdParser {
public:
    CCmdParser();
    void splitstring(const string& s, vector<string>& v, const string& c);
    int query();
    int process_wasd(const string& direction);

    // �԰��������
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

// �԰�������ƫŧi
void fightstatus(CMonster* m, CFighter* f);
int battle_attack(vector<string>& tokens);
int battle_skill(vector<string>& tokens);
int battle_ultimate(vector<string>& tokens);

// �C�������B�z���
void handle_game_over();
void handle_battle_end(CFighter* player, CMonster* enemy);

#endif