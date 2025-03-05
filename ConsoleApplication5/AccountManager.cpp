#include "UserAccount.h"
#include "PasswordHasher.h"
#include "AccountManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <random>
#include <algorithm>
#include <windows.h>
#include <wincrypt.h>
#pragma comment(lib, "Crypt32.lib")
#include "SaveDirectoryManager.h"
#include "SaveSystem.h"
#include "GlobalInfo.h" 
#include "User.h"
#include "Food.h"




using namespace std;

const string AccountManager::ACCOUNTS_FILE = "accounts.txt";
const string AccountManager::CHARACTERS_FILE = "characters.txt";
const string AccountManager::ADMIN_ACCOUNT = "admin";
const string AccountManager::ADMIN_PASSWORD_HASH = "your_admin_hash_here"; // �ݭn�ͦ�
const string AccountManager::ADMIN_SALT = "your_admin_salt_here"; // �ݭn�ͦ�

AccountManager AccountManager::instance;

AccountManager& AccountManager::getInstance() {
    return instance;
}

bool AccountManager::createAccount(const string& username, const string& password) {
    if (username.empty() || password.empty()) {
        return false;
    }
    // �ˬd�b���O�_�w�s�b
    if (accounts.find(username) != accounts.end()) {
        return false;
    }

    // �ͦ�salt�M�K�Xhash
    string salt = PasswordHasher::generateSalt();
    string passwordHash = PasswordHasher::hashPassword(password, salt);

    // �Ыطs�b��
    accounts.emplace(username, UserAccount(username, passwordHash, salt));

    // �x�s���ɮ�
    return saveAccounts();
}



CFighter* AccountManager::selectCharacter(const string& characterName) {
    // �ˬd�Τ�O�_�w�n�J
    if (!currentUser) {
        cout << "���~�G�Τ�|���n�J" << endl;
        return nullptr;
    }

    // �˯�����
    CFighter* fighter = currentUser->getCharacter(characterName);
    if (!fighter) {
        cout << "���~�G�䤣�쨤�� " << characterName << endl;
        return nullptr;
    }

    // ���o�s�ɸ��|
    auto& saveManager = SaveDirectoryManager::getInstance();
    string savePath = saveManager.getCharacterSavePath(getCurrentUsername(), characterName);
    cout << "���bŪ���s�ɡG" << savePath << endl;

    // �ˬd�s�ɤ��O�_�s�b
    if (GetFileAttributesA(savePath.c_str()) != INVALID_FILE_ATTRIBUTES) {
        cout << "���s�ɤ��" << endl;

        // �b���J�e�M�z�I�]
        CBag* bag = fighter->getBag();
        if (bag) {
            vector<CBagEntry*> currentEntries;
            bag->getAllItems(currentEntries);
            for (auto entry : currentEntries) {
                bag->item_delete(entry);
            }
        }

        // �O�����J�e�����A�]�Ω󰣿��^
        cout << "���J�e���A�G" << endl;
        cout << "���šG" << fighter->getLevel() << endl;
        cout << "�g��ȡG" << fighter->getCurrentExp() << endl;
        cout << "�����G" << fighter->getMoney() << endl;
        cout << "HP�G" << fighter->getHP() << "/" << fighter->getMAXHP() << endl;
        cout << "SP�G" << fighter->getSP() << "/" << fighter->getMAXSP() << endl;

        // ���ո��J�s�ɼƾ�
        if (SaveSystem::loadCharacterData(fighter, savePath)) {
            cout << "����ƾڸ��J���\�I" << endl;

            // �O�����J�᪺���A�]�Ω󰣿��^
            cout << "���J�᪬�A�G" << endl;
            cout << "���šG" << fighter->getLevel() << endl;
            cout << "�g��ȡG" << fighter->getCurrentExp() << endl;
            cout << "�����G" << fighter->getMoney() << endl;
            cout << "HP�G" << fighter->getHP() << "/" << fighter->getMAXHP() << endl;
            cout << "SP�G" << fighter->getSP() << "/" << fighter->getMAXSP() << endl;

            // �Ыئ۰ʳƥ�
            string backupPath = saveManager.getCharacterBackupPath(
                getCurrentUsername(),
                characterName
            );
            if (CopyFileA(savePath.c_str(), backupPath.c_str(), FALSE)) {
                cout << "�w�Ыئs�ɳƥ�" << endl;
            }
            else {
                cout << "ĵ�i�G�ƥ��Ыإ��ѡA���~�N�X�G" << GetLastError() << endl;
            }
        }
        else {
            cout << "ĵ�i�G�L�k���J�s�ɼƾڡA�N�ϥΪ�l���A" << endl;

            // �i�H�b�o�̲K�[���m���⪬�A���޿�
            fighter->setLevel(15);  // �]�m��l����
            fighter->setCurrentExp(0);
            fighter->setMoney(100000);
            // ... ��L��l�Ƴ]�m ...
        }
    }
    else {
        cout << "�����s�ɤ��A�N�ϥΪ�l���A" << endl;
        // �Ĥ@���ϥγo�Ө���A���ݭn�S�O�B�z
    }

    // �T�O���⪬�A���X�z��
    if (fighter->getHP() <= 0) fighter->setHP(fighter->getMAXHP());
    if (fighter->getSP() <= 0) fighter->setSP(fighter->getMAXSP());

    Sleep(1000);  // ���Τ�ɶ��d�ݪ��A
    return fighter;
}
vector<string> AccountManager::getCurrentUserCharacters() const {
    if (!currentUser) {
        return vector<string>();
    }

    return currentUser->getCharacterNames();
}

