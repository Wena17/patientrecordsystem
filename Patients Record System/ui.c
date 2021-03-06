#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "curses.h"
#include "ui.h"
#include "report.h"

bool get_yes_no(WINDOW *win, int row, int col);
/* Show a menu with a null-pointer terminated list of items and return the index of the user's selection.
 *
 * The format parameter can be:
 *   0 - Centered on the screen
 *   1 - Compact menu at the bootom of the screen.
 */
int menu(const char **items)
{
    refresh();
    /* Determine the length of the longest menu item. */
    int num_items = 0;
    int max_width = 0;
    while (items[num_items])
    {
        int l = strlen(items[num_items]);
        if (l > max_width)
            max_width = l;
        num_items++;
    }
    if (max_width == 0 || num_items == 0)
        return -1; // This should not happen.
    max_width += 4; // Add to allow numbering.
    if (max_width < 18)
        max_width = 18; // Make sure the input prompt can be displayed.

    /* Now calculate the starting position for the window. */
    const int start_col = (COLS - max_width - 4) / 2;

    /* Create a window with the given size and coords and write the contents. */
    WINDOW *win = newwin(num_items + 4, max_width + 4, 12, start_col);
    wclear(win);
    wborder(win, 0, 0, 0, 0, 0, 0, 0, 0);
    mvwprintw(win, 1, 2, "%s", "Enter your choice:");
    for (int i = 0; i < num_items; i++)
    {
        mvwprintw(win, i + 3, 2, "(%i) %s", i + 1, items[i]);
    }
    wrefresh(win);
    int choice;
    cbreak();
    do
    {
        choice = getch() - '1'; // Neat way to translate the character to the zero-based index of the item.
    }
    while (choice < 0 || choice >= num_items);
    nocbreak();
    wclear(win); // Clear the window.
    wrefresh(win); // Display the changes.
    delwin(win); // Free up the window memory.
    return choice;
}

void show_message(char *msg)
{
    const char s[] = " - Press any key.";
    int l = LINES - 6;
    int c = (COLS - strlen(msg) - strlen(s)) / 2;
    move(l, 0);
    clrtoeol();
    mvprintw(l, c, "%s%s", msg, s);
    getch();
    move(l, 0);
    clrtoeol();
    refresh();
}

