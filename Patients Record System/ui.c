#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "curses.h"
#include "ui.h"

/* Show a menu with a null-pointer terminated list of items and return the index of the user's selection.
 *
 * The format parameter can be:
 *   0 - Centered on the screen
 *   1 - Compact menu at the bootom of the screen.
 */
int menu(const char **items)
{
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
    do
    {
        choice = getch() - '1'; // Neat way to translate the character to the zero-based index of the item.
    }
    while (choice < 0 || choice >= num_items);
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
    mvwprintw(win, 1, 0, "í~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~í");
    mvwprintw(win, 2, 0, "!!                                                  !!");
    mvwprintw(win, 3, 0, "!!                PATIENT'S RECORDS                 !!");
    mvwprintw(win, 4, 0, "!!                                                  !!");
    mvwprintw(win, 5, 0, "í~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~í");
    mvwprintw(win, 6, 0, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    mvwprintw(win, 7, 0, "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=");
    mvwprintw(win, 8, (len - strlen(message)) / 2, "%s", message);
    mvwprintw(win, 9, 0, "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=");
    wrefresh(win);
    delwin(win);
}
