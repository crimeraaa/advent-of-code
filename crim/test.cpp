#include <cstring>
#include <string>
#include "string.hpp"
#include "io.hpp"

int main(int argc, char *argv[])
{
    // Invokes the constructor w/ argument const char *
    crim::string pname = argv[0];
    crim::string greet = "Hi mom!";

    // Invokes the constructor w/ no arguments
    crim::string nothing;

    printf("Program name: \"%s\"\n", pname.c_str());
    printf("Greeting: \"%s\"\n", greet.c_str());
    printf("Nothing: \"%s\"\n", nothing.c_str());

    // Test the compound addition operator
    greet += " Hello there!";
    printf("Appended: \"%s\"\n", greet.c_str());

    // Testing if get_string, readline, readendl and isendl works
    crim::string uname = crim::get_string("Enter your name: ");
    printf("Hi %s!\n", uname.c_str());
    return 0;
}
