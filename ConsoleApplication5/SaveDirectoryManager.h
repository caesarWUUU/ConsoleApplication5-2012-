#ifndef SAVE_DIRECTORY_MANAGER_H
#define SAVE_DIRECTORY_MANAGER_H

#include <string>
#include <vector>
#include <windows.h>
#include <ctime>       // ���F�ϥ� time()
#include <sstream>     // ���F�ϥ� stringstream

using namespace std;

class SaveDirectoryManager {
public:
    static SaveDirectoryManager& getInstance();

    // ��l�Ʀs�ɥؿ�
    bool initialize(const std::string& basePath = "saves");

    // ����s�ɸ��|
    std::string getSavePath(const std::string& username) const;

    // �ˬd�s�ɬO�_�s�b
    bool hasSaveFile(const std::string& username) const;

    // �Ыسƥ�
    bool createBackup(const std::string& username);

    // �M�z�³ƥ�
    void cleanOldBackups(const std::string& username, int keepCount = 5);

    // ����Ҧ��s�ɦC��
    vector<std::string> getAllSaves() const;

    // �������S�w���s�ɸ��|
    string getCharacterSavePath(const string& username, const string& characterName) const {
        return saveDirPath + "\\" + username + "_" + characterName + ".sav";
    }

    // �������S�w���ƥ����|
    string getCharacterBackupPath(const string& username, const string& characterName) const {
        time_t currentTime = time(nullptr);
        stringstream ss;
        ss << saveDirPath << "\\backups\\"
            << username << "_"
            << characterName << "_"
            << currentTime
            << ".bak";
        return ss.str();
    }

private:
    SaveDirectoryManager() = default;
    std::string saveDirPath;

    bool createDirectory(const std::string& path) const;
    std::string getBackupPath(const std::string& username) const;
};

#endif