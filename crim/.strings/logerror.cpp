#include <stdexcept>
#include "logerror.hpp"

void delta() {
    try {
        throw std::runtime_error("Oooooooohhhh! An error!!!!");
    } catch (const std::runtime_error &err) {
        crim_logger_fn(err.what());
        crim_logerror("delta", "caught an error!");
    }    
}

void charlie() {
    crim_logerror("charlie", "tres");
    delta();
}

void bravo() {
    crim_logerror("bravo", "dos");
    charlie();
}

void alpha() {
    crim_logerror("alpha", "uno");
    bravo();
}

int main() {
    alpha();
    return 0;
}
