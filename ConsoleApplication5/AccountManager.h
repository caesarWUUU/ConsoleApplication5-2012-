#ifndef ACCOUNT_MANAGER_H
#define ACCOUNT_MANAGER_H

#include <string>
#include <map>
#include "UserAccount.h"
#include "Fighter.h"

using namespace std;

class AccountManager {
public:
    static AccountManager& getInstance();

    // �b���޲z
    bool createAccount(const std::string& username, const std::string& password);
    bool login(const std::string& username, const std::string& password);
    void logout();
    bool resetPassword(const std::string& adminPassword, const std::string& username,
        const std::string& newPassword);

    // ����޲z
    bool createCharacter(const std::string& characterName);
    bool deleteCharacter(const std::string& characterName);
    std::vector<std::string> getCurrentUserCharacters() const;
    CFighter* selectCharacter(const std::string& characterName);

    // �ɮצs��
    bool loadAccounts();
    bool saveAccounts();
    bool initializeSystem();

    // ���A�d��
    bool isLoggedIn() const { return currentUser != nullptr; }
    string getCurrentUsername() const;
    UserAccount* getCurrentUser() const { return currentUser; }
    bool verifyAdmin(const string& adminPassword);
    bool isAccountExist(const string& username);

private:
    static AccountManager instance;
    std::map<std::string, UserAccount> accounts;
    UserAccount* currentUser;

    // �ɮ׸��|
    static const std::string ACCOUNTS_FILE;
    static const std::string CHARACTERS_FILE;
    static const std::string ADMIN_ACCOUNT;
    static const std::string ADMIN_PASSWORD_HASH;
    static const std::string ADMIN_SALT;

    // �s�W����s�ɬ������
    bool loadCharacters();
    bool saveCharacters();

    string serializeCharacterData(const CFighter* fighter) const;
    CFighter* deserializeCharacterData(const string& data);
  

    static const string ACCOUNTS_FILE;
    static const string CHARACTERS_FILE;
};

#endif