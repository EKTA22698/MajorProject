#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utils.h"
#include "pantry.h"
#include "expenses.h"
#include "health.h"
#include "movies.h"

float userBudget = 10000.0f;
void trim_newline(char *s)
{
    if (!s) return;
    size_t L = strlen(s);
    if (L > 0 && s[L - 1] == '\n') {
        s[L - 1] = '\0';
    }
}
void readLine(char *buf, int size)
{
    if (fgets(buf, size, stdin) == NULL) {
        buf[0] = '\0';
        return;
    }

    int len = (int)strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    }
}

int stringToInt(const char *s, int *out)
{
    if (s == NULL || s[0] == '\0') return 0;
    int value;
    if (sscanf(s, "%d", &value) != 1) return 0;
    *out = value;
    return 1;
}

int stringToFloat(const char *s, float *out)
{
    if (s == NULL || s[0] == '\0') return 0;
    float value;
    if (sscanf(s, "%f", &value) != 1) return 0;
    *out = value;
    return 1;
}

void today_str(char *buf, int n)
{
    time_t t = time(NULL);
    struct tm tmv = *localtime(&t);
    snprintf(buf, n, "%02d-%02d-%04d",
             tmv.tm_mday, tmv.tm_mon + 1, tmv.tm_year + 1900);
}
long date_to_days(const char *s)
{
    if (!s || !*s) return 0;
    int d = 0, m = 0, y = 0;

    if (sscanf(s, "%d-%d-%d", &d, &m, &y) != 3) {
        sscanf(s, "%d/%d/%d", &d, &m, &y);
    }

    struct tm tmv;
    memset(&tmv, 0, sizeof(tmv));
    tmv.tm_mday = d;
    tmv.tm_mon  = m - 1;
    tmv.tm_year = y - 1900;
    tmv.tm_hour = 12;
    tmv.tm_isdst = -1;

    time_t t = mktime(&tmv);
    if (t == (time_t)-1) return 0;

    return (long)(t / (24 * 60 * 60));
}

long today_days(void)
{
    time_t t = time(NULL);
    return (long)(t / (24 * 60 * 60));
}
void loadAll(void)
{
    loadPantry();
    loadExpenses();
    loadHealth();
    loadMovies();
}
void saveAll(void)
{
    savePantry();
    saveExpenses();
    saveHealth();
    saveMovies();
}
void editBudget(void)
{
    char buf[BUF];
    float newBudget;

    printf("Current monthly budget: %.2f\n", userBudget);
    printf("Enter new monthly budget (leave blank to keep the same): ");
    readLine(buf, sizeof(buf));

    if (buf[0] == '\0') {
        printf("Budget not changed.\n");
        return;
    }

    if (stringToFloat(buf, &newBudget) && newBudget >= 0) {
        userBudget = newBudget;
        printf("Budget updated to: %.2f\n", userBudget);
    } else {
        printf("Invalid amount. Budget not changed.\n");
    }
}
