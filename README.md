# Quick Compiler

Quick Compiler is a lightweight and efficient compiler implementation designed for educational and experimental purposes. It showcases the fundamentals of lexical analysis, parsing (which contains the parser, domain analysis, type analysis) and code generation.

## Features

- Lexical Analysis: Tokenizes source code into symbols.

- Parsing: Analyzes symbols input for syntactic correctness.

- Code Generation: Produces output or executable binary.

- Modular Design: Clearly separated modules for flexibility.


## Prerequisites

- Compiler: ```GCC``` or any C compiler.
- Build Tool: ```make``` utility.
- Operating System: Linux (tested on Arch Linux).

## Installation

Clone the repository:
``` 
git clone https://github.com/vladvcojocaru/quick-compiler
cd quick-compiler
```

## Build

To build the project, run:
```
make
```

This will generate the binary in the ```bin/``` directory after which you can test the program on the test files from ```tests/```

## Lexical rules

```
ID: [a-zA-Z_] [a-zA-Z0-9_]*

### Key workds ###
VAR: 'var'
FUNCTION: 'function'
IF: 'if'
ELSE: 'else'
WHILE: 'while'
END: 'end'
RETURN: 'return'
TYPE_INT: 'int'
TYPE_REAL: 'real'
TYPE_STR: 'str'

### Constants ###
COMMA: ','
COLON: ':'
SEMICOLON: ';'
LPAR: '('
RPAR: ')'
FINISH: '\0' | EOF

### Operators ###
ADD: '+'
SUB: '-'
MUL: '*'
DIV: '/'
AND: '&&'
OR: '||'
NOT: '!'
ASSIGN: '='
EQUAL: '=='
NOTEQ: '!='
LESS: '<'
GREATER: '>'
GREATEREQ: '>='

### White spaces ###
SPACE: [ \n\r\t] 
COMMENT: '#' [^\n\r\0]*
```

## Semantic rules
```
program ::= ( defVar | defFunc | block )* FINISH
defVar ::= VAR ID COLON baseType SEMICOLON
baseType ::= TYPE_INT | TYPE_REAL | TYPE_STR
defFunc ::= FUNCTION ID LPAR funcParams? RPAR COLON baseType defVar* block END
block ::= instr+
funcParams ::= funcParam ( COMMA funcParam )*
funcParam ::= ID COLON baseType
instr ::= expr? SEMICOLON
| IF LPAR expr RPAR block ( ELSE block )? END
| RETURN expr SEMICOLON
| WHILE LPAR expr RPAR block END
expr ::= exprLogic
exprLogic ::= exprAssign ( ( AND | OR ) exprAssign )*
exprAssign ::= ( ID ASSIGN )? exprComp
exprComp ::= exprAdd ( ( LESS | EQUAL ) exprAdd )?
exprAdd ::= exprMul ( ( ADD | SUB ) exprMul )*
exprMul ::= exprPrefix ( ( MUL | DIV ) exprPrefix )*
exprPrefix ::= ( SUB | NOT )? factor
factor ::= INT
| REAL
| STR
| LPAR expr RPAR
| ID ( LPAR ( expr ( COMMA expr )* )? RPAR )?
```