bool AccountManager::initializeSystem() {
    if (!CreateDirectoryA("saves", NULL)) {
        if (GetLastError() != ERROR_ALREADY_EXISTS) {
            cout << "�L�k�Ыئs�ɥؿ�!" << endl;
            return false;
        }
    }

    if (!CreateDirectoryA("saves\\backups", NULL)) {
        if (GetLastError() != ERROR_ALREADY_EXISTS) {
            cout << "�L�k�Ыسƥ��ؿ�!" << endl;
            // �~�����A�]���o���O�P�R���~
        }
    }
    bool needInit = false;

    // �ˬd accounts.txt �O�_�s�b
    ifstream accountCheck(ACCOUNTS_FILE);
    if (!accountCheck) {
        needInit = true;
    }
    accountCheck.close();

    if (needInit) {
        // �Ы� accounts.txt �M�޲z���b��
        ofstream accountFile(ACCOUNTS_FILE);
        if (accountFile) {
            // �Ыغ޲z�̱b��
            string adminPassword = "admin123";
            string adminSalt = PasswordHasher::generateSalt();
            string adminHash = PasswordHasher::hashPassword(adminPassword, adminSalt);

            accountFile << "admin:" << adminHash << ":" << adminSalt << endl;
            accountFile.close();
        }
    }

    // �ˬd characters.txt �O�_�s�b
    ifstream charCheck(CHARACTERS_FILE);
    if (!charCheck) {
        ofstream charFile(CHARACTERS_FILE);
        charFile.close();
    }
    charCheck.close();

    // ���J�Ҧ��b�����
    if (!loadAccounts()) {
        return false;
    }

    // ���J������
    return loadCharacters();
}
bool AccountManager::login(const string& username, const string& password) {
    auto it = accounts.find(username);
    if (it == accounts.end()) {
        return false;
    }

    if (PasswordHasher::verifyPassword(password, it->second.getSalt(),
        it->second.getPasswordHash())) {
        currentUser = &(it->second);
        return true;
    }
    return false;
}

