#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

typedef enum CrimErrorString {
    CRIM_STRING_SUCCESS,
    CRIM_STRING_ERROR
} CrimErrorString;

void logerror(const char *file, int line, const char *info) {
    fprintf(stderr, "%s:%i: ", file, line);
    perror(info);
}

#define logerror(info) logerror(__FILE__, __LINE__, info)

typedef struct CrimString {
    char *m_pbuffer;
    size_t m_ncount;
    size_t m_ncapacity;
} CrimString;

CrimErrorString crim_string_init(CrimString *self) {
    // Hopefully errno wasn't reset by now
    if (self == NULL) {
        logerror("crim_string_init");
        return CRIM_STRING_ERROR;
    }
    self->m_pbuffer = NULL;
    self->m_ncount = 0;
    self->m_ncapacity = 0;
    return CRIM_STRING_SUCCESS;
}

CrimString *crim_string_new() {
    CrimString *inst = malloc(sizeof(*inst));
    if (inst == NULL) {
        logerror("crim_string_new, malloc");
    }
    return inst;
}

void crim_string_delete(CrimString *self) {
    if (self == NULL) {
        return;
    }
    // Will be more complicated with arrays of heap-allocated memory
    free(self->m_pbuffer);
}

char crim_string_at(CrimString *self, size_t n_index) {
    if (n_index > self->m_ncount) {
        logerror("crim_string_at: invalid index");
        return '\0';
    }
    return self->m_pbuffer[n_index];
}

CrimErrorString crim_string_resize(CrimString *self, size_t n_newsize) {
    char *p_dummy = malloc(sizeof(*p_memory) * n_newsize);
    if (p_dummy == NULL) {
        logerror("crim_string_resize, malloc");
        return CRIM_STRING_ERROR;
    }
    // If trying to shorten buffer, we'll use newsize as our count
    if (n_newsize > self->m_ncount) {
        self->m_ncount = n_newsize;
    }
    for (size_t i = 0; i < m_ncount; i++) {
        p_dummy[i] = self->m_buffer[i];
    }
    free(self->m_pbuffer);
    self->m_pbuffer = p_dummy;
    self->m_nsize = n_newsize;
    return CRIM_STRING_SUCCESS;
}

CrimStringError crim_string_pushback(CrimString *self, char ch) {
    if (self->m_ncount + 1 > p_inst->m_ncapacity) {
        size_t n_size = (self->m_ncapacity == 0) ? 1 : self->m_ncapacity * 2;
        if (crim_string_resize(self, n_size) == CRIM_STRING_ERROR) {
            return CRIM_STRING_ERROR;
        }
    }
    self->m_pbuffer[p_inst->m_ncount++] = ch;
    return CRIM_STRING_SUCCESS;
}

char crim_string_popback(CrimString *self) {
    // m_ncount is 1-based so pre subtract 1 to get the correct top index
    size_t index_top = --self->m_ncount;
    char ch_top = self->m_pbuffer[index_top];
    self->m_pbuffer[self->m_ncount] = '\0';
    return ch_top;
}

