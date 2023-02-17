//
// Created by Godfrey Bafana on 2023/02/09.
//

#ifndef TASKMANAGERTERMINAL_USERINTERFACELAYER_H
#define TASKMANAGERTERMINAL_USERINTERFACELAYER_H

#define MAIN_TITLE std::string ascii_art = R"(
================================================================================================
 ____  ____   __   ____  _  _   ___  ____   __   _  _   __   ____  _  _         __   ____  ____
(  _ \(  _ \ /  \ (    \/ )( \ / __)(_  _) (  ) / )( \ (  ) (_  _)( \/ )       / _\ (  _ \(  _ \
 ) __/ )   /(  O ) ) D () \/ (( (__   )(    )(  \ \/ /  )(    )(   )  /       /    \ ) __/ ) __/
(__)  (__\_) \__/ (____/\____/ \___) (__)  (__)  \__/  (__)  (__) (__/        \_/\_/(__)  (__)

================================================================================================
)"

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
    static WINDOW *createSubWindow(WINDOW *pSt);
    void RenderTaskCards();
    static void
    collectUserInput(WINDOW *form_win, std::vector<std::tuple<std::string, std::string, int>> &fields, int form_height,
                     int y, int max_x, int x);
    static void printTaskCard(int column, int column_width, int j, const Task &task) ;

    static void printAsciiArt(WINDOW *win, int distance_from_center, int ascii_title_offset_y, int ascii_title_offset_x,
                       std::string ascii_art);
};

#endif // TASKMANAGERTERMINAL_USERINTERFACELAYER_H
