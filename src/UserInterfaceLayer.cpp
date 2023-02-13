//
// Created by Godfrey Bafana on 2023/02/09.
//

#include "../include/UserInterfaceLayer.h"
#include "vector"

#define NUM_COLS sizeof(TaskStatus)
#define CARD_INNER_HEIGHT 8               // y axis printable portion of card
#define PADDING_TOP 3                     // padding between top of the screen and first card (includes screen border and lane title text)
#define CARD_HEIGHT (CARD_INNER_HEIGHT + 2) // Fixed as these cards roll off the bottom of the screen. Width is dynamic based on screen
#define BORDER_CHAR 1
#define PADDING_CARD_TEXT_Y (BORDER_CHAR)  
#define PADDING_CARD_TEXT_X (BORDER_CHAR + 1)
#define PADDING_CARD_OUTER 1              // vertical spacing between cards

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

    int scr_height, scr_width;
    getmaxyx(stdscr, scr_height, scr_width);

    TaskStatus task_statuses[4] = {TaskStatus::TODO, TaskStatus::IN_PROGRESS, TaskStatus::IN_REVIEW, TaskStatus::DONE};
    int color_pairs[4] = {1, 2, 3, 4};

    for (auto &[k, v] : m_lanes)
    {
        int i = (int)k; // for mapping x-axis start pos for cols
        // build lanes
        m_lanes[k] = newwin(scr_height, scr_width / NUM_COLS, 0, (scr_width / NUM_COLS) * i);
        wclear(m_lanes[k]);
        box(m_lanes[k], 0, 0);
        wattron(m_lanes[k], COLOR_PAIR(color_pairs[i]));
        mvwprintw(m_lanes[k], PADDING_CARD_TEXT_Y, PADDING_CARD_TEXT_X, BusinessLogicLayer::TaskStatusPrettyPrint(task_statuses[i]));
        // wbkgd(lanes[k], COLOR_PAIR(6+i));

        refresh();
        box(m_lanes[k], 0, 0);
        wrefresh(m_lanes[k]);
        RefreshTasks(m_lanes[k], task_statuses[i], i + 1);
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
    const char left_arrow[4] = "<<<";
    const char right_arrow[4] = ">>>";
    std::vector<Task> tasks = business_logic_layer.GetTasks(status);
    int _, scr_width;
    getmaxyx(win, _, _); // TODO why is this here?
    getmaxyx(stdscr, _, scr_width);

    int column_width = scr_width / NUM_COLS; // Divide the screen width by number of columns to find the width of each column
    int j = PADDING_TOP;
    for (const auto &task : tasks)
    {
        WINDOW *card = subwin(win, CARD_HEIGHT, column_width - (PADDING_CARD_TEXT_X * 2), j, (column - 1) * column_width + 2);
        wattron(card, COLOR_PAIR(5));
        mvwprintw(card, PADDING_CARD_TEXT_Y, PADDING_CARD_TEXT_X, task.name.c_str());
        mvwprintw(card, PADDING_CARD_TEXT_Y + 2, PADDING_CARD_TEXT_X, task.description.c_str());
        mvwprintw(card, PADDING_CARD_TEXT_Y + 5, PADDING_CARD_TEXT_X, task.created_at.c_str());
        if (column != 1)
        {
            mvwprintw(card, CARD_INNER_HEIGHT, 2, left_arrow);
        }

        if (column != NUM_COLS)
        {
            mvwprintw(card, CARD_INNER_HEIGHT, column_width - (sizeof(right_arrow) + (PADDING_CARD_TEXT_X * 2) + 1), right_arrow);
        }
        wattroff(card, COLOR_PAIR(5));

        refresh();
        box(card, 0, 0);
        wrefresh(card);

        j += CARD_INNER_HEIGHT + 1 + PADDING_CARD_OUTER; // include line and
    }

    refresh();
    box(win, 0, 0);
    wrefresh(win);
}
