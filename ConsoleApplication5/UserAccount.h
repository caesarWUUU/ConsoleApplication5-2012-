#ifndef USER_ACCOUNT_H
#define USER_ACCOUNT_H

#include <string>
#include <vector>
#include "Fighter.h"
class SaveDirectoryManager;  // �e�V�ŧi
class SaveSystem;           // �e�V�ŧi

using namespace std;

class UserAccount {
public:
    UserAccount(const std::string& username = "", const std::string& passwordHash = "",
        const std::string& salt = "");

    // �򥻱b����T
    string getUsername() const { return username; }
    string getPasswordHash() const { return passwordHash; }
    string getSalt() const { return salt; }

    // �K�X����
    void setPasswordHash(const string& newHash) { passwordHash = newHash; }
    void setSalt(const string& newSalt) { salt = newSalt; }

    // �������
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