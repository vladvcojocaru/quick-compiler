#pragma once // makes sure the header file is included only once
enum {
    ID,
    
    // key words
    TYPE_INT,
    TYPE_REAL,
    VAR,
    FUNCTION,
    IF,
    ELSE,
    WHILE,
    END,
    RETURN,
    TYPE_STR,

    // delimiters
    COMMA,
    FINISH,
    COLON,
    SEMICOLON,
    LPAR,
    RPAR,

    // operators
    ADD,
    SUB,
    MUL,
    DIV,
    AND,
    OR,
    NOT,
    ASSIGN,
    EQUAL,
    NOTEQ,
    LESS,
    LESSEQ,
    GREATER,
    GREATEREQ,
};

#define MAX_STR 127

typedef struct {
    int code;  // ID, TYPE_INT etc.
    int line;  // Line from the input file
    union {
        char text[MAX_STR + 1];  // ID
        int i;                   // TYPE_INT
        double r;                // TYPE REAL
    };

} Token;

#define MAX_TOKENS 4096
extern Token tokens[];
extern int nTokens;

void tokenize(const char *pch);
void showTokens();
