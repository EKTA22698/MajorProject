#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#define MAX      200
#define NAME_LEN 128
#define CAT_LEN  40
#define DATE_LEN 20
#define BUF      512

#define PANTRYFILE "pantry.txt"
#define EXPFILE    "expenses.txt"
#define HEALTHFILE "health.txt"
#define MOVIEFILE  "movies.txt"

/* -------- Data structures -------- */

typedef struct {
    char name[NAME_LEN];
    char cat[CAT_LEN];
    int  qty;
    char expiry[DATE_LEN];
} Pantry;

typedef struct {
    char name[NAME_LEN];
    char cat[CAT_LEN];
    float amount;
    char date[DATE_LEN];
} Expense;

typedef struct {
    char date[DATE_LEN];
    float water;
    int   cal;
    float sleep;
    int   steps;
} Health;

typedef struct {
    char title[NAME_LEN];
    char genre[CAT_LEN];
    float rating;
} Movie;

extern float userBudget;

void trim_newline(char *s);
void readLine(char *buf, int size);
int  stringToInt(const char *s, int *out);
int  stringToFloat(const char *s, float *out);
void today_str(char *buf, int n);
long date_to_days(const char *s);
long today_days(void);

void loadAll(void);
void saveAll(void);
void editBudget(void);

#endif
