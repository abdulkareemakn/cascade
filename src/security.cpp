#include "security.h"

#include <argon2.h>
#include <openssl/rand.h>

#include <stdexcept>
#include <vector>

namespace security {

constexpr uint32_t T_COST = 2;
constexpr uint32_t M_COST = 65536;
constexpr uint32_t PARALLELISM = 1;
constexpr uint32_t SALT_LEN = 16;
constexpr uint32_t HASH_LEN = 32;

std::string hashPassword(const std::string& password) {
    uint8_t salt[SALT_LEN];
    if (RAND_bytes(salt, SALT_LEN) != 1) {
        throw std::runtime_error("Failed to generate random salt");
    }

    size_t encodedLen = argon2_encodedlen(T_COST, M_COST, PARALLELISM, SALT_LEN,
                                          HASH_LEN, Argon2_id);

    std::vector<char> encoded(encodedLen);

    int result = argon2id_hash_encoded(
        T_COST, M_COST, PARALLELISM, password.c_str(), password.length(), salt,
        SALT_LEN, HASH_LEN, encoded.data(), encodedLen);

    if (result != ARGON2_OK) {
        throw std::runtime_error(std::string("Argon2 hashing failed: ") +
                                 argon2_error_message(result));
    }

    return std::string(encoded.data());
}

bool verifyPassword(const std::string& password, const std::string& hash) {
    int result =
        argon2id_verify(hash.c_str(), password.c_str(), password.length());
    return result == ARGON2_OK;
}

}  // namespace security
