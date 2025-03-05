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
#define LOCATION_CENTER 1     // �]�����ߡ^
#define LOCATION_MARKET 2     // �]�����^
#define LOCATION_FOREST 3     // �]�´˪L�^
#define LOCATION_DESERT 4     // �]�F�z�^
#define LOCATION_CAVE 5       // �]�}�]�^
#define LOCATION_CASTLE 6     // �]�����^
#define LOCATION_TEMPLE 7     // �]�`�s���q�^
#define LOCATION_TECH 8       // �]�޳N���^
#define LOCATION_GRASSLAND 9  // �]���~���^
#define LOCATION_MINE 10      // �]�o���q�|�^
#define LOCATION_JOB 11       // �]��¾�ҡ^

void askforbaglist(CFighter* f) {
    int num = f->showAllBagItems();
    if (num == 0)
        return;
    cout << f->getname() << " �n�ϥέI�]���~��?\n0 --> No, Others --> Yes, �ϥΤ����~�s��: ";
    int selection;
    cin >> selection;
    if (selection) {
        if (!f->useBagItems(selection))
            cout << "�L���ﶵ�s�b" << endl;
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
    cout << "�A�ٳѤU " << f->getHP() << "/" << f->getMAXHP() << " ��q" << endl;
    Sleep(1000);
}

CMonster* MonsterSelection(CFighter* f) {
    int selection;
    askforbaglist(f);
    cout << "�п�ܩǪ�����: 1. ²�� 2. ���q 3. �x�� 4. ���` " << endl;
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
    if (!m) {  // �ˬd�O�_���\�Ы�
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
            cout << "�Ǫ��m�o�����A���X���H" << endl;
            first = (CLifeEntity*)m;
            second = (CLifeEntity*)f;
        }
        else {
            cout << "�A�m�o�����A���X���H" << endl;
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
        cout << "�Ǫ��w���A�q�Ǫ����W�����D��" << endl;
        CItemData* id = CGlobalInfo::itm_data;
        f->captureItem(id->getRand());
    }
}

void Initialize() {
    srand((unsigned int)time(NULL));

    // ��l�ƩҦ��׸ɹ�H
    CGlobalInfo::map_data = new CMapData();
    CGlobalInfo::monster_data = new CMonsterData();
    CGlobalInfo::user = new CUser();
    CGlobalInfo::itm_data = new CItemData();
    CGlobalInfo::parser = new CCmdParser();

    // ��l�Ʀs�ɨt��
    SaveDirectoryManager::getInstance().initialize("saves");

    // �إߥ��n���ؿ�
    if (!CreateDirectoryA("saves", NULL)) {
        if (GetLastError() != ERROR_ALREADY_EXISTS) {
            cout << "�L�k�Ыئs�ɥؿ�" << endl;
        }
    }
    
    if (!CreateDirectoryA("saves\\backups", NULL)) {
        if (GetLastError() != ERROR_ALREADY_EXISTS) {
            cout << "�L�k�Ыسƥ��ؿ�" << endl;
        }
    }

    // ��l�Ʊb���t��
    AccountManager::getInstance().initializeSystem();

    // ��l�ƹD��ƾکM�ͦ��Ǫ�
    if (CGlobalInfo::itm_data) {
        CGlobalInfo::itm_data->Initialize();
    }
    if (CGlobalInfo::map_data) {
        CGlobalInfo::map_data->generate_monsters();
    }
}



// ���T�O�@�P�ʡA��L�ϥΨ� Windows API ����Ƥ]���Өϥ� A ����
bool SaveDirectoryManager::hasSaveFile(const string& username) const {
    DWORD attr = GetFileAttributesA(getSavePath(username).c_str());
    return (attr != INVALID_FILE_ATTRIBUTES && 
            !(attr & FILE_ATTRIBUTE_DIRECTORY));
}
void showLoginMenu() {
    cout << "=== �C���n�J�t�� ===" << endl;
    cout << "1. �n�J" << endl;
    cout << "2. �Ыرb��" << endl;
    cout << "3. ���]�K�X" << endl;
    cout << "4. ���}" << endl;
    cout << "�п��: ";
}

void showCharacterMenu() {
    cout << "=== ������ ===" << endl;
    cout << "1. �Ыطs����" << endl;
    cout << "2. ��ܨ���" << endl;
    cout << "3. �R������" << endl;
    cout << "4. �n�X�b��" << endl;
    cout << "�п�ܥ\�� (1-4): ";
}
int main() {
    try {
        // ��l�ƹC���t��
        Initialize();
        AccountManager& accountManager = AccountManager::getInstance();

        bool running = true;
        while (running) {
            if (!accountManager.isLoggedIn()) {
                system("cls");  // �M���e��
                showLoginMenu();

                int choice;
                string username, password;
                cin >> choice;
                cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

                switch (choice) {
                case 1: {  // �n�J
                    cout << "�п�J�b��: ";
                    getline(cin, username);
                    cout << "�п�J�K�X: ";
                    getline(cin, password);

                    if (accountManager.login(username, password)) {
                        cout << "�n�J���\�I�w�� " << username << " �^�ӡI" << endl;
                        Sleep(1000);  // �Ȱ�1��
                    }
                    else {
                        cout << "�n�J���ѡG�b���αK�X���~" << endl;
                        Sleep(1500);
                    }
                    break;
                }

                case 2: {  // �Ыرb��
                    cout << "�п�J�s�b��: ";
                    getline(cin, username);
                    cout << "�п�J�K�X: ";
                    getline(cin, password);

                    if (username.empty() || password.empty()) {
                        cout << "�b���αK�X���ର�šI" << endl;
                    }
                    else if (accountManager.createAccount(username, password)) {
                        cout << "�b���Ыئ��\�I�Э��s�n�J" << endl;
                    }
                    else {
                        cout << "�b���Ыإ��ѡG�b���w�s�b" << endl;
                    }
                    Sleep(1500);
                    break;
                }

                case 3: {  // ���]�K�X
                    string adminAccount, adminPassword, targetUser, newPassword, confirmPassword;
                    cout << "=== �ѰO�K�X���]�t�� ===" << endl;

                    // 1. ��J�޲z�̱b��
                    cout << "�п�J�޲z���b��: ";
                    getline(cin, adminAccount);

                    // �T�{�O�_���޲z���b��
                    if (adminAccount != "admin") {
                        cout << "���b�����O�޲z���b���I" << endl;
                        Sleep(1500);
                        break;
                    }

                    // 2. ��J�޲z�̱K�X
                    cout << "�п�J�޲z���K�X: ";
                    getline(cin, adminPassword);

                    // ���Һ޲z���K�X
                    if (!accountManager.verifyAdmin(adminPassword)) {
                        cout << "�޲z���K�X���~�I" << endl;
                        Sleep(1500);
                        break;
                    }

                    // 3. ��J�n���]���b��
                    cout << "�п�J�n���]�K�X���b��: ";
                    getline(cin, targetUser);

                    // �T�{�b���s�b
                    if (!accountManager.isAccountExist(targetUser)) {
                        cout << "���b�����s�b�I" << endl;
                        Sleep(1500);
                        break;
                    }

                    // 4. ��J�s�K�X
                    cout << "�п�J�s�K�X: ";
                    getline(cin, newPassword);

                    // �T�{�s�K�X������
                    if (newPassword.empty()) {
                        cout << "�s�K�X���ର�šI" << endl;
                        Sleep(1500);
                        break;
                    }

                    // 5. �T�{�s�K�X
                    cout << "�ЦA����J�s�K�X: ";
                    getline(cin, confirmPassword);

                    // �T�{�⦸�K�X�ۦP
                    if (newPassword != confirmPassword) {
                        cout << "�⦸�K�X���ۦP�I" << endl;
                        Sleep(1500);
                        break;
                    }

                    // 6. ����K�X���]
                    if (accountManager.resetPassword(adminPassword, targetUser, newPassword)) {
                        cout << "�K�X���]���\�I" << endl;
                    }
                    else {
                        cout << "�K�X���]���ѡI" << endl;
                    }
                    Sleep(1500);
                    break;
                }

                case 4:  // ���}�C��
                    cout << "�P�¹C���A�A���I" << endl;
                    running = false;
                    break;

                default:
                    cout << "�L�Ī���ܡA�Э��s��J" << endl;
                    Sleep(1000);
                    break;
                }
            }
            else {
                // �w�n�J���A�A��ܨ�����
                system("cls");
                showCharacterMenu();

                int choice;
                cin >> choice;
                cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

                switch (choice) {
                case 1: {  // �Ыطs����
                    string characterName;
                    cout << "�п�J����W��: ";
                    getline(cin, characterName);

                    if (accountManager.createCharacter(characterName)) {
                        cout << "����Ыئ��\�I" << endl;
                    }
                    else {
                        cout << "����Ыإ��ѡG�w�F����ƤW���ΦW�٭���" << endl;
                    }
                    Sleep(1500);
                    break;
                }

                case 2: {  // ��ܨ���öi�J�C��
                    vector<string> characters = accountManager.getCurrentUserCharacters();
                    if (characters.empty()) {
                        cout << "�A�٨S���إߥ��󨤦�I" << endl;
                        Sleep(1500);
                        break;
                    }

                    cout << "�A������C��G" << endl;
                    for (size_t i = 0; i < characters.size(); ++i) {
                        cout << (i + 1) << ". " << characters[i] << endl;
                    }

                    cout << "�п�ܨ��� (1-" << characters.size() << "): ";
                    int charChoice;
                    cin >> charChoice;

                    if (charChoice > 0 && charChoice <= static_cast<int>(characters.size())) {
                        CFighter* fighter = accountManager.selectCharacter(characters[charChoice - 1]);
                        if (fighter) {
                            // �]�m��e����
                            if (CGlobalInfo::user) {
                                CGlobalInfo::user->set_user((CLifeEntity*)fighter);
                            }

                            // ��ܷ�e��m��T
                            if (CGlobalInfo::user && CGlobalInfo::map_data) {
                                system("cls");  // �M�����e�����e
                                int cur_city = CGlobalInfo::user->get_current_city();
                                CGlobalInfo::map_data->show_description(cur_city);
                            }

                            // �i�J�C���D�`��
                            if (CGlobalInfo::parser) {
                                bool stayInGame = true;
                                while (stayInGame) {
                                    int result = CGlobalInfo::parser->query();
                                    if (result < 0) {
                                        // �h�X�C��
                                        running = false;
                                        stayInGame = false;
                                    }
                                    else if (result == 1) {
                                        // �n�X�A��^�n�J�e��
                                        stayInGame = false;
                                        // ���ݭn�]�m running = false�A�]���n�^��n�J�e��
                                    }
                                    // result == 0 ��ܥ��`���O����A�~��C���`��
                                }
                            }
                            else {
                                cout << "���~�G�L�k��l�ƹC���t��" << endl;
                                Sleep(1500);
                            }
                        }
                        else {
                            cout << "���~�G�L�k���J��ܪ�����" << endl;
                            Sleep(1500);
                        }
                    }
                    else {
                        cout << "�L�Ī���ܡI" << endl;
                        Sleep(1500);
                    }
                    break;
                }

                case 3: {  // �R������
                    vector<string> characters = accountManager.getCurrentUserCharacters();
                    if (characters.empty()) {
                        cout << "�z�|���Ыإ��󨤦�I" << endl;
                        Sleep(1500);
                        break;
                    }

                    cout << "�z������C��G" << endl;
                    for (size_t i = 0; i < characters.size(); ++i) {
                        cout << (i + 1) << ". " << characters[i] << endl;
                    }

                    cout << "�п�ܭn�R�������� (1-" << characters.size() << "): ";
                    int charChoice;
                    cin >> charChoice;

                    if (charChoice > 0 && charChoice <= static_cast<int>(characters.size())) {
                        if (accountManager.deleteCharacter(characters[charChoice - 1])) {
                            cout << "����R�����\�I" << endl;
                        }
                        else {
                            cout << "����R�����ѡI" << endl;
                        }
                    }
                    Sleep(1500);
                    break;
                }

                case 4:  // �n�X
                    accountManager.logout();
                    cout << "�w�n�X�b��" << endl;
                    Sleep(1000);
                    break;

                default:
                    cout << "�L�Ī���ܡA�Э��s��J" << endl;
                    Sleep(1000);
                    break;
                }
            }
        }

        // �M�z�귽
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
        cerr << "�o�Ϳ��~: " << e.what() << endl;
    }
    catch (...) {
        cerr << "�o�ͥ������~" << endl;
    }

    return 0;
}