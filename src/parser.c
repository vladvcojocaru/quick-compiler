#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "../include/lexer.h"

int iTk;	// the iterator in tokens
Token *consumed;	// the last consumed token

// same as err, but also prints the line of the current token
_Noreturn void tkerr(const char *fmt,...){
	fprintf(stderr,"error in line %d: ",tokens[iTk].line);
	va_list va;
	va_start(va,fmt);
	vfprintf(stderr,fmt,va);
	va_end(va);
	fprintf(stderr,"\n");
	exit(EXIT_FAILURE);
	}

bool consume(int code){

	printf("consume(%s)\n",getTokenName(code));
	if(tokens[iTk].code==code){
		consumed=&tokens[iTk++];
		return true;
		}
	return false;
	}
//

// exprComp ::= exprAdd ( ( LESS | EQUAL ) exprAdd )?
bool exprComp(){
	int start = iTk;
	printf("#exprComp: %s\n", getTokenName(tokens[iTk].code));

	if(exprAdd()){
		if(consume(LESS) || consume(EQUAL)){
			if(exprAdd()){
			} else return false;

		}

		return true;
	}

	iTk = start;
	return false;
}
// exprAssign ::= ( ID ASSIGN )? exprComp
bool exprAssign(){
	int start = iTk;
	printf("#exprAssign: %s\n", getTokenName(tokens[iTk].code));

	if(consume(ID)){
		if(consume(ASSIGN)){

		} else {
			return false;
		}
	}

	if(exprComp()){
		return true;
	}
	
	iTk = start;
	return false;
}
// exprLogic ::= exprAssign ( ( AND | OR ) exprAssign )*
bool exprLogic(){
	int start = iTk;
	printf("#exprLogic: %s\n", getTokenName(tokens[iTk].code));

	if(exprAssign()){
		for(;;){
			if(consume(AND) || consume(OR)){
				if(exprAssign()){
					continue;
				}

			} else break;	
		}
		return true;
	}

	iTk = start;
	return false;
}

//expr ::= exprLogic
bool expr(){
	int start = iTk;
	printf("#expr: %s\n", getTokenName(tokens[iTk].code));

	if(exprLogic()){
		return true;
	}

	iTk = start;
	return false;
}

