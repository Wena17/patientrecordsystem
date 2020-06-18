#include <stdlib.h>

#include "user.h"
#include "curses.h"
#include "ui.h"
#include "AddingPatient.h"

void new_patient()
{
    headMessage("New Patient");
    User *u = malloc(sizeof(User));
    int width = COLS / 2;
    int height = 15;
    WINDOW *win = newwin(height, width, (LINES - height) / 2, (COLS - width) / 2);
    wclear(win);
    mvwprintw(win, 2, 2, "Name: ");
    mvwprintw(win, 3, 2, "Age: ");
    mvwprintw(win, 4, 2, "Gender: ");
    mvwprintw(win, 5, 2, "Nationality: ");
    mvwprintw(win, 6, 2, "Contact #: ");
    mvwprintw(win, 7, 2, "Date today (mm.dd.yy): ");
    mvwprintw(win, 8, 2, "Address: ");
    mvwprintw(win, 9, 2, "Number of person in the household: ");
    mvwprintw(win, 10, 2, "Other health conditions: ");
    mvwprintw(win, 11, 2, "Do you have history of travel \n to other countries with COVID-19 (Yes/No): "); // TODO fix formatting
    mvwprintw(win, 13, 2, "Do you have history of exposure \n to a confirmed COVID-19 patient (Yes/No): "); // TODO fix formatting
    wrefresh(win);
    echo(); // Turn on echo so the user sees what they are typing.
    curs_set(1); // Show the cursor so the user sees where they are typing.
    mvwscanw(win, 2, 15, "%50[^\n]", u->name);
    mvwscanw(win, 3, 15, "%d", &u->age);
    mvwscanw(win, 4, 15, "%20[^\n]", u->gender);
    mvwscanw(win, 5, 15, "%50[^\n]", u->nationality);
    mvwscanw(win, 6, 15, "%50[^\n]", &u->phone);
    mvwscanw(win, 7, 25, "%d.%d.%d", &u->mm,&u->dd,&u->yy);
    mvwscanw(win, 8, 15, "%255[^\n]", u->address);
    mvwscanw(win, 9, 37, "%d", &u->h_num);
    mvwscanw(win, 10, 35, "%50[^\n]", u->health_con);
    mvwscanw(win, 12, 43, "%7[^\n]", u->travel);
    mvwscanw(win, 14, 43, "%7[^\n]", u->expo);
    curs_set(0); // Hide the cursor again.
    noecho(); // Don't show what the users types.
    if (add_user(u) == NULL) // Add user and check if something went wrong.
    {
        mvwprintw(win, 6, 2, "Something went wrong. Press any key to continue.");
        wrefresh(win);
        getch();
    } // TODO display code
    wclear(win);
    wrefresh(win);
    delwin(win);
    //display_user();
    return;
}
