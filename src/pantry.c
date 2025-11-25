#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pantry.h"

static Pantry pantryList[MAX];
static int pantryCount = 0;

void loadPantry(void)
{
    pantryCount = 0;
    FILE *f = fopen(PANTRYFILE, "r");
    if (!f) return;

    char line[BUF];
    while (fgets(line, sizeof(line), f) && pantryCount < MAX) {
        trim_newline(line);
        if (line[0] == '\0') continue;

        char tmp[BUF];
        strncpy(tmp, line, sizeof(tmp) - 1);
        tmp[sizeof(tmp) - 1] = 0;

        char *tok = strtok(tmp, ",");
        if (!tok) continue;

        strncpy(pantryList[pantryCount].name, tok, NAME_LEN - 1);
        pantryList[pantryCount].name[NAME_LEN - 1] = '\0';

        tok = strtok(NULL, ",");
        if (tok)
            strncpy(pantryList[pantryCount].cat, tok, CAT_LEN - 1);
        else
            pantryList[pantryCount].cat[0] = 0;

        tok = strtok(NULL, ",");
        if (tok)
            pantryList[pantryCount].qty = atoi(tok);
        else
            pantryList[pantryCount].qty = 0;

        tok = strtok(NULL, ",");
        if (tok)
            strncpy(pantryList[pantryCount].expiry, tok, DATE_LEN - 1);
        else
            pantryList[pantryCount].expiry[0] = 0;

        pantryList[pantryCount].expiry[DATE_LEN - 1] = '\0';
        pantryCount++;
    }

    fclose(f);
}

void savePantry(void)
{
    FILE *f = fopen(PANTRYFILE, "w");
    if (!f) {
        printf("Unable to save pantry file.\n");
        return;
    }

    for (int i = 0; i < pantryCount; i++) {
        fprintf(f, "%s,%s,%d,%s\n",
                pantryList[i].name,
                pantryList[i].cat,
                pantryList[i].qty,
                pantryList[i].expiry[0] ? pantryList[i].expiry : "");
    }

    fclose(f);
}

void pantryAlerts(void)
{
    int any = 0;

    for (int i = 0; i < pantryCount; i++) {
        if (pantryList[i].qty <= 2) {
            printf("   Low stock: %s (qty=%d)\n",
                   pantryList[i].name, pantryList[i].qty);
            any = 1;
        }

        if (pantryList[i].expiry[0]) {
            long dexp = date_to_days(pantryList[i].expiry);
            long td   = today_days();
            long diff = dexp - td;

            if (diff < 0) {
                printf("   Expired: %s (expired %ld days ago on %s)\n",
                       pantryList[i].name, -diff, pantryList[i].expiry);
                any = 1;
            } else if (diff <= 5) {
                printf("  Near expiry: %s (in %ld day(s) on %s)\n",
                       pantryList[i].name, diff, pantryList[i].expiry);
                any = 1;
            }
        }
    }

    if (!any) {
        printf("   No pantry alerts.\n");
    }
}

void pantryAdd(void)
{
    char buffer[BUF];
    int quantity;

    if (pantryCount >= MAX) {
        printf("Pantry list is full. Cannot add more items.\n");
        return;
    }

    printf("Enter item name: ");
    readLine(buffer, sizeof(buffer));
    if (buffer[0] == '\0') {
        printf("Item name cannot be empty.\n");
        return;
    }
    strncpy(pantryList[pantryCount].name, buffer, NAME_LEN - 1);
    pantryList[pantryCount].name[NAME_LEN - 1] = '\0';

    printf("Enter category: ");
    readLine(pantryList[pantryCount].cat, CAT_LEN);
    pantryList[pantryCount].cat[CAT_LEN - 1] = '\0';

    printf("Enter quantity (number): ");
    readLine(buffer, sizeof(buffer));
    if (!stringToInt(buffer, &quantity) || quantity < 0) {
        printf("Invalid quantity. Item not added.\n");
        return;
    }
    pantryList[pantryCount].qty = quantity;

    printf("Enter expiry date (DD-MM-YYYY), or leave blank: ");
    readLine(pantryList[pantryCount].expiry, DATE_LEN);
    pantryList[pantryCount].expiry[DATE_LEN - 1] = '\0';

    pantryCount++;

    savePantry();
    printf("Pantry item added and file updated.\n");
}

void pantryListShow(void)
{
    if (pantryCount == 0) {
        printf("No pantry items.\n");
        return;
    }

    printf("\n%-4s %-25s %-15s %-12s %-12s\n",
           "No", "Name", "Category", "Amount", "Date");
    printf("---------------------------------------------------------------\n");

    for (int i = 0; i < pantryCount; i++) {
        printf("%-3d %-30s %-14s %-4d %s\n",
               i + 1,
               pantryList[i].name,
               pantryList[i].cat,
               pantryList[i].qty,
               pantryList[i].expiry);
    }

    printf("\n-- Pantry Alerts --\n");
    pantryAlerts();
}

void pantryEdit(void)
{
    pantryListShow();
    if (pantryCount == 0) return;

    char buf[BUF];
    int idx, q;

    printf("Enter item number to edit: ");
    readLine(buf, sizeof(buf));
    if (!stringToInt(buf, &idx) || idx < 1 || idx > pantryCount) {
        printf("Invalid number.\n");
        return;
    }

    idx--;

    printf("New name (enter to keep '%s'): ", pantryList[idx].name);
    readLine(buf, sizeof(buf));
    if (buf[0]) {
        strncpy(pantryList[idx].name, buf, NAME_LEN - 1);
        pantryList[idx].name[NAME_LEN - 1] = '\0';
    }

    printf("New category (enter to keep '%s'): ", pantryList[idx].cat);
    readLine(buf, sizeof(buf));
    if (buf[0]) {
        strncpy(pantryList[idx].cat, buf, CAT_LEN - 1);
        pantryList[idx].cat[CAT_LEN - 1] = '\0';
    }

    printf("New quantity (enter to keep %d): ", pantryList[idx].qty);
    readLine(buf, sizeof(buf));
    if (buf[0]) {
        if (!stringToInt(buf, &q) || q < 0)
            printf("Invalid quantity. Old value kept.\n");
        else
            pantryList[idx].qty = q;
    }

    printf("New expiry (enter to keep '%s'): ", pantryList[idx].expiry);
    readLine(buf, sizeof(buf));
    if (buf[0]) {
        strncpy(pantryList[idx].expiry, buf, DATE_LEN - 1);
        pantryList[idx].expiry[DATE_LEN - 1] = '\0';
    }

    savePantry();
    printf("Pantry updated and saved.\n");
}

void pantryDelete(void)
{
    pantryListShow();
    if (pantryCount == 0) return;

    char buf[BUF];
    int idx;

    printf("Enter item number to delete: ");
    readLine(buf, sizeof(buf));
    if (!stringToInt(buf, &idx) || idx < 1 || idx > pantryCount) {
        printf("Invalid number.\n");
        return;
    }

    idx--;

    for (int i = idx; i < pantryCount - 1; i++) {
        pantryList[i] = pantryList[i + 1];
    }

    pantryCount--;
    savePantry();
    printf("Pantry item deleted and saved.\n");
}

