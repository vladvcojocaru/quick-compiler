#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "lexer.h"

Token tokens[MAX_TOKENS];
int nTokens;

int line = 1;  // the current line in the input file

// Adds a new token to the end of tokens array and returns it
// sets its code and line
Token *addToken(int code) {
    if (nTokens == MAX_TOKENS)
        // TODO: err("too many tokens");
        return 0;

    Token *tk = &tokens[nTokens];
    tk->code = code;
    tk->line = line;
    nTokens++;

    return tk;
}

char *copyn(char *dest, const char *begin, const char *end) {
    char *p = dest;

    if (end - begin > MAX_STR)
        // TODO: err("string too long");
        return 0;

    while (begin != end) *p++ = *begin++;

    *p = '\0';
    return dest;
}

void tokenize(const char *pch) {
    const char *start;
    Token *tk;
    char buf[MAX_STR + 1];

    for (;;) {
        switch (*pch) {
            // SPACES -> we just pretend like they don't exist
            case ' ':

            case '\t':
                pch++;
                break;

            case '\r':
                if (pch[1] == '\n') pch++;

            case '\n':
                line++;
                pch++;
                break;
                
            // DELIMITERS
            case '\0':
                addToken(FINISH);
                return;

            case ',':
                addToken(COMMA);
                pch++;
                break;

            case ':':
                addToken(COLON);
                pch++;
                break;
                
            case ';':
                addToken(SEMICOLON);
                pch++;
                break;

            case '(':
                addToken(LPAR);
                pch++;
                break;

            case ')':
                addToken(RPAR);
                pch++;
                break;

            // OPERATORS
            case '=':
                if (pch[1] == '=') {
                    addToken(EQUAL);
                    pch += 2;
                } else {
                    addToken(ASSIGN);
                    pch++;
                }

            // KEY WORDS
            default:
                if (isalpha(*pch) || *pch == '_') {
                    for (start = pch++; isalnum(*pch) || (*pch) == '_'; pch++) {
                    }

                    char *text = copyn(buf, start, pch);

                    if (strcmp(text, "int")) {
                        addToken(TYPE_INT);
                    } else if (strcmp(text, "real")) {
                        addToken(TYPE_REAL);
                    } else if (strcmp(text, "string")) {
                        addToken(TYPE_STR);
                    } else if (strcmp(text, "var")) {
                        addToken(VAR);
                    } else if (strcmp(text, "function")) {
                        addToken(FUNCTION);
                    } else if (strcmp(text, "if")) {
                        addToken(IF);
                    } else if (strcmp(text, "else")) {
                        addToken(ELSE);
                    } else if (strcmp(text, "while")) {
                        addToken(WHILE);
                    } else if (strcmp(text, "end")) {
                        addToken(END);
                    } else if (strcmp(text, "return")) {
                        addToken(RETURN);
                    } else {
                        tk = addToken(ID);
                        strcpy(tk->text, text);
                    }
                    pch++;
                }
        }
    }
}
