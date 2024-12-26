#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/ad.h"
#include "../include/lexer.h"
#include "../include/parser.h"

int iTk;         // the iterator in tokens
Token* consumed; // the last consumed token

// same as err, but also prints the line of the current token
_Noreturn void tokenError(const char* fmt, ...) {
    fprintf(stderr, "error in line %d: ", tokens[iTk].line);
    va_list va;
    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}

bool consume(int code) {

    if (tokens[iTk].code == code) {
        consumed = &tokens[iTk++];

        return true;
    }

    return false;
}

// program ::= ( defVar | defFunc | block )* FINISH
bool program() {
    addDomain();
    for (;;) {
        if (defVar()) {
        }
        else if (defFunc()) {
        }
        else if (block()) {
        }
        else
            break;
    }
    if (consume(FINISH)) {
        delDomain();
        return true;
    }
    else
        tokenError("syntax error");
    return false;
}

void parse() {
    iTk = 0;
    program();
}

// defVar ::= VAR ID COLON baseType SEMICOLON
bool defVar() {
    if (consume(VAR)) {
        if (consume(ID)) {

            const char* name = consumed->text;
            Symbol* s = searchInCurrentDomain(name);
            if (s) {
                tokenError("symbol redefinition: %s in current domain", name);
            }
            s = addSymbol(name, KIND_VAR);
            s->local = (crtFn != NULL);

            if (consume(COLON)) {
                if (baseType()) {

                    s->type = ret.type;

                    if (consume(SEMICOLON)) {
                        return true;
                    }
                    else
                        tokenError("Missing ';' after variable declaration");
                }
            }
            else
                tokenError("Missing ':' in variable declaration");
        }
        else
            tokenError("Missing identifier in variable declaration");
    }
    return false;
}

// baseType ::= TYPE_INT | TYPE_REAL | TYPE_STR
bool baseType() {

    if (consume(TYPE_INT)) {
        ret.type = TYPE_INT;
        return true;
    }
    else if (consume(TYPE_REAL)) {
        ret.type = TYPE_REAL;
        return true;
    }
    else if (consume(TYPE_STR)) {
        ret.type = TYPE_STR;
        return true;
    }
    return false;
}

// defFunc ::= FUNCTION ID LPAR funcParams? RPAR COLON baseType defVar* block
// END
bool defFunc() {
    if (consume(FUNCTION)) {
        if (consume(ID)) {
            // DOMAIN CODE
            const char* name = consumed->text;
            Symbol* s = searchInCurrentDomain(name);
            if (s) {
                tokenError("symbol redefinition: %s", name);
            }
            crtFn = addSymbol(name, KIND_FN);
            crtFn->args = NULL;
            addDomain();

            if (consume(LPAR)) {
                funcParams();
                if (consume(RPAR)) {
                    if (consume(COLON)) {
                        if (baseType()) {
                            // DOMAIN CODE
                            crtFn->type = ret.type;

                            while (defVar()) {
                            }
                            if (block()) {
                                if (consume(END)) {
                                    // DOMAIN CODE
                                    delDomain();
                                    crtFn = NULL;

                                    return true;
                                }
                                else
                                    tokenError("Missing 'end' after function "
                                        "definition");
                            }
                        }
                        else {
                            tokenError("Invalid base type for function return");
                        }
                    }
                    else
                        tokenError("Missing ':' in function declaration");
                }
                else
                    tokenError("Missing ')' after function parameters");
            }
            else
                tokenError("Missing '(' after function identifier");
        }
        else
            tokenError("Missing function identifier");
    }
    return false;
}

// block ::= instr+
bool block() {
    if (instr()) {
        while (instr()) {
        }
        return true;
    }
    return false;
}

// funcParams ::= funcParam ( COMMA funcParam )*
bool funcParams() {
    if (funcParam()) {
        while (consume(COMMA)) {
            if (!funcParam())
                tokenError("Invalid function parameter after ','");
        }
        return true;
    }
    return false;
}

// funcParam ::= ID COLON baseType
bool funcParam() {
    if (consume(ID)) {
        // DOMAIN CODE
        const char* name = consumed->text;
        Symbol* s = searchInCurrentDomain(name);
        if (s) {
            tokenError("symbol redefinition: %s", name);
        }
        s = addSymbol(name, KIND_ARG);
        Symbol* sFnParam = addFnArg(crtFn, name);

        if (consume(COLON)) {
            if (baseType()) {
                // DOMAIN CODE
                s->type = ret.type;
                sFnParam->type = ret.type;

                return true;
            }
            else
                tokenError("Invalid base type in function parameter");
        }
        else
            tokenError("Missing ':' in function parameter");
    }
    return false;
}

