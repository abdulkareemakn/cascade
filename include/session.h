#pragma once

#include <filesystem>
#include <optional>

namespace session {
std::filesystem::path getSessionPath();
bool login(int userId);
bool logout();
std::optional<int> getCurrentUser();
bool isLoggedIn();
};  // namespace session
