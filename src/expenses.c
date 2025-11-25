#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expenses.h"

static Expense expList[MAX];
static int expCount = 0;

void loadExpenses(void)
{
    expCount = 0;
    FILE *f = fopen(EXPFILE, "r");
    if (!f) return;

    char line[BUF];
    while (fgets(line, sizeof(line), f) && expCount < MAX) {
        trim_newline(line);
        if (line[0] == '\0') continue;

        char tmp[BUF];
        strncpy(tmp, line, sizeof(tmp) - 1);
        tmp[sizeof(tmp) - 1] = 0;

        char *tok = strtok(tmp, ",");
        if (!tok) continue;

        strncpy(expList[expCount].name, tok, NAME_LEN - 1);
        expList[expCount].name[NAME_LEN - 1] = '\0';

        tok = strtok(NULL, ",");
        if (tok)
            strncpy(expList[expCount].cat, tok, CAT_LEN - 1);
        else
            expList[expCount].cat[0] = 0;

        tok = strtok(NULL, ",");
        if (tok)
            expList[expCount].amount = (float)atof(tok);
        else
            expList[expCount].amount = 0.0f;

        tok = strtok(NULL, ",");
        if (tok)
            strncpy(expList[expCount].date, tok, DATE_LEN - 1);
        else
            expList[expCount].date[0] = 0;

        expList[expCount].date[DATE_LEN - 1] = '\0';
        expCount++;
    }

    fclose(f);
}

void saveExpenses(void)
{
    FILE *f = fopen(EXPFILE, "w");
    if (!f) {
        printf("Unable to save expenses file.\n");
        return;
    }

    for (int i = 0; i < expCount; i++) {
        fprintf(f, "%s,%s,%.2f,%s\n",
                expList[i].name,
                expList[i].cat,
                expList[i].amount,
                expList[i].date[0] ? expList[i].date : "");
    }

    fclose(f);
}

void expenseAlerts(void)
{
    float total = 0.0f;
    int i;

    if (expCount == 0) {
        printf("  No expenses recorded yet.\n");
        return;
    }

    for (i = 0; i < expCount; i++) {
        total += expList[i].amount;
    }

    printf("  Total expenses so far: %.2f (Budget: %.2f)\n",
           total, userBudget);

    if (total > userBudget) {
        printf("  Alert: You have crossed your monthly budget.\n");
        printf("  Extra amount spent: %.2f\n", total - userBudget);
    } else if (total >= 0.8f * userBudget) {
        printf("  Warning: You have already used more than 80%% of your budget.\n");
    } else {
        printf("  Expenses are currently under control.\n");
    }

    printf("  Checking for big expenses (>= 1000)...\n");
    int anyBig = 0;
    for (i = 0; i < expCount; i++) {
        if (expList[i].amount >= 1000.0f) {
            printf("    Big expense: %s (%.2f) on %s\n",
                   expList[i].name,
                   expList[i].amount,
                   expList[i].date);
            anyBig = 1;
        }
    }
    if (!anyBig) {
        printf("  No big individual expenses found.\n");
    }
}

void expenseAdd(void)
{
    if (expCount >= MAX) {
        printf("Expenses full.\n");
        return;
    }

    char buf[BUF];
    float amt;

    printf("Enter expense name: ");
    readLine(expList[expCount].name, NAME_LEN);
    if (expList[expCount].name[0] == '\0') {
        printf("Name required.\n");
        return;
    }

    printf("Enter category: ");
    readLine(expList[expCount].cat, CAT_LEN);

    printf("Enter amount: ");
    readLine(buf, sizeof(buf));
    if (!stringToFloat(buf, &amt) || amt < 0.0f) {
        printf("Invalid amount.\n");
        return;
    }
    expList[expCount].amount = amt;

    today_str(expList[expCount].date, DATE_LEN);
    expCount++;

    saveExpenses();
    printf("Expense added and saved.\n");
    expenseAlerts();
}

void expenseListShow(void)
{
    if (expCount == 0) {
        printf("No expenses.\n");
        return;
    }

    printf("\n%-4s %-15s %-15s %-10s %-12s\n",
           "No", "Name", "Category", "Amount", "Date");
    printf("---------------------------------------------------------------\n");

    for (int i = 0; i < expCount; i++) {
        char amtbuf[20];
        sprintf(amtbuf, "₹%.2f", expList[i].amount);

        printf("%-4d %-15s %-15s %-10s %-12s\n",
               i + 1,
               expList[i].name,
               expList[i].cat,
               amtbuf,
               expList[i].date);
    }

    float total = 0.0f;
    for (int i = 0; i < expCount; i++) {
        total += expList[i].amount;
    }

    printf("\nTotal expenses: ₹%.2f\n", total);

    expenseAlerts();
}

void expenseEdit(void)
{
    char buf[BUF];
    int index;
    float newAmount;

    expenseListShow();
    if (expCount == 0) {
        return;
    }

    printf("Enter the expense number you want to edit: ");
    readLine(buf, sizeof(buf));
    if (!stringToInt(buf, &index) || index < 1 || index > expCount) {
        printf("Invalid expense number.\n");
        return;
    }

    index--;

    printf("Enter new name (press Enter to keep '%s'): ",
           expList[index].name);
    readLine(buf, sizeof(buf));
    if (buf[0] != '\0') {
        strncpy(expList[index].name, buf, NAME_LEN - 1);
        expList[index].name[NAME_LEN - 1] = '\0';
    }

    printf("Enter new category (press Enter to keep '%s'): ",
           expList[index].cat);
    readLine(buf, sizeof(buf));
    if (buf[0] != '\0') {
        strncpy(expList[index].cat, buf, CAT_LEN - 1);
        expList[index].cat[CAT_LEN - 1] = '\0';
    }

    printf("Enter new amount (press Enter to keep %.2f): ",
           expList[index].amount);
    readLine(buf, sizeof(buf));
    if (buf[0] != '\0') {
        if (stringToFloat(buf, &newAmount) && newAmount >= 0) {
            expList[index].amount = newAmount;
        } else {
            printf("Invalid amount. Old value kept.\n");
        }
    }

    saveExpenses();
    printf("Expense updated and file saved.\n");
}

void expenseDelete(void)
{
    expenseListShow();
    if (expCount == 0) return;

    char buf[BUF];
    int idx;

    printf("Enter expense number to delete: ");
    readLine(buf, sizeof(buf));
    if (!stringToInt(buf, &idx) || idx < 1 || idx > expCount) {
        printf("Invalid number.\n");
        return;
    }

    idx--;

    for (int i = idx; i < expCount - 1; i++) {
        expList[i] = expList[i + 1];
    }

    expCount--;
    saveExpenses();
    printf("Expense deleted and saved.\n");
}
