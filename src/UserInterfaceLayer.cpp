//
// Created by Godfrey Bafana on 2023/02/09.
//

#include <sstream>
#include <array>
#include "../include/UserInterfaceLayer.h"
#include "vector"
#include "thread"

#define NUM_COLS sizeof(TaskStatus)
#define CARD_INNER_HEIGHT 8               // y axis printable portion of card
#define PADDING_TOP 3                     // padding between top of the screen and first card (includes screen border and lane title text)
#define CARD_HEIGHT (CARD_INNER_HEIGHT + 2) // Fixed as these cards roll off the bottom of the screen. Width is dynamic based on screen
#define BORDER_CHAR 1
#define PADDING_CARD_TEXT_Y (BORDER_CHAR)
#define PADDING_CARD_TEXT_X (BORDER_CHAR + 1)

enum MenuOption {
    TASK_MANAGER = '1',
    CALENDAR = '2',
    PLANNER = '3',
    EXIT = '4'
};

UserInterfaceLayer::UserInterfaceLayer() {
    initscr();
    noecho();
    cbreak();
    start_color();

    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(5, COLOR_WHITE, COLOR_BLACK);
    init_pair(6, COLOR_BLUE, COLOR_BLACK);
}

void UserInterfaceLayer::Start() {
    // Initialize ncurses
    if (initscr() == nullptr) {
        throw std::runtime_error("Unable to initialize ncurses");
    }
    cbreak();
    noecho();

    // Get the size of the parent screen
    const std::unique_ptr<WINDOW, decltype(&delwin)> menu_win(createSubWindow(stdscr), &delwin);

    // Render the menu options.
    RenderMenu(menu_win.get());

    // Wait for user input and handle menu options
    int choice;
    while ((choice = wgetch(menu_win.get())) != MenuOption::EXIT) {
        switch (choice) {
            case MenuOption::TASK_MANAGER:
                // Open the task manager
                clear();
                RenderTaskManager();
                RenderMenu(menu_win.get());
                break;
            case MenuOption::CALENDAR:
            case MenuOption::PLANNER:
            default:
                break;
        }
    }

    // Clean up
    menu_win.get_deleter()(menu_win.get());
    clear();
    endwin();
    refresh();
}

WINDOW *UserInterfaceLayer::createSubWindow(WINDOW *parent) {
    int parent_height, parent_width;
    getmaxyx(parent, parent_height, parent_width);

    // Create a menu window with minimum size of half the parent screen
    int menu_height = std::max(parent_height * 3 / 4, 10);
    int menu_width = std::max(parent_width * 3 / 4, 40);
    int menu_y = (parent_height - menu_height) / 2;
    int menu_x = (parent_width - menu_width) / 2;

    return newwin(menu_height, menu_width, menu_y, menu_x);
}

void UserInterfaceLayer::RenderMenu(WINDOW *menu_win) {
    int distance_from_center = (getmaxx(menu_win) - 18) / 2;
    int ascii_title_offset_y = 40;
    int ascii_title_offset_x = 7;
    MAIN_TITLE;
    printAsciiArt(menu_win, distance_from_center, ascii_title_offset_y, ascii_title_offset_x, ascii_art);

    std::vector<std::string> options = {
            "1. Open Task Manager",
            "2. Calendar - (coming soon)",
            "3. Planner - (coming soon)",
            "4. Exit"
    };

    int y = getmaxy(menu_win) / 2 + 5;
    int x = distance_from_center + 12;
    for (size_t i = 0; i < options.size(); i++) {
        mvprintw(y + 2 * i, x, options[i].c_str());
    }

    box(menu_win, 0, 0);
    wrefresh(menu_win);
    refresh();
}

void UserInterfaceLayer::printAsciiArt(WINDOW *win, int distance_from_center, int ascii_title_offset_y, int ascii_title_offset_x, std::string ascii_art) {
    wattron(win, COLOR_PAIR(1));
    std::istringstream iss(ascii_art);
    std::string line;
    while (std::getline(iss, line)) {
        mvwprintw(win, ascii_title_offset_x, distance_from_center - ascii_title_offset_y, "%s", line.c_str());
        ascii_title_offset_x++;
    }
    wattroff(win, COLOR_PAIR(1));
    wrefresh(win);
    refresh();
}


