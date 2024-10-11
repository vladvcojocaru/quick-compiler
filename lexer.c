#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "lexer.h"

Token tokens[MAX_TOKENS];
int nTokens;

int line = 1;  // the current line in the input file

const char* getTokenName(int code);
Token *addToken(int code);
char *copyn(char *dest, const char *begin, const char *end);
void showTokens();
void tokenize(const char *pch);

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

void showTokens(){
    for(int i = 0; i< nTokens; i++){
        Token *tk = &tokens[i];
        printf("Token %d: %s (line %d)\n", i, getTokenName(tk->code), tk->line);
        if(tk->code == ID){
            printf("Text: %s\n", tk->text);
        }
    }
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
                break;

            case '\n':
                line++;
                pch++;
                break;

            case '#':
                while(*pch != '\n' && *pch != '\0')
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
                if(pch[1] == '&'){
                    addToken(AND);
                    pch += 2;
                } else {
                    addToken(BITWISE_AND);
                    pch++;
                }
                break;

            case '|':
                if(pch[1] == '|'){
                    addToken(OR);
                    pch += 2;
                } else {
                    addToken(BITWISE_OR);
                    pch++;
                }
                break;

            case '!':
                if(pch[1] == '='){
                    addToken(NOTEQ);
                    pch += 2;
                } else {
                    addToken(NOT);
                    pch++;
                }
                break;

            case '^':
                addToken(BITWISE_XOR);
                pch++;
                break;

            case '~':
                addToken(BITWISE_NOT);
                pch++;
                break;

            case '<':
                if(pch[1] == '='){
                    addToken(LESSEQ);
                    pch += 2;
                } else if (pch[1] == '<'){
                    addToken(BITWISE_SHIFT_LEFT);
                    pch += 2;
                } else {
                    addToken(LESS);
                    pch++;
                }
            break;

            case '>':
                 if(pch[1] == '='){
                    addToken(GREATEREQ);
                    pch += 2;
                } else if (pch[1] == '>'){
                    addToken(BITWISE_SHIFT_RIGHT);
                    pch += 2;
                } else {
                    addToken(GREATER);
                    pch++;
                }
            break;
            

            // KEY WORDS
            default:
                // For alphabetic key words
                if (isalpha(*pch) || *pch == '_') {
                    for (start = pch++; isalnum(*pch) || (*pch) == '_'; pch++) {
                    }

                    char *text = copyn(buf, start, pch);

                    if (strcmp(text, "int") == 0) {
                        addToken(TYPE_INT);
                    } else if (strcmp(text, "real") == 0) {
                        addToken(TYPE_REAL);
                    } else if (strcmp(text, "string") == 0) {
                        addToken(TYPE_STR);
                    } else if (strcmp(text, "var") == 0) {
                        addToken(VAR);
                    } else if (strcmp(text, "function") == 0) {
                        addToken(FUNCTION);
                    } else if (strcmp(text, "if") == 0) {
                        addToken(IF);
                    } else if (strcmp(text, "else") == 0) {
                        addToken(ELSE);
                    } else if (strcmp(text, "while") == 0) {
                        addToken(WHILE);
                    } else if (strcmp(text, "end") == 0) {
                        addToken(END);
                    } else if (strcmp(text, "return") == 0) {
                        addToken(RETURN);
                    } else {
                        tk = addToken(ID);
                        strcpy(tk->text, text);
                    }
                } else {
                    pch++;
                }
                break;
        }
    }
}


// Just for readability
const char* getTokenName(int code) {
    switch (code) {
        case ID: return "ID";
        case TYPE_INT: return "TYPE_INT";
        case TYPE_REAL: return "TYPE_REAL";
        case VAR: return "VAR";
        case FUNCTION: return "FUNCTION";
        case IF: return "IF";
        case ELSE: return "ELSE";
        case WHILE: return "WHILE";
        case END: return "END";
        case RETURN: return "RETURN";
        case TYPE_STR: return "TYPE_STR";
        case COMMA: return "COMMA";
        case FINISH: return "FINISH";
        case COLON: return "COLON";
        case SEMICOLON: return "SEMICOLON";
        case LPAR: return "LPAR";
        case RPAR: return "RPAR";
        case ADD: return "ADD";
        case SUB: return "SUB";
        case MUL: return "MUL";
        case DIV: return "DIV";
        case AND: return "AND";
        case OR: return "OR";
        case NOT: return "NOT";
        case ASSIGN: return "ASSIGN";
        case EQUAL: return "EQUAL";
        case NOTEQ: return "NOTEQ";
        case LESS: return "LESS";
        case LESSEQ: return "LESSEQ";
        case GREATER: return "GREATER";
        case GREATEREQ: return "GREATEREQ";
        case BITWISE_AND: return "BITWISE_AND";
        case BITWISE_OR: return "BITWISE_OR";
        case BITWISE_XOR: return "BITWISE_XOR";
        case BITWISE_NOT: return "BITWISE_NOT";
        case BITWISE_SHIFT_LEFT: return "BITWISE_SHIFT_LEFT";
        case BITWISE_SHIFT_RIGHT: return "BITWISE_SHIFT_RIGHT";
        default: return "UNKNOWN";
    }
}