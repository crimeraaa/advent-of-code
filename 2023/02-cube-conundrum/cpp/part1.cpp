#include "part1.hpp"

using namespace std; // I cannot be bothered to deal with C++'s crap today

// Remove first leading whitespace
// @warning too lazy to error check lmao
string trim(string src) {
    char leading_whitespace = src.find_first_of(' ');
    if (leading_whitespace == 0) {
        return src.erase(0, 1);
    }
    // No leading whitespace so don't try to erase this index
    return src;
}

// Inspired by Lua's `string.sub`, cause C++'s `std::string::substr` is stupid.
// `std::string::substr` has string length as 2nd argument for some reason...
// @param start inclusive index.
// @param stop exclusive index. Defaults to remainder of `source`.
// @warning No error checking! Good luck!
string substring(const string &source, size_t start, size_t stop = string::npos) {
    string sub;
    for (size_t i = start; i < stop && i < source.length(); i++) {
        sub.push_back(source.at(i));
    }
    return sub;
}

// Given a set that was separated from the others via the semicolon,
// e.g. `"6 red, 1 blue, 3 green;"` or `"2 blue, 1 red, 2 green"`
Set make_set(const string &set) {
    Set cubes;
    size_t start = 0, comma = 0;
    do {
        comma = set.find(',', start);
        string cube = trim(substring(set, start, comma));
        // index of whitespace in the string
        size_t space = cube.find(' '); 
        // everything before space
        int count = stoi(substring(cube, 0, space)); 
        // everything after space
        string color = substring(cube, space + 1); 
        CUBE_ID id;
        // This sucks but whatever
        if (color == "red") {
            id = CUBE_ID::RED;
        } else if (color == "green") {
            id = CUBE_ID::GREEN;
        } else if (color == "blue") {
            id = CUBE_ID::BLUE;
        } else {
            eprintf("Invalid Cube ID!");
        }
        cubes[id].count = count;
        cubes[id].color = color;
        start = comma + 1;
    } while (comma != set.npos);
    return cubes;
}

bool check_cube(Set &set, CUBE_ID id) {
    const Cube &cube = set[id];
    if (cube.count == 0) {
        return true;
    }
    printf("%i %s, ", cube.count, cube.color.c_str());
    // Compare against Elf's proposed values
    switch (id) {
        case CUBE_ID::RED:   return cube.count <= 12;
        case CUBE_ID::GREEN: return cube.count <= 13;
        case CUBE_ID::BLUE:  return cube.count <= 14;
        default:             eprintf("Invalid cube ID value!");
    }
    return false;
}
 
int tokenize_game(const string &buffer) {
    // This is a VERY bad idea but it DOES work...
    // int id = atoi(gameinfo.substr(gameinfo.find(' ') + 1).c_str());
    
    // excludes `':'` in resulting substrings, keep this around for later!
    size_t colon = buffer.find(':'); 

    // substring `"Game <number>"`, exclude whitespace and colon
    int id = stoi(substring(buffer, buffer.find(' ') + 1, colon)); 

    // remainder of main string past the colon, remove leading whitespace
    string results = trim(substring(buffer, colon + 1));

    // scanf family format reference: https://en.cppreference.com/w/c/io/fscanf
    // sscanf(number.c_str(), "Game %d", &game.id);

    printf("Game: %i\n", id);
    size_t start = 0, semi = 0; // keep track of substring indexes
    int count = 1;
    do {
        semi = results.find(';', start);
        // Indiv set: `"6 red, 1 blue, 3 green;"` or `"2 blue, 1 red, 2 green"`
        Set set = make_set(trim(substring(results, start, semi)));
        printf("\tSet %i: ", count++);
        bool fits = check_cube(set, CUBE_ID::RED)
                 && check_cube(set, CUBE_ID::GREEN)
                 && check_cube(set, CUBE_ID::BLUE);
        printf("\n");
        start = semi + 1;
        if (!fits) {
            printf("\tGame does not fit Elf's criteria!\n");
            // id = 0; // uncomment this to verify if all sets being printed
            return 0;
        }
    } while (semi != results.npos);
    return id;
} 

int main(int argc, char *argv[]) {
    // Game numbers, comma separated elems, semicolon separated sets
    ifstream file((argc == 2) ? argv[1] : "../part1.txt");
    if (!file.is_open()) {
        eprintf("Failed to open input file!");
        return 1;
    }
    int sum = 0; // sum of valid games
    string buffer; // a line ends at first newline char
    // Fill buffer w/ contents of line, sans newline
    while (getline(file, buffer)) {
        sum += tokenize_game(buffer);
    }
    printf("Sum: %i\n", sum);
    file.close();
    return 0;
}
