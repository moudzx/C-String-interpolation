#include "rprintf.h"

#define MAX_VARS 256

typedef struct {
    const char *name;
    Arg         val;
} VarEntry;

typedef struct {
    VarEntry entries[MAX_VARS];
    int       count;
} VarRegistry;

static VarRegistry registry = {0};

Arg make_int(int x) { return (Arg){ .type = TYPE_INT, .i = x }; }
Arg make_double(double x) { return (Arg){ .type = TYPE_DOUBLE, .d = x }; }
Arg make_char(char x) { return (Arg){ .type = TYPE_CHAR, .c = x }; }
Arg make_string(const char *x) { return (Arg){ .type = TYPE_STRING, .str = { x } }; }
Arg make_unknown_ptr(const void *x) { return (Arg){ .type = TYPE_UNKNOWN_PTR, .unk = { x } }; }

Arg make_int_arr(const int *x) {
    size_t len = 0;
    while (x[len] != 0) len++;
    return (Arg){ .type = TYPE_INT_ARR, .iarr = { x, len } };
}

Arg make_double_arr(const double *x) {
    size_t len = 0;
    while (x[len] != 0.0) len++;
    return (Arg){ .type = TYPE_DOUBLE_ARR, .darr = { x, len } };
}

static void print_arg(const Arg *a) {
    switch (a->type) {
    case TYPE_INT:         printf("%d",  a->i);       break;
    case TYPE_DOUBLE:      printf("%g",  a->d);       break;
    case TYPE_CHAR:        printf("%c",  a->c);       break;
    case TYPE_STRING:      printf("%s",  a->str.ptr); break;
    case TYPE_UNKNOWN_PTR: printf("%p",  a->unk.ptr); break;
    case TYPE_INT_ARR:
        printf("[");
        for (size_t i = 0; i < a->iarr.len; i++)
            printf(i ? ", %d" : "%d", a->iarr.ptr[i]);
        printf("]");
        break;
    case TYPE_DOUBLE_ARR:
        printf("[");
        for (size_t i = 0; i < a->darr.len; i++)
            printf(i ? ", %g" : "%g", a->darr.ptr[i]);
        printf("]");
        break;
    }
}

void registry_set(const char *name, Arg val) {
    for (int i = 0; i < registry.count; i++) {
        if (strcmp(registry.entries[i].name, name) == 0) {
            registry.entries[i].val = val;
            return;
        }
    }
    if (registry.count < MAX_VARS) {
        registry.entries[registry.count].name = name;
        registry.entries[registry.count].val  = val;
        registry.count++;
    }
}

Arg *registry_get(const char *name) {
    for (int i = 0; i < registry.count; i++)
        if (strcmp(registry.entries[i].name, name) == 0)
            return &registry.entries[i].val;
    return NULL;
}

int rprintf(const char *fmt) {
    const char *p = fmt;
    int resolved = 0;

    while (*p) {
        if (*p == '\\' && *(p+1) == '{') { putchar('{'); p += 2; continue; }
        if (*p == '\\' && *(p+1) == 'n') { putchar('\n'); p += 2; continue; }
        if (*p == '\\' && *(p+1) == 't') { putchar('\t'); p += 2; continue; }

        if (*p != '{') { putchar(*p++); continue; }

        const char *close = strchr(p, '}');
        if (!close) { putchar('{'); p++; continue; }

        size_t name_len = close - p - 1;
        char name[64] = {0};
        if (name_len < sizeof(name))
            strncpy(name, p + 1, name_len);

        Arg *a = registry_get(name);
        if (a) { print_arg(a); resolved++; }
        else   printf("{%s:undefined}", name);

        p = close + 1;
    }
    return resolved;
}
