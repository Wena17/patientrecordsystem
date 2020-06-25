#ifndef UI_H_INCLUDED
#define UI_H_INCLUDED

#include <stdbool.h>

#include "user.h"

int menu(const char **items); // This is a big menu with a box.
void show_message(char *msg);
void headMessage(const char *message);
void popUpMessage(const char *pop_message, const char *pmsg);
void PUM_screen(User *u);
void new_patient();
char show_edit_menu(const bool has_edit, const int records_per_page);

#endif // UI_H_INCLUDED
