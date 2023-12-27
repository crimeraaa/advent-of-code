#include <stdarg.h>
#include <stdio.h>

wchar_t *wformat(const wchar_t *fmt, ...) {
    va_list argp;
    va_start(argp, fmt);

    // Standard wide-char version is vsw, not vsnw.
    wint_t n_size = vswprintf(NULL, 0, fmt, argp);
    va_end(argp);
    if (n_size < 0) {
        perror("Failed to determine required size for formatted string");
        return NULL;
    }
    n_size++;

    wchar_t *p_buffer = new wchar_t[n_size];
    va_start(argp, fmt);
    n_size = vswprintf(p_buffer, n_size, fmt, argp);
    va_end(argp);

    if (n_size < 0) {
        delete[] p_buffer;
        perror("AAAA");
        return NULL;
    }
    return p_buffer;
}

char *format(const char *fmt, ...) {
    va_list argp;
    va_start(argp, fmt);

    // Determine the required size for this string: and yes, this is standard!
    int n_size = vsnprintf(NULL, 0, fmt, argp);
    va_end(argp);
    if (n_size < 0) {
        perror("Failed to determine required size for formatted string");
        return NULL;
    }
    n_size += 1;  // 1 more byte for nul '\0'

    char *p_buffer = new char[n_size]; // could throw std::bad_alloc!
    va_start(argp, fmt); // reset cuz was consumed by vsnprintf
    n_size = vsnprintf(p_buffer, n_size, fmt, argp);
    va_end(argp);

    if (n_size < 0) {
        delete[] p_buffer;
        perror("Failed to write to formatted string");
        return NULL;
    }
    return p_buffer;
}

// This is a template function specialization, where the implementation has to be
// redefined.
// template char *format<char>(const char *fmt, ...);

int main(int argc, char *argv[]) {
    return 0;
}
