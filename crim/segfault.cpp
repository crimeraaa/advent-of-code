#include <stdio.h>
#include "string.hpp"
#include "io.hpp"

int main() {
    crim::string name = crim::get_string("What's your name? ");
    crim::string fave = crim::get_string("Hi %s! What's your favorite animal? ", name.c_str());
    printf("Cool, I love '%s' too!\n", fave.c_str());
    return 0;
}
