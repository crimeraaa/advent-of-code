#include <cstdio>

#include "../input.hpp"
#include "../base_string.tcc"

namespace test {
    using string = crim::cstring;
    void print_what(const string &s) {
        std::printf(
            "\"%s\"\n\tcount: %zu, capacity: %zu\n", 
            s.c_str(),
            s.length(), 
            s.capacity()
        );
    }

    using test_fn = string (*)(string &src); 
    void test_body(const char *p_msg, test_fn fn) {
        string s1 = p_msg;
        string s2 = fn(s1);
        print_what(s1);
        print_what(s2);
    }
    
    void test_body_copy(const char *p_msg) {
        test_body(p_msg, [](string &src)->string {
            return src;
        });
    }
    
    void test_body_move(const char *p_msg) {
        test_body(p_msg, [](string &src)->string {
            return std::move(src);
        });
    }
    
    void ctor_short() {
        string s = "Short ctor.";
        print_what(s);
    }

    void ctor_long() {
        string s = "I am the basic constructor for long strings.";
        print_what(s);
    }
    
    void copy_short() {
        test_body_copy("Short copy!");
    }
    
    void copy_long() {
        test_body_copy("Loooooooooooong copy!!!");
    }
    
    void move_short() {
        test_body_move("Short move!");
    }

    void move_long() {
        test_body_move("Looooooooooooooooooooong move!");
    }
    
    void run_tests() {
        ctor_short();
        ctor_long();
        copy_short();
        move_short();
        copy_long();
        move_long();
    }
    
    void test_get_string() {
        using crim::get_string;
        auto name = get_string("Enter your name: ");
        auto food = get_string("Hi %s! What's your favorite food? ", name.c_str());
        print_what(name);
        print_what(food);
    }
}

int main() {
    test::run_tests();
    test::test_get_string();
    return 0;
}
