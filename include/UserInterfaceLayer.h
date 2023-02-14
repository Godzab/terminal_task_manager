//
// Created by Godfrey Bafana on 2023/02/09.
//

#ifndef TASKMANAGERTERMINAL_USERINTERFACELAYER_H
#define TASKMANAGERTERMINAL_USERINTERFACELAYER_H

#include <ncurses.h>
#include <map>
#include "BusinessLogicLayer.h"

class UserInterfaceLayer
{
public:
    UserInterfaceLayer() = default;
    ~UserInterfaceLayer() = default;
    void Start();
    void RenderTaskManager();

private:
    BusinessLogicLayer business_logic_layer{};
    std::map<TaskStatus, WINDOW *> m_lanes{{TaskStatus::TODO, nullptr}, {TaskStatus::IN_PROGRESS, nullptr}, {TaskStatus::IN_REVIEW, nullptr}, {TaskStatus::DONE, nullptr}};
    void ReloadTasks(WINDOW *win, TaskStatus status, int column);

    static void RenderMenu(WINDOW *menu_win) ;
};

#endif // TASKMANAGERTERMINAL_USERINTERFACELAYER_H
