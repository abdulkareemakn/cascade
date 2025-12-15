#pragma once
#include <ctime>
#include <optional>
#include <string>

#include "models.h"
#include "tabulate.hpp"

constexpr int SECONDS_PER_DAY = 86400;
constexpr int SECONDS_PER_WEEK = 604800;
constexpr int SECONDS_PER_MONTH = 2592000; 

std::optional<TaskStatus> parseTaskStatus(const std::string &str);

int taskStatusToInt(const std::string &str, int defaultValue = 0);

std::time_t parseDate(const std::string &date);

std::string formatDate(std::time_t timestamp);

std::string statusToString(int status);

void styleTable(tabulate::Table &table);

void printStyledTable(tabulate::Table &table);