// instr ::= expr? SEMICOLON | IF LPAR expr RPAR block ( ELSE block )? END |
// RETURN expr SEMICOLON | WHILE LPAR expr RPAR block END
bool instr() {
    int start = iTk;
    if (expr()) {
        if (consume(SEMICOLON)) {
            return true;
        }
        else {
            tokenError("Expected ';' after expression");
        }

        iTk = start;
    }
    if (consume(SEMICOLON)) {
        return true;
    }
    if (consume(IF)) {
        if (consume(LPAR)) {
            if (expr()) {
                if (consume(RPAR)) {
                    if (block()) {
                        if (consume(ELSE)) {
                            if (!block())
                                tokenError("Expected block after 'else'");
                        }
                        if (consume(END)) {
                            return true;
                        }
                        else
                            tokenError("Missing 'end' after 'if' statement");
                    }
                }
                else {
                    tokenError("Missing ')' after if condition");
                }
            }
        }
    }
    else if (consume(RETURN)) {
        if (expr()) {
            if (consume(SEMICOLON)) {
                return true;
            }
            else
                tokenError("Missing ';' after return statement");
        }
        else
            tokenError("Missing expression in return statement");
    }
    else if (consume(WHILE)) {
        if (consume(LPAR)) {
            if (expr()) {
                if (consume(RPAR)) {
                    if (block()) {
                        if (consume(END)) {
                            return true;
                        }
                        else
                            tokenError("Missing 'end' after 'while' loop");
                    }
                }
            }
        }
    }
    return false;
}

// expr ::= exprLogic
bool expr() { return exprLogic(); }

// exprLogic ::= exprAssign ( ( AND | OR ) exprAssign )*
bool exprLogic() {
    if (exprAssign()) {
        while (consume(AND) || consume(OR)) {
            if (!exprAssign())
                tokenError("Invalid expression after 'and/or'");
        }
        return true;
    }
    return false;
}

// exprAssign ::= ( ID ASSIGN )? exprComp
bool exprAssign() {
    int start = iTk;
    if (consume(ID)) {
        if (consume(ASSIGN)) {
            if (exprComp()) {
                return true;
            }
            else
                tokenError("Invalid expression after '='");
        }
        iTk = start;
    }
    return exprComp();
}

// exprComp ::= exprAdd ( ( LESS | EQUAL ) exprAdd )?
bool exprComp() {
    if (exprAdd()) {
        if (consume(LESS) || consume(EQUAL)) {
            if (!exprAdd())
                tokenError("Invalid expression after '<' or '='");
        }
        return true;
    }
    return false;
}

// exprAdd ::= exprMul ( ( ADD | SUB ) exprMul )*
bool exprAdd() {
    if (exprMul()) {
        while (consume(ADD) || consume(SUB)) {
            if (!exprMul())
                tokenError("Invalid expression after '+' or '-'");
        }
        return true;
    }
    return false;
}

// exprMul ::= exprPrefix ( ( MUL | DIV ) exprPrefix )*
bool exprMul() {
    if (exprPrefix()) {
        while (consume(MUL) || consume(DIV)) {
            if (!exprPrefix())
                tokenError("Invalid expression after '*' or '/'");
        }
        return true;
    }
    return false;
}

// exprPrefix ::= ( SUB | NOT )? factor
bool exprPrefix() {
    if (consume(SUB) || consume(NOT)) {
        return factor();
    }
    return factor();
}

// factor ::= INT | REAL | STR | LPAR expr RPAR | ID ( LPAR ( expr ( COMMA expr
// )* )? RPAR )?
bool factor() {
    if (consume(INT) || consume(REAL) || consume(STRING)) {
        return true;
    }
    if (consume(LPAR)) {
        if (expr()) {
            if (consume(RPAR)) {
                return true;
            }
            else
                tokenError("Missing ')' after expression");
        }
    }
    if (consume(ID)) {
        if (consume(LPAR)) {
            if (expr()) {
                while (consume(COMMA)) {
                    if (!expr())
                        tokenError(
                            "Invalid expression after ',' in function call");
                }
            }
            if (consume(RPAR)) {
                return true;
            }
            else
                tokenError("Missing ')' after function arguments");
        }
        return true;
    }
    return false;
}
