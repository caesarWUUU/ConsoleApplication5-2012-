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

using namespace std;


string PasswordHasher::generateSalt() {
    static const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, sizeof(charset) - 2);

    string salt;
    salt.reserve(SALT_LENGTH);
    for (int i = 0; i < SALT_LENGTH; ++i) {
        salt += charset[dis(gen)];
    }
    return salt;
}

string PasswordHasher::hashPassword(const string& password, const string& salt) {
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    string hashedPassword;

    try {
        // 獲取加密提供者
        if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
            throw runtime_error("Failed to acquire crypto context");
        }

        // 創建雜湊對象
        if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
            CryptReleaseContext(hProv, 0);
            throw runtime_error("Failed to create hash");
        }

        // 組合密碼和salt
        string combined = password + salt;

        // 添加數據到雜湊
        if (!CryptHashData(hHash, (BYTE*)combined.c_str(), combined.length(), 0)) {
            CryptDestroyHash(hHash);
            CryptReleaseContext(hProv, 0);
            throw runtime_error("Failed to hash data");
        }

        // 獲取雜湊值
        DWORD hashLen = 0;
        DWORD hashLenSize = sizeof(DWORD);
        if (!CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE*)&hashLen, &hashLenSize, 0)) {
            CryptDestroyHash(hHash);
            CryptReleaseContext(hProv, 0);
            throw runtime_error("Failed to get hash size");
        }

        vector<BYTE> buffer(hashLen);
        if (!CryptGetHashParam(hHash, HP_HASHVAL, &buffer[0], &hashLen, 0)) {
            CryptDestroyHash(hHash);
            CryptReleaseContext(hProv, 0);
            throw runtime_error("Failed to get hash value");
        }

        // 轉換為十六進制字符串
        stringstream ss;
        for (size_t i = 0; i < buffer.size(); ++i) {
            ss << hex << setw(2) << setfill('0') << (int)buffer[i];
        }
        hashedPassword = ss.str();

        // 清理
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);

        return hashedPassword;
    }
    catch (const exception& e) {
        if (hHash) CryptDestroyHash(hHash);
        if (hProv) CryptReleaseContext(hProv, 0);
        throw;
    }
}

bool PasswordHasher::verifyPassword(const string& password, const string& salt,
    const string& storedHash) {
    string computedHash = hashPassword(password, salt);
    return computedHash == storedHash;
}
