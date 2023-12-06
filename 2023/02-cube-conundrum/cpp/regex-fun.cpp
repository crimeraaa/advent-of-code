#include <cstdio>
#include <string>
#include <regex>

using namespace std; // cannot be bothered to deal with C++'s crap today

int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        printf("Raw: %s\n", argv[i]);
        string thing = regex_replace(
            argv[i],
            regex("[^[:digit:]]*([[:digit:]]+).*"), // grab only first sequence of digits
            string("$1") // return first capture only, nothing else
        );
        printf("Regex: %s\n", thing.c_str());
    }
    return 0;
}
