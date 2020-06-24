#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "curses.h"
#include "ui.h"
#include "user.h"
#include "report.h"

#define ADMIN_PW "BigSecret!"

LinkedList *users = NULL;

/* Declare functions before use. */
void welcomeMessage();
void homePage();
bool admin_login();
void admin_screen();
User *patient_login();
void show_patients();

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
    const char *m[] = {"Patients", "Change Admin Details",  "Exit", NULL }; // The NULL pointer marks the end of the list.
    bool is_running = true;
    while (is_running)
    {
        switch (menu(m))
        {
        case 0:
            show_patients();
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

void show_patients()
{
    clear();
    headMessage("PATIENT'S");
    int col = COLS;
    int page = 0;
    User *selected_user = NULL; // Here we store the current selection or NULL if nothing is seletec.
    User *displayed_users[9]; // Here we remember which user is displayed at which position on the screen.
    mvprintw(10, col - 10, "No.");
    mvprintw(10, col + 15, "Code");
    mvprintw(10, col + 25, "Name");
    mvprintw(10, col + 35, "Age");
    mvprintw(10, col + 40, "Gender");
    mvprintw(10, col + 47, "Nationality");
    mvprintw(10, col + 60, "Phone");
    mvprintw(10, col + 75, "Started Date");
    mvprintw(10, col + 87, "Address");
    mvprintw(10, col + 105, "Household number");
    mvprintw(10, col + 110, "Other health condition");
    mvprintw(10, col + 125, "Travel with countries with COVID 19");
    mvprintw(10, col + 140, "Exposure to confirmed COVID 19 patient");
    while(true)
    {
        int line = 12; // Display always starts at line 12.
        move(line, 0);
        clrtobot();
        for (int i = 0; i < 9; i++)
            displayed_users[i] = NULL; // Erase the array of displayed users.
        User *current = get_users(); // Get the start of the list of users.
        int i = 0; // Variable for counting.
        while (current != NULL && i < 9 * (page + 1)) // Only need to loop until the last user that is on the current page.
        {
                if (i >= 9 * page && i < 9 * (page + 1)) // Check if the current user should be displayed on the current page.
                {
                    displayed_users[i % 9] = current; // Now remember we are displaying the current user at the current position.
                    mvprintw(line, col - 10, "%d   =>", i % 9 + 1);
                    mvprintw(line, col + 15, "%s", current->code);
                    clrtoeol();
                    mvprintw(line, col + 25, "%s", current->name);
                    clrtoeol();
                    mvprintw(line, col + 60, "%d", current->age);
                    clrtoeol();
                    mvprintw(line, col + 70, "%s", current->gender);
                    clrtoeol();
                    mvprintw(line, col + 70, "%s", current->nationality);
                    clrtoeol();
                    mvprintw(line, col + 70, "%s", current->phone);
                    clrtoeol();
                    mvprintw(line, col + 70, "%d/%d/%d", current->mm, current->dd, current->yy);
                    clrtoeol();
                    mvprintw(line, col + 70, "%s", current->address);
                    clrtoeol();
                    mvprintw(line, col + 70, "%d", current->h_num);
                    clrtoeol();
                    mvprintw(line, col + 70, "%s", current->health_con);
                    clrtoeol();
                    mvprintw(line, col + 70, "%s", current->travel);
                    clrtoeol();
                    mvprintw(line, col + 70, "%s", current->expo);
                    clrtoeol();
                    if (current == selected_user)
                    {
                        mvprintw(line, col - 15, "(Selected)");
                    }
                    line++; // Go to next line in the table.
                }
                i++; // Wether this user is on this page or not, count up.

            users = list_append(users, current);
        }
        bool is_last_page = current == NULL;
        char c = show_edit_menu(false);
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
        case 'd':
            //TODO
//            if (selected_user)
//            {
//                if (get_product_count(selected_user))
//                {
//                    show_message("You can't delete a seller with active products.");
//                }
//                else
//                {
//                    delete_user(selected_user);
//                    selected_user = NULL;
//                }
//            }
            break;
        default:
            show_message("Invalid selection.");
            break;
        }
    }
}
