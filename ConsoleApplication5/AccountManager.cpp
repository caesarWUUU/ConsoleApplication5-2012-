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
const string AccountManager::ADMIN_PASSWORD_HASH = "your_admin_hash_here"; // 需要生成
const string AccountManager::ADMIN_SALT = "your_admin_salt_here"; // 需要生成

AccountManager AccountManager::instance;

AccountManager& AccountManager::getInstance() {
    return instance;
}

bool AccountManager::createAccount(const string& username, const string& password) {
    if (username.empty() || password.empty()) {
        return false;
    }
    // 檢查帳號是否已存在
    if (accounts.find(username) != accounts.end()) {
        return false;
    }

    // 生成salt和密碼hash
    string salt = PasswordHasher::generateSalt();
    string passwordHash = PasswordHasher::hashPassword(password, salt);

    // 創建新帳號
    accounts.emplace(username, UserAccount(username, passwordHash, salt));

    // 儲存到檔案
    return saveAccounts();
}



CFighter* AccountManager::selectCharacter(const string& characterName) {
    // 檢查用戶是否已登入
    if (!currentUser) {
        cout << "錯誤：用戶尚未登入" << endl;
        return nullptr;
    }

    // 檢索角色
    CFighter* fighter = currentUser->getCharacter(characterName);
    if (!fighter) {
        cout << "錯誤：找不到角色 " << characterName << endl;
        return nullptr;
    }

    // 取得存檔路徑
    auto& saveManager = SaveDirectoryManager::getInstance();
    string savePath = saveManager.getCharacterSavePath(getCurrentUsername(), characterName);
    cout << "正在讀取存檔：" << savePath << endl;

    // 檢查存檔文件是否存在
    if (GetFileAttributesA(savePath.c_str()) != INVALID_FILE_ATTRIBUTES) {
        cout << "找到存檔文件" << endl;

        // 在載入前清理背包
        CBag* bag = fighter->getBag();
        if (bag) {
            vector<CBagEntry*> currentEntries;
            bag->getAllItems(currentEntries);
            for (auto entry : currentEntries) {
                bag->item_delete(entry);
            }
        }

        // 記錄載入前的狀態（用於除錯）
        cout << "載入前狀態：" << endl;
        cout << "等級：" << fighter->getLevel() << endl;
        cout << "經驗值：" << fighter->getCurrentExp() << endl;
        cout << "金錢：" << fighter->getMoney() << endl;
        cout << "HP：" << fighter->getHP() << "/" << fighter->getMAXHP() << endl;
        cout << "SP：" << fighter->getSP() << "/" << fighter->getMAXSP() << endl;

        // 嘗試載入存檔數據
        if (SaveSystem::loadCharacterData(fighter, savePath)) {
            cout << "角色數據載入成功！" << endl;

            // 記錄載入後的狀態（用於除錯）
            cout << "載入後狀態：" << endl;
            cout << "等級：" << fighter->getLevel() << endl;
            cout << "經驗值：" << fighter->getCurrentExp() << endl;
            cout << "金錢：" << fighter->getMoney() << endl;
            cout << "HP：" << fighter->getHP() << "/" << fighter->getMAXHP() << endl;
            cout << "SP：" << fighter->getSP() << "/" << fighter->getMAXSP() << endl;

            // 創建自動備份
            string backupPath = saveManager.getCharacterBackupPath(
                getCurrentUsername(),
                characterName
            );
            if (CopyFileA(savePath.c_str(), backupPath.c_str(), FALSE)) {
                cout << "已創建存檔備份" << endl;
            }
            else {
                cout << "警告：備份創建失敗，錯誤代碼：" << GetLastError() << endl;
            }
        }
        else {
            cout << "警告：無法載入存檔數據，將使用初始狀態" << endl;

            // 可以在這裡添加重置角色狀態的邏輯
            fighter->setLevel(15);  // 設置初始等級
            fighter->setCurrentExp(0);
            fighter->setMoney(100000);
            // ... 其他初始化設置 ...
        }
    }
    else {
        cout << "未找到存檔文件，將使用初始狀態" << endl;
        // 第一次使用這個角色，不需要特別處理
    }

    // 確保角色狀態的合理性
    if (fighter->getHP() <= 0) fighter->setHP(fighter->getMAXHP());
    if (fighter->getSP() <= 0) fighter->setSP(fighter->getMAXSP());

    Sleep(1000);  // 給用戶時間查看狀態
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
            cout << "無法創建存檔目錄!" << endl;
            return false;
        }
    }

    if (!CreateDirectoryA("saves\\backups", NULL)) {
        if (GetLastError() != ERROR_ALREADY_EXISTS) {
            cout << "無法創建備份目錄!" << endl;
            // 繼續執行，因為這不是致命錯誤
        }
    }
    bool needInit = false;

    // 檢查 accounts.txt 是否存在
    ifstream accountCheck(ACCOUNTS_FILE);
    if (!accountCheck) {
        needInit = true;
    }
    accountCheck.close();

    if (needInit) {
        // 創建 accounts.txt 和管理員帳號
        ofstream accountFile(ACCOUNTS_FILE);
        if (accountFile) {
            // 創建管理者帳號
            string adminPassword = "admin123";
            string adminSalt = PasswordHasher::generateSalt();
            string adminHash = PasswordHasher::hashPassword(adminPassword, adminSalt);

            accountFile << "admin:" << adminHash << ":" << adminSalt << endl;
            accountFile.close();
        }
    }

    // 檢查 characters.txt 是否存在
    ifstream charCheck(CHARACTERS_FILE);
    if (!charCheck) {
        ofstream charFile(CHARACTERS_FILE);
        charFile.close();
    }
    charCheck.close();

    // 載入所有帳號資料
    if (!loadAccounts()) {
        return false;
    }

    // 載入角色資料
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

        // 保存到角色存檔
        auto& saveManager = SaveDirectoryManager::getInstance();
        string savePath = saveManager.getCharacterSavePath(
            getCurrentUsername(),
            currentFighter->getname()
        );

        // 製作備份
        if (GetFileAttributesA(savePath.c_str()) != INVALID_FILE_ATTRIBUTES) {
            string backupPath = saveManager.getCharacterBackupPath(
                getCurrentUsername(),
                currentFighter->getname()
            );
            CopyFileA(savePath.c_str(), backupPath.c_str(), FALSE);
        }

        // 保存當前狀態
        if (SaveSystem::saveCharacterData(currentFighter, savePath)) {
            cout << "已保存角色數據" << endl;
        }
        else {
            cout << "警告：保存角色數據失敗" << endl;
        }

        // 更新 characters.txt
        saveCharacters();
    }

    currentUser = nullptr;
}
bool AccountManager::verifyAdmin(const string& adminPassword) {
    // 尋找管理員帳號
    auto it = accounts.find("admin");
    if (it == accounts.end()) {
        return false;  // 找不到管理員帳號
    }

    // 使用管理員帳號的 salt 和 hash 來驗證密碼
    return PasswordHasher::verifyPassword(adminPassword,
        it->second.getSalt(),
        it->second.getPasswordHash());
}

