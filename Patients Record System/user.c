#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "user.h"
#include "linkedlist.h"
#include "report.h"

#define FILENAME "users.csv"

#define BUF_SIZE 2048

LinkedList *users = NULL; // This is where we will keep the users.
int users_max_id = 0; // We keep track of the highest ID so we know which one to use next.

/* Loads all users from the file and returns the number of users read, or -1 if there was an error. */
int load_users()
{
    if (users) return -1; // Users already loaded.
    FILE *f = fopen(FILENAME, "r");
    if (f == NULL) // Could not open the file.
    {
        return -1;
    }
    char buf[BUF_SIZE]; // We'll use this variable to load lines of our file into. C is not very convenient when it comes to strings, you have to do a lot of stuff by hand.
    int count = 0; // Let's count the users.
    users_max_id = 0; // To keep track of the highest id, we reset it first.
    while (true) // Endless loop.
    {
        if (fgets(buf, BUF_SIZE, f) == NULL) //  We read the next line from the file into our buf variable. If we get NULL, we've reached the end of the file.
            break; // ... so exit the loop.
        User *u = malloc(sizeof(User)); // Allocate memory for one user.
        u->reports = NULL; // Initialize the stack of reports.
        char travel[BUF_SIZE];
        char expo[BUF_SIZE];
        int rc = sscanf(buf, "%d,%255[^,\n],%255[^,\n],%d,%255[^,\n],%255[^,\n],%255[^,\n],%d/%d/%d,%255[^,\n],%d,%255[^,\n],%255[^,\n],%255[^,\n]",
                        &u->id,
                        u->code,
                        u->name,
                        &u->age,
                        u->gender,
                        u->nationality,
                        u->phone,
                        &u->mm,
                        &u->dd,
                        &u->yy,
                        u->address,
                        &u->h_num,
                        u->health_con,
                        travel,
                        expo);
        if (rc < 15) // The number of fields read is in rc. This should be 5 unless it's somehow an invalid line. If it's invalid, simply skip it.
        {
            free(u); // Free the allocated memory because we're skipping, so we don't run out of memory eventually. It's the opposite of malloc.
            fprintf(stderr, "Skipping invalid line. %s:%d\n", __FUNCTION__, __LINE__); // Be nice and print a notice.
            continue; // Loop around.
        }
        u->travel = (strcmp(travel, "Yes") == 0);
        u->expo = (strcmp(expo, "Yes") == 0);
        // fprintf(stderr, "Read user %d: %s\n", u->id, u->user_name);
        /* At this point, we've read the fields of one line into the User in variable u. */
        users = list_append(users, u);
        count++; // We've read one more user. Increase the count of users by one.
        if (u->id > users_max_id) // If we found a higher id ...
            users_max_id = u->id; // ... then remember it. This is only necessary if we delete users later and the numbers are not strictly ascending anymore. But hey, let's be safe.
    }
    fclose(f); // We're done here. File is read completely. We get here only through the break statement further up.
    return 0; // Return the number of users, just to be nice.
}

/* Save all users in the file and return 0 if everything worked, -1 if we couldn't open the file, and -2 if something else went wrong. */
int save_users()
{
    FILE *f = fopen(FILENAME, "w+");
    if (f == NULL)
    {
        fprintf(stderr, "%s:%d Could not open file.\n", __FUNCTION__, __LINE__); // Print a nice error message with function name and line number.
        return -1;
    }
    LinkedList *link = users;
    while (link)
    {
        User *u = (User *) link->elem;
        int written = fprintf(f, "%d,%s,%s,%d,%s,%s,%s,%d/%d/%d,%s,%d,%s,%s,%s\n",
                              u->id,
                              u->code,
                              u->name,
                              u->age,
                              u->gender,
                              u->nationality,
                              u->phone,
                              u->mm,
                              u->dd,
                              u->yy,
                              u->address,
                              u->h_num,
                              u->health_con,
                              u->travel ? "Yes" : "No",
                              u->expo ? "Yes" : "No");
        if (written < 0 || written >= BUF_SIZE)
        {
            fclose(f); // We don't want dangling open files in case of an error.
            fprintf(stderr, "%s:%d Could not write file.\n", __FUNCTION__, __LINE__);
            return -2;
        }
        link = link->next;
    }
    fclose(f); // We're done here.
    return 0;
}

User *add_user(User *u)
{
    ++users_max_id; //increment *before* assignment.
    u->id = users_max_id; // Assign the next id.
    sprintf(u->code, "PUM%05d", users_max_id);
    users = list_append(users, u);
    save_users();
    return u;
}

User *get_user(const int id)
{
    LinkedList *current = users;
    while (current != NULL)
    {
        User *u = (User *) current->elem;
        if (u->id == id)
            return u;
        else
            current = current->next;
    }
    return NULL;
}

User *get_user_by_code(const char *code)
{
    LinkedList *current = users;
    while (current != NULL)
    {
        User *u = (User *) current->elem;
        if (strcmp(u->code, code) == 0)
            return u;
        else
            current = current->next;
    }
    return NULL;
}

LinkedList *get_users()
{
    return users;
}

void delete_user(User *user)
{
    users = list_delete(users, user);
    save_users(); // Save our changes to disk.
}

bool has_report(User *u, int day) {
    LinkedList *link = u->reports;
    while (link) {
        Report *r = (Report *) link->elem;
        if (r->day == day) return true;
        link = link->next;
    }
    return false;
}
