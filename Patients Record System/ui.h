#ifndef UI_H_INCLUDED
#define UI_H_INCLUDED

#include "user.h"

int menu(const char **items); // This is a big menu with a box.
void show_message(char *msg);
void headMessage(const char *message);
void PUM_screen(User *u);
void new_patient();

#endif // UI_H_INCLUDED
