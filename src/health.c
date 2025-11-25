#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "health.h"

static Health healthList[MAX];
static int healthCount = 0;

void loadHealth(void)
{
    healthCount = 0;
    FILE *f = fopen(HEALTHFILE, "r");
    if (!f) return;

    char line[BUF];
    while (fgets(line, sizeof(line), f) && healthCount < MAX) {
        trim_newline(line);
        if (line[0] == '\0') continue;

        char tmp[BUF];
        strncpy(tmp, line, sizeof(tmp) - 1);
        tmp[sizeof(tmp) - 1] = 0;

        char *tok = strtok(tmp, ",");
        if (!tok) continue;

        strncpy(healthList[healthCount].date, tok, DATE_LEN - 1);
        healthList[healthCount].date[DATE_LEN - 1] = '\0';

        tok = strtok(NULL, ",");
        if (tok)
            healthList[healthCount].water = (float)atof(tok);
        else
            healthList[healthCount].water = 0.0f;

        tok = strtok(NULL, ",");
        if (tok)
            healthList[healthCount].cal = atoi(tok);
        else
            healthList[healthCount].cal = 0;

        tok = strtok(NULL, ",");
        if (tok)
            healthList[healthCount].sleep = (float)atof(tok);
        else
            healthList[healthCount].sleep = 0.0f;

        tok = strtok(NULL, ",");
        if (tok)
            healthList[healthCount].steps = atoi(tok);
        else
            healthList[healthCount].steps = 0;

        healthCount++;
    }

    fclose(f);
}

void saveHealth(void)
{
    FILE *f = fopen(HEALTHFILE, "w");
    if (!f) {
        printf("Unable to save health file.\n");
        return;
    }

    for (int i = 0; i < healthCount; i++) {
        fprintf(f, "%s,%.2f,%d,%.2f,%d\n",
                healthList[i].date,
                healthList[i].water,
                healthList[i].cal,
                healthList[i].sleep,
                healthList[i].steps);
    }

    fclose(f);
}

void healthAlerts(void)
{
    int any = 0;

    for (int i = 0; i < healthCount; i++) {
        if (healthList[i].water < 2.0f) {
            printf("  Low water on %s: %.2f L\n",
                   healthList[i].date, healthList[i].water);
            any = 1;
        }
        if (healthList[i].sleep < 6.0f) {
            printf("  Low sleep on %s: %.2f hrs\n",
                   healthList[i].date, healthList[i].sleep);
            any = 1;
        }
        if (healthList[i].steps < 5000) {
            printf("  Low steps on %s: %d steps\n",
                   healthList[i].date, healthList[i].steps);
            any = 1;
        }
    }

    if (!any) {
        printf("   No health alerts.\n");
    }
}

void healthAdd(void)
{
    if (healthCount >= MAX) {
        printf("Health log full.\n");
        return;
    }

    char buf[BUF];
    float w, s;
    int c, st;

    today_str(healthList[healthCount].date, DATE_LEN);
    printf("Date: %s\n", healthList[healthCount].date);

    printf("Enter water intake (liters): ");
    readLine(buf, sizeof(buf));
    if (!stringToFloat(buf, &w) || w < 0.0f) {
        printf("Invalid.\n");
        return;
    }
    healthList[healthCount].water = w;

    printf("Enter calories: ");
    readLine(buf, sizeof(buf));
    if (!stringToInt(buf, &c) || c < 0) {
        printf("Invalid.\n");
        return;
    }
    healthList[healthCount].cal = c;

    printf("Enter sleep hours (e.g. 7.5): ");
    readLine(buf, sizeof(buf));
    if (!stringToFloat(buf, &s) || s < 0.0f) {
        printf("Invalid.\n");
        return;
    }
    healthList[healthCount].sleep = s;

    printf("Enter steps: ");
    readLine(buf, sizeof(buf));
    if (!stringToInt(buf, &st) || st < 0) {
        printf("Invalid.\n");
        return;
    }
    healthList[healthCount].steps = st;

    healthCount++;
    saveHealth();
    printf("Health record added and saved.\n");
    healthAlerts();
}

void healthListShow(void)
{
    if (healthCount == 0) {
        printf("No health records.\n");
        return;
    }

    printf("\nNo  Date        Water(L)  Calories  Sleep(h)  Steps\n");
    printf("---------------------------------------------------\n");
    for (int i = 0; i < healthCount; i++) {
        printf("%-3d %-11s %-9.2f %-9d %-9.2f %-6d\n",
               i + 1,
               healthList[i].date,
               healthList[i].water,
               healthList[i].cal,
               healthList[i].sleep,
               healthList[i].steps);
    }

    printf("\n-- Health Alerts --\n");
    healthAlerts();
}

void healthEdit(void)
{
    char buf[BUF];
    int index;
    float fvalue;
    int ivalue;

    healthListShow();
    if (healthCount == 0) {
        return;
    }

    printf("Enter the health record number you want to edit: ");
    readLine(buf, sizeof(buf));
    if (!stringToInt(buf, &index) || index < 1 || index > healthCount) {
        printf("Invalid record number.\n");
        return;
    }

    index--;

    printf("New water intake in liters (press Enter to keep %.2f): ",
           healthList[index].water);
    readLine(buf, sizeof(buf));
    if (buf[0] != '\0') {
        if (stringToFloat(buf, &fvalue) && fvalue >= 0) {
            healthList[index].water = fvalue;
        } else {
            printf("Invalid input. Old water value kept.\n");
        }
    }

    printf("New calories (press Enter to keep %d): ",
           healthList[index].cal);
    readLine(buf, sizeof(buf));
    if (buf[0] != '\0') {
        if (stringToInt(buf, &ivalue) && ivalue >= 0) {
            healthList[index].cal = ivalue;
        } else {
            printf("Invalid input. Old calorie value kept.\n");
        }
    }

    printf("New sleep hours (press Enter to keep %.2f): ",
           healthList[index].sleep);
    readLine(buf, sizeof(buf));
    if (buf[0] != '\0') {
        if (stringToFloat(buf, &fvalue) && fvalue >= 0) {
            healthList[index].sleep = fvalue;
        } else {
            printf("Invalid input. Old sleep value kept.\n");
        }
    }

    printf("New steps (press Enter to keep %d): ",
           healthList[index].steps);
    readLine(buf, sizeof(buf));
    if (buf[0] != '\0') {
        if (stringToInt(buf, &ivalue) && ivalue >= 0) {
            healthList[index].steps = ivalue;
        } else {
            printf("Invalid input. Old steps kept.\n");
        }
    }

    saveHealth();
    printf("Health record updated and saved.\n");
}

void healthDelete(void)
{
    healthListShow();
    if (healthCount == 0) return;

    char buf[BUF];
    int idx;

    printf("Enter record number to delete: ");
    readLine(buf, sizeof(buf));
    if (!stringToInt(buf, &idx) || idx < 1 || idx > healthCount) {
        printf("Invalid number.\n");
        return;
    }

    idx--;

    for (int i = idx; i < healthCount - 1; i++) {
        healthList[i] = healthList[i + 1];
    }

    healthCount--;
    saveHealth();
    printf("Health record deleted and saved.\n");
}
