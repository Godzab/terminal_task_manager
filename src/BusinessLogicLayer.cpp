//
// Created by Godfrey Bafana on 2023/02/09.
//

#include "../include/BusinessLogicLayer.h"
#include "vector"

bool BusinessLogicLayer::CreateTask(const std::string& name, const std::string& description,
                TaskStatus status, const std::string& objective) {

    std::string sql = "INSERT INTO tasks (name, description, status, objective) ";
    sql += "VALUES ('" + name + "', '" + description + "', '" +
           TaskStatusToString(status) + "', '" + objective + "');";

    if (!data_layer.ExecuteSQL(sql)) {
        return false;
    }

    data_layer.Close();
    return true;
}

bool BusinessLogicLayer::UpdateTaskStatus(int task_id, TaskStatus status) {
    std::string sql = "UPDATE tasks SET status = '" + std::string (BusinessLogicLayer::TaskStatusToString(status)) +
                      "' WHERE id = " + std::to_string(task_id) + ";";

    if (!data_layer.ExecuteSQL(sql)) {
        return false;
    }

    data_layer.Close();
    return true;
}

std::vector<Task> BusinessLogicLayer::GetTasks(TaskStatus status) {
    std::vector<Task> tasks;
    std::string sql = "SELECT name, description, status, created_at, objective FROM tasks WHERE status = "+std::to_string(
            static_cast<double>(status)) + ";";
    data_layer.GetResults(sql, taskCallback, &tasks);
    return tasks;
}

char * BusinessLogicLayer::TaskStatusToString(TaskStatus status) {
    switch (status) {
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

int BusinessLogicLayer::taskCallback(void *arg, [[maybe_unused]] int argc, char **argv, [[maybe_unused]] char **azColName) {
    auto *tasks = static_cast<std::vector<Task>*>(arg);
    Task task;
    task.name = argv[0];
    task.description = argv[1];
    task.status = std::stoi(argv[2]);
    task.created_at = argv[3];
    task.objective = argv[4];
    tasks->push_back(task);
    return 0;
}