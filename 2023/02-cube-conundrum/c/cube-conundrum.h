#ifndef AOC_2023_CUBE_CONUNDRUM_H
#define AOC_2023_CUBE_CONUNDRUM_H

#define ELF_CRITERIA_RED    12
#define ELF_CRITERIA_GREEN  13
#define ELF_CRITERIA_BLUE   14

// Hardcoded cause VSCode debugger uses workspace as CWD
#ifdef _WIN32
#define FALLBACK_DIRECTORY \
"C:/Users/crimeraaa/repos/advent-of-code/2023/02-cube-conundrum/"
#else
// Remember that Linux has a vastly different file system.
#define FALLBACK_DIRECTORY \
"/home/crimeraaa/repos/advent-of-code/2023/02-cube-conundrum/"
#endif

typedef enum CUBE_COLOR_INFO {
    CUBE_COLOR_RED, 
    CUBE_COLOR_GREEN, 
    CUBE_COLOR_BLUE, 
    // CUBE_COLOR_COUNT,
    // CUBE_COLOR_INVALID
} CUBE_COLOR_INFO;

typedef struct Cube {
    int count;
    CUBE_COLOR_INFO color;
} Cube;

typedef struct Set {
    struct Cube red, green, blue;
} Set;

#endif // AOC_2023_CUBE_CONUNDRUM_H
