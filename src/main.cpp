#include <ctime>
#include <exception>
#include <iostream>
#include <print>
#include <string>

#include "CLI11.hpp"
#include "PriorityQueue.h"
#include "commands.h"
#include "database.h"
#include "models.h"
#include "repository.h"
#include "sorting.h"
#include "tabulate.hpp"
#include "util.h"

constexpr const char *VERSION = "1.0.0";

int main(int argc, char **argv) {
    CLI::App app{
        "Cascade - A Personal Task Manager\n"
        "Manage tasks with priorities and deadlines.",
        "cascade"};
    app.require_subcommand(0, 1);
    app.set_version_flag("-v,--version", VERSION);

    db::initDatabase();

    std::string username;
    CommandArgs args;

    std::string banner =
        " _____                         _      \n"
        "/  __ \\                       | |     \n"
        "| /  \\/ __ _ ___  ___ __ _  __| | ___ \n"
        "| |    / _` / __|/ __/ _` |/ _` |/ _ \\\n"
        "| \\__/\\ (_| \\__ \\ (_| (_| | (_| |  __/\n"
        " \\____/\\__,_|___/\\___\\__,_|\\__,_|\\___|\n";

    std::println("{}", banner);

    if (!db::hasUser()) {
        std::println("Welcome to Cascade");
        std::print("What should we call you: ");
        std::cin >> username;
        db::createUser(username);
    }

    User user = db::getUser();


    auto *whoami =
        app.add_subcommand("whoami", "Display current user information");
    whoami->callback(
        [&user]() { std::println("Username: {}", user.username); });


    auto *task = app.add_subcommand("task",
                                    "Task management commands\n"
                                    "Create, view, update, and delete tasks.");

    auto *task_add = task->add_subcommand(
        "add",
        "Create a new task\n"
        "Examples:\n"
        "  cascade task add \"Buy groceries\"\n"
        "  cascade task add \"Finish report\" -p 1 --due tomorrow\n"
        "  cascade task add \"Review PR\" --priority 2 --due 2025-12-20");

    task_add->add_option("title", args.task.title, "Title of the task")
        ->required();
    task_add->add_option("-p,--priority", args.task.priority,
                         "Priority level (1=highest, 4=lowest). Default: 2");
    task_add->add_option(
        "-s,--status", args.task.status,
        "Initial status: todo | in_progress | complete | wont_do. Default: todo");
    task_add->add_option(
        "--due", args.task.dueDate,
        "Due date: YYYY-MM-DD");

    task_add->callback([&args]() {
        db::createTask(args.task.title, args.task.priority,
                       taskStatusToInt(args.task.status),
                       parseDate(args.task.dueDate));
        std::println("Created task: {}", args.task.title);
    });

    auto *task_list = task->add_subcommand(
        "list",
        "List tasks with optional filters and sorting\n"
        "By default, shows only incomplete tasks (TODO, IN_PROGRESS).\n"
    );

    task_list->add_flag("--all", args.task.showAll,
                        "Include completed and cancelled tasks");
    task_list->add_flag("--status", args.task.sortByStatus,
                        "Sort tasks by status");
    task_list->add_flag("--priority", args.task.sortByPriority,
                        "Sort tasks by priority level");
    task_list->add_option("--sort", args.task.sortBy,
                          "Sort by: priority, date (due date), or created");

    task_list->callback([&args]() {
        repo::listTasks(args.task.showAll, args.task.sortByStatus,
                        args.task.sortByPriority, args.task.sortBy);
    });


    auto *task_show =
        task->add_subcommand("show",
                             "Show detailed information about a specific task\n"
        );

    task_show->add_option("id", args.task.taskId, "Task ID to display")
        ->required();

    task_show->callback([&args]() { repo::showTask(args.task.taskId); });


    auto *task_next = task->add_subcommand(
        "next",
        "Get the next task to work on based on priority and due date\n"
        "Priority is determined by: (1) priority level, (2) due date");

    task_next->callback([]() { repo::getNextPriorityTask(); });


    auto *task_update = task->add_subcommand(
        "update",
        "Update properties of an existing task\n"
    );

    task_update->add_option("id", args.task.taskId, "Task ID to update")
        ->required();
    task_update->add_option("--priority", args.task.updatePriority,
                            "New priority level (1-4)");
    task_update->add_option("--status", args.task.updateStatus,
                            "New status: todo | in_progress | complete | wont_do");
    task_update->add_option(
        "--due", args.task.updateDueDate,
        "New due date: YYYY-MM-DD");
    task_update->add_option("--title", args.task.updateTitle, "New title");

    task_update->callback([&args]() {
        bool updated = false;
        if (args.task.updatePriority >= 0) {
            repo::updateTaskPriority(args.task.taskId,
                                     args.task.updatePriority);
            updated = true;
        }
        if (!args.task.updateStatus.empty()) {
            repo::updateTaskStatus(args.task.taskId,
                                   taskStatusToInt(args.task.updateStatus, -1));
            updated = true;
        }
        if (!args.task.updateDueDate.empty()) {
            repo::updateTaskDueDate(args.task.taskId,
                                    parseDate(args.task.updateDueDate));
            updated = true;
        }
        if (!args.task.updateTitle.empty()) {
            repo::updateTaskTitle(args.task.taskId, args.task.updateTitle);
            updated = true;
        }
        if (!updated) {
            std::println(
                "No updates specified. Use --priority, --status, --due, or "
                "--title.");
        }
    });


    auto *task_delete = task->add_subcommand(
        "delete",
        "Permanently delete a task\n"
    );

    task_delete->add_option("id", args.task.taskId, "Task ID to delete")
        ->required();

    task_delete->callback([&args]() { repo::deleteTask(args.task.taskId); });


    auto *task_done = task->add_subcommand(
        "done",
        "Mark a task as complete (shortcut for update --status complete)\n"
    );

    task_done->add_option("id", args.task.taskId, "Task ID to mark as done")
        ->required();

    task_done->callback([&args]() {
        repo::updateTaskStatus(args.task.taskId,
                               static_cast<int>(TaskStatus::COMPLETE));
    });


    auto *task_start =
        task->add_subcommand("start",
                             "Mark a task as in progress (shortcut for update "
                             "--status in_progress)\n"
        );

    task_start->add_option("id", args.task.taskId, "Task ID to start")
        ->required();

    task_start->callback([&args]() {
        repo::updateTaskStatus(args.task.taskId,
                               static_cast<int>(TaskStatus::IN_PROGRESS));
    });

    CLI11_PARSE(app, argc, argv);

    return 0;
}