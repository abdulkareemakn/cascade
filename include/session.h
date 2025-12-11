#pragma once

#include <filesystem>
#include <optional>

namespace session {
std::filesystem::path getSessionPath();
bool saveSession(const std::string &username);
std::optional<int> getCurrentUser();
};  // namespace session
