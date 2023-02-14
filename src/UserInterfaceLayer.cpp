//
// Created by Godfrey Bafana on 2023/02/09.
//

#include "../include/UserInterfaceLayer.h"
#include "vector"

const int NUM_COLS = sizeof(TaskStatus);
const int CARD_INNER_HEIGHT = 8;               // y-axis printable portion of card
const int PADDING_TOP = 3;                     // padding between top of the screen and first card (includes screen border and lane title text)
const int CARD_HEIGHT = CARD_INNER_HEIGHT + 2; // Fixed as these cards roll off the bottom of the screen. Width is dynamic based on screen
const int BORDER_CHAR = 1;
const int PADDING_CARD_TEXT_Y = BORDER_CHAR;
const int PADDING_CARD_TEXT_X = BORDER_CHAR + 1;
const int PADDING_CARD_OUTER = 1; // vertical spacing between cards

void UserInterfaceLayer::Start()
{
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();

    // Get the size of the parent screen
    int parent_height, parent_width;
    getmaxyx(stdscr, parent_height, parent_width);

    // Create a menu window with minimum size of half the parent screen
    int menu_height = std::max(parent_height * 3 / 4, 10);
    int menu_width = std::max(parent_width * 3 / 4, 40);
    int menu_y = (parent_height - menu_height) / 2;
    int menu_x = (parent_width - menu_width) / 2;
    WINDOW *menu_win = newwin(menu_height, menu_width, menu_y, menu_x);

    // Render the menu options.
    RenderMenu(menu_win);

    // Wait for user input and handle menu options
    int choice;
    while ((choice = wgetch(menu_win)) != '4')
    {
        switch (choice)
        {
        case '1':
            // Open the task manager
            RenderTaskManager();
            RenderMenu(menu_win);
            break;
        case '2':
            // Handle option 2
            break;
        case '3':
            // Handle option 3
            break;
        default:
            break;
        }
        wrefresh(menu_win);
    }

    // Clean up
    delwin(menu_win);
    endwin();
    refresh();
}

void UserInterfaceLayer::RenderMenu(WINDOW *menu_win)
{
    refresh();
    wrefresh(menu_win);
    init_pair(8, COLOR_CYAN, COLOR_BLACK);
    int distance_from_center = (getmaxx(menu_win) - 18) / 2;
    int ascii_title_offset_y = 40;
    int ascii_title_offset_x = 7;

    // Display the menu options ref: https://patorjk.com/software/taag/#p=display&h=0&f=Graceful&t=Productivity%20App%20
    wattron(menu_win, COLOR_PAIR(8));
    mvwprintw(menu_win, ++ascii_title_offset_x, distance_from_center - ascii_title_offset_y, " ____  ____   __   ____  _  _   ___  ____   __   _  _   __   ____  _  _         __   ____  ____ \n");
    mvwprintw(menu_win, ++ascii_title_offset_x, distance_from_center - ascii_title_offset_y, "(  _ \\(  _ \\ /  \\ (    \\/ )( \\ / __)(_  _) (  ) / )( \\ (  ) (_  _)( \\/ )       / _\\ (  _ \\(  _ \\ \n");
    mvwprintw(menu_win, ++ascii_title_offset_x, distance_from_center - ascii_title_offset_y, " ) __/ )   /(  O ) ) D () \\/ (( (__   )(    )(  \\ \\/ /  )(    )(   )  /       /    \\ ) __/ ) __/ \n");
    mvwprintw(menu_win, ++ascii_title_offset_x, distance_from_center - ascii_title_offset_y, "(__)  (__\\_) \\__/ (____/\\____/ \\___) (__)  (__)  \\__/  (__)  (__) (__/        \\_/\\_/(__)  (__)\n");
    wattroff(menu_win, COLOR_PAIR(8));
    wrefresh(menu_win);
    refresh();

    mvwprintw(menu_win, getmaxy(menu_win) / 2 - 1, distance_from_center - 5, "1. Open Task Manager ");
    mvwprintw(menu_win, getmaxy(menu_win) / 2 + 1, distance_from_center - 5, "2. Calendar - (coming soon) ");
    mvwprintw(menu_win, getmaxy(menu_win) / 2 + 3, distance_from_center - 5, "3. Planner - (Coming soon) ");
    mvwprintw(menu_win, getmaxy(menu_win) / 2 + 5, distance_from_center - 5, "4. Exit ");
    box(menu_win, 0, 0);
    wrefresh(menu_win);
}

void UserInterfaceLayer::ReloadTasks(WINDOW *win, TaskStatus status, int column)
{
    const std::string left_arrow = "<<<";
    const std::string right_arrow = ">>>";
    std::cerr << "length of right arrow" << right_arrow.length() << std::endl;
    std::vector<Task> tasks = business_logic_layer.GetTasks(status);
    int _, scr_width;
    getmaxyx(stdscr, _, scr_width);

    const int column_width = (int)(scr_width / NUM_COLS); // Divide the screen width by number of columns to find the width of each column
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
            mvwprintw(card, CARD_INNER_HEIGHT, 2, &(const char &)(left_arrow));
        }

        if (column != NUM_COLS)
        {
            mvwprintw(card, CARD_INNER_HEIGHT, (int)(column_width - right_arrow.length() + (PADDING_CARD_TEXT_X * 2) + 1), &(const char &)(right_arrow));
        }
        wattroff(card, COLOR_PAIR(5));

        box(card, 0, 0); // Card borders

        j += CARD_INNER_HEIGHT + 1 + PADDING_CARD_OUTER;
    }

    box(win, 0, 0);
    wrefresh(win);
}

void UserInterfaceLayer::RenderTaskManager()
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

    int scr_height, scr_width;
    getmaxyx(stdscr, scr_height, scr_width);

    TaskStatus task_statuses[4] = {TaskStatus::TODO, TaskStatus::IN_PROGRESS, TaskStatus::IN_REVIEW, TaskStatus::DONE};
    int color_pairs[4] = {1, 2, 3, 4};

    for (const auto &[k, v] : m_lanes)
    {
        const int i = (int)k; // for mapping x-axis start pos for cols
        // build lanes
        m_lanes[k] = newwin(scr_height, (int)(scr_width / NUM_COLS), 0, (int)(scr_width / NUM_COLS) * i);
        wclear(m_lanes[k]);
        box(m_lanes[k], 0, 0);
        wattron(m_lanes[k], COLOR_PAIR(color_pairs[i]));
        mvwprintw(m_lanes[k], PADDING_CARD_TEXT_Y, PADDING_CARD_TEXT_X, BusinessLogicLayer::TaskStatusPrettyPrint(task_statuses[i]));
        wattroff(m_lanes[k], COLOR_PAIR(color_pairs[i]));
        ReloadTasks(m_lanes[k], task_statuses[i], i + 1);
    }

    refresh();

    int ch;
    while ((ch = getch()) != 'q') // BLOCKING
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
        default:
            break;
        }
        ch = getch();
    }
    clear();
    refresh();
}
