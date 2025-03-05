#ifndef USER_ACCOUNT_H
#define USER_ACCOUNT_H

#include <string>
#include <vector>
#include "Fighter.h"
class SaveDirectoryManager;  // 前向宣告
class SaveSystem;           // 前向宣告

using namespace std;

class UserAccount {
public:
    UserAccount(const std::string& username = "", const std::string& passwordHash = "",
        const std::string& salt = "");

    // 基本帳號資訊
    string getUsername() const { return username; }
    string getPasswordHash() const { return passwordHash; }
    string getSalt() const { return salt; }

    // 密碼相關
    void setPasswordHash(const string& newHash) { passwordHash = newHash; }
    void setSalt(const string& newSalt) { salt = newSalt; }

    // 角色相關
    bool addCharacter(CFighter* character);
    bool removeCharacter(const std::string& characterName);
    CFighter* getCharacter(const std::string& characterName);
    std::vector<std::string> getCharacterNames() const;
    int getCharacterCount() const { return characters.size(); }
    const std::vector<CFighter*>& getCharacters() const { return characters; }

    static const int MAX_CHARACTERS = 3;

private:
    std::string username;
    std::string passwordHash;
    std::string salt;
    std::vector<CFighter*> characters;
};
#endif