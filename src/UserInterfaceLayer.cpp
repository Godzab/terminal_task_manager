//
// Created by Godfrey Bafana on 2023/02/09.
//

#include "UserInterfaceLayer.h"
#include "vector"

void UserInterfaceLayer::Start() {
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

    for (int i = 0; i < 4; i++) {
        WINDOW* status_win = newwin(row, col / 4, 0, (col / 4) * i);
        wclear(status_win);
        box(status_win, 0, 0);
        wattron(status_win, COLOR_PAIR(color_pairs[i]));
        mvwprintw(status_win, 1, 2, BusinessLogicLayer::TaskStatusToString(task_statuses[i]));
        //wbkgd(status_win, COLOR_PAIR(6+i));

        refresh();
        box(status_win, 0, 0);
        wrefresh(status_win);
        RefreshTasks(status_win, task_statuses[i], i+1);
    }




    /*RefreshTasks(todo_win, TaskStatus::TODO);
    RefreshTasks(in_progress_win, TaskStatus::IN_PROGRESS);
    RefreshTasks(blocked_win, TaskStatus::BLOCKED);
    RefreshTasks(done_win, TaskStatus::DONE);*/

    int ch = getch();
    while (ch != 'q') {
        switch (ch) {
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

void UserInterfaceLayer::RefreshTasks(WINDOW * win, TaskStatus status, int column) {
    std::vector<Task> tasks = business_logic_layer.GetTasks(status);
    int row, col, scr_col, scr_row;
    getmaxyx(win, row, col);
    getmaxyx(stdscr, scr_row, scr_col);

    int column_width = scr_col / 4; // Divide the screen width by 4 to find the width of each column
    int j = 3;
    for (unsigned i = 0; i < tasks.size(); i++) {
        WINDOW* task_win = subwin(win, 10, column_width-4, j, (column-1) * column_width + 2);
        Task task = tasks[i];
        j+=10;
        wattron(task_win, COLOR_PAIR(5));
        mvwprintw(task_win, 1, 2, task.name.c_str());
        mvwprintw(task_win, 3, 2, task.description.c_str());
        mvwprintw(task_win, 6, 2, task.created_at.c_str());
        if(column != 1){
            mvwprintw(task_win, 8, 2, "<<<");
        }

        if (column != 4){
            mvwprintw(task_win, 8, column_width-9, ">>>");
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

