#if defined(__MINGW32__) || defined(__MINGW64__)
#define __USE_MINGW_ANSI_STDIO_1
#endif

#include <cstdio>
#include <cmath>
#include <climits>
#include <iostream>
#include <sstream>
#include <utility>

#include "../memory.tcc"
#include "../bitmanip.hpp"

void list_powers_of_2()
{
    constexpr int bit_size = crim::bit::size<int>();
    for (int i = 0; i <= bit_size; i++) {
        std::printf("2^%i = %f\n", i, std::pow(2, i));
    }
}

// constexpr parameters aren't allowed so mimic them w/ template instantiations.
// e.g: `dump_bitlens<crim::bit::bit_size<int>()>();`
//  - https://stackoverflow.com/a/70322991
template<class T> void dump_bitlens()
{
    constexpr int bits = crim::bit::size<T>();
    int pad = crim::count_digits(bits);
    for (int i = 0; i <= bits; i++) {
        int result = crim::bit::next_power(i);
        std::printf("i: %*i\tNearest power of 2: %i\n", pad, i, result);
    }
}

int get_base(const std::string &s)
{
    // It's possible user just input "0" and nothing else
    if (s.at(0) == '0' && s.length() > 1) {
        switch (s.at(1)) {
            case 'b': return 2;
            case 'o': return 8;
            case 'd': return 10;
            case 'x': return 16;
        }
    }
    return 10;
}

int get_number(const char *prompt)
{
    std::string input;
    // This is a valid loop condition: https://stackoverflow.com/a/47612234 
    // You can press CTRL-D or CTRL-C to stop asking for input in this function.
    while ((std::cout << prompt) && (std::getline(std::cin, input))) {
        try {
            return std::stoi(input, nullptr, get_base(input));         
        } catch (const std::invalid_argument &err) {
            std::cout << err.what() << ": not a number!\n";
        } catch (const std::out_of_range &err) {
            std::cout << err.what() << ": out of range!\n";
        }
    }
    return 0;
}

void test_next_power()
{
    int n{get_number("Enter a number: ")};

    // This function already checks when value == 0;
    size_t power{crim::bit::next_power(n)};

    // Subtract 1 so that powers of 2 get the correct length
    size_t exponent{(n == 0) ? 0 : crim::bit::length(n - 1)};

    std::cout << "The nearest power of 2 is " << power << ", "
              << "A.K.A. 2 raised to " << exponent << ".\n";
}

int main()
{
    // User can press CTRL-D to end this loop.
    while (!std::cin.eof()) {
        test_next_power();
    }
    return 0;
}
