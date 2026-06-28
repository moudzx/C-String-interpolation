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

Arg make_int(int x) {
	return (Arg){ .type = TYPE_INT,    .i   = x };
}

Arg make_double(double x) {
	return (Arg){ .type = TYPE_DOUBLE, .d   = x };
}

Arg make_char(char x) {
	return (Arg){ .type = TYPE_CHAR,   .c   = x };
}

Arg make_string(const char *x) {
	return (Arg){ .type = TYPE_STRING, .str = { x } };
}

Arg make_unknown_ptr(const void *x){
	return (Arg){ .type = TYPE_UNKNOWN_PTR, .unk = { x } };
}

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

void print_arg(const Arg *a) {
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

#define MAX_VARS 256

typedef struct {
	const char *name;
	Arg         val;
} VarEntry;

typedef struct {
	VarEntry entries[MAX_VARS];
	int      count;
} VarRegistry;

static VarRegistry registry = {0};

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

#define let(var, value)                     \
	__typeof__(value) var = (value);        \
	registry_set(#var, MAKE_ARG(var))

#define watch(var) registry_set(#var, MAKE_ARG(var))

int rprintf(const char *fmt) {
	const char *p = fmt;
	int resolved = 0;
	int placeholders = 0;

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

		placeholders++;

		Arg *a = registry_get(name);
		if (a) { print_arg(a); resolved++; }
		else     printf("{%s:undefined}", name);

		p = close + 1;
	}

	return resolved;
}

int main() {
	let(age, 26);
	let(pi, 3.14159);
	let(grade, 'A');
	let(name, "Moss");

	int scores[] = { 10, 20, 30, 0 };
	double readings[] = { 1.1, 2.2, 3.3, 0.0 };
	watch(scores);
	watch(readings);

	int n;

	n = rprintf("hello {name}, you are {age} years old\n");
	printf("resolved %d variable(s)\n", n);

	n = rprintf("pi is {pi}, grade is {grade}\n");
	printf("resolved %d variable(s)\n", n);

	n = rprintf("scores: {scores}\n");
	printf("resolved %d variable(s)\n", n);

	n = rprintf("repeat: {name} is still {name}\n");
	printf("resolved %d variable(s)\n", n);

	n = rprintf("escape \\{name} and undefined {ghost}\n");
	printf("resolved %d variable(s)\n", n);
}
