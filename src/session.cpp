#include "session.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <optional>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

fs::path session::getSessionPath() {
    const char *stateHome = std::getenv("XDG_STATE_HOME");
    if (stateHome != nullptr) {
        return fs::path(stateHome) / "cascade" / "session";
    }

    const char *home = std::getenv("HOME");
    if (home == nullptr) {
        throw std::runtime_error("HOME environment variable not set");
    }
    return fs::path(home) / ".local" / "state" / "cascade" / "session";
}

bool saveSession(const std::string &username) {
    fs::path sessionPath = session::getSessionPath();
    fs::create_directories(sessionPath.parent_path());

    std::ofstream sessionFile(sessionPath);
    if (!sessionFile) {
        return false;
    }

    sessionFile << username << "\n";
    return sessionFile.good();
}

std::optional<std::string> session::getSession() {
    fs::path sessionPath = getSessionPath();

    if (!fs::exists(sessionPath)) {
        return std::nullopt;
    }

    std::ifstream sessionFile(sessionPath);
    if (!sessionFile) {
        return std::nullopt;
    }

    std::string username;
    sessionFile >> username;
    return username;
}
