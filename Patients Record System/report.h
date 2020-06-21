#ifndef SYMPTOMS_H_INCLUDED
#define SYMPTOMS_H_INCLUDED

#include <stdbool.h>

#include "user.h"


#define SIZE 8

typedef struct s_report
{
    int day;
    int mm, dd, yy;
    bool fever;
    bool sore_throat;
    bool cough;
    bool nose;
    bool breath;
    bool fatigue;
    bool pain;
    bool chills;
    bool vomit;
    bool diarrhea;
    bool other;
    User *patient;
} Report;

int save_reports();
int load_reports();

// Add a report to the user's list. Save to file if the third parameter is true.
int add_report(User *u, Report *r, bool save);

#endif // SYMPTOMS_H_INCLUDED
