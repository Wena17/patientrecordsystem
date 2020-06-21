#ifndef USER_H_INCLUDED
#define USER_H_INCLUDED

#include <stdlib.h>
#include <time.h>

#include "linkedlist.h"

#define MAXLEN 256
#define CODE_LEN 8

typedef struct s_user
{
    unsigned int id;
    char code[CODE_LEN + 1];
    char name[MAXLEN];
    int age;
    char gender[CODE_LEN];
    char nationality[MAXLEN];
    char phone[MAXLEN];
    int mm;
    int dd;
    int yy;
    char address[MAXLEN];
    int h_num;
    char health_con[MAXLEN];
    char travel[CODE_LEN];
    char expo[CODE_LEN];
    LinkedList *reports;
} User;

int load_users();
int save_users();

LinkedList *get_users();
User *add_user();
User *get_user(const int id);
User *get_user_by_code(const char *code);
void delete_user(User *user);
int get_user_count();

#endif // USER_H_INCLUDED
