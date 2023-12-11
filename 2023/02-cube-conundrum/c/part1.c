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
    printf("%i %s, ", cube.count, game_get_color(cube.color)); //! DEBUG
    return cube;
}

// Compares the current cube to the elf's criteria.
bool game_validate_cube(const Cube cube)
{
    switch (cube.color)
    {
        case CUBE_COLOR_RED:    return cube.count <= ELF_CRITERIA_RED;
        case CUBE_COLOR_GREEN:  return cube.count <= ELF_CRITERIA_GREEN;
        case CUBE_COLOR_BLUE:   return cube.count <= ELF_CRITERIA_BLUE;
    }
    return false;
}

// Determines if this set fits the elf's criteria.
bool game_get_set(const char *sets, const char *semi)
{
    const char *comma = NULL;
    while (comma < semi) {
        comma = strchr(sets, ',');
        // limit comma if out of bounds for this set or is very last set
        comma = (comma == NULL || comma >= semi) ? semi : comma;
        Cube cube = game_get_cube(sets, comma);
        if (!game_validate_cube(cube)) {
            return false;
        }
        sets = strchr(comma, ' ') + 1; // try to point to start of next element
    }
    return true;
}

bool game_evalute_sets(const char *sets, const char *endl)
{
    const char *semi = NULL;
    int setno = 1;
    while ((semi < endl) && (sets != NULL)) {
        printf("set %i: ", setno++);
        semi = strchr(sets, ';'); // points to exact position in string
        semi = (semi == NULL) ? endl : semi; // account for very last set
        bool set_fits_criteria = game_get_set(sets, semi);
        printf("\n"); // Slightly prettier printout between sets
        if (!set_fits_criteria) {
            return false;
        }
        // Find whitespace after semi and point to char past that
        // @note Ternary is to avoid reading unknown memory.
        sets = (semi < endl) ? strchr(semi + 1, ' ') + 1 : sets;
    }
    return true;
}

// Tests if given game matches the elf's criteria and sums up all valid games.
int game_get_sum(char **lines, int linecount)
{
    int sum = 0;
    for (int i = 0; i < linecount; i++) {
        const char *line = lines[i];
        const char *endl = strchr(line, '\0');
        const char *colon = strchr(line, ':');
        // ptr to very first set's char
        const char *sets = strchr(colon, ' ') + 1; 
        int game_id = 0;
        // ptrdiff_t gameinfo_len = colon - line; // Substr. length w/o colon
        sscanf(line, "Game %d:", &game_id); // dangerous but I'm lazy

        printf("<GAME> %d: %s\n", game_id, sets);
        bool game_fits_criteria = game_evalute_sets(sets, endl);
        if (game_fits_criteria) {
            printf("<VALID>: Met the elf's criteria!\n"); //! DEBUG
        } else {
            printf("<INVALID>: Didn't meet the elf's criteria!\n"); //! DEBUG
        }
        printf("\n");
        sum += (game_fits_criteria) ? game_id : 0;
    }
    return sum;
}

int main(int argc, char *argv[])
{
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
    return EXIT_SUCCESS;
}
