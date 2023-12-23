#include <stdio.h>

namespace crim {
    template<class CharT> struct Parent;
    struct Child;
};

template<class CharT> struct crim::Parent {
    const CharT *m_name; // new phone who dis
    Parent(const CharT *name = "Bobby") : m_name{name} {
        printf("(Parent) Calling constructor for %s!\n", m_name);
    }
    ~Parent() {
        printf("(Parent) Bye %s :(\n", m_name);
    }
    // If not overriden by child classes, this one will be called!
    virtual void greet() {
        printf("(Parent) Hi %s!\n", m_name);
    }
};

struct crim::Child : public crim::Parent<char> {
    Child(const char *name = "Timmy") : crim::Parent<char>(name) {
        printf("(Child) Calling constructor for %s!\n", m_name);
    }
    // ~Child() {
    //     printf("(Child) Bye %s :(\n", m_name);
    // }
    // virtual void greet() {
    //     printf("(Child) Hi %s!\n", m_name);
    // }
};

int main(int argc, char *argv[]) {
    /*```cpp
    // constructor heirarchy
    crim::Parent<char>::Parent("Bobby");

    // destructor heirarchy
    crim::Parent::~Parent<char>()
    ```*/
    crim::Parent<char> bobby;

    /*```cpp
    // constructor heirarchy:
    crim::Child::Child("Timmy"); crim::Parent<char>::Parent("Timmy");

    // destructor heirarchy:
    crim::Child::~Child("Timmy"); crim::Parent<char>::~Parent("Timmy");
    ```*/
    crim::Child timmy;

    bobby.greet();
    timmy.greet();
    // crim::Parent<char> *base = &timmy; // Valid, but still uses timmy's greet
    // crim::Child *inherited = &bobby; // Not valid!
    // base->greet();
    
    return 0;
}
