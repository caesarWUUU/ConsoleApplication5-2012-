#ifndef PASSWORD_HASHER_H
#define PASSWORD_HASHER_H

#include <string>

using namespace std;

class PasswordHasher {
public:
    static std::string generateSalt();
    static std::string hashPassword(const std::string& password, const std::string& salt);
    static bool verifyPassword(const std::string& password, const std::string& salt,
        const std::string& storedHash);

private:
    static const int SALT_LENGTH = 16;
};
#endif

