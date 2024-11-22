#include "../include/lexer.h"

#include <ctype.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/utils.h"

Token tokens[MAX_TOKENS];
int nTokens;

int line = 1; // the current line in the input file

const char *getTokenName(int code);
Token *addToken(int code);
char *copyn(char *dest, const char *begin, const char *end);
void showTokens();
void tokenize(const char *pch);

// Adds a new token to the end of tokens array and returns it
// sets its code and line
Token *addToken(int code) {
    if (nTokens >= MAX_TOKENS) {
        err("too many tokens\n");
    }
    Token *tk = &tokens[nTokens];
    tk->code = code;
    tk->line = line;

    // Debug statement
    // printf("Added token: %s (code: %d) at line: %d\n", getTokenName(code),
    // code,
    //        line);
    nTokens++;

    return tk;
}

char *copyn(char *dest, const char *begin, const char *end) {
    char *p = dest;

    if (end - begin > MAX_STR)
        err("string too long (line: %d)\n", line);

    while (begin != end)
        *p++ = *begin++;

    *p = '\0';
    return dest;
}

void showTokens() {
    for (int i = 0; i < nTokens; i++) {
        Token *tk = &tokens[i];
        printf("%d %s", tk->line, getTokenName(tk->code));

        // Check if the token code has associated text or numeric values to
        // display.
        if (tk->code == ID || tk->code == STRING) {
            printf(":%s", tk->text);
        } else if (tk->code == REAL) {
            printf(":%.2f", tk->r);
        } else if (tk->code == INT) {
            printf(":%d", tk->i);
        }

        // Move to the next line for each token.
        printf("\n");
    }
}

void tokenize(const char *pch) {
    for (;;) {
        switch (*pch) {
        case ' ':

        case '\t':
            pch++;
            break;

        case '\r':
            if (pch[1] == '\n')
                pch++;
            break;

        case '\n':
            line++;
            pch++;
            break;

        case '#':
            while (*pch != '\n' && *pch != '\0')
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
            break;

        case '+':
            addToken(ADD);
            pch++;
            break;

        case '-':
            addToken(SUB);
            pch++;
            break;

        case '*':
            addToken(MUL);
            pch++;
            break;

        case '/':
            addToken(DIV);
            pch++;
            break;

        case '&':
            if (pch[1] == '&') {
                addToken(AND);
                pch += 2;
            } else {
                err("invalid expression (line: %d)\n", line);
            }
            break;

        case '|':
            if (pch[1] == '|') {
                addToken(OR);
                pch += 2;
            } else {
                err("invalid expression (line: %d)\n", line);
            }
            break;

        case '!':
            if (pch[1] == '=') {
                addToken(NOTEQ);
                pch += 2;
            } else {
                addToken(NOT);
                pch++;
            }
            break;

        case '<':
            addToken(LESS);
            pch++;

            break;

        case '>':
            if (pch[1] == '=') {
                addToken(GREATEREQ);
                pch += 2;
            } else {
                addToken(GREATER);
                pch++;
            }
            break;

            // KEY WORDS & TYPES
        default:
            // Identifiers and keywords
            if (isalpha(*pch) || *pch == '_') {
                const char *start = pch;

                while (isalnum(*pch) || *pch == '_')
                    pch++;

                char buf[pch - start + 1];
                copyn(buf, start, pch);

                if (strcmp(buf, "int") == 0) {
                    addToken(TYPE_INT);
                } else if (strcmp(buf, "real") == 0) {
                    addToken(TYPE_REAL);
                } else if (strcmp(buf, "str") == 0) {
                    addToken(TYPE_STR);
                } else if (strcmp(buf, "var") == 0) {
                    addToken(VAR);
                } else if (strcmp(buf, "function") == 0) {
                    addToken(FUNCTION);
                } else if (strcmp(buf, "if") == 0) {
                    addToken(IF);
                } else if (strcmp(buf, "else") == 0) {
                    addToken(ELSE);
                } else if (strcmp(buf, "while") == 0) {
                    addToken(WHILE);
                } else if (strcmp(buf, "end") == 0) {
                    addToken(END);
                } else if (strcmp(buf, "return") == 0) {
                    addToken(RETURN);
                } else { // Identifier
                    Token *tk = addToken(ID);
                    strcpy(tk->text, buf);
                }
            } else if (isdigit(*pch)) {
                const char *start = pch;

                while (isdigit(*pch)) {
                    pch++;
                }

                if (*pch == '.') {
                    if (isdigit(pch[1])) {
                        pch++;
                        while (isdigit(*pch)) {
                            pch++;
                        }
                        char buf[pch - start + 1];
                        copyn(buf, start, pch);
                        Token *tk = addToken(REAL);
                        tk->r = atof(buf);
                    } else {
                        err("Digit missing after '.' (line: %d)", line);
                    }
                } else {
                    char buf[pch - start + 1];
                    copyn(buf, start, pch);
                    Token *tk = addToken(INT);
                    tk->i = atoi(buf);
                }
            } else if (*pch == '"') {
                pch++;
                const char *start = pch;

                while (*pch != '"' && *pch != '\0') {
                    pch++;
                }

                if (*pch == '\0') {
                    err("Unterminated string literal (line: %d)", line);
                } else {
                    char buf[pch - start + 1];
                    copyn(buf, start, pch);

                    Token *tk = addToken(STRING);
                    strcpy(tk->text, buf);

                    pch++;
                }

            } else {
                fprintf(stderr,
                        "Unrecognized character '%c' (ASCII: %d) at line: %d\n",
                        *pch, *pch, line);
                err("ID ERROR (line: %d)", line);
            }

            break;
        }
    }
}

const char *getTokenName(int code) {
    switch (code) {
    case ID:
        return "ID";
    case TYPE_INT:
        return "TYPE_INT";
    case TYPE_REAL:
        return "TYPE_REAL";
    case VAR:
        return "VAR";
    case FUNCTION:
        return "FUNCTION";
    case IF:
        return "IF";
    case ELSE:
        return "ELSE";
    case WHILE:
        return "WHILE";
    case END:
        return "END";
    case RETURN:
        return "RETURN";
    case TYPE_STR:
        return "TYPE_STR";
    case COMMA:
        return "COMMA";
    case FINISH:
        return "FINISH";
    case COLON:
        return "COLON";
    case SEMICOLON:
        return "SEMICOLON";
    case LPAR:
        return "LPAR";
    case RPAR:
        return "RPAR";
    case ADD:
        return "ADD";
    case SUB:
        return "SUB";
    case MUL:
        return "MUL";
    case DIV:
        return "DIV";
    case AND:
        return "AND";
    case OR:
        return "OR";
    case NOT:
        return "NOT";
    case ASSIGN:
        return "ASSIGN";
    case EQUAL:
        return "EQUAL";
    case NOTEQ:
        return "NOTEQ";
    case LESS:
        return "LESS";
    case GREATER:
        return "GREATER";
    case GREATEREQ:
        return "GREATEREQ";
    case REAL:
        return "REAL";
    case INT:
        return "INT";
    case STRING:
        return "STRING";
    default:
        return "UNKNOWN";
    }
}
