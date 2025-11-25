#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "movies.h"

static Movie movieList[MAX];
static int movieCount = 0;

void loadMovies(void)
{
    movieCount = 0;
    FILE *f = fopen(MOVIEFILE, "r");
    if (!f) return;

    char line[BUF];
    while (fgets(line, sizeof(line), f) && movieCount < MAX) {
        trim_newline(line);
        if (line[0] == '\0') continue;

        char tmp[BUF];
        strncpy(tmp, line, sizeof(tmp) - 1);
        tmp[sizeof(tmp) - 1] = 0;

        char *tok = strtok(tmp, ",");
        if (!tok) continue;

        strncpy(movieList[movieCount].title, tok, NAME_LEN - 1);
        movieList[movieCount].title[NAME_LEN - 1] = '\0';

        tok = strtok(NULL, ",");
        if (tok)
            strncpy(movieList[movieCount].genre, tok, CAT_LEN - 1);
        else
            movieList[movieCount].genre[0] = 0;

        tok = strtok(NULL, ",");
        if (tok)
            movieList[movieCount].rating = (float)atof(tok);
        else
            movieList[movieCount].rating = 0.0f;

        movieCount++;
    }

    fclose(f);
}

void saveMovies(void)
{
    FILE *f = fopen(MOVIEFILE, "w");
    if (!f) {
        printf("Unable to save movies file.\n");
        return;
    }

    for (int i = 0; i < movieCount; i++) {
        fprintf(f, "%s,%s,%.1f\n",
                movieList[i].title,
                movieList[i].genre,
                movieList[i].rating);
    }

    fclose(f);
}

void movieAlerts(void)
{
    int any = 0;

    if (movieCount == 0) {
        printf("   No movies in list.\n");
        return;
    }

    for (int i = 0; i < movieCount; i++) {
        if (movieList[i].rating < 5.0f) {
            printf("  Low-rated movie: %s (%.1f)\n",
                   movieList[i].title, movieList[i].rating);
            any = 1;
        }
    }

    if (!any) {
        printf("  No movie alerts.\n");
    }
}

void movieAdd(void)
{
    if (movieCount >= MAX) {
        printf("Movie list full.\n");
        return;
    }

    char buf[BUF];
    float r;

    printf("Enter movie name: ");
    readLine(movieList[movieCount].title, NAME_LEN);
    if (movieList[movieCount].title[0] == '\0') {
        printf("Name required.\n");
        return;
    }

    printf("Enter genre: ");
    readLine(movieList[movieCount].genre, CAT_LEN);

    printf("Enter rating (0-10): ");
    readLine(buf, sizeof(buf));
    if (!stringToFloat(buf, &r) || r < 0.0f || r > 10.0f) {
        printf("Invalid rating.\n");
        return;
    }

    movieList[movieCount].rating = r;
    movieCount++;

    saveMovies();
    printf("Movie added and saved.\n");
    movieAlerts();
}

void movieListShow(void)
{
    if (movieCount == 0) {
        printf("No movies added.\n");
        return;
    }

    for (int i = 0; i < movieCount; i++) {
        if (movieList[i].title[0] == '\0') continue;

        printf("%d. %s | %s | %.1f\n",
               i + 1,
               movieList[i].title,
               movieList[i].genre,
               movieList[i].rating);
    }
}

void movieEdit(void)
{
    char buf[BUF];
    int index;
    float newRating;

    movieListShow();
    if (movieCount == 0) {
        return;
    }

    printf("Enter the movie number you want to edit: ");
    readLine(buf, sizeof(buf));
    if (!stringToInt(buf, &index) || index < 1 || index > movieCount) {
        printf("Invalid movie number.\n");
        return;
    }

    index--;

    printf("Enter new title (press Enter to keep '%s'): ",
           movieList[index].title);
    readLine(buf, sizeof(buf));
    if (buf[0] != '\0') {
        strncpy(movieList[index].title, buf, NAME_LEN - 1);
        movieList[index].title[NAME_LEN - 1] = '\0';
    }

    printf("Enter new genre (press Enter to keep '%s'): ",
           movieList[index].genre);
    readLine(buf, sizeof(buf));
    if (buf[0] != '\0') {
        strncpy(movieList[index].genre, buf, CAT_LEN - 1);
        movieList[index].genre[CAT_LEN - 1] = '\0';
    }

    printf("Enter new rating (0 to 10, press Enter to keep %.1f): ",
           movieList[index].rating);
    readLine(buf, sizeof(buf));
    if (buf[0] != '\0') {
        if (stringToFloat(buf, &newRating) &&
            newRating >= 0.0f && newRating <= 10.0f) {
            movieList[index].rating = newRating;
        } else {
            printf("Invalid rating. Old rating is kept.\n");
        }
    }

    saveMovies();
    printf("Movie details updated and file saved.\n");
}

void movieDelete(void)
{
    char buf[BUF];
    int index;

    movieListShow();
    if (movieCount == 0) {
        return;
    }

    printf("Enter the movie number you want to delete: ");
    readLine(buf, sizeof(buf));
    if (!stringToInt(buf, &index) || index < 1 || index > movieCount) {
        printf("Invalid movie number.\n");
        return;
    }

    index--;

    for (int i = index; i < movieCount - 1; i++) {
        movieList[i] = movieList[i + 1];
    }

    movieCount--;
    saveMovies();

    printf("Movie removed and file updated.\n");
}
