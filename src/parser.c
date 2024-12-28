#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/ad.h"
#include "../include/at.h"
#include "../include/lexer.h"
#include "../include/parser.h"

int iTk;         // the iterator in tokens
Token *consumed; // the last consumed token

// same as err, but also prints the line of the current token
_Noreturn void tkerr(const char *fmt, ...) {
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
    // AT
    addPredefinedFns();
    for (;;) {
        if (defVar()) {
        } else if (defFunc()) {
        } else if (block()) {
        } else
            break;
    }
    if (consume(FINISH)) {
        delDomain();
        return true;
    } else
        tkerr("syntax error");
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

            const char *name = consumed->text;
            Symbol *s = searchInCurrentDomain(name);
            if (s) {
                tkerr("symbol redefinition: %s in current domain", name);
            }
            s = addSymbol(name, KIND_VAR);
            s->local = (crtFn != NULL);

            if (consume(COLON)) {
                if (baseType()) {

                    s->type = ret.type;

                    if (consume(SEMICOLON)) {
                        return true;
                    } else
                        tkerr("Missing ';' after variable declaration");
                }
            } else
                tkerr("Missing ':' in variable declaration");
        } else
            tkerr("Missing identifier in variable declaration");
    }
    return false;
}

// baseType ::= TYPE_INT | TYPE_REAL | TYPE_STR
bool baseType() {

    if (consume(TYPE_INT)) {
        ret.type = TYPE_INT;
        return true;
    } else if (consume(TYPE_REAL)) {
        ret.type = TYPE_REAL;
        return true;
    } else if (consume(TYPE_STR)) {
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
            const char *name = consumed->text;
            Symbol *s = searchInCurrentDomain(name);
            if (s) {
                tkerr("symbol redefinition: %s", name);
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
                                } else
                                    tkerr("Missing 'end' after function "
                                          "definition");
                            }
                        } else {
                            tkerr("Invalid base type for function return");
                        }
                    } else
                        tkerr("Missing ':' in function declaration");
                } else
                    tkerr("Missing ')' after function parameters");
            } else
                tkerr("Missing '(' after function identifier");
        } else
            tkerr("Missing function identifier");
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
                tkerr("Invalid function parameter after ','");
        }
        return true;
    }
    return false;
}

// funcParam ::= ID COLON baseType
bool funcParam() {
    if (consume(ID)) {
        // DOMAIN CODE
        const char *name = consumed->text;
        Symbol *s = searchInCurrentDomain(name);
        if (s) {
            tkerr("symbol redefinition: %s", name);
        }
        s = addSymbol(name, KIND_ARG);
        Symbol *sFnParam = addFnArg(crtFn, name);

        if (consume(COLON)) {
            if (baseType()) {
                // DOMAIN CODE
                s->type = ret.type;
                sFnParam->type = ret.type;

                return true;
            } else
                tkerr("Invalid base type in function parameter");
        } else
            tkerr("Missing ':' in function parameter");
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
        } else {
            tkerr("Expected ';' after expression");
        }

        iTk = start;
    }
    if (consume(SEMICOLON)) {
        return true;
    }
    if (consume(IF)) {
        if (consume(LPAR)) {
            if (expr()) {
                // AT
                if (ret.type == TYPE_STR) {
                    tkerr("the if condition must have type int or real");
                }
                if (consume(RPAR)) {
                    if (block()) {
                        if (consume(ELSE)) {
                            if (!block())
                                tkerr("Expected block after 'else'");
                        }
                        if (consume(END)) {
                            return true;
                        } else
                            tkerr("Missing 'end' after 'if' statement");
                    }
                } else {
                    tkerr("Missing ')' after if condition");
                }
            }
        }
    } else if (consume(RETURN)) {
        if (expr()) {
            // AT
            if (!crtFn) {
                tkerr("return can be used only in a function");
            }
            if (ret.type != crtFn->type) {
                tkerr("the return type must be the same as the function "
                      "return type");
            }

            if (consume(SEMICOLON)) {
                return true;
            } else
                tkerr("Missing ';' after return statement");
        } else
            tkerr("Missing expression in return statement");
    } else if (consume(WHILE)) {
        if (consume(LPAR)) {
            if (expr()) {
                // AT
                if (ret.type == TYPE_STR) {
                    tkerr("the while condition must have type int or real");
                }

                if (consume(RPAR)) {
                    if (block()) {
                        if (consume(END)) {
                            return true;
                        } else
                            tkerr("Missing 'end' after 'while' loop");
                    }
                }
            }
        }
    }
    return false;
}

// !HERE IT WORKS

// expr ::= exprLogic
bool expr() { return exprLogic(); }

// exprLogic ::= exprAssign ( ( AND | OR ) exprAssign )*
bool exprLogic() {
    if (exprAssign()) {
        while (consume(AND) || consume(OR)) {
            // AT
            Ret leftType = ret;
            if (leftType.type == TYPE_STR) {
                tkerr("the left operand of && or || cannot be of type str");
            }

            if (!exprAssign())
                tkerr("Invalid expression after 'and/or'");

            // AT
            if (ret.type == TYPE_STR) {
                tkerr("the right operand of && or || cannot be of type str");
            }
            setRet(TYPE_INT, false);
        }
        return true;
    }
    return false;
}

// exprAssign ::= ( ID ASSIGN )? exprComp
bool exprAssign() {
    int start = iTk;
    if (consume(ID)) {
        // AT
        const char *name = consumed->text;

        if (consume(ASSIGN)) {
            if (exprComp()) {
                // AT
                Symbol *s = searchSymbol(name);
                if (!s)
                    tkerr("undefined symbol: %s", name);
                if (s->kind == KIND_FN)
                    tkerr("a function (%s) cannot be used as a destination for "
                          "assignment ",
                          name);
                if (s->type != ret.type)
                    tkerr("the source and destination for assignment must have "
                          "the same type");
                ret.lval = false;

                return true;
            } else
                tkerr("Invalid expression after '='");
        }
        iTk = start;
    }
    return exprComp();
}
// !WORKS