bool AccountManager::isAccountExist(const string& username) {
    return accounts.find(username) != accounts.end();
}

bool AccountManager::resetPassword(const string& adminPassword, const string& username,
    const string& newPassword) {
    // 驗證管理員密碼
    if (!verifyAdmin(adminPassword)) {
        return false;
    }

    // 查找要重設密碼的帳號
    auto it = accounts.find(username);
    if (it == accounts.end()) {
        return false;
    }

    // 產生新的 salt 和密碼 hash
    string newSalt = PasswordHasher::generateSalt();
    string newHash = PasswordHasher::hashPassword(newPassword, newSalt);

    // 更新密碼
    it->second.setPasswordHash(newHash);
    it->second.setSalt(newSalt);

    // 儲存更改
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
    // 檢查accounts.txt是否存在，不存在則創建
    ifstream accountCheck("accounts.txt");
    if (!accountCheck) {
        ofstream accountFile("accounts.txt");
        if (accountFile) {
            // 創建管理員帳號
            string adminPassword = "admin123";
            string adminSalt = PasswordHasher::generateSalt();
            string adminHash = PasswordHasher::hashPassword(adminPassword, adminSalt);

            accountFile << "admin:" << adminHash << ":" << adminSalt << endl;
        }
        accountFile.close();
    }
    accountCheck.close();

    // 檢查characters.txt是否存在，不存在則創建
    ifstream charCheck("characters.txt");
    if (!charCheck) {
        ofstream charFile("characters.txt");
        charFile.close();
    }
    charCheck.close();
}

// 用於保存角色狀態的輔助函數
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

