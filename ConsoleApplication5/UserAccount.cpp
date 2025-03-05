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
    // �ˬd�O�_�F�쨤��ƤW��
    if (characters.size() >= MAX_CHARACTERS) {
        return false;
    }

    // �ˬd����O�_����
    if (!character) {
        return false;
    }

    // �ˬd�O�_�w�s�b�P�W����
    auto it = find_if(characters.begin(), characters.end(),
        [&](const CFighter* f) { return f->getname() == character->getname(); });
    if (it != characters.end()) {
        return false;
    }

    // �K�[����
    characters.push_back(character);

    // �ߧY�O�s����ƾ�
    try {
        auto& saveManager = SaveDirectoryManager::getInstance();
        string savePath = saveManager.getCharacterSavePath(username, character->getname());
        if (SaveSystem::saveCharacterData(character, savePath)) {
            cout << "�����ƫO�s���\�I" << endl;
            return true;
        }
        else {
            cout << "ĵ�i�G�����ƫO�s���ѡI" << endl;
            return false;
        }
    }
    catch (const exception& e) {
        cout << "�O�s�����Ʈɵo�Ϳ��~�G" << e.what() << endl;
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