void AccountManager::logout() {
    if (currentUser && CGlobalInfo::user && CGlobalInfo::user->get_user()) {
        CFighter* currentFighter = (CFighter*)CGlobalInfo::user->get_user();

        // �O�s�쨤��s��
        auto& saveManager = SaveDirectoryManager::getInstance();
        string savePath = saveManager.getCharacterSavePath(
            getCurrentUsername(),
            currentFighter->getname()
        );

        // �s�@�ƥ�
        if (GetFileAttributesA(savePath.c_str()) != INVALID_FILE_ATTRIBUTES) {
            string backupPath = saveManager.getCharacterBackupPath(
                getCurrentUsername(),
                currentFighter->getname()
            );
            CopyFileA(savePath.c_str(), backupPath.c_str(), FALSE);
        }

        // �O�s��e���A
        if (SaveSystem::saveCharacterData(currentFighter, savePath)) {
            cout << "�w�O�s����ƾ�" << endl;
        }
        else {
            cout << "ĵ�i�G�O�s����ƾڥ���" << endl;
        }

        // ��s characters.txt
        saveCharacters();
    }

    currentUser = nullptr;
}
bool AccountManager::verifyAdmin(const string& adminPassword) {
    // �M��޲z���b��
    auto it = accounts.find("admin");
    if (it == accounts.end()) {
        return false;  // �䤣��޲z���b��
    }

    // �ϥκ޲z���b���� salt �M hash �����ұK�X
    return PasswordHasher::verifyPassword(adminPassword,
        it->second.getSalt(),
        it->second.getPasswordHash());
}

bool AccountManager::isAccountExist(const string& username) {
    return accounts.find(username) != accounts.end();
}

bool AccountManager::resetPassword(const string& adminPassword, const string& username,
    const string& newPassword) {
    // ���Һ޲z���K�X
    if (!verifyAdmin(adminPassword)) {
        return false;
    }

    // �d��n���]�K�X���b��
    auto it = accounts.find(username);
    if (it == accounts.end()) {
        return false;
    }

    // ���ͷs�� salt �M�K�X hash
    string newSalt = PasswordHasher::generateSalt();
    string newHash = PasswordHasher::hashPassword(newPassword, newSalt);

    // ��s�K�X
    it->second.setPasswordHash(newHash);
    it->second.setSalt(newSalt);

    // �x�s���
    return saveAccounts();
}

string AccountManager::getCurrentUsername() const {
    return currentUser ? currentUser->getUsername() : "";
}

bool AccountManager::loadAccounts() {
    ifstream file(ACCOUNTS_FILE);
    if (!file) {
        return false;
    }

    accounts.clear();
    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string username, passwordHash, salt;

        if (getline(iss, username, ':') &&
            getline(iss, passwordHash, ':') &&
            getline(iss, salt)) {
            accounts.emplace(username, UserAccount(username, passwordHash, salt));
        }
    }
    return true;
}

bool AccountManager::saveAccounts() {
    ofstream file(ACCOUNTS_FILE);
    if (!file) {
        return false;
    }

    for (const auto& pair : accounts) {
        const UserAccount& account = pair.second;
        file << account.getUsername() << ":"
            << account.getPasswordHash() << ":"
            << account.getSalt() << "\n";
    }
    return true;
}
void initializeAccountSystem() {
    // �ˬdaccounts.txt�O�_�s�b�A���s�b�h�Ы�
    ifstream accountCheck("accounts.txt");
    if (!accountCheck) {
        ofstream accountFile("accounts.txt");
        if (accountFile) {
            // �Ыغ޲z���b��
            string adminPassword = "admin123";
            string adminSalt = PasswordHasher::generateSalt();
            string adminHash = PasswordHasher::hashPassword(adminPassword, adminSalt);

            accountFile << "admin:" << adminHash << ":" << adminSalt << endl;
        }
        accountFile.close();
    }
    accountCheck.close();

    // �ˬdcharacters.txt�O�_�s�b�A���s�b�h�Ы�
    ifstream charCheck("characters.txt");
    if (!charCheck) {
        ofstream charFile("characters.txt");
        charFile.close();
    }
    charCheck.close();
}

