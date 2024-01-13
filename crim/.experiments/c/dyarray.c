#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

void logerror(const char *file, int line, const char *info) {
    fprintf(stderr, "%s%i: ", file, line);
    perror(info);
}

#define logerror(info) logerror(__FILE__, __LINE__, info)

typedef struct dyarray {
    void *m_pbuffer; // Pointer to 1D heap-allocated memory of whatever.
    size_t m_ncount; // 1-based number of elements currently in the buffer.
    size_t m_ncapacity; // 1-based total allocated memory for the buffer.
    size_t m_nsizeof; // Size of the "templated" type.
    // HACK: Simulate C++'s type templated functions, must be user-provided!
    void (*at)(struct dyarray *self, size_t n_index, void *p_where);
} dyarray;

typedef void (*dyarray_at_fn)(dyarray *self, size_t n_index, void *p_where);

static void dyarray_default_at(dyarray *self, size_t n_index, void *p_where) {
    // Treat pointer as consisting of "bytes", use unsigned for max guarantees
    unsigned char *p_item = self->m_pbuffer;
    memcpy(p_where, p_item + (n_index + self->m_nsizeof), self->m_nsizeof);
}

void dyarray_init(dyarray *self, size_t n_sizeof, dyarray_at_fn fn) {
    if (self == NULL) {
        logerror("dyarray_init: null pointer");
        return;
    }
    memset(self, 0, sizeof(*self));
    self->m_nsizeof = n_sizeof;
    self->at = (fn != NULL) ? fn : dyarray_default_at;
}