void UserInterfaceLayer::RefreshTasks(WINDOW *win, TaskStatus status, int column) {
    std::vector<Task> tasks = business_logic_layer.GetTasks(status);
    [[maybe_unused]] int column_height, column_width;
    getmaxyx(win, column_height, column_width);

    int j = PADDING_TOP + 7;
    for (const auto &task: tasks) {
        printTaskCard(column, column_width, j, task);
        j += CARD_HEIGHT; // include line and
    }

    refresh();
    box(win, 0, 0);
    wrefresh(win);
}

void UserInterfaceLayer::printTaskCard(int column, int column_width, int j, const Task &task) {
    const char left_arrow[4] = "<<<";
    const char right_arrow[4] = ">>>";

    WINDOW *card = subwin(stdscr, CARD_HEIGHT, column_width - 2, j, column * column_width + 6);
    wattron(card, COLOR_PAIR(5));
    mvwprintw(card, PADDING_CARD_TEXT_Y, PADDING_CARD_TEXT_X, task.name.c_str());
    mvwprintw(card, PADDING_CARD_TEXT_Y + 2, PADDING_CARD_TEXT_X, task.description.c_str());
    mvwprintw(card, PADDING_CARD_TEXT_Y + 5, PADDING_CARD_TEXT_X, task.created_at.c_str());
    if (column != 1) {
        mvwprintw(card, CARD_INNER_HEIGHT, 2, left_arrow);
    }

    if (column != NUM_COLS) {
        mvwprintw(card, CARD_INNER_HEIGHT, column_width - (sizeof(right_arrow) + (PADDING_CARD_TEXT_X * 2) + 1),
                  right_arrow);
    }
    wattroff(card, COLOR_PAIR(5));

    box(card, 0, 0);
    wrefresh(card);
}

void UserInterfaceLayer::RenderTaskManager() {
    int scr_height, scr_width;
    getmaxyx(stdscr, scr_height, scr_width);
    WINDOW *header_win = subwin(stdscr,6, scr_width - 10, 1,5);
    wattron(header_win, COLOR_PAIR(2));
    wattron(header_win, A_BOLD);
    mvwprintw(header_win, 2, 2, "TERMINAL KANBAN");
    wattroff(header_win, A_BOLD);
    wattron(header_win, COLOR_PAIR(6));
    mvwprintw(header_win, 4, 2, "Q - Back to menu  |   A - Add Task");
    wattroff(header_win, COLOR_PAIR(6));
    wattroff(header_win, COLOR_PAIR(2));
    box(header_win, 0,0);
    wrefresh(header_win);
    refresh();

    RenderTaskCards();
    auto form_win = createSubWindow(stdscr);
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
            case 'a':
                RenderNewTaskForm(form_win);
                break;
            default:
                break;
        }
        RenderTaskCards();
        form_win = createSubWindow(stdscr);
        ch = getch();
    }
    clear();
    refresh();
}

void UserInterfaceLayer::RenderTaskCards() {
    int scr_height, scr_width;
    getmaxyx(stdscr, scr_height, scr_width);
    int parent_height = std::max(scr_height - 8, 10);
    int parent_width = std::max(scr_width - 10, 10);

    std::array<TaskStatus, 4> task_statuses = {TaskStatus::TODO, TaskStatus::IN_PROGRESS, TaskStatus::IN_REVIEW,
                                               TaskStatus::DONE};
    std::array<int, 4> color_pairs = {1, 2, 3, 4};

    for (auto &[k, v]: m_lanes) {
        int i = (int) k; // for mapping x-axis start pos for cols
        // build lanes

        m_lanes[k] = subwin(stdscr, parent_height, parent_width / NUM_COLS, 7, (parent_width / NUM_COLS) * i + 5);
        wclear(m_lanes[k]);
        box(m_lanes[k], 0, 0);
        wattron(m_lanes[k], COLOR_PAIR(color_pairs[i]));
        mvwprintw(m_lanes[k], PADDING_CARD_TEXT_Y, PADDING_CARD_TEXT_X,
                  BusinessLogicLayer::TaskStatusPrettyPrint(task_statuses[i]));
        wattroff(m_lanes[k], COLOR_PAIR(color_pairs[i]));
        RefreshTasks(m_lanes[k], task_statuses[i], i);
    }
}