// 用於載入角色狀態的輔助函數
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
    // TODO: 設置其他屬性...

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
            // 格式：username:serializedCharacterData
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

        // 先讀取用戶名
        if (getline(iss, username, ':') && getline(iss, characterData)) {
            // 找到對應的帳號
            auto it = accounts.find(username);
            if (it != accounts.end()) {
                // 反序列化角色數據並添加到帳號
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

    // 檢查是否已達角色數量上限
    if (currentUser->getCharacterCount() >= UserAccount::MAX_CHARACTERS) {
        return false;
    }

    // 檢查角色名稱是否已存在
    vector<string> existingCharacters = currentUser->getCharacterNames();
    if (find(existingCharacters.begin(), existingCharacters.end(), characterName)
        != existingCharacters.end()) {
        return false;
    }

    // 創建新角色
    CFighter* newCharacter = new CFighter(1000, 2000, 0, 1);  // 初始HP、SP、Lucky和城市
    newCharacter->setname(characterName);
    newCharacter->setLevel(15);  // 初始等級
    newCharacter->setMoney(100000);  // 初始金錢

    // 將角色加入到當前用戶
    bool success = currentUser->addCharacter(newCharacter);

    // 如果成功創建角色，保存角色資料
    if (success) {
        // 立即保存新角色數據
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
        return saveCharacters();  // 保存更改
    }
    return false;
}
string AccountManager::serializeCharacterData(const CFighter* fighter) const {
    stringstream ss;
    // 保存基本屬性，確保使用最新的金錢數量
    ss << fighter->getname() << ":"
        << fighter->getLevel() << ":"
        << fighter->getCurrentExp() << ":"
        << fighter->getMAXHP() << ":"
        << fighter->getMAXSP() << ":"
        << fighter->getStrength() << ":"
        << fighter->getIntelligence() << ":"
        << fighter->getTenacity() << ":"
        << fighter->getLucky() << ":"
        << fighter->getMoney() << ":"  // 使用當前金錢數量
        << fighter->get_current_city() << ":";

    // 序列化背包內容
    const CBag* bag = fighter->getBag();
    vector<CBagEntry*> entries;
    bag->getAllItems(entries);

    // 寫入物品數量
    ss << entries.size() << ":";

    // 序列化每個物品
    for (const CBagEntry* entry : entries) {
        const CItem* item = entry->getItem();
        ss << item->isA() << ","
            << item->getID() << ","
            << item->getName() << ","
            << entry->getNum() << ",";  // 物品數量

        // 根據物品類型保存額外屬性
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

    // 添加校驗和以確保數據完整性
    ss << "|checksum|";  // 可以在這裡添加數據校驗

    return ss.str();
}

CFighter* AccountManager::deserializeCharacterData(const string& data) {
    try {
        stringstream ss(data);
        string name, token;

        // 讀取基本資料
        if (!getline(ss, name, ':')) {
            cout << "Error: 無法讀取角色名稱\n";
            return nullptr;
        }

        // 安全的字符串轉換函數
        auto safe_stoi = [](const string& str, const string& field) -> int {
            try {
                return str.empty() ? 0 : stoi(str);
            }
            catch (const exception& e) {
                cout << "Error: 轉換 " << field << " 失敗: " << str << "\n";
                return 0;
            }
            };

        // 讀取基本屬性
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

        // 轉換基本屬性
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

        // 創建角色實例
        CFighter* fighter = new CFighter(maxhp, maxsp, lucky, city);
        if (!fighter) {
            cout << "Error: 創建角色實例失敗\n";
            return nullptr;
        }

        // 設置基本屬性
        fighter->setname(name);
        fighter->setLevel(level);
        fighter->setCurrentExp(exp);
        fighter->setStrength(strength);
        fighter->setIntelligence(intelligence);
        fighter->setTenacity(tenacity);
        fighter->setMoney(money);

        // 清空背包
        CBag* bag = fighter->getBag();
        if (bag) {
            vector<CBagEntry*> currentEntries;
            bag->getAllItems(currentEntries);
            for (auto entry : currentEntries) {
                bag->item_delete(entry);
            }
        }

        // 讀取物品數量
        string itemCountStr;
        if (!getline(ss, itemCountStr, ':')) {
            cout << "Warning: 無物品數據\n";
            return fighter;
        }

        int itemCount = safe_stoi(itemCountStr, "itemCount");
        if (itemCount < 0 || itemCount > 1000) {  // 添加合理的上限檢查
            cout << "Error: 物品數量異常: " << itemCount << "\n";
            return fighter;
        }

        // 讀取每個物品
        for (int i = 0; i < itemCount; i++) {
            string itemData;
            if (!getline(ss, itemData, ';')) {
                cout << "Warning: 物品數據不完整，在第 " << i << " 個物品\n";
                break;
            }

            try {
                stringstream itemStream(itemData);
                string typeStr, idStr, itemName, quantityStr;

                // 讀取物品基本信息
                getline(itemStream, typeStr, ',');
                getline(itemStream, idStr, ',');
                getline(itemStream, itemName, ',');
                getline(itemStream, quantityStr, ',');

                int itemType = safe_stoi(typeStr, "itemType");
                int itemID = safe_stoi(idStr, "itemID");
                int quantity = safe_stoi(quantityStr, "quantity");

                if (quantity <= 0 || quantity > 99) {  // 添加數量合理性檢查
                    cout << "Warning: 物品數量異常，跳過: " << quantity << "\n";
                    continue;
                }

                // 根據物品類型創建對應物品
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

                    if (armorType >= 0 && armorType <= 5) {  // 假設裝備類型範圍是0-5
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
                    cout << "Warning: 未知的物品類型: " << itemType << "\n";
                    continue;
                }

                // 將物品添加到背包
                if (item && bag) {
                    bag->item_insert(item, quantity);
                }
                else if (!item) {
                    cout << "Warning: 創建物品失敗: " << itemName << "\n";
                }
            }
            catch (const exception& e) {
                cout << "Warning: 處理物品時發生錯誤: " << e.what() << "\n";
                continue;
            }
        }

        return fighter;
    }
    catch (const exception& e) {
        cout << "Error: 反序列化角色數據失敗: " << e.what() << "\n";
        return nullptr;
    }
}