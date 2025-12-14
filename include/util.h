#pragma once
#include <ctime>
#include <optional>
#include <print>
#include <string>

#include "database.h"
#include "models.h"
#include "session.h"
#include "tabulate.hpp"

std::optional<TaskStatus> parseTaskStatus(const std::string &str);

int taskStatusToInt(const std::string &str, int defaultValue = 0);

std::time_t parseDate(const std::string &date);

std::string formatDate(std::time_t timestamp);

std::string statusToString(int status);

void styleTable(tabulate::Table &table);
