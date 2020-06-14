#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "curses.h"
#include "ui.h"
#include "user.h"

#define ADMIN_PW "BigSecret!"

/* Declare functions before use. */
void welcomeMessage();
void homePage();
bool admin_login();
void admin_screen();

int main()
{
    initscr(); // Initialize the curses library.
    int cursor_setting = curs_set(0);
    noecho(); // Typed characters are not automatically printed on the screen.
    welcomeMessage();
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
    move(9, 0);
    printw("\n\n\n\t\tWELCOME\n\t\t\tTO\n\t\tPATIENTS RECORD");
    printw("\n\n\n\n\t\t\t\t\tPress any key to continue.....");
    refresh();
    getch();
    clear();
    refresh();
}

void homePage()
{
    headMessage("HOMEPAGE");
    const char *m[] = { "Admin", "Person Under Monitoring", "Person Under Investegation", "Exit", NULL }; // The NULL pointer marks the end of the list.
    bool is_running = true;
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
            //TODO
            break;
        case 2:
            //TODO
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
// TODO
}
