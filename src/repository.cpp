#include "repository.h"

#include <string>

#include "database.h"
#include "models.h"
#include "security.h"

bool db::createUser(const std::string& username, const std::string& email,
                    const std::string& password) {
    auto storage = getStorage();
    std::string hash = security::hashPassword(password);

    User user{.username = username, .email = email, .passwordHash = hash};
    auto insertedId = storage.insert(user);
    user.id = insertedId;
    return user.id;
}
