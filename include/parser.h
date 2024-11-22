#pragma once
#include <stdbool.h>
// parse the extracted tokens
void parse();
bool consume(int code);
bool program();
void parse();
bool defVar();
bool baseType();
bool defFunc();
bool block();
bool funcParams();
bool funcParam();
bool instr();
bool expr();
bool exprLogic();
bool exprAssign();
bool exprComp();
bool exprAdd();
bool exprMul();
bool exprPrefix();
bool factor();