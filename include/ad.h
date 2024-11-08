#pragma once

#include <stdbool.h>

typedef struct {
    int type;
    bool lval;
} Ret;

enum { KIND_VAR, KIND_ARG, KIND_FN };

struct Symbol;
typedef struct Symbol Symbol;

struct Symbol {
    const char *name;
    int kind;
    int type;
    union {
        Symbol *args;
        bool local;
    };
    Symbol *next;
};

struct Domain {
    Domain *parent;
    Symbol *symbols;
};

extern Domain *symTable;
