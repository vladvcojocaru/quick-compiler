#pragma once

#include <stdbool.h>

// Represents the type and properties of a return value during parsing
typedef struct {
    int type;  // Type of the value (e.g., integer, float, etc.)
    bool lval; // Whether the value is a left-value (modifiable)
} Ret;

// Global variable to store return values from parsing rules
extern Ret ret;

// Enumerations for different kinds of symbols in the symbol table
enum { KIND_VAR, KIND_ARG, KIND_FN };

// Forward declaration for Symbol structure
struct Symbol;
typedef struct Symbol Symbol;

// Represents an identifier (variable, function, or argument) in the symbol
// table
struct Symbol {
    const char *name; // Name of the identifier (points to a token's name)
    int kind;         // Type of the symbol (KIND_VAR, KIND_ARG, or KIND_FN)
    int type;         // Data type of the symbol (e.g., TYPE_INT, TYPE_FLOAT)
    union {
        Symbol *args; // For functions, points to a linked list of arguments
        bool local;   // For variables, indicates whether it is a local variable
    };
    Symbol *next; // Pointer to the next symbol in the linked list
};

// Forward declaration for Domain structure
struct Domain;
typedef struct Domain Domain;

// Represents a scope or "domain" in the program
struct Domain {
    Domain *parent;  // Pointer to the parent domain (or NULL if global scope)
    Symbol *symbols; // Linked list of symbols defined in this domain
};

// Global pointer to the current domain (top of the symbol table stack)
extern Domain *symTable;

// Pointer to the current function being parsed (NULL if outside a function)
extern Symbol *crtFn;

// Adds a new scope to the symbol table and makes it the current domain
Domain *addDomain();

// Removes the current domain from the symbol table, reverting to the parent
void delDomain();

// Searches for a symbol by name in the current domain only
Symbol *searchInCurrentDomain(const char *name);

// Searches for a symbol by name in the current and parent domains
Symbol *searchSymbol(const char *name);

// Adds a new symbol (variable, function, etc.) to the current domain
Symbol *addSymbol(const char *name, int kind);

// Adds an argument to a function's symbol
Symbol *addFnArg(Symbol *fn, const char *argName);