// TODO: Refactor code cause it's ugly asf
// instr ::= expr? SEMICOLON | IF LPAR expr RPAR block ( ELSE block )? END | RETURN expr SEMICOLON | WHILE LPAR expr RPAR block END
bool instr(){
	int start = iTk;
	printf("#funcParam: %s\n", getTokenName(tokens[iTk].code));

	if(expr()){}

	if(consume(SEMICOLON)){
		// *Copy this in else if
		if(consume(LPAR)){
			if(expr()){
				if(consume(RPAR)){
					if(block()){
						if(consume(ELSE)){
							if(block()){}
							else return false;	
						}
						if(consume(END)){
							if(expr()){
								if(consume(SEMICOLON)){
									if(consume(LPAR)){
										if(expr()){
											if(consume(RPAR)){
												if(block()){
													if(consume(END))
														return true;
												}
											}
										}
									}
								}
								else if(consume(WHILE)){
									
									if(consume(LPAR)){
										if(expr()){
											if(consume(RPAR)){
												if(block()){
													if(consume(END))
														return true;
												}
											}
										}
									}
								}
							}
						}
						else if(consume(RETURN)){
							
							if(expr()){
								if(consume(SEMICOLON)){
									if(consume(LPAR)){
										if(expr()){
											if(consume(RPAR)){
												if(block()){
													if(consume(END))
														return true;
												}
											}
										}
									}
								}
								else if(consume(WHILE)){
									
									if(consume(LPAR)){
										if(expr()){
											if(consume(RPAR)){
												if(block()){
													if(consume(END))
														return true;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	else if(consume(IF)){

		if(consume(LPAR)){
			if(expr()){
				if(consume(RPAR)){
					if(block()){
						if(consume(ELSE)){
							if(block()){}
							else return false;	
						}
						if(consume(END)){
							if(expr()){
								if(consume(SEMICOLON)){
									if(consume(LPAR)){
										if(expr()){
											if(consume(RPAR)){
												if(block()){
													if(consume(END))
														return true;
												}
											}
										}
									}
								}
								else if(consume(WHILE)){
									
									if(consume(LPAR)){
										if(expr()){
											if(consume(RPAR)){
												if(block()){
													if(consume(END))
														return true;
												}
											}
										}
									}
								}
							}
						}
						else if(consume(RETURN)){
							
							if(expr()){
								if(consume(SEMICOLON)){
									if(consume(LPAR)){
										if(expr()){
											if(consume(RPAR)){
												if(block()){
													if(consume(END))
														return true;
												}
											}
										}
									}
								}
								else if(consume(WHILE)){
									
									if(consume(LPAR)){
										if(expr()){
											if(consume(RPAR)){
												if(block()){
													if(consume(END))
														return true;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	iTk = start;
	return false;
}

// funcParam ::= ID COLON baseType
bool funcParam(){
	int start = iTk;
	printf("#funcParam: %s\n", getTokenName(tokens[iTk].code));

	if(consume(ID)){
		if(consume(COLON)){
			if(baseType()){
				return true;
			}
		}
	}

	iTk = start;
	return false;
}

// funcParams ::= funcParam ( COMMA funcParam )*
bool funcParams(){
	int start = iTk;
	printf("#funcParams: %s\n", getTokenName(tokens[iTk].code));
	
	if(funcParam()){
		for(;;){
			if(consume(COMMA)){
				if(funcParam()){} 
				else
				return false;
			}
		}
		return true;
	}

	iTk = start;
	return false;
}

// block ::= instr+
bool block(){
	int start = iTk;
	printf("#block: %s\n", getTokenName(tokens[iTk].code));

	if(instr()){
		for(;;){
			if(instr()){}
			else break;
		}
		return true;
	}

	iTk = start;
	return false;
}

// defFunc ::= FUNCTION ID LPAR funcParams? RPAR COLON baseType defVar* block END
bool defFunc(){
	int start = iTk;
	printf("#defFunc: %s\n", getTokenName(tokens[iTk].code));

	if(consume(FUNCTION)){
		if(consume(ID)){
			if(consume(LPAR)){
				if(funcParams()){}
				if(consume(RPAR)){
					if(consume(COLON)){
						if(baseType()){
							for(;;){
								if(defVar()){}
								else break;
							}
							if(block()){
								if(consume(END)){
									return true;
								}
							}
						}
					}
				}
			}
		}
	}
	iTk = start;
	return false;
}

// baseType ::= TYPE_INT | TYPE_REAL | TYPE_STR
bool baseType(){
	int start = iTk;
	printf("#baseType: %s\n", getTokenName(tokens[iTk].code));

	if(consume(TYPE_INT))
		return true;
	else if(consume(TYPE_REAL))
		return true;
	else if(consume(TYPE_STR))
		return true;
	
	iTk = start;
	return false;
}

// defVar ::= VAR ID COLON baseType SEMICOLON
bool defVar(){
	int start = iTk;
	printf("#defVar: %s\n", getTokenName(tokens[iTk].code));

	if (!consume(VAR)) {
        tkerr("Expected 'VAR' at the beginning of a variable declaration");
    }
    if (!consume(ID)) {
        tkerr("Expected identifier after 'VAR'");
    }
    if (!consume(COLON)) {
        tkerr("Expected ':' after identifier in variable declaration");
    }
    if (!baseType()) {
        tkerr("Expected base type after ':' in variable declaration");
    }
    if (!consume(SEMICOLON)) {
        tkerr("Expected ';' at the end of variable declaration");
    }

	iTk = start;
	return false;
}

//* Helper function
bool processTopLevelElement(){
	return defVar() || defFunc() || block();
}
// program ::= ( defVar | defFunc | block )* FINISH
bool program(){
	while (processTopLevelElement())

	if(consume(FINISH))
		return true;
	else 
		tkerr("syntax error");
		
}

void parse(){
	iTk=0;
	program();
	}