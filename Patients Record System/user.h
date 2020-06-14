#ifndef USER_H_INCLUDED
#define USER_H_INCLUDED

#include <stdlib.h>

#include "linkedlist.h"

#define MAXLEN 256
#define CODE_LEN 8

typedef struct User
{
    unsigned int id;
    char code[CODE_LEN + 1];
} User;

int load_users();
int save_users();

LinkedList *get_users();
User *add_user(const char *code);
User *get_user(const int id);
void delete_user(User *user);
int get_user_count();

#endif // USER_H_INCLUDED