void headMessage(const char *message)
{
    refresh();
    const int len = strlen("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    const int col = (COLS - len) / 2; // Calculate the column to center the string.
    WINDOW *win = newwin(10, len, 0, col);
    wclear(win);
    mvwprintw(win, 0, 0, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    mvwprintw(win, 1, 0, "�~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~�");
    mvwprintw(win, 2, 0, "!!                                                  !!");
    mvwprintw(win, 3, 0, "!!                 PATIENT RECORDS                  !!");
    mvwprintw(win, 4, 0, "!!                                                  !!");
    mvwprintw(win, 5, 0, "�~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~�");
    mvwprintw(win, 6, 0, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    mvwprintw(win, 7, 0, "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=");
    mvwprintw(win, 8, (len - strlen(message)) / 2, "%s", message);
    mvwprintw(win, 9, 0, "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=");
    wrefresh(win);
    delwin(win);
}

void popUpMessage(const char *pmsg, const char *pop_message)
{
    int w1 = strlen(pmsg);
    int w2 = strlen(pop_message);
    int wmax = w1 >= w2 ? w1 : w2;
    int width = (COLS - wmax - 4) / 2;
    int height = 7;
    WINDOW *win = newwin(height, width, (LINES - height) / 2, (COLS - width) / 2);
    wclear(win);
    wborder(win, 0, 0, 0, 0, 0, 0,0,0);
    mvwprintw(win, 2, (width - strlen(pmsg)) / 2, "%s", pmsg);
    mvwprintw(win, 3, (width - strlen(pop_message)) / 2, "%s", pop_message);
    wrefresh(win);
    getch();
    wclear(win);
    wrefresh(win);
    delwin(win);
}

char show_edit_menu(const int records_per_page)
{
    const int col = (COLS - 53) / 2;
    char allowed[] = "vdepn0123456789";
    allowed[6 + records_per_page] = '\0'; // Do not allow selections beyond the number of records per page.
    mvprintw(LINES - 2, col, "(v) View");
    mvprintw(LINES - 2, col + 22, "(p)revious");
    mvprintw(LINES - 2, col + 36, "(n)ext");
    mvprintw(LINES - 2, col + 46, "(0)back");
    refresh();
    cbreak();
    noecho();
    while (true)
    {
        char c = tolower(getch());
        const char *s = allowed;
        while (*s)
        {
            if (*s == c )
                return c;
            s++;
        }
    }
}

void PUM_screen(User *current_patient)
{
    clear();
    refresh();
    headMessage("Person Under Monitoring");
    refresh();
    Report *s = malloc(sizeof(Report));
    char temp[256];
    time_t current_time;
    time(&current_time); // Get the current time.
    struct tm *local_time = localtime(&current_time); // Convert to a struct containing the local time.
    s->mm = local_time->tm_mon + 1; // Get the month, but January is 0!
    s->dd = local_time->tm_mday; // Get the day of the month, starting at 1 as it should.
    s->yy = local_time->tm_year + 1900; // Get the year, where 0 actually means 1900.
    free(local_time);
    s->patient = current_patient;
    s->day = get_day_number(s);
    if (s->day >= 14) {
        free(s);
        popUpMessage("You have already completed", "your 14 day reporting period.");
    } else if (has_report(current_patient, s->day)) {
        free(s);
        popUpMessage("You have already reported today.", "Come back tomorrow");
    } else {
        int width = COLS / 2;
        int height = 12;
        WINDOW *win = newwin(height, width, 11, (COLS - width) / 2);
        wclear(win);
        mvwprintw(win, 0, 2, "Date: ................... %02d/%02d/%d", s->mm, s->dd, s->yy); //"02" 2 means two digits then 0 means zero in front if necessary.
        mvwprintw(win, 1, 2, "Fever: ..................");
        mvwprintw(win, 2, 2, "Sore throat: ............");
        mvwprintw(win, 3, 2, "Cough: ..................");
        mvwprintw(win, 4, 2, "Runny nose: .............");
        mvwprintw(win, 5, 2, "Shortness of breathing: .");
        mvwprintw(win, 6, 2, "Fatigue: ................");
        mvwprintw(win, 7, 2, "Muscle pain / Joint pain:");
        mvwprintw(win, 8, 2, "Chills: .................");
        mvwprintw(win, 9, 2, "Vommit / Nausea: ........");
        mvwprintw(win, 10, 2, "Diarrhea: ...............");
        mvwprintw(win, 11, 2, "Other symptoms: .........");
        wrefresh(win);
        s->fever = get_yes_no(win, 1, 28);
        s->sore_throat = get_yes_no(win, 2, 28);
        s->cough = get_yes_no(win, 3, 28);
        s->nose = get_yes_no(win, 4, 28);
        s->breath = get_yes_no(win, 5, 28);
        s->fatigue = get_yes_no(win, 6, 28);
        s->pain = get_yes_no(win, 7, 28);
        s->chills = get_yes_no(win, 8, 28);
        s->vomit = get_yes_no(win, 9, 28);
        s->diarrhea = get_yes_no(win, 10, 28);
        s->other = get_yes_no(win, 11, 28); // TODO make other symptoms a text field
        if (add_report(current_patient, s, true)) // Add report and check if something went wrong.
        {
            free(s);
            show_message("Something went wrong. Press any key to continue.");
            wrefresh(win);
            getch();
        }
        wclear(win);
        wrefresh(win);
        delwin(win);
        sprintf(temp, "Day %d", s->day);
        if(s->fever || s->sore_throat || s->cough || s->nose || s->breath || s->fatigue || s->pain || s->chills || s->vomit || s->diarrhea || s->other)
        {
            popUpMessage("We see any symptoms on your", temp);
            popUpMessage("We will refer you", "to a quarantine facilities");
            popUpMessage("Don't be afraid", "Please bear with our personel");
        }
        else
        {
            popUpMessage("Your done with", temp);
        }
    }
    return;
}

void new_patient()
{
    headMessage("New Patient");
    User *u = malloc(sizeof(User));
    u->reports = NULL;
    time_t current_time;
    time(&current_time); // Get the current time.
    struct tm *local_time = localtime(&current_time); // Convert to a struct containing the local time.
    u->mm = local_time->tm_mon + 1; // Get the month, but January is 0!
    u->dd = local_time->tm_mday; // Get the day of the month, starting at 1 as it should.
    u->yy = local_time->tm_year + 1900; // Get the year, where 0 actually means 1900.
    free(local_time);
    int width = COLS / 2;
    int height = 13;
    WINDOW *win = newwin(height, width, 11, (COLS - width) / 2);
    wclear(win);
    mvwprintw(win, 0, 2, "Name: ");
    mvwprintw(win, 1, 2, "Age: ");
    mvwprintw(win, 2, 2, "Gender: ");
    mvwprintw(win, 3, 2, "Nationality: ");
    mvwprintw(win, 4, 2, "Contact #: ");
    mvwprintw(win, 5, 2, "Date today:  %02d/%02d/%d", u->mm, u->dd, u->yy); //"02" 2 means two digits then 0 means zero in front if necessary.
    mvwprintw(win, 6, 2, "Address: ");
    mvwprintw(win, 7, 2, "Number of person in the household: ");
    mvwprintw(win, 8, 2, "Other health conditions: ");
    mvwprintw(win, 9, 2, "Do you have history of travel \n  -to other countries with COVID-19 (Yes/No): ");
    mvwprintw(win, 11, 2, "Do you have history of exposure \n  -to a confirmed COVID-19 patient (Yes/No): ");
    wrefresh(win);
    echo(); // Turn on echo so the user sees what they are typing.
    curs_set(1); // Show the cursor so the user sees where they are typing.
    mvwscanw(win, 0, 15, "%50[^\n]", u->name);
    mvwscanw(win, 1, 15, "%d", &u->age);
    mvwscanw(win, 2, 15, "%20[^\n]", u->gender);
    mvwscanw(win, 3, 15, "%50[^\n]", u->nationality); // TODO Let user choose from a list of nationalities.
    mvwscanw(win, 4, 15, "%50[^\n]", &u->phone);
    mvwscanw(win, 6, 15, "%255[^\n]", u->address);
    mvwscanw(win, 7, 37, "%d", &u->h_num);
    mvwscanw(win, 8, 32, "%50[^\n]", u->health_con);
    u->travel = get_yes_no(win, 10, 46);
    u->expo = get_yes_no(win, 12, 45);
    curs_set(0); // Hide the cursor again.
    noecho(); // Don't show what the users types.
    if (add_user(u) == NULL) // Add user and check if something went wrong.
    {
        show_message("Something went wrong. Press any key to continue.");
        wrefresh(win);
        getch();
    }
    wclear(win);
    wrefresh(win);
    delwin(win);
    popUpMessage("Your code", u->code);
    return;
}

bool get_yes_no(WINDOW *win, int row, int col) {
    echo();
    curs_set(1);
    cbreak();
    char input;
    bool done = false;
    bool result;
    do {
        wmove(win, row, col);
        input = tolower(wgetch(win));
        if (input == 'y') {
            result = true;
            done = true;
        } else if (input == 'n') {
            result = false;
            done = true;
        } else {
            wmove(win, row, col);
            wclrtoeol(win);
            wrefresh(win);
        }
    } while (!done);
    curs_set(0);
    noecho();
    nocbreak();
    mvwprintw(win, row, col, result ? "Yes" : "No");
    return result;
}
