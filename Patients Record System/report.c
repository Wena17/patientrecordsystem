#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "report.h"
#include "user.h"
#include "linkedlist.h"

#define FILENAME "reports.csv"
#define BUF_SIZE 2048

int num = 0;

int add_report(User *patient, Report *current_report, bool save)
{
    current_report->day = ++num;
    current_report->patient = patient;
    patient->reports = list_append(patient->reports, current_report);
    if (save)
    {
        save_users();
        save_reports();
    }
    return 0;
}

int save_reports()
{
    FILE *f = fopen(FILENAME, "w+");

    if (f == NULL)
    {
        fprintf(stderr, "%s:%d Could not open file.\n", __FUNCTION__, __LINE__); // Print a nice error message with function name and line number.
        return -1;
    }
    LinkedList *ulink = get_users();
    while (ulink)
    {
        User *u = (User *) ulink->elem;
        LinkedList *rlink = u->reports;
        while (rlink)
        {
            Report *r = (Report *) rlink->elem;
            int written = fprintf(f, "%d,%d,%d/%d/%d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
                                  u->id,
                                  r->day,
                                  r->mm,
                                  r->dd,
                                  r->yy,
                                  r->fever ? "Yes" : "No",
                                  r->sore_throat? "Yes" : "No",
                                  r->cough ? "Yes" : "No",
                                  r->nose ? "Yes" : "No",
                                  r->breath ? "Yes" : "No",
                                  r->fatigue ? "Yes" : "No",
                                  r->pain ? "Yes" : "No",
                                  r->chills ? "Yes" : "No",
                                  r->vomit ? "Yes" : "No",
                                  r->diarrhea ? "Yes" : "No",
                                  r->other ? "Yes" : "No");
            if (written < 0 || written >= BUF_SIZE)
            {
                fclose(f); // We don't want dangling open files in case of an error.
                fprintf(stderr, "%s:%d Could not write file.\n", __FUNCTION__, __LINE__);
                return -2;
            }
            rlink = rlink->next;
        }
        ulink = ulink->next;
    }
    fclose(f); // We're done here.
    return 0;
}

//TODO
int load_reports()
{
    FILE *f = fopen(FILENAME, "r");

    if (f == NULL) // Could not open the file.
    {
        return -1;
    }
    char buf[BUF_SIZE]; // We'll use this variable to load lines of our file into. C is not very convenient when it comes to strings, you have to do a lot of stuff by hand.
    int count = 0; // Let's count the users.
    while (true) // Endless loop.
    {
        if (fgets(buf, BUF_SIZE, f) == NULL) //  We read the next line from the file into our buf variable. If we get NULL, we've reached the end of the file.
            break; // ... so exit the loop.
        Report *r = malloc(sizeof(Report)); // Allocate memory for one user.
        unsigned int uid;
        char fever[BUF_SIZE];
        char sore_throat[BUF_SIZE];
        char cough[BUF_SIZE];
        char nose[BUF_SIZE];
        char breath[BUF_SIZE];
        char fatigue[BUF_SIZE];
        char pain[BUF_SIZE];
        char chills[BUF_SIZE];
        char vomit[BUF_SIZE];
        char diarrhea[BUF_SIZE];
        char other[BUF_SIZE];
        int rc = sscanf(buf, "%d,%d,%d/%d/%d,%255[^,\n],%255[^,\n],%255[^,\n],%255[^,\n],%255[^,\n],%255[^,\n],%255[^,\n],%255[^,\n],%255[^,\n],%255[^,\n],%255[^,\n]",
                        &uid,
                        &r->day,
                        &r->mm,
                        &r->dd,
                        &r->yy,
                        fever,
                        sore_throat,
                        cough,
                        nose,
                        breath,
                        fatigue,
                        pain,
                        chills,
                        vomit,
                        diarrhea,
                        other);
        if (rc < 16 ) // The number of fields read is in rc. This should be 5 unless it's somehow an invalid line. If it's invalid, simply skip it.
        {
            free(r); // Free the allocated memory because we're skipping, so we don't run out of memory eventually. It's the opposite of malloc.
            fprintf(stderr, "Skipping invalid line; read %d fields. %s:%d\n", rc, __FUNCTION__, __LINE__); // Be nice and print a notice.
            continue; // Loop around.
        }
        r->fever = (strcmp(fever, "Yes") == 0);
        r->sore_throat = (strcmp(sore_throat, "Yes") == 0);
        r->cough = (strcmp(cough, "Yes") == 0);
        r->nose = (strcmp(nose, "Yes") == 0);
        r->breath = (strcmp(breath, "Yes") == 0);
        r->fatigue = (strcmp(fatigue, "Yes") == 0);
        r->pain = (strcmp(pain, "Yes") == 0);
        r->chills = (strcmp(chills, "Yes") == 0);
        r->vomit = (strcmp(vomit, "Yes") == 0);
        r->diarrhea = (strcmp(diarrhea, "Yes") == 0);
        r->other = (strcmp(other, "Yes") == 0);
        add_report(get_user(uid), r, false); // Do not save the file while loading it.
        count++; // We've read one more report.
    }
    fclose(f); // We're done here. File is read completely. We get here only through the break statement further up.
    return count; // Return the number of reports, just to be nice.
}
