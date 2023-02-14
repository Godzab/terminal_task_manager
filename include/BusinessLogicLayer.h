//
// Created by Godfrey Bafana on 2023/02/09.
//

#ifndef TASKMANAGERTERMINAL_BUSINESSLOGICLAYER_H
#define TASKMANAGERTERMINAL_BUSINESSLOGICLAYER_H
#include "DataSourceLayer.h"
#include "vector"

enum class TaskStatus
{
    TODO,
    IN_PROGRESS,
    IN_REVIEW,
    DONE
};

struct Task
{
    std::string name;
    std::string description;
    int status;
    std::string created_at;
    std::string objective;
};

class BusinessLogicLayer
{
public:
    BusinessLogicLayer() = default;
    ~BusinessLogicLayer() = default;
    bool CreateTask(const std::string &, const std::string &, TaskStatus, const std::string &);
    bool UpdateTaskStatus(int, TaskStatus);
    std::vector<Task> GetTasks(TaskStatus);
    static char *TaskStatusToString(TaskStatus);
    static char *TaskStatusPrettyPrint(TaskStatus);

private:
    DataSourceLayer data_layer{};
    // Callback function to retrieve data from the tasks table
    static int taskCallback(void *, int, char **, char **);
};

#endif // TASKMANAGERTERMINAL_BUSINESSLOGICLAYER_H
