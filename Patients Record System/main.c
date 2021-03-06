#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "curses.h"
#include "ui.h"
#include "user.h"
#include "report.h"

#define ADMIN_PW "BigSecret!"

/* Declare functions before use. */
void welcomeMessage();
void homePage();
bool admin_login();
void admin_screen();
User *patient_login();
void show_patients();
void display_reports(User *u);

int main()
{
    initscr(); // Initialize the curses library.
    int cursor_setting = curs_set(0);
    noecho(); // Typed characters are not automatically printed on the screen.
    welcomeMessage();
    load_users();
    load_reports();
    homePage();
    curs_set(cursor_setting); // Back to normal.
    echo(); // Back to normal.
    endwin(); // Close the curses library.
    return 0;
}

void welcomeMessage()
{
    headMessage("Welcome");
    move(11, 0);
    printw("\n\t\t\t\t\t     ��                       ��");
    printw("\n\t\t\t\t\t    �  �                     �  �");
    printw("\n\t\t\t\t\t     ��                       ��");
    printw("\n\t\t\t\t\t    ||        WELCOME          ||  ");
    printw("\n\t\t\t\t\t   ||            TO             ||  ");
    printw("\n\t\t\t\t\t  ||      PATIENT'S RECORD       ||  ");
    printw("\n\t\t\t\t\t   ||         SYSTEM            ||  ");
    printw("\n\t\t\t\t\t    ||                         ||  ");
    printw("\n\t\t\t\t\t     �|||||||||������||||||||||�  ");
    printw("\n\t\t\t\t\t              ��    ��");
    printw("\n\t\t\t\t\t               ������");
    printw("\n\t\t\t\t\t  �              ��");
    printw("\n\t\t\t\t\t � �**_**_**_**_��");
    printw("\n\t\t\t\t\t  �");
    printw("\n\n\t\t\t\t\tPress any key to continue.....");
    refresh();
    getch();
    clear();
    refresh();
}
void homePage()
{
    refresh();
    headMessage("HOMEPAGE");
    const char *m[] = { "Admin", "New Patient", "Person Under Monitoring", "Exit", NULL }; // The NULL pointer marks the end of the list.
    bool is_running = true;
    User *u = NULL;
    while (is_running)
    {
        switch (menu(m))
        {
        case 0:
            if (admin_login())
            {
                admin_screen();
            }
            break;
        case 1:
            new_patient();
            break;
        case 2:
            u = patient_login();
            if (u != NULL)
            {
                popUpMessage("NOTE: Just press n for NO", "and y for YES");
                PUM_screen(u);
            }
            else
            {
                show_message("Patient not found");
            }
            break;
        case 3:
            exit(0);
        }
    }
}

bool admin_login()
{
    refresh();
    headMessage("LOGIN");
    char pass[20];
    int width = COLS / 2;
    int height = 7;
    int i = 0;
    bool success = false;
    WINDOW *win = newwin(height, width, (LINES - height) / 2, (COLS - width) / 2);
    wclear(win);
    wborder(win, 0, 0, 0, 0, 0, 0,0,0);
    mvwprintw(win, 3, 2, "Password: ");
    wrefresh(win);
    while (!success)
    {
        curs_set(1);
        noecho();
        mvwscanw(win, 3, 12, "%10s", &pass);
        curs_set(0);
        if (strcmp(ADMIN_PW, pass))
        {
            show_message("Wrong password!");
            i++;
            if (i >= 3)
            {
                show_message("You've reached the times to login.");
                break;
            }
        }
        else
        {
            success = true;
        }
    }
    echo();
    wclear(win);
    wrefresh(win);
    delwin(win);
    return success;
}

void admin_screen()
{
    const char *m[] = {"Patients", /*"Change Admin Details",*/  "Exit", NULL }; // The NULL pointer marks the end of the list.
    bool is_running = true;
    while (is_running)
    {
        clear();
        refresh();
        headMessage("ADMIN SCREEN");
        switch (menu(m))
        {
        case 0:
            show_patients();
            break;
//        case 1:
//            //TODO
//            break;
        case 1:
            return;
        }
    }
}

