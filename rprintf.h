#ifndef RPRINTF_H
#define RPRINTF_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum {
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_INT_ARR,
    TYPE_DOUBLE_ARR,
    TYPE_UNKNOWN_PTR
} ArgType;

typedef struct {
    ArgType type;
    union {
        int    i;
        double d;
        char   c;
        struct { const char   *ptr;             } str;
        struct { const int    *ptr; size_t len; } iarr;
        struct { const double *ptr; size_t len; } darr;
        struct { const void   *ptr;             } unk;
    };
} Arg;

Arg make_int(int x);
Arg make_double(double x);
Arg make_char(char x);
Arg make_string(const char *x);
Arg make_unknown_ptr(const void *x);
Arg make_int_arr(const int *x);
Arg make_double_arr(const double *x);

void registry_set(const char *name, Arg val);
Arg *registry_get(const char *name);

int rprintf(const char *fmt);

#define MAKE_ARG(x) _Generic((x),          \
    int:           make_int,                \
    double:        make_double,             \
    char:          make_char,               \
    char*:         make_string,             \
    const char*:   make_string,             \
    int*:          make_int_arr,            \
    const int*:    make_int_arr,            \
    double*:       make_double_arr,         \
    const double*: make_double_arr,         \
    default:       make_unknown_ptr         \
)(x)

#define let(var, value)                     \
    __typeof__(value) var = (value);        \
    registry_set(#var, MAKE_ARG(var))

#define watch(var) registry_set(#var, MAKE_ARG(var))

#endif
