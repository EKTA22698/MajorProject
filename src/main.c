#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "pantry.h"
#include "expenses.h"
#include "health.h"
#include "movies.h"

static void alertCenter(void)
{
    printf("\n--- ALERT CENTER ---\n");
    printf("Pantry Alerts:\n");
    pantryAlerts();
    printf("Expense Alerts:\n");
    expenseAlerts();
    printf("Health Alerts:\n");
    healthAlerts();
    printf("Movie Alerts:\n");
    movieAlerts();
    printf("--------------------\n");
}

int main(void)
{
    loadAll();
    printf("=== Life Organizing System (Humanized) ===\n");

    char choice[80];

    while (1) {
        printf("\nMain Menu:\n");
        printf("1) Pantry     2) Expenses    3) Health    4) Movies\n");
        printf("5) Alert Center    6) Edit Budget    0) Save & Exit\n");
        printf("Enter choice: ");

        readLine(choice, sizeof(choice));
        int ch;
        if (!stringToInt(choice, &ch)) {
            printf("Invalid choice.\n");
            continue;
        }

        if (ch == 0) {
            saveAll();
            printf("Saved all. Bye!\n");
            break;
        }

        if (ch == 1) {
            printf("\nPantry Menu: 1.Add 2.List 3.Edit 4.Delete 5.Save 0.Back\nChoice: ");
            readLine(choice, sizeof(choice));
            int sc;
            if (!stringToInt(choice, &sc)) {
                printf("Invalid.\n");
                continue;
            }
            if      (sc == 1) pantryAdd();
            else if (sc == 2) pantryListShow();
            else if (sc == 3) pantryEdit();
            else if (sc == 4) pantryDelete();
            else if (sc == 5) {
                savePantry();
                printf("Pantry saved.\n");
            }
        }
        else if (ch == 2) {
            printf("\nExpenses Menu: 1.Add 2.List 3.Edit 4.Delete 5.Save 0.Back\nChoice: ");
            readLine(choice, sizeof(choice));
            int sc;
            if (!stringToInt(choice, &sc)) {
                printf("Invalid.\n");
                continue;
            }
            if      (sc == 1) expenseAdd();
            else if (sc == 2) expenseListShow();
            else if (sc == 3) expenseEdit();
            else if (sc == 4) expenseDelete();
            else if (sc == 5) {
                saveExpenses();
                printf("Expenses saved.\n");
            }
        }
        else if (ch == 3) {
            printf("\nHealth Menu: 1.Add 2.List 3.Edit 4.Delete 5.Save 0.Back\nChoice: ");
            readLine(choice, sizeof(choice));
            int sc;
            if (!stringToInt(choice, &sc)) {
                printf("Invalid.\n");
                continue;
            }
            if      (sc == 1) healthAdd();
            else if (sc == 2) healthListShow();
            else if (sc == 3) healthEdit();
            else if (sc == 4) healthDelete();
            else if (sc == 5) {
                saveHealth();
                printf("Health saved.\n");
            }
        }
        else if (ch == 4) {
            printf("\nMovies Menu: 1.Add 2.List 3.Edit 4.Delete 5.Save 0.Back\nChoice: ");
            readLine(choice, sizeof(choice));
            int sc;
            if (!stringToInt(choice, &sc)) {
                printf("Invalid.\n");
                continue;
            }
            if      (sc == 1) movieAdd();
            else if (sc == 2) movieListShow();
            else if (sc == 3) movieEdit();
            else if (sc == 4) movieDelete();
            else if (sc == 5) {
                saveMovies();
                printf("Movies saved.\n");
            }
        }
        else if (ch == 5) {
            alertCenter();
        }
        else if (ch == 6) {
            editBudget();
        }
        else {
            printf("Invalid main menu option.\n");
        }
    }

    return 0;
}
