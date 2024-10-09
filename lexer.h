enum {
    ID,
    // key words
    TYPE_INT,
    // delimiters
    COMMA,
    FINISH,
    // operators
    ASSIGN,
    EQUAL,

};

#define MAX_STR 127

typedef struct {
    int code;  // ID, TYPE_INT etc.
    int line;  // Line from the input file
    union {
        char text[MAX_STR + 1];  // TYPE ID
        int i;                   // TYPE_INT
        double r;                // TYPE REAL
    };

}Token;

#define MAX_TOKENS 4096
extern Token tokens[];
extern int nTokens;

void tokenize(const char *pch);
void showTokens();
