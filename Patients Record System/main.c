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
    /* This can be optimized to use screen coordinates. */
    move(11, 0);
    printw("\n\t\t\t\t\t     °°                       °°");
    printw("\n\t\t\t\t\t    °  °                     °  °");
    printw("\n\t\t\t\t\t     °°                       °°");
    printw("\n\t\t\t\t\t    ||        WELCOME          ||  ");
    printw("\n\t\t\t\t\t   ||            TO             ||  ");
    printw("\n\t\t\t\t\t  ||      PATIENT'S RECORD       ||  ");
    printw("\n\t\t\t\t\t   ||         SYSTEM            ||  ");
    printw("\n\t\t\t\t\t    ||                         ||  ");
    printw("\n\t\t\t\t\t     í|||||||||°°°°°°||||||||||í  ");
    printw("\n\t\t\t\t\t              °°    °°");
    printw("\n\t\t\t\t\t               °°°°°°");
    printw("\n\t\t\t\t\t  °              íí");
    printw("\n\t\t\t\t\t ° °**_**_**_**_íí");
    printw("\n\t\t\t\t\t  °");
    printw("\n\n\t\t\t\t\tPress any key to continue.....");
    refresh();
    getch();
    clear();
    refresh();
}
void homePage()
{
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
            // TODO warning if code was entered wrong and no patient was found.
            break;
        case 3:
            exit(0);
        }
    }
}

bool admin_login()
{
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
    headMessage("ADMIN SCREEN");
    const char *m[] = {"Patients Under Monitoring", "Change Admin Details",  "Exit", NULL }; // The NULL pointer marks the end of the list.
    bool is_running = true;
    while (is_running)
    {
        switch (menu(m))
        {
        case 0:
            //TODO
            break;
        case 1:
            //TODO
            break;
        case 2:
            return;
        }
    }
}

User *patient_login()
{
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


