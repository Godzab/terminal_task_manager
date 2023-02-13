//
// Created by Godfrey Bafana on 2023/02/09.
//

#include "../include/UserInterfaceLayer.h"
#include "vector"
#include <map>

#define NUM_COLS 4
#define PADDING_TOP 10

void UserInterfaceLayer::Start()
{
    initscr();
    noecho();
    cbreak();
    start_color();

    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(5, COLOR_WHITE, COLOR_BLACK);

    init_pair(6, COLOR_WHITE, COLOR_GREEN);
    init_pair(7, COLOR_WHITE, COLOR_YELLOW);
    init_pair(8, COLOR_WHITE, COLOR_CYAN);
    init_pair(9, COLOR_WHITE, COLOR_MAGENTA);

    int row, col;
    getmaxyx(stdscr, row, col);

    TaskStatus task_statuses[4] = {TaskStatus::TODO, TaskStatus::IN_PROGRESS, TaskStatus::IN_REVIEW, TaskStatus::DONE};
    int color_pairs[4] = {1, 2, 3, 4};

    std::map<TaskStatus, WINDOW *> lanes{{TaskStatus::TODO, nullptr}, {TaskStatus::IN_PROGRESS, nullptr}, {TaskStatus::IN_REVIEW, nullptr}, {TaskStatus::DONE, nullptr}};
    for (auto &[k, v] : lanes)
    {
        int i = (int)k;

        lanes[k] = newwin(row, col / NUM_COLS, 0, (col / NUM_COLS) * i);
        wclear(lanes[k]);
        box(lanes[k], 0, 0);
        wattron(lanes[k], COLOR_PAIR(color_pairs[i]));
        mvwprintw(lanes[k], 1, 2, BusinessLogicLayer::TaskStatusPrettyPrint(task_statuses[i]));
        // wbkgd(lanes[k], COLOR_PAIR(6+i));

        refresh();
        box(lanes[k], 0, 0);
        wrefresh(lanes[k]);
        RefreshTasks(lanes[k], task_statuses[i], i + 1);
    }

    int ch = getch();
    while (ch != 'q')
    {
        switch (ch)
        {
        case KEY_LEFT:
            // Handle navigation to the previous column
            break;
        case KEY_RIGHT:
            // Handle navigation to the next column
            break;
        case KEY_UP:
            // Handle navigation to the previous task
            break;
        case KEY_DOWN:
            // Handle navigation to the next task
            break;
        }
        ch = getch();
    }

    endwin();
}

void UserInterfaceLayer::RefreshTasks(WINDOW *win, TaskStatus status, int column)
{
    std::vector<Task> tasks = business_logic_layer.GetTasks(status);
    int _, scr_col;
    getmaxyx(win, _, _); // TODO why is this here?
    getmaxyx(stdscr, _, scr_col);

    int column_width = scr_col / NUM_COLS; // Divide the screen width by number of columns to find the width of each column
    int j = PADDING_TOP;
    for (const auto &task : tasks)
    {
        WINDOW *task_win = subwin(win, 10, column_width - NUM_COLS, j, (column - 1) * column_width + 2);
        j += 10;
        wattron(task_win, COLOR_PAIR(5));
        mvwprintw(task_win, 1, 2, task.name.c_str());
        mvwprintw(task_win, 3, 2, task.description.c_str());
        mvwprintw(task_win, 6, 2, task.created_at.c_str());
        if (column != 1)
        {
            mvwprintw(task_win, 8, 2, "<<<");
        }

        if (column != NUM_COLS)
        {
            mvwprintw(task_win, 8, column_width - 9, ">>>");
        }
        wattroff(task_win, COLOR_PAIR(5));

        refresh();
        box(task_win, 0, 0);
        wrefresh(task_win);
    }

    refresh();
    box(win, 0, 0);
    wrefresh(win);
}