#ifndef SAVE_DIRECTORY_MANAGER_H
#define SAVE_DIRECTORY_MANAGER_H

#include <string>
#include <vector>
#include <windows.h>
#include <ctime>       // 為了使用 time()
#include <sstream>     // 為了使用 stringstream

using namespace std;

class SaveDirectoryManager {
public:
    static SaveDirectoryManager& getInstance();

    // 初始化存檔目錄
    bool initialize(const std::string& basePath = "saves");

    // 獲取存檔路徑
    std::string getSavePath(const std::string& username) const;

    // 檢查存檔是否存在
    bool hasSaveFile(const std::string& username) const;

    // 創建備份
    bool createBackup(const std::string& username);

    // 清理舊備份
    void cleanOldBackups(const std::string& username, int keepCount = 5);

    // 獲取所有存檔列表
    vector<std::string> getAllSaves() const;

    // 獲取角色特定的存檔路徑
    string getCharacterSavePath(const string& username, const string& characterName) const {
        return saveDirPath + "\\" + username + "_" + characterName + ".sav";
    }

    // 獲取角色特定的備份路徑
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