// �Ω�O�s���⪬�A�����U���
string serializeCharacterData(CFighter* fighter) {
    stringstream ss;
    ss << fighter->getname() << ":"
        << fighter->getLevel() << ":"
        << fighter->getCurrentExp() << ":"
        << fighter->getMAXHP() << ":"
        << fighter->getMAXSP() << ":"
        << fighter->getStrength() << ":"
        << fighter->getIntelligence() << ":"
        << fighter->getTenacity() << ":"
        << fighter->getLucky() << ":"
        << fighter->getMoney() << ":"
        << fighter->get_current_city();
    return ss.str();
}

// �Ω���J���⪬�A�����U���
CFighter* deserializeCharacterData(const string& data) {
    stringstream ss(data);
    string name, token;
    int level, exp, hp, sp, strength, intelligence, tenacity, lucky, money, city;

    getline(ss, name, ':');
    getline(ss, token, ':'); level = stoi(token);
    getline(ss, token, ':'); exp = stoi(token);
    getline(ss, token, ':'); hp = stoi(token);
    getline(ss, token, ':'); sp = stoi(token);
    getline(ss, token, ':'); strength = stoi(token);
    getline(ss, token, ':'); intelligence = stoi(token);
    getline(ss, token, ':'); tenacity = stoi(token);
    getline(ss, token, ':'); lucky = stoi(token);
    getline(ss, token, ':'); money = stoi(token);
    getline(ss, token, ':'); city = stoi(token);

    CFighter* fighter = new CFighter(hp, sp, lucky, city);
    fighter->setname(name);
    // TODO: �]�m��L�ݩ�...

    return fighter;
}
bool AccountManager::saveCharacters() {
    ofstream file(CHARACTERS_FILE);
    if (!file) {
        return false;
    }

    for (const auto& account : accounts) {
        const string& username = account.first;
        const vector<CFighter*>& characters = account.second.getCharacters();

        for (const CFighter* character : characters) {
            // �榡�Gusername:serializedCharacterData
            file << username << ":" << serializeCharacterData(character) << endl;
        }
    }
    return true;
}

bool AccountManager::loadCharacters() {
    ifstream file(CHARACTERS_FILE);
    if (!file) {
        return false;
    }

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string username, characterData;

        // ��Ū���Τ�W
        if (getline(iss, username, ':') && getline(iss, characterData)) {
            // ���������b��
            auto it = accounts.find(username);
            if (it != accounts.end()) {
                // �ϧǦC�ƨ���ƾڨòK�[��b��
                CFighter* fighter = deserializeCharacterData(characterData);
                if (fighter) {
                    it->second.addCharacter(fighter);
                }
            }
        }
    }
    return true;
}
bool AccountManager::createCharacter(const string& characterName) {
    if (!currentUser) {
        return false;
    }

    // �ˬd�O�_�w�F����ƶq�W��
    if (currentUser->getCharacterCount() >= UserAccount::MAX_CHARACTERS) {
        return false;
    }

    // �ˬd����W�٬O�_�w�s�b
    vector<string> existingCharacters = currentUser->getCharacterNames();
    if (find(existingCharacters.begin(), existingCharacters.end(), characterName)
        != existingCharacters.end()) {
        return false;
    }

    // �Ыطs����
    CFighter* newCharacter = new CFighter(1000, 2000, 0, 1);  // ��lHP�BSP�BLucky�M����
    newCharacter->setname(characterName);
    newCharacter->setLevel(15);  // ��l����
    newCharacter->setMoney(100000);  // ��l����

    // �N����[�J���e�Τ�
    bool success = currentUser->addCharacter(newCharacter);

    // �p�G���\�Ыب���A�O�s������
    if (success) {
        // �ߧY�O�s�s����ƾ�
        auto& saveManager = SaveDirectoryManager::getInstance();
        string savePath = saveManager.getCharacterSavePath(
            currentUser->getUsername(),
            characterName
        );
        SaveSystem::saveCharacterData(newCharacter, savePath);
    }

    return success;
}
bool AccountManager::deleteCharacter(const string& characterName) {
    if (!currentUser) {
        return false;
    }

    if (currentUser->removeCharacter(characterName)) {
        return saveCharacters();  // �O�s���
    }
    return false;
}
string AccountManager::serializeCharacterData(const CFighter* fighter) const {
    stringstream ss;
    // �O�s���ݩʡA�T�O�ϥγ̷s�������ƶq
    ss << fighter->getname() << ":"
        << fighter->getLevel() << ":"
        << fighter->getCurrentExp() << ":"
        << fighter->getMAXHP() << ":"
        << fighter->getMAXSP() << ":"
        << fighter->getStrength() << ":"
        << fighter->getIntelligence() << ":"
        << fighter->getTenacity() << ":"
        << fighter->getLucky() << ":"
        << fighter->getMoney() << ":"  // �ϥη�e�����ƶq
        << fighter->get_current_city() << ":";

    // �ǦC�ƭI�]���e
    const CBag* bag = fighter->getBag();
    vector<CBagEntry*> entries;
    bag->getAllItems(entries);

    // �g�J���~�ƶq
    ss << entries.size() << ":";

    // �ǦC�ƨC�Ӫ��~
    for (const CBagEntry* entry : entries) {
        const CItem* item = entry->getItem();
        ss << item->isA() << ","
            << item->getID() << ","
            << item->getName() << ","
            << entry->getNum() << ",";  // ���~�ƶq

        // �ھڪ��~�����O�s�B�~�ݩ�
        switch (item->isA()) {
        case eweapon:
            ss << ((CWeapon*)item)->getattackbonus() << ",";
            break;
        case earmor: {
            const CArmor* armor = static_cast<const CArmor*>(item);
            ss << armor->getdefensebonus() << ","
                << static_cast<int>(armor->getArmorType()) << ","
                << static_cast<int>(armor->getArmorSet()) << ",";
            break;
        }
        case efood:
            ss << ((CFood*)item)->gethpbonus() << ",";
            break;
        }
        ss << ";";
    }

    // �K�[����M�H�T�O�ƾڧ����
    ss << "|checksum|";  // �i�H�b�o�̲K�[�ƾڮ���

    return ss.str();
}

