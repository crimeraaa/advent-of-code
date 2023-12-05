#include <cstdio>
#include <string>
#include <fstream>
#include <array>
#include <vector>

#define eprintf(msg) std::fprintf(stderr, __FILE__ "%i: " msg "\n", __LINE__) 

const std::array<const char*, 9> SPELLINGS = {
    "one", 
    "two", 
    "three", 
    "four", 
    "five", 
    "six", 
    "seven", 
    "eight", 
    "nine"
};

// Linear time operations which, when called in the main algorithm,
// prolly increases time taken quadratically! But it does work...
int match_spelling(const std::string &test) {
    for (size_t i = 0; i < SPELLINGS.size(); i++) {
        // Of type `const char *const &`
        auto &spelling = SPELLINGS[i];
        // Check if `spelling` is a subtring of `test`.
        if (test.find(spelling) != test.npos) {
            // Add 1 to get the integral value represented by this word
            return i + 1;
        }
    }
    return 0;
}

int match_numbers(const std::string &line) {
    std::vector<int> digits;
    std::string test = "";
    for (const char &c : line) {
        if (std::isdigit(c)) {
            // Adjust for ASCII encoding, remember: (char)'0' != (int)0
            digits.push_back(c - '0');
            test.clear();
            continue;
        } 
        // Implied else: assume everything else is a char LMAO
        test.push_back(c);
        int value = match_spelling(test);
        if (value > 0) {
            digits.push_back(value);
            // test = test.substr(test.length() - 1);
            // Reassign to be the latest char
            test.clear();
            test = c;
        }
    }
    if (digits.empty()) {
        return 0;
    }
    return (digits.at(0) * 10) + digits.at(digits.size() - 1);
}


int main(int argc, char *argv[]) {
    // Totally safe
    const char *name = (argc == 2) ? argv[1] : "./part2.txt";
    // Get file handle and validate it
    std::ifstream file(name);
    if (!file.is_open()) {
        eprintf("Could not open input file!");
        return 1;
    }
    // Read one line at a time
    std::string line;
    int sum = 0;
    while (std::getline(file, line)) {
        int digits = match_numbers(line);
        std::printf("%s : %i\n", line.c_str(), digits);
        sum += digits;
    }
    std::printf("Calibration Value: %i\n", sum);
    file.close();
    return 0;
}
