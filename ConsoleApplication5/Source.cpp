#include <iostream>
#include <cstdlib>
#include <string>
#include <iterator>
#include <sstream>
#include <ctime>
#include <windows.h>
#include "Fighter.h"
#include "Monster.h"
#include "food.h"
#include "ItemData.h"
#include "MapData.h"
#include "GlobalInfo.h"
#include "CmdParser.h"
#include "User.h"
#include "MonsterData.h"
#include "AccountManager.h"
#include <limits>
#include "SaveDirectoryManager.h"
using namespace std;
using std::numeric_limits;

#define MAXBARLEN 40
#define LOCATION_CENTER 1     // （城鎮中心）
#define LOCATION_MARKET 2     // （市集）
#define LOCATION_FOREST 3     // （黑森林）
#define LOCATION_DESERT 4     // （沙漠）
#define LOCATION_CAVE 5       // （洞窟）
#define LOCATION_CASTLE 6     // （城堡）
#define LOCATION_TEMPLE 7     // （深山神廟）
#define LOCATION_TECH 8       // （技術場）
#define LOCATION_GRASSLAND 9  // （郊外草原）
#define LOCATION_MINE 10      // （廢棄礦坑）
#define LOCATION_JOB 11       // （轉職所）

void askforbaglist(CFighter* f) {
    int num = f->showAllBagItems();
    if (num == 0)
        return;
    cout << f->getname() << " 要使用背包物品嗎?\n0 --> No, Others --> Yes, 使用之物品編號: ";
    int selection;
    cin >> selection;
    if (selection) {
        if (!f->useBagItems(selection))
            cout << "無此選項存在" << endl;
    }
}

void bloodbarshow(int maxhp, int hp) {
    cout << "HP     |";
    float hpslotlen = (float)maxhp / MAXBARLEN;
    int numhp = (int)ceil(hp / hpslotlen);
    for (int i = 0; i < numhp; i++) {
        cout << "#";
    }
    numhp = (int)floor((maxhp - hp) / hpslotlen);
    for (int i = 0; i < numhp; i++) {
        cout << " ";
    }
    cout << "|  " << hp << "/" << maxhp;
    cout << endl;
}

void fightstatus(CMonster* m, CFighter* f) {
    cout << endl << f->getname() << endl;
    bloodbarshow(f->getMAXHP(), f->getHP());
    cout << "SP     |";
    float spslotlen = (float)f->getMAXSP() / MAXBARLEN;
    int numsp = (int)ceil(f->getSP() / spslotlen);
    for (int i = 0; i < numsp; i++) {
        cout << "#";
    }
    int numempty = (int)floor((f->getMAXSP() - f->getSP()) / spslotlen);
    for (int i = 0; i < numempty; i++) {
        cout << " ";
    }
    cout << "|  " << f->getSP() << "/" << f->getMAXSP() << endl;

    cout << m->getname() << endl;
    bloodbarshow(m->getMAXHP(), m->getHP());
    cout << "SP     |";
    spslotlen = (float)m->getMAXSP() / MAXBARLEN;
    numsp = (int)ceil(m->getSP() / spslotlen);
    for (int i = 0; i < numsp; i++) {
        cout << "#";
    }
    numempty = (int)floor((m->getMAXSP() - m->getSP()) / spslotlen);
    for (int i = 0; i < numempty; i++) {
        cout << " ";
    }
    cout << "|  " << m->getSP() << "/" << m->getMAXSP() << endl;
    cout << endl;
}

void fightshow(CMonster* m, CFighter* f) {
    fightstatus(m, f);
    cout << "你還剩下 " << f->getHP() << "/" << f->getMAXHP() << " 血量" << endl;
    Sleep(1000);
}

