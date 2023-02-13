//
// Created by Godfrey Bafana on 2023/02/09.
//

#include "../include/BusinessLogicLayer.h"
#include "vector"
#include "sqlite3.h"

bool BusinessLogicLayer::CreateTask(const std::string &name, const std::string &description,
                                    TaskStatus status, const std::string &objective)
{

    sqlite3_stmt *sql;
    if (!data_layer.PrepareStatement("INSERT INTO tasks (name, description, status, objective) VALUES (?, ?, ?, ?)", &sql)) {
        std::cerr << "Error preparing SQL statement." << std::endl;
    }

    sqlite3_bind_text(sql, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(sql, 2, description.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(sql, 3, TaskStatusToString(status), -1, SQLITE_STATIC);
    sqlite3_bind_text(sql, 4, objective.c_str(), -1, SQLITE_STATIC);

    if (!data_layer.ExecuteSQL(sql))
    {
        return false;
    }

    data_layer.Close();
    return true;
}

bool BusinessLogicLayer::UpdateTaskStatus(int task_id, TaskStatus status)
{
    std::string sql = "UPDATE tasks SET status = '" + std::string(BusinessLogicLayer::TaskStatusToString(status)) +
                      "' WHERE id = " + std::to_string(task_id) + ";";

    if (!data_layer.ExecuteSQL(sql))
    {
        return false;
    }

    data_layer.Close();
    return true;
}

std::vector<Task> BusinessLogicLayer::GetTasks(TaskStatus status)
{
    std::vector<Task> tasks;
    std::string sql = "SELECT name, description, status, created_at, objective FROM tasks WHERE status = " + std::to_string(static_cast<double>(status)) + ";";
    data_layer.GetResults(sql, taskCallback, &tasks);
    return tasks;
}

char *BusinessLogicLayer::TaskStatusToString(TaskStatus status)
{
    switch (status)
    {
    case TaskStatus::TODO:
        return const_cast<char *>("TODO");
    case TaskStatus::IN_PROGRESS:
        return const_cast<char *>("IN_PROGRESS");
    case TaskStatus::IN_REVIEW:
        return const_cast<char *>("IN_REVIEW");
    case TaskStatus::DONE:
        return const_cast<char *>("DONE");
    }
    return const_cast<char *>("");
}

char *BusinessLogicLayer::TaskStatusPrettyPrint(TaskStatus status)
{
    switch (status)
    {
    case TaskStatus::TODO:
        return const_cast<char *>("To do");
    case TaskStatus::IN_PROGRESS:
        return const_cast<char *>("In Progress");
    case TaskStatus::IN_REVIEW:
        return const_cast<char *>("In Review");
    case TaskStatus::DONE:
        return const_cast<char *>("Done!");
    }
    return const_cast<char *>("");
}

int BusinessLogicLayer::taskCallback(void *arg, [[maybe_unused]] int argc, char **argv, [[maybe_unused]] char **azColName)
{
    auto *tasks = static_cast<std::vector<Task> *>(arg);
    Task task;
    task.name = argv[0];
    task.description = argv[1];
    task.status = std::stoi(argv[2]);
    task.created_at = argv[3];
    task.objective = argv[4];
    tasks->push_back(task);
    return 0;
}