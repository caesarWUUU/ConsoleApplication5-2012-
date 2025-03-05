#include "SaveDirectoryManager.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <fstream>
#include <ctime>
#include <algorithm>

SaveDirectoryManager& SaveDirectoryManager::getInstance() {
    static SaveDirectoryManager instance;
    return instance;
}

bool SaveDirectoryManager::initialize(const std::string& basePath) {
    saveDirPath = basePath;

    // 創建主存檔目錄
    if (!createDirectory(saveDirPath)) {
        return false;
    }

    // 創建備份目錄
    if (!createDirectory(saveDirPath + "\\backups")) {
        return false;
    }

    std::cout << "存檔系統初始化完成。主目錄：" << saveDirPath << std::endl;
    return true;
}

bool SaveDirectoryManager::createDirectory(const std::string& path) const {
    if (CreateDirectoryA(path.c_str(), NULL) ||
        GetLastError() == ERROR_ALREADY_EXISTS) {
        return true;
    }

    std::cout << "無法創建目錄：" << path
        << " 錯誤代碼：" << GetLastError() << std::endl;
    return false;
}

std::string SaveDirectoryManager::getSavePath(const std::string& username) const {
    return saveDirPath + "\\" + username + ".sav";
}



std::string SaveDirectoryManager::getBackupPath(const std::string& username) const {
    SYSTEMTIME st;
    GetLocalTime(&st);

    std::stringstream ss;
    ss << saveDirPath << "\\backups\\" << username << "_"
        << st.wYear << std::setfill('0') << std::setw(2) << st.wMonth
        << std::setw(2) << st.wDay << "_"
        << std::setw(2) << st.wHour << std::setw(2) << st.wMinute
        << std::setw(2) << st.wSecond << ".bak";

    return ss.str();
}

bool SaveDirectoryManager::createBackup(const std::string& username) {
    std::string sourcePath = getSavePath(username);
    if (!hasSaveFile(username)) {
        std::cout << "找不到源存檔文件：" << sourcePath << std::endl;
        return false;
    }

    std::string backupPath = getBackupPath(username);
    if (CopyFileA(sourcePath.c_str(), backupPath.c_str(), FALSE)) {
        std::cout << "已創建備份：" << backupPath << std::endl;
        return true;
    }

    std::cout << "創建備份失敗，錯誤代碼：" << GetLastError() << std::endl;
    return false;
}

void SaveDirectoryManager::cleanOldBackups(const std::string& username, int keepCount) {
    std::string searchPath = saveDirPath + "\\backups\\" + username + "_*.bak";
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        return;
    }

    struct BackupFile {
        std::string filename;
        FILETIME writeTime;
    };
    std::vector<BackupFile> backups;

    do {
        BackupFile backup;
        backup.filename = saveDirPath + "\\backups\\" + findData.cFileName;
        backup.writeTime = findData.ftLastWriteTime;
        backups.push_back(backup);
    } while (FindNextFileA(hFind, &findData));

    FindClose(hFind);

    // 按修改時間排序
    std::sort(backups.begin(), backups.end(),
        [](const BackupFile& a, const BackupFile& b) {
            return CompareFileTime(&a.writeTime, &b.writeTime) > 0;
        });

    // 刪除多餘的備份
    for (size_t i = keepCount; i < backups.size(); ++i) {
        if (DeleteFileA(backups[i].filename.c_str())) {
            std::cout << "已刪除舊備份：" << backups[i].filename << std::endl;
        }
    }
}

std::vector<std::string> SaveDirectoryManager::getAllSaves() const {
    std::vector<std::string> saves;
    std::string searchPath = saveDirPath + "\\*.sav";
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        return saves;
    }

    do {
        std::string filename = findData.cFileName;
        // 移除 .sav 後綴
        saves.push_back(filename.substr(0, filename.length() - 4));
    } while (FindNextFileA(hFind, &findData));

    FindClose(hFind);
    return saves;
}