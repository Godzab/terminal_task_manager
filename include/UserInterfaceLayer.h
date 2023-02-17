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
    UserInterfaceLayer();
    ~UserInterfaceLayer() = default;
    void Start();
    void RenderTaskManager();

private:
    BusinessLogicLayer business_logic_layer{};
    std::map<TaskStatus, WINDOW *> m_lanes{{TaskStatus::TODO, nullptr}, {TaskStatus::IN_PROGRESS, nullptr}, {TaskStatus::IN_REVIEW, nullptr}, {TaskStatus::DONE, nullptr}};
    void RefreshTasks(WINDOW *, TaskStatus, int);
    static void RenderMenu(WINDOW *menu_win);
    static void RenderNewTaskForm(WINDOW *form_win);
    static std::string GetCurrentTimestamp();
    static WINDOW *createSubWindow() ;
    void RenderTaskCards();
    static void printAsciiArt(WINDOW *win, int distance_from_center, int ascii_title_offset_y, int ascii_title_offset_x);
    static void
    collectUserInput(WINDOW *form_win, std::vector<std::tuple<std::string, std::string, int>> &fields, int form_height,
                     int y, int max_x, int x);
    static void printTaskCard(int column, int column_width, int j, const Task &task) ;
};

#endif // TASKMANAGERTERMINAL_USERINTERFACELAYER_H
