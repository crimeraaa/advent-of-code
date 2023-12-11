#include <ctype.h> // isdigit
#include <stdbool.h>
#include <stddef.h> // ptrdiff_t
#include <stdlib.h> // malloc family, NULL
#include <stdio.h> // printf family, FILE*, f_ family
#include <string.h> // str_ family

// Header-only cause I cannot be bothered to deal with translatoin units today
#define CRI_IO_HELPERS_IMPL 1
#include <helpers.h> // Pass -I../.. or /I../.. or --include=../.. flag
#include "cube-conundrum.h"

/** 
 * Determine output string to use.
 * @param color_id Preferably pass in `(Cube) cube.color`
*/
const char *game_get_color(CubeColor color_id)
{
    switch (color_id)
    {
        case CUBE_COLOR_RED:    return "red"; 
        case CUBE_COLOR_GREEN:  return "green";
        case CUBE_COLOR_BLUE:   return "blue";
    }
    return "(null)";
}

Cube game_get_cube(const char *elem_start, const char *elem_end)
{
    Cube cube = {0};
    const char *elem = elem_start;
    char c;
    // Construct a number going right
    while (elem < elem_end && (c = *elem) && isdigit(c)) {
        cube.count = (cube.count * 10) + c - '0';
        elem++;
    }
    const char *color = strchr(elem, ' ') + 1; // 1st non-digit
    ptrdiff_t length = elem_end - color;

    // Limit comparison via substring's determined length.
    if (strncmp("red", color, length) == 0) {
        cube.color = CUBE_COLOR_RED;
    } else if (strncmp("green", color, length) == 0) {
        cube.color = CUBE_COLOR_GREEN;
    } else if (strncmp("blue", color, length) == 0) {
        cube.color = CUBE_COLOR_BLUE;
    }
    // Don't separate cubes in a set with newlines, takes up too much space
    printf("%i %s, ", cube.count, game_get_color(cube.color));
    return cube;
}

// Tallies the current semicolon separated set.
Set game_get_set(int setno, const char *sets, const char *semi)
{
    Set set = {0}; // current set
    const char *comma = NULL;
    while (comma < semi) {
        comma = strchr(sets, ','); 
        // limit `comma` if out of bounds for the current set or is last set
        comma = (comma == NULL || comma >= semi) ? semi : comma;
        Cube cube = game_get_cube(sets, comma);
        // Update current set counter so that all colors are accounted for.
        switch (cube.color)
        {
            case CUBE_COLOR_RED: set.red = cube.count; break;
            case CUBE_COLOR_GREEN: set.green = cube.count; break;
            case CUBE_COLOR_BLUE: set.blue = cube.count; break;
        }
        sets = strchr(comma, ' ') + 1; // try to point to start of next element
    }
    return set; // copy by value is fine, it's just 3 ints
}

// Update `max->{color}`, passed as `original`.
// @warning Can't declare as just `inline`: https://stackoverflow.com/a/19069107
static inline int get_higher(int comparison, int original)
{
    return (comparison > original) ? comparison : original;
}

// Determines the minimum number of cubes per color needed for this game.
int game_get_power(const char *sets, const char *endl)
{
    Set max = {0};
    const char *semi = NULL;
    int setno = 1;
    while ((semi < endl) && (sets != NULL)) {
        printf("set %i: ", setno++);
        semi = strchr(sets, ';'); // exact position in string
        semi = (semi == NULL) ? endl : semi; // limit to endl if out of bounds

        // ptrdiff_t setlen = semi - set; // Substr. length w/o semi

        // Split the current set into its cube counts.
        Set set = game_get_set(setno, sets, semi);

        // Update all 3 maximum colors separately
        max.red = get_higher(set.red, max.red);
        max.green = get_higher(set.green, max.green);
        max.blue = get_higher(set.blue, max.blue);

        // Find whitespace after semi and point to char past that
        // @note Ternary is to avoid reading unknown memory.
        sets = (semi < endl) ? strchr(semi + 1, ' ') + 1 : endl;
        printf("\n"); // Prettier printout between sets
    }
    printf("\n"); // Prettier printout between games
    return max.red * max.green * max.blue;
}

int game_get_sum(char **lines, int linecount)
{
    int sum = 0;
    for (int i = 0; i < linecount; i++) {
        char *line = lines[i];
        const char *endl = strchr(line, '\0');
        const char *colon = strchr(line, ':');
        const char *sets = strchr(colon, ' ') + 1; // ptr to very first set's char
        int game_id = 0;
        // ptrdiff_t gameinfo_len = colon - line; // Substr. length w/o colon
        sscanf(line, "Game %d:", &game_id);
        printf("<GAME> %d : \"%s\"\n", game_id, sets);
        sum += game_get_power(sets, endl);
    }
    return sum;
}

int main(int argc, char *argv[])
{
    int exitcode = EXIT_SUCCESS;
    if (argc != 1 && argc != 2) {
        printf("Usage: %s [inputfile]\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char *filename = (argc == 2) ? argv[1] : FALLBACK_DIRECTORY "sample.txt";
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        eprintf("Failed to open input file.");
        return 2;
    }
    StrVector lines = cri_readfile(file);
    if (lines.buffer == NULL) {
        fclose(file);
        return 3;
    }
    printf("<SUM>: %i\n", game_get_sum(lines.buffer, lines.last));
    cri_strvec_delete(&lines); // Not 100% needed for toy programs
    fclose(file); // but these are good habits to develop anyways
    return exitcode;
}