User *patient_login()
{
    clear();
    refresh();
    headMessage("PATIENT LOGIN");
    char code[20];
    int width = COLS / 2;
    int height = 7;
    int i = 0;
    bool success = false;
    WINDOW *win = newwin(height, width, (LINES - height) / 2, (COLS - width) / 2);
    wclear(win);
    wborder(win, 0, 0, 0, 0, 0, 0,0,0);
    mvwprintw(win, 3, 2, "Enter code: ");
    wrefresh(win);
    User *u;
    while (!success)
    {
        curs_set(1);
        echo();
        mvwscanw(win, 3, 15, "%10s", &code);
        curs_set(0);
        u = get_user_by_code(code);
        if (u == NULL)
        {
            wmove(win, 3, 15);
            wclrtoeol(win);
            wrefresh(win);
            show_message("Wrong code!");
            i++;
            if (i >= 3)
            {
                show_message("You've reached the times to login.");
                break;
            }
        }
        else
        {
            success = true;
        }
    }
    echo();
    wclear(win);
    wrefresh(win);
    delwin(win);
    return u;
}

void show_patients()
{
    int col = (COLS - 72) / 2;
    int page = 0;
    int records_per_page = (LINES - 2 - 13) / 3;
    User *selected_user = NULL; // Here we store the current selection or NULL if nothing is seletec.
    User *displayed_users[records_per_page]; // Here we remember which user is displayed at which position on the screen.
    while(true)
    {
        clear();
        headMessage("PATIENTS");
        mvprintw(10, col + 3, "#");
        mvprintw(10, col + 6, "Code");
        mvprintw(10, col + 15, "Name");
        mvprintw(10, col + 31, "Age");
        mvprintw(10, col + 35, "Gender");
        mvprintw(10, col + 42, "Nat");
        mvprintw(10, col + 46, "Phone");
        mvprintw(10, col + 64, "Started Date");
        mvprintw(11, col + 6, "Address");
        mvprintw(11, col + 31, "#HH");
        mvprintw(11, col + 35, "Health condition");
        mvprintw(11, col + 56, "Travel");
        mvprintw(11, col + 64, "Exposure");
        refresh();
        int line = 13; // Display always starts at line 12.
        move(line, 0);
        clrtobot();
        for (int i = 0; i < records_per_page; i++)
            displayed_users[i] = NULL; // Erase the array of displayed users.
        LinkedList *link = get_users(); // Get the start of the list of users.
        int i = 0; // Variable for counting.
        while (link != NULL && i < records_per_page * (page + 1)) // Only need to loop until the last user that is on the current page.
        {
            User *current = (User *) link->elem;
            if (i >= records_per_page * page && i < records_per_page * (page + 1)) // Check if the current user should be displayed on the current page.
            {
                displayed_users[i % records_per_page] = current; // Now remember we are displaying the current user at the current position.
                mvprintw(line, col + 3, "%d", i % 9 + 1);
                mvprintw(line, col + 6, "%s", current->code);
                clrtoeol();
                mvprintw(line, col + 15, "%s", current->name);
                clrtoeol();
                mvprintw(line, col + 31, "%d", current->age);
                clrtoeol();
                mvprintw(line, col + 35, "%s", current->gender);
                clrtoeol();
                mvprintw(line, col + 42, "%c%c", current->nationality[0], current->nationality[1]);
                clrtoeol();
                mvprintw(line, col + 46, "%s", current->phone);
                clrtoeol();
                mvprintw(line, col + 64, "%d/%d/%d", current->mm, current->dd, current->yy);
                clrtoeol();
                mvprintw(line + 1, col + 6, "%s", current->address);
                clrtoeol();
                mvprintw(line + 1, col + 31, "%d", current->h_num);
                clrtoeol();
                mvprintw(line + 1, col + 35, "%s", current->health_con);
                clrtoeol();
                mvprintw(line + 1, col + 56, "%s", current->travel ? "Yes" : "No");
                clrtoeol();
                mvprintw(line + 1, col + 64, "%s", current->expo ? "Yes" : "No");
                clrtoeol();
                if (current == selected_user)
                {
                    mvprintw(line, col, "=>");
                }
                line += 3; // Go to next line in the table.
            }
            i++; // Wether this user is on this page or not, count up.
            link = link->next;
        }
        refresh();
        bool is_last_page = link == NULL;
        char c = show_edit_menu(records_per_page);
        switch (c)
        {
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            selected_user = displayed_users[c - '1'];
            break;
        case '0':
            move(10, 0);
            clrtobot();
            refresh();
            return;
        case 'p':
            if (page == 0)
            {
                show_message("You're in the first page already.");
                break;
            }
            page--;
            selected_user = NULL; // We've changed the page, so unselect any selection.
            break;
        case 'n':
            if(is_last_page)
            {
                show_message("No more records.");
            }
            else
            {
                page++;
                selected_user = NULL; // We've changed the page, so unselect.
            }
            break;
        case 'v':
            if (selected_user)
            {
                display_reports(selected_user);
                selected_user = NULL;
            }
            else
            {
                show_message("No patient selected");
            }
            break;
        default:
            show_message("Invalid selection.");
            break;
        }
    }
}