CFighter* AccountManager::deserializeCharacterData(const string& data) {
    try {
        stringstream ss(data);
        string name, token;

        // Ū���򥻸��
        if (!getline(ss, name, ':')) {
            cout << "Error: �L�kŪ������W��\n";
            return nullptr;
        }

        // �w�����r�Ŧ��ഫ���
        auto safe_stoi = [](const string& str, const string& field) -> int {
            try {
                return str.empty() ? 0 : stoi(str);
            }
            catch (const exception& e) {
                cout << "Error: �ഫ " << field << " ����: " << str << "\n";
                return 0;
            }
            };

        // Ū�����ݩ�
        string levelStr, expStr, maxhpStr, maxspStr;
        string strStr, intStr, tenStr, luckyStr, moneyStr, cityStr;

        getline(ss, levelStr, ':');
        getline(ss, expStr, ':');
        getline(ss, maxhpStr, ':');
        getline(ss, maxspStr, ':');
        getline(ss, strStr, ':');
        getline(ss, intStr, ':');
        getline(ss, tenStr, ':');
        getline(ss, luckyStr, ':');
        getline(ss, moneyStr, ':');
        getline(ss, cityStr, ':');

        // �ഫ���ݩ�
        int level = safe_stoi(levelStr, "level");
        int exp = safe_stoi(expStr, "exp");
        int maxhp = safe_stoi(maxhpStr, "maxhp");
        int maxsp = safe_stoi(maxspStr, "maxsp");
        int strength = safe_stoi(strStr, "strength");
        int intelligence = safe_stoi(intStr, "intelligence");
        int tenacity = safe_stoi(tenStr, "tenacity");
        int lucky = safe_stoi(luckyStr, "lucky");
        int money = safe_stoi(moneyStr, "money");
        int city = safe_stoi(cityStr, "city");

        // �Ыب�����
        CFighter* fighter = new CFighter(maxhp, maxsp, lucky, city);
        if (!fighter) {
            cout << "Error: �Ыب����ҥ���\n";
            return nullptr;
        }

        // �]�m���ݩ�
        fighter->setname(name);
        fighter->setLevel(level);
        fighter->setCurrentExp(exp);
        fighter->setStrength(strength);
        fighter->setIntelligence(intelligence);
        fighter->setTenacity(tenacity);
        fighter->setMoney(money);

        // �M�ŭI�]
        CBag* bag = fighter->getBag();
        if (bag) {
            vector<CBagEntry*> currentEntries;
            bag->getAllItems(currentEntries);
            for (auto entry : currentEntries) {
                bag->item_delete(entry);
            }
        }

        // Ū�����~�ƶq
        string itemCountStr;
        if (!getline(ss, itemCountStr, ':')) {
            cout << "Warning: �L���~�ƾ�\n";
            return fighter;
        }

        int itemCount = safe_stoi(itemCountStr, "itemCount");
        if (itemCount < 0 || itemCount > 1000) {  // �K�[�X�z���W���ˬd
            cout << "Error: ���~�ƶq���`: " << itemCount << "\n";
            return fighter;
        }

        // Ū���C�Ӫ��~
        for (int i = 0; i < itemCount; i++) {
            string itemData;
            if (!getline(ss, itemData, ';')) {
                cout << "Warning: ���~�ƾڤ�����A�b�� " << i << " �Ӫ��~\n";
                break;
            }

            try {
                stringstream itemStream(itemData);
                string typeStr, idStr, itemName, quantityStr;

                // Ū�����~�򥻫H��
                getline(itemStream, typeStr, ',');
                getline(itemStream, idStr, ',');
                getline(itemStream, itemName, ',');
                getline(itemStream, quantityStr, ',');

                int itemType = safe_stoi(typeStr, "itemType");
                int itemID = safe_stoi(idStr, "itemID");
                int quantity = safe_stoi(quantityStr, "quantity");

                if (quantity <= 0 || quantity > 99) {  // �K�[�ƶq�X�z���ˬd
                    cout << "Warning: ���~�ƶq���`�A���L: " << quantity << "\n";
                    continue;
                }

                // �ھڪ��~�����Ыع������~
                CItem* item = nullptr;
                switch (itemType) {
                case eweapon: {
                    string attackStr;
                    getline(itemStream, attackStr, ',');
                    int attackBonus = safe_stoi(attackStr, "attackBonus");
                    item = new CWeapon(itemName, 0, 0, itemID, attackBonus);
                    break;
                }
                case earmor: {
                    string defenseStr, armorTypeStr, setStr;
                    getline(itemStream, defenseStr, ',');
                    getline(itemStream, armorTypeStr, ',');
                    getline(itemStream, setStr, ',');

                    int defenseBonus = safe_stoi(defenseStr, "defenseBonus");
                    int armorType = safe_stoi(armorTypeStr, "armorType");

                    if (armorType >= 0 && armorType <= 5) {  // ���]�˳������d��O0-5
                        item = new CArmor(itemName, 0, 0, itemID, defenseBonus,
                            static_cast<ArmorType>(armorType));
                    }
                    break;
                }
                case efood: {
                    string hpStr;
                    getline(itemStream, hpStr, ',');
                    int hpBonus = safe_stoi(hpStr, "hpBonus");
                    item = new CFood(itemName, 0, 0, itemID, hpBonus);
                    break;
                }
                default:
                    cout << "Warning: ���������~����: " << itemType << "\n";
                    continue;
                }

                // �N���~�K�[��I�]
                if (item && bag) {
                    bag->item_insert(item, quantity);
                }
                else if (!item) {
                    cout << "Warning: �Ыت��~����: " << itemName << "\n";
                }
            }
            catch (const exception& e) {
                cout << "Warning: �B�z���~�ɵo�Ϳ��~: " << e.what() << "\n";
                continue;
            }
        }

        return fighter;
    }
    catch (const exception& e) {
        cout << "Error: �ϧǦC�ƨ���ƾڥ���: " << e.what() << "\n";
        return nullptr;
    }
}