CMonster* MonsterSelection(CFighter* f) {
    int selection;
    askforbaglist(f);
    cout << "請選擇怪物等級: 1. 簡單 2. 普通 3. 困難 4. 死亡 " << endl;
    cin >> selection;
    int m_MaxHP, m_MaxSP, m_MaxRough;
    switch (selection) {
    case 1:
        m_MaxHP = f->getMAXHP() / 2;
        m_MaxSP = f->getMAXSP() / 2;
        m_MaxRough = (int)(f->getMAXHP() * 0.1);
        break;
    case 2:
        m_MaxHP = f->getMAXHP();
        m_MaxSP = f->getMAXSP();
        m_MaxRough = (int)(f->getMAXHP() * 0.2);
        break;
    case 3:
        m_MaxHP = (int)(f->getMAXHP() * 1.5);
        m_MaxSP = (int)(f->getMAXSP() * 1.2);
        m_MaxRough = (int)(f->getMAXHP() * 0.2);
        break;
    default:
        m_MaxHP = f->getMAXHP() * 2;
        m_MaxSP = f->getMAXSP() * 2;
        m_MaxRough = (int)(f->getMAXHP() * 0.5);
    }
    CMonster* m = new CMonster(m_MaxHP, m_MaxSP, m_MaxRough);
    if (!m) {  // 檢查是否成功創建
        throw runtime_error("Failed to create monster");
    }
    system("CLS");
    return m;
}

void startfight(CMonster* m, CFighter* f) {
    if (!m || !f) {
        throw invalid_argument("Invalid fight participants");
    }
    int f_damage = 0, s_damage = 0;
    CLifeEntity* first, * second;
    int whofirst;
    while (!m->isdead() && !f->isdead()) {
        whofirst = rand() % 2;
        if (whofirst == 0) {
            cout << "怪物搶得先機，先出手攻人" << endl;
            first = (CLifeEntity*)m;
            second = (CLifeEntity*)f;
        }
        else {
            cout << "你搶得先機，先出手攻人" << endl;
            first = (CLifeEntity*)f;
            second = (CLifeEntity*)m;
        }

        s_damage = first->attack(second);
        if (second->isdead()) {
            whofirst == 0 ? fightshow((CMonster*)first, (CFighter*)second) : fightshow((CMonster*)second, (CFighter*)first);
            break;
        }

        f_damage = second->attack(first);
        whofirst == 0 ? fightshow((CMonster*)first, (CFighter*)second) : fightshow((CMonster*)second, (CFighter*)first);
    }
    if (m->isdead() && !f->isdead()) {
        cout << "怪物已死，從怪物身上掉落道具" << endl;
        CItemData* id = CGlobalInfo::itm_data;
        f->captureItem(id->getRand());
    }
}

void Initialize() {
    srand((unsigned int)time(NULL));

    // 初始化所有修補對象
    CGlobalInfo::map_data = new CMapData();
    CGlobalInfo::monster_data = new CMonsterData();
    CGlobalInfo::user = new CUser();
    CGlobalInfo::itm_data = new CItemData();
    CGlobalInfo::parser = new CCmdParser();

    // 初始化存檔系統
    SaveDirectoryManager::getInstance().initialize("saves");

    // 建立必要的目錄
    if (!CreateDirectoryA("saves", NULL)) {
        if (GetLastError() != ERROR_ALREADY_EXISTS) {
            cout << "無法創建存檔目錄" << endl;
        }
    }
    
    if (!CreateDirectoryA("saves\\backups", NULL)) {
        if (GetLastError() != ERROR_ALREADY_EXISTS) {
            cout << "無法創建備份目錄" << endl;
        }
    }

    // 初始化帳號系統
    AccountManager::getInstance().initializeSystem();

    // 初始化道具數據和生成怪物
    if (CGlobalInfo::itm_data) {
        CGlobalInfo::itm_data->Initialize();
    }
    if (CGlobalInfo::map_data) {
        CGlobalInfo::map_data->generate_monsters();
    }
}



// 為確保一致性，其他使用到 Windows API 的函數也應該使用 A 版本
bool SaveDirectoryManager::hasSaveFile(const string& username) const {
    DWORD attr = GetFileAttributesA(getSavePath(username).c_str());
    return (attr != INVALID_FILE_ATTRIBUTES && 
            !(attr & FILE_ATTRIBUTE_DIRECTORY));
}
void showLoginMenu() {
    cout << "=== 遊戲登入系統 ===" << endl;
    cout << "1. 登入" << endl;
    cout << "2. 創建帳號" << endl;
    cout << "3. 重設密碼" << endl;
    cout << "4. 離開" << endl;
    cout << "請選擇: ";
}

