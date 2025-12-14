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
        return now + SECONDS_PER_DAY;
    }
    if (date == "next-week") {
        return now + SECONDS_PER_WEEK;
    }
    if (date == "next-month") {
        return now + SECONDS_PER_MONTH;
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
    // Option 7: Header-only border style
    // Configure table-level format
    table.format()
        .border_top("")
        .border_left("")
        .border_right("")
        .border_bottom("")
        .corner("")
        .column_separator("│");

    if (table.size() > 0) {
        // Style header row: bold
        table[0].format().font_style({tabulate::FontStyle::bold});

        // Add separator below header using border_top on second row
        if (table.size() > 1) {
            table[1]
                .format()
                .border_top("─")
                .corner_top_left("")
                .corner_top_right("");
        }
    }
}

void printStyledTable(tabulate::Table &table) {
    styleTable(table);
    std::cout << table << "\n";
}
