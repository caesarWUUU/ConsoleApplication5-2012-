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


using namespace std;

UserAccount::UserAccount(const string& username, const string& passwordHash,
    const string& salt)
    : username(username), passwordHash(passwordHash), salt(salt) {
    cout << "Created user account for " << username << endl;
}

bool UserAccount::addCharacter(CFighter* character) {
    // 檢查是否達到角色數上限
    if (characters.size() >= MAX_CHARACTERS) {
        return false;
    }

    // 檢查角色是否有效
    if (!character) {
        return false;
    }

    // 檢查是否已存在同名角色
    auto it = find_if(characters.begin(), characters.end(),
        [&](const CFighter* f) { return f->getname() == character->getname(); });
    if (it != characters.end()) {
        return false;
    }

    // 添加角色
    characters.push_back(character);

    // 立即保存角色數據
    try {
        auto& saveManager = SaveDirectoryManager::getInstance();
        string savePath = saveManager.getCharacterSavePath(username, character->getname());
        if (SaveSystem::saveCharacterData(character, savePath)) {
            cout << "角色資料保存成功！" << endl;
            return true;
        }
        else {
            cout << "警告：角色資料保存失敗！" << endl;
            return false;
        }
    }
    catch (const exception& e) {
        cout << "保存角色資料時發生錯誤：" << e.what() << endl;
        return false;
    }
}

bool UserAccount::removeCharacter(const string& characterName) {
    vector<CFighter*>::iterator it = find_if(characters.begin(), characters.end(),
        [&](const CFighter* f) { return f->getname() == characterName; });

    if (it != characters.end()) {
        delete* it;
        characters.erase(it);
        return true;
    }
    return false;
}

CFighter* UserAccount::getCharacter(const string& characterName) {
    auto it = find_if(characters.begin(), characters.end(),
        [&](const CFighter* f) { return f->getname() == characterName; });

    return (it != characters.end()) ? *it : nullptr;
}

vector<string> UserAccount::getCharacterNames() const {
    vector<string> names;
    for (const auto* character : characters) {
        names.push_back(character->getname());
    }
    return names;
}