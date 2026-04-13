#include "../../include/common.h"
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <sstream>
#include <iomanip>

namespace CryptoUtils {

std::string md5(const std::string& input) {
    unsigned char hash[MD5_DIGEST_LENGTH];
    MD5((unsigned char*)input.c_str(), input.length(), hash);

    std::stringstream ss;
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return ss.str();
}

std::string sha256(const std::string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)input.c_str(), input.length(), hash);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return ss.str();
}

std::string generateSalt() {
    const int SALT_LENGTH = 32;
    unsigned char salt[SALT_LENGTH];

    if (RAND_bytes(salt, SALT_LENGTH) != 1) {
        // 如果RAND_bytes失败，使用备用方法
        srand(time(nullptr));
        for (int i = 0; i < SALT_LENGTH; i++) {
            salt[i] = rand() % 256;
        }
    }

    std::stringstream ss;
    for (int i = 0; i < SALT_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)salt[i];
    }

    return ss.str();
}

std::string hashPassword(const std::string& password, const std::string& salt) {
    return sha256(password + salt);
}

bool verifyPassword(const std::string& password, const std::string& hash, const std::string& salt) {
    std::string computedHash = hashPassword(password, salt);
    return computedHash == hash;
}

} // namespace CryptoUtils