void display_reports(User *u)
{
    clear();
    refresh();
    headMessage("Reports");
    mvprintw(11, 0, "Patient name:        %s", u->name);
    struct tm start;
    start.tm_mday = u->dd;
    start.tm_mon = u->mm - 1;
    start.tm_year = u->yy - 1900;
    start.tm_hour = 0;
    start.tm_min = 0;
    start.tm_sec = 0;
    time_t fin = mktime(&start) + 13 * 24 * 3600;
    struct tm *finish = localtime(&fin);
    mvprintw(12, 0, "Day:                %02d/%02d/%02d   %02d/%02d/%02d",
             finish->tm_mon + 1,
             finish->tm_mday,
             finish->tm_year + 1900,
             u->mm, u->dd, u->yy);
    mvprintw(13, 20, "|             |");
    mvprintw(14, 0, "Report:             --------------");
    mvprintw(14, 60, "--------------");
    mvprintw(15, 0, "Fever:");
    mvprintw(16, 0, "Sore throat:");
    mvprintw(17, 0, "Cough:");
    mvprintw(18, 0, "Runny nose:");
    mvprintw(19, 0, "Diff. breathing:");
    mvprintw(15, 40, "Fatigue:");
    mvprintw(16, 40, "Muscle/joint pain:");
    mvprintw(17, 40, "Chills:");
    mvprintw(18, 40, "Nausea/vomiting:");
    mvprintw(19, 40, "Diarrhea:");
    mvprintw(20, 40, "Other symptoms:");
    LinkedList *link = u->reports;  // This is a stack.
    while (link)
    {
        Report *report = (Report *) link->elem;
        int col = 34 - get_day_number(report);
        mvprintw(14, col, "Y");
        mvprintw(14, col + 40, "Y");
        mvprintw(15, col, report->fever ? "Y" : "-");
        mvprintw(16, col, report->sore_throat ? "Y" : "-");
        mvprintw(17, col, report->cough ? "Y" : "-");
        mvprintw(18, col, report->nose ? "Y" : "-");
        mvprintw(19, col, report->breath ? "Y" : "-");
        mvprintw(15, col + 40, report->fatigue ? "Y" : "-");
        mvprintw(16, col + 40, report->pain ? "Y" : "-");
        mvprintw(17, col + 40, report->chills ? "Y" : "-");
        mvprintw(18, col + 40, report->vomit ? "Y" : "-");
        mvprintw(19, col + 40, report->diarrhea ? "Y" : "-");
        mvprintw(20, col + 40, report->other ? "Y" : "-");
        link = link->next;
    }
    show_message("Press key to return");
    move(11,0);
    clrtobot();
    refresh();
    return;
}
