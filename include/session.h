#pragma once

#include <filesystem>
#include <optional>
#include <string>

namespace session {
std::filesystem::path getSessionPath();
bool saveSession(const std::string &username);
std::optional<std::string> getSession();
};  // namespace session
