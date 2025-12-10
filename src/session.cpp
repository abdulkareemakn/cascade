#include "session.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <optional>
#include <stdexcept>

namespace fs = std::filesystem;

fs::path session::getSessionPath() {
    const char* stateHome = std::getenv("XDG_STATE_HOME");
    if (stateHome != nullptr) {
        return fs::path(stateHome) / "cascade" / "session";
    }

    const char* home = std::getenv("HOME");
    if (home == nullptr) {
        throw std::runtime_error("HOME environment variable not set");
    }
    return fs::path(home) / ".local" / "state" / "session";
}

bool session::login(int userId) {
    fs::path sessionPath = getSessionPath();

    fs::create_directories(sessionPath.parent_path());

    std::ofstream sessionFile(sessionPath);
    if (!sessionFile) {
        return false;
    }

    sessionFile << userId << "\n";
    return sessionFile.good();
}

bool session::logout() {
    fs::path sessionPath = getSessionPath();

    if (!fs::exists(sessionPath)) {
        return false;
    }
    return fs::remove(sessionPath);
}

bool session::isLoggedIn() { return fs::exists(getSessionPath()); }

std::optional<int> session::getCurrentUser() {
    fs::path sessionPath = getSessionPath();

    if (!fs::exists(sessionPath)) {
        return std::nullopt;
    }

    std::ifstream sessionFile(sessionPath);
    if (!sessionFile) {
        return std::nullopt;
    }

    int userId;
    sessionFile >> userId;
    return userId;
}