// exprComp ::= exprAdd ( ( LESS | EQUAL ) exprAdd )?
bool exprComp() {
    if (exprAdd()) {
        if (consume(LESS) || consume(EQUAL)) {
            // AT
            Ret leftType = ret;

            if (!exprAdd())
                tkerr("Invalid expression after '<' or '='");
            // AT
            if (leftType.type != ret.type)
                tkerr("different types for the operands of < or ==");
            setRet(TYPE_INT, false); // the result of comparation is int 0 or 1
        }
        return true;
    }
    return false;
}

// exprAdd ::= exprMul ( ( ADD | SUB ) exprMul )*
bool exprAdd() {
    if (exprMul()) {
        while (consume(ADD) || consume(SUB)) {
            // AT
            Ret leftType = ret;
            if (leftType.type == TYPE_STR)
                tkerr("the operands of + or - cannot be of type str");

            if (!exprMul())
                tkerr("Invalid expression after '+' or '-'");
            // AT
            if (leftType.type != ret.type)
                tkerr("different types for the operands of + or -");
            ret.lval = false;
        }
        return true;
    }
    return false;
}

// exprMul ::= exprPrefix ( ( MUL | DIV ) exprPrefix )*
bool exprMul() {
    if (exprPrefix()) {
        while (consume(MUL) || consume(DIV)) {
            // AT
            Ret leftType = ret;
            if (leftType.type == TYPE_STR)
                tkerr("the operands of * or / cannot be of type str");

            if (!exprPrefix())
                tkerr("Invalid expression after '*' or '/'");
            // AT
            if (leftType.type != ret.type)
                tkerr("different types for the operands of * or /");
            ret.lval = false;
        }
        return true;
    }
    return false;
}

// exprPrefix ::= ( SUB | NOT )? factor
// bool exprPrefix() {
//     if (consume(SUB) || consume(NOT)) {
//         return factor();
//     }
//     return factor();
// }

bool exprPrefix() {
    if (consume(SUB)) {
        if (factor()) {
            // AT
            if (ret.type == TYPE_STR)
                tkerr("the expression of unary - must be of type int or real");
            ret.lval = false;

            return true;
        } else
            return false;
    }
    if (consume(NOT)) {
        if (factor()) {
            // AT
            if (ret.type == TYPE_STR)
                tkerr("the expression of ! must be of type int or real");
            setRet(TYPE_INT, false);

            return true;
        } else
            return false;
    }

    return factor();
}
// factor ::= INT | REAL | STR | LPAR expr RPAR | ID ( LPAR ( expr ( COMMA expr
// )* )? RPAR )?
// bool factor() {
//     if (consume(INT) || consume(REAL) || consume(STRING)) {
//         return true;
//     }
//     if (consume(LPAR)) {
//         if (expr()) {
//             if (consume(RPAR)) {
//                 return true;
//             } else
//                 tkerr("Missing ')' after expression");
//         }
//     }
//     if (consume(ID)) {
//         if (consume(LPAR)) {
//             if (expr()) {
//                 while (consume(COMMA)) {
//                     if (!expr())
//                         tkerr("Invalid expression after ',' in function call");
//                 }
//             }
//             if (consume(RPAR)) {
//                 return true;
//             } else
//                 tkerr("Missing ')' after function arguments");
//         }
//         return true;
//     }
//     return false;
// }
bool factor() {
    if (consume(INT)) {
        setRet(TYPE_INT, false);
        return true;
    }
    if (consume(REAL)) {
        setRet(TYPE_REAL, false);
        return true;
    }
    if (consume(STRING)) {
        setRet(TYPE_STR, false);
        return true;
    }
    if (consume(LPAR)) {
        if (expr()) {
            if (consume(RPAR)) {
                return true;
            } else
                tkerr("Missing ')' after expression");
        } else
            return false; // DELETE LATER
    }

    if (consume(ID)) {
        Symbol *s = searchSymbol(consumed->text);
        if (!s)
            tkerr("undefined symbol: %s", consumed->text);

        if (consume(LPAR)) {
            if (s->kind != KIND_FN)
                tkerr("%s cannot be called, because it is not a function",
                      s->name);
            Symbol *argDef = s->args;

            if (expr()) {
                if (!argDef)
                    tkerr("the function %s is called with too many arguments",
                          s->name);
                if (argDef->type != ret.type)
                    tkerr("the argument type at function %s call is different "
                          "from the one given at its definition",
                          s->name);
                argDef = argDef->next;

                while (consume(COMMA)) {
                    if (!expr())
                        tkerr("Invalid expression after ',' in function call");

                    if (!argDef)
                        tkerr(
                            "the function %s is called with too many arguments",
                            s->name);
                    if (argDef->type != ret.type)
                        tkerr("the argument type at function %s call is "
                              "different from the one given at its definition",
                              s->name);
                    argDef = argDef->next;
                }
            }

            if (consume(RPAR)) {
                if (argDef)
                    tkerr("the function %s is called with too few arguments",
                          s->name);
                setRet(s->type, false);
                return true;
            } else {
                // VERIFICA
                tkerr("Missing ')' after function arguments");
                return false;
            }
        } else {
            if (s->kind == KIND_FN)
                tkerr("the function %s can only be called", s->name);
            setRet(s->type, true);
            return true;
        }
    }
    return false;
}
