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
    constexpr int bit_size = crim::digits::bit_size<int>();
    for (int i = 0; i <= bit_size; i++) {
        std::printf("2^%i = %f\n", i, std::pow(2, i));
    }
}

// constexpr parameters aren't allowed so mimic them w/ template instantiations.
// e.g: `dump_bitlens<crim::digits::bit_size<int>()>();`
//  - https://stackoverflow.com/a/70322991
template<class T> void dump_bitlens()
{
    constexpr int bits = crim::digits::bit_size<T>();
    int pad = crim::digits::count_digits(bits);
    for (int i = 0; i <= bits; i++) {
        int result = crim::digits::bit_next_power(i);
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

std::pair<int, int> get_power_exponent_pair(int value)
{
    // The function already checks when value == 0.
    int power = crim::digits::bit_next_power(value);

    // When value == 0, bit length of -1 gives us 1 bits which is not desired!
    int exponent = (value == 0) ? 0 : crim::digits::bit_length(value - 1);    

    return std::make_pair(power, exponent);
}

void test_next_power(std::stringstream &output)
{
    int n = get_number("Enter a number: ");
    const auto [power, exponent] = get_power_exponent_pair(n);
    output << "The nearest power of 2 is " << power << ", "
           << "A.K.A. 2 raised to " << exponent << ".\n";
    std::cout << output.str();
    // To actually clear a stringstream: https://stackoverflow.com/a/2848109
    // It's marginally more efficient to call std::string's empty constructor 
    // than to call the std::string constructor with an empty C-string.
    output.str(std::string());
    output.clear();
}

int main()
{
    std::stringstream output;
    // User can press CTRL-D to end this loop.
    while (!std::cin.eof()) {
        test_next_power(output);
    }
    return 0;
}
