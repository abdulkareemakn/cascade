#include "util.h"

#include <ctime>
#include <iomanip>
#include <optional>
#include <sstream>
#include <string>

#include "models.h"

std::optional<TaskStatus> parseTaskStatus(const std::string &str) {
    if (str == "todo") return TaskStatus::TODO;
    if (str == "in_progress") return TaskStatus::IN_PROGRESS;
    if (str == "complete") return TaskStatus::COMPLETE;
    if (str == "wont_do") return TaskStatus::WONT_DO;
    return std::nullopt;
}

int taskStatusToInt(const std::string &str, int defaultValue) {
    auto status = parseTaskStatus(str);
    if (status.has_value()) {
        return static_cast<int>(status.value());
    }
    return defaultValue;
}

std::time_t parseDate(const std::string &date) {
    // Handle relative dates
    std::time_t now = std::time(nullptr);

    if (date == "today") {
        return now;
    }
    if (date == "tomorrow") {
        return now + 86400;  // 24 * 60 * 60
    }
    if (date == "next-week") {
        return now + 604800;  // 7 * 24 * 60 * 60
    }
    if (date == "next-month") {
        return now + 2592000;  // 30 * 24 * 60 * 60
    }

    // Parse YYYY-MM-DD format
    std::tm tm = {};
    std::istringstream ss(date);
    ss >> std::get_time(&tm, "%Y-%m-%d");

    if (ss.fail()) {
        return now;
    }
    return std::mktime(&tm);
}

std::string formatDate(std::time_t timestamp) {
    if (timestamp == 0) {
        return "Not set";
    }

    std::tm *tm = std::localtime(&timestamp);
    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%d");
    return oss.str();
}

std::string statusToString(int status) {
    switch (status) {
        case 0:
            return "TODO";
        case 1:
            return "IN_PROGRESS";
        case 2:
            return "COMPLETE";
        case 3:
            return "WONT_DO";
        default:
            return "UNKNOWN";
    }
}

void styleTable(tabulate::Table &table) {
    // Remove all borders for a clean, minimal look
    table.format()
        .border_top("")
        .border_bottom("")
        .border_left("")
        .border_right("")
        .corner("")
        .column_separator("  ");

    // Style header row (first row bodies) - bold with underline border
    if (table.size() > 0) {
        table[0].format().font_style({tabulate::FontStyle::bold});
    }
}