void showCharacterMenu() {
    cout << "=== 角色選單 ===" << endl;
    cout << "1. 創建新角色" << endl;
    cout << "2. 選擇角色" << endl;
    cout << "3. 刪除角色" << endl;
    cout << "4. 登出帳號" << endl;
    cout << "請選擇功能 (1-4): ";
}
int main() {
    try {
        // 初始化遊戲系統
        Initialize();
        AccountManager& accountManager = AccountManager::getInstance();

        bool running = true;
        while (running) {
            if (!accountManager.isLoggedIn()) {
                system("cls");  // 清除畫面
                showLoginMenu();

                int choice;
                string username, password;
                cin >> choice;
                cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

                switch (choice) {
                case 1: {  // 登入
                    cout << "請輸入帳號: ";
                    getline(cin, username);
                    cout << "請輸入密碼: ";
                    getline(cin, password);

                    if (accountManager.login(username, password)) {
                        cout << "登入成功！歡迎 " << username << " 回來！" << endl;
                        Sleep(1000);  // 暫停1秒
                    }
                    else {
                        cout << "登入失敗：帳號或密碼錯誤" << endl;
                        Sleep(1500);
                    }
                    break;
                }

                case 2: {  // 創建帳號
                    cout << "請輸入新帳號: ";
                    getline(cin, username);
                    cout << "請輸入密碼: ";
                    getline(cin, password);

                    if (username.empty() || password.empty()) {
                        cout << "帳號或密碼不能為空！" << endl;
                    }
                    else if (accountManager.createAccount(username, password)) {
                        cout << "帳號創建成功！請重新登入" << endl;
                    }
                    else {
                        cout << "帳號創建失敗：帳號已存在" << endl;
                    }
                    Sleep(1500);
                    break;
                }

                case 3: {  // 重設密碼
                    string adminAccount, adminPassword, targetUser, newPassword, confirmPassword;
                    cout << "=== 忘記密碼重設系統 ===" << endl;

                    // 1. 輸入管理者帳號
                    cout << "請輸入管理員帳號: ";
                    getline(cin, adminAccount);

                    // 確認是否為管理員帳號
                    if (adminAccount != "admin") {
                        cout << "此帳號不是管理員帳號！" << endl;
                        Sleep(1500);
                        break;
                    }

                    // 2. 輸入管理者密碼
                    cout << "請輸入管理員密碼: ";
                    getline(cin, adminPassword);

                    // 驗證管理員密碼
                    if (!accountManager.verifyAdmin(adminPassword)) {
                        cout << "管理員密碼錯誤！" << endl;
                        Sleep(1500);
                        break;
                    }

                    // 3. 輸入要重設的帳號
                    cout << "請輸入要重設密碼的帳號: ";
                    getline(cin, targetUser);

                    // 確認帳號存在
                    if (!accountManager.isAccountExist(targetUser)) {
                        cout << "此帳號不存在！" << endl;
                        Sleep(1500);
                        break;
                    }

                    // 4. 輸入新密碼
                    cout << "請輸入新密碼: ";
                    getline(cin, newPassword);

                    // 確認新密碼不為空
                    if (newPassword.empty()) {
                        cout << "新密碼不能為空！" << endl;
                        Sleep(1500);
                        break;
                    }

                    // 5. 確認新密碼
                    cout << "請再次輸入新密碼: ";
                    getline(cin, confirmPassword);

                    // 確認兩次密碼相同
                    if (newPassword != confirmPassword) {
                        cout << "兩次密碼不相同！" << endl;
                        Sleep(1500);
                        break;
                    }

                    // 6. 執行密碼重設
                    if (accountManager.resetPassword(adminPassword, targetUser, newPassword)) {
                        cout << "密碼重設成功！" << endl;
                    }
                    else {
                        cout << "密碼重設失敗！" << endl;
                    }
                    Sleep(1500);
                    break;
                }

                case 4:  // 離開遊戲
                    cout << "感謝遊玩，再見！" << endl;
                    running = false;
                    break;

                default:
                    cout << "無效的選擇，請重新輸入" << endl;
                    Sleep(1000);
                    break;
                }
            }
            else {
                // 已登入狀態，顯示角色選單
                system("cls");
                showCharacterMenu();

                int choice;
                cin >> choice;
                cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

                switch (choice) {
                case 1: {  // 創建新角色
                    string characterName;
                    cout << "請輸入角色名稱: ";
                    getline(cin, characterName);

                    if (accountManager.createCharacter(characterName)) {
                        cout << "角色創建成功！" << endl;
                    }
                    else {
                        cout << "角色創建失敗：已達角色數上限或名稱重複" << endl;
                    }
                    Sleep(1500);
                    break;
                }

                case 2: {  // 選擇角色並進入遊戲
                    vector<string> characters = accountManager.getCurrentUserCharacters();
                    if (characters.empty()) {
                        cout << "你還沒有建立任何角色！" << endl;
                        Sleep(1500);
                        break;
                    }

                    cout << "你的角色列表：" << endl;
                    for (size_t i = 0; i < characters.size(); ++i) {
                        cout << (i + 1) << ". " << characters[i] << endl;
                    }

                    cout << "請選擇角色 (1-" << characters.size() << "): ";
                    int charChoice;
                    cin >> charChoice;

                    if (charChoice > 0 && charChoice <= static_cast<int>(characters.size())) {
                        CFighter* fighter = accountManager.selectCharacter(characters[charChoice - 1]);
                        if (fighter) {
                            // 設置當前角色
                            if (CGlobalInfo::user) {
                                CGlobalInfo::user->set_user((CLifeEntity*)fighter);
                            }

                            // 顯示當前位置資訊
                            if (CGlobalInfo::user && CGlobalInfo::map_data) {
                                system("cls");  // 清除之前的內容
                                int cur_city = CGlobalInfo::user->get_current_city();
                                CGlobalInfo::map_data->show_description(cur_city);
                            }

                            // 進入遊戲主循環
                            if (CGlobalInfo::parser) {
                                bool stayInGame = true;
                                while (stayInGame) {
                                    int result = CGlobalInfo::parser->query();
                                    if (result < 0) {
                                        // 退出遊戲
                                        running = false;
                                        stayInGame = false;
                                    }
                                    else if (result == 1) {
                                        // 登出，返回登入畫面
                                        stayInGame = false;
                                        // 不需要設置 running = false，因為要回到登入畫面
                                    }
                                    // result == 0 表示正常指令執行，繼續遊戲循環
                                }
                            }
                            else {
                                cout << "錯誤：無法初始化遊戲系統" << endl;
                                Sleep(1500);
                            }
                        }
                        else {
                            cout << "錯誤：無法載入選擇的角色" << endl;
                            Sleep(1500);
                        }
                    }
                    else {
                        cout << "無效的選擇！" << endl;
                        Sleep(1500);
                    }
                    break;
                }

                case 3: {  // 刪除角色
                    vector<string> characters = accountManager.getCurrentUserCharacters();
                    if (characters.empty()) {
                        cout << "您尚未創建任何角色！" << endl;
                        Sleep(1500);
                        break;
                    }

                    cout << "您的角色列表：" << endl;
                    for (size_t i = 0; i < characters.size(); ++i) {
                        cout << (i + 1) << ". " << characters[i] << endl;
                    }

                    cout << "請選擇要刪除的角色 (1-" << characters.size() << "): ";
                    int charChoice;
                    cin >> charChoice;

                    if (charChoice > 0 && charChoice <= static_cast<int>(characters.size())) {
                        if (accountManager.deleteCharacter(characters[charChoice - 1])) {
                            cout << "角色刪除成功！" << endl;
                        }
                        else {
                            cout << "角色刪除失敗！" << endl;
                        }
                    }
                    Sleep(1500);
                    break;
                }

                case 4:  // 登出
                    accountManager.logout();
                    cout << "已登出帳號" << endl;
                    Sleep(1000);
                    break;

                default:
                    cout << "無效的選擇，請重新輸入" << endl;
                    Sleep(1000);
                    break;
                }
            }
        }

        // 清理資源
        delete CGlobalInfo::map_data;
        delete CGlobalInfo::monster_data;
        delete CGlobalInfo::user;
        delete CGlobalInfo::itm_data;
        delete CGlobalInfo::parser;

        CGlobalInfo::map_data = nullptr;
        CGlobalInfo::monster_data = nullptr;
        CGlobalInfo::user = nullptr;
        CGlobalInfo::itm_data = nullptr;
        CGlobalInfo::parser = nullptr;

    }
    catch (const exception& e) {
        cerr << "發生錯誤: " << e.what() << endl;
    }
    catch (...) {
        cerr << "發生未知錯誤" << endl;
    }

    return 0;
}