std::string UserInterfaceLayer::GetCurrentTimestamp() {
    std::time_t current_time = std::time(nullptr);
    std::tm local_time = *std::localtime(&current_time);
    char buffer[20];
    std::strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", &local_time);
    return {buffer};
}

void UserInterfaceLayer::RenderNewTaskForm(WINDOW *form_win) {
    clear();
    // Define form fields
    std::vector<std::tuple<std::string, std::string, int>> fields = {
            {"Name: ",        "", 0},
            {"Description: ", "", 0},
            {"Status: ",      "", 16},
            {"Objective: ",   "", 0},
            {"Created At: ",  "", 0}
    };

    // Calculate field positions
    int form_height = getmaxy(form_win) - 2;
    int y = form_height / 2 - fields.size();
    int max_x = getmaxx(form_win) - 2;
    int x = max_x / 2 - 8;

    wattron(form_win, COLOR_PAIR(2));
    wattron(form_win, A_BOLD);
    mvwprintw(form_win, 7, x - 5, "ENTER TASK DETAILS");
    mvwprintw(form_win, 8, x - 5, "===================");
    wattroff(form_win, COLOR_PAIR(2));
    wattroff(form_win, A_BOLD);

    // Render form fields
    for (auto &field: fields) {
        std::string prompt = std::get<0>(field);
        std::string &input = std::get<1>(field);
        int input_size = std::get<2>(field);
        wattron(form_win, A_BOLD);
        mvwprintw(form_win, y, x - prompt.length(), prompt.c_str());
        wattroff(form_win, A_BOLD);

        if (input_size == 0) {
            if (prompt == "Created At: ") {
                input = UserInterfaceLayer::GetCurrentTimestamp();
            } else {
                input_size = max_x - prompt.length() - 1;
            }
        }

        if (input_size > 0) {
            mvwhline(form_win, y, x + 2, ' ', input_size);
        }

        mvwprintw(form_win, y, x + 2, input.c_str());

        y += 2;
    }

    box(form_win, 0, 0);
    refresh();
    wrefresh(form_win);

    collectUserInput(form_win, fields, form_height, y, max_x, x);

    // Save new task to database
    BusinessLogicLayer bll;
    std::string name = std::get<1>(fields[0]);
    std::string description = std::get<1>(fields[1]);
    TaskStatus status = TaskStatus::TODO;
    std::string objective = std::get<1>(fields[3]);
    std::string created_at = std::get<1>(fields[4]);
    bll.CreateTask(name, description, status, objective, created_at);

    // Clean up
    wclear(form_win);
}

void
UserInterfaceLayer::collectUserInput(WINDOW *form_win, std::vector<std::tuple<std::string, std::string, int>> &fields,
                                     int form_height, int y, int max_x, int x) {// Get user input
    y = form_height / 2 - fields.size();
    for (auto &field: fields) {
        std::string prompt = std::get<0>(field);
        std::string &input = std::get<1>(field);
        int input_size = std::get<2>(field);

        if (input_size == 0) {
            input_size = max_x - prompt.length() - 1;
        }

        if (prompt != "Created At: ") {
            char *input_buffer = new char[input_size + 1]();
            wmove(form_win, y, x + 2);
            echo();
            wgetnstr(form_win, input_buffer, input_size);
            noecho();

            input = input_buffer;
            delete[] input_buffer;
        }

        y += 2;
    }
}

