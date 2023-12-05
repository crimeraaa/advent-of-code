#include <cstdio>
#include <string>
#include <fstream>
#include <vector>

#define eprintf(msg) std::fprintf(stderr, __FILE__ "%i: " msg "\n", __LINE__) 

// Get the leftmost and rightmost digit in the string `line`.
// @note Named return value optimization is a thing, so can return container by value!
int match_numbers(const std::string &line) {
    // Prolly memory intensive but eh
    std::vector<int> digits;
    for (auto &c : line) {
        if (std::isdigit(c)) {
            // Adjust for ASCII encoding, need the integral value represented
            digits.push_back(c - '0');
        }
    }
    // Avoid runtime exceptions cause we need to index into 0 at least!
    if (digits.empty()) {
        return 0;
    }
    // Even if length == 1, digits.size() = 1 so digits.size() - 1 = 0
    return (digits[0] * 10) + digits[digits.size() - 1];
}

int main(int argc, char *argv[]) {
    // Totally safe
    const char *name = (argc == 2) ? argv[1] : "./part1.txt";
    // Open file and test if we actually got a handle
    std::ifstream file(name);
    if (!file.is_open()) {
        eprintf("Could not open input file!");
        return 1;
    }
    // Read one line at a time
    std::string line;
    int sum = 0;
    // Use std::getline as an iterator of sorts
    while (std::getline(file, line)) {
        int first_last = match_numbers(line);
        // ! DEBUG PURPOSES ONLY
        // std::printf("line: '%s', digits: %i\n", line.c_str(), first_last);
        sum += first_last;
    }
    std::printf("Final answer: %i\n", sum);
    file.close();
    return 0;
}
