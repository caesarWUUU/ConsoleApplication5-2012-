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

    // �ЫإD�s�ɥؿ�
    if (!createDirectory(saveDirPath)) {
        return false;
    }

    // �Ыسƥ��ؿ�
    if (!createDirectory(saveDirPath + "\\backups")) {
        return false;
    }

    std::cout << "�s�ɨt�Ϊ�l�Ƨ����C�D�ؿ��G" << saveDirPath << std::endl;
    return true;
}

bool SaveDirectoryManager::createDirectory(const std::string& path) const {
    if (CreateDirectoryA(path.c_str(), NULL) ||
        GetLastError() == ERROR_ALREADY_EXISTS) {
        return true;
    }

    std::cout << "�L�k�Ыإؿ��G" << path
        << " ���~�N�X�G" << GetLastError() << std::endl;
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
        std::cout << "�䤣�췽�s�ɤ��G" << sourcePath << std::endl;
        return false;
    }

    std::string backupPath = getBackupPath(username);
    if (CopyFileA(sourcePath.c_str(), backupPath.c_str(), FALSE)) {
        std::cout << "�w�Ыسƥ��G" << backupPath << std::endl;
        return true;
    }

    std::cout << "�Ыسƥ����ѡA���~�N�X�G" << GetLastError() << std::endl;
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

    // ���ק�ɶ��Ƨ�
    std::sort(backups.begin(), backups.end(),
        [](const BackupFile& a, const BackupFile& b) {
            return CompareFileTime(&a.writeTime, &b.writeTime) > 0;
        });

    // �R���h�l���ƥ�
    for (size_t i = keepCount; i < backups.size(); ++i) {
        if (DeleteFileA(backups[i].filename.c_str())) {
            std::cout << "�w�R���³ƥ��G" << backups[i].filename << std::endl;
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
        // ���� .sav ���
        saves.push_back(filename.substr(0, filename.length() - 4));
    } while (FindNextFileA(hFind, &findData));

    FindClose(hFind);
    return saves;
}