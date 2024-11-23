#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/ad.h"
#include "../include/utils.h"

// // Global variables defined in the header file
// Ret ret;
// Domain *symTable = NULL; // Starts with an empty symbol table (global scope)
// Symbol *crtFn = NULL;    // No function is being parsed initially

// /**
//  * Allocates and initializes a new domain, setting it as the current domain.
//  * Returns: Pointer to the newly created domain.
//  */
// Domain *addDomain() {
//     puts("Creating a new domain...");
//     Domain *d = (Domain *)safeAlloc(sizeof(Domain));
//     d->parent = symTable; // Link the new domain to the current domain as its parent
//     d->symbols = NULL;    // Initialize the symbol list as empty
//     symTable = d;         // Make the new domain the current domain
//     return d;
// }


// void delSymbols(Symbol *list);

// /**
//  * Deletes a symbol and frees its associated memory.
//  * If the symbol is a function, its arguments are also deleted.
//  */
// void delSymbol(Symbol *s) {
//     printf("Deleting symbol: %s\n", s->name);
//     if (s->kind == KIND_FN) {
//         delSymbols(s->args); // Delete function arguments if the symbol is a function
//     }
//     free(s); // Free the symbol itself
// }

// /**
//  * Deletes a linked list of symbols and frees their memory.
//  */
// void delSymbols(Symbol *list) {
//     for (Symbol *s1 = list, *s2; s1; s1 = s2) {
//         s2 = s1->next; // Save the next symbol before deleting the current one
//         delSymbol(s1);
//     }
// }

// /**
//  * Deletes the current domain and restores its parent as the current domain.
//  */
// void delDomain() {
//     puts("Deleting the current domain...");
//     if (!symTable) {
//         printf("Error: Attempted to delete a domain when none exists.\n");
//         exit(EXIT_FAILURE); // Fail fast
//     }
//     Domain *parent = symTable->parent; // Save the parent domain
//     delSymbols(symTable->symbols);     // Delete all symbols in the current domain
//     free(symTable);                    // Free the current domain structure
//     symTable = parent;                 // Restore the parent as the current domain
//     puts("Returned to the parent domain.");
// }

// /**
//  * Searches for a symbol by name in a linked list of symbols.
//  * Returns: Pointer to the symbol if found, otherwise NULL.
//  */
// Symbol *searchInList(Symbol *list, const char *name) {
//     for (Symbol *s = list; s; s = s->next) {
//         if (!strcmp(s->name, name)) { // Compare the names
//             return s;
//         }
//     }
//     return NULL; // Symbol not found
// }

// /**
//  * Searches for a symbol by name in the current domain only.
//  * Returns: Pointer to the symbol if found, otherwise NULL.
//  */
// Symbol *searchInCurrentDomain(const char *name) {
//     if (!symTable) {
//         printf("Error: No current domain while searching for symbol '%s'.\n", name);
//         exit(EXIT_FAILURE);
//     }
//     return searchInList(symTable->symbols, name);
// }

// /**
//  * Searches for a symbol by name in the current and parent domains.
//  * Returns: Pointer to the symbol if found, otherwise NULL.
//  */
// Symbol *searchSymbol(const char *name) {
//     for (Domain *d = symTable; d; d = d->parent) { // Traverse all domains
//         Symbol *s = searchInList(d->symbols, name);
//         if (s) {
//             return s;
//         }
//     }
//     return NULL; // Symbol not found in any domain
// }

// /**
//  * Creates a new symbol with the specified name and kind.
//  * Returns: Pointer to the newly created symbol.
//  */
// Symbol *createSymbol(const char *name, int kind) {
//     Symbol *s = (Symbol *)safeAlloc(sizeof(Symbol));
//     s->name = name;
//     s->kind = kind;
//     s->next = NULL; // Initialize the next pointer as NULL
//     return s;
// }

// /**
//  * Adds a new symbol to the current domain.
//  * Returns: Pointer to the added symbol.
//  */
// Symbol *addSymbol(const char *name, int kind) {
//     printf("Adding symbol: %s\n", name);
//      if (!symTable) {
//         printf("Error: No current domain to add symbol '%s'.\n", name);
//         exit(EXIT_FAILURE); // Fail fast
//     }
//     Symbol *s = createSymbol(name, kind); // Create the symbol
//     s->next = symTable->symbols;         // Link it to the current list
//     symTable->symbols = s;               // Update the list head
//     return s;
// }

// /**
//  * Adds an argument to a function's symbol.
//  * Returns: Pointer to the newly added argument symbol.
//  */
// Symbol *addFnArg(Symbol *fn, const char *argName) {
//     printf("Adding argument: %s\n", argName);
//     Symbol *s = createSymbol(argName, KIND_ARG); // Create the argument symbol
//     s->next = NULL;
//     if (fn->args) { // If the function already has arguments
//         Symbol *p;
//         for (p = fn->args; p->next; p = p->next) {
//             // Traverse to the end of the argument list
//         }
//         p->next = s; // Append the new argument to the list
//     } else {
//         fn->args = s; // Set the new argument as the first argument
//     }
//     return s;
// }


Ret ret;
Domain *symTable;
Symbol *crtFn;

Domain *addDomain(){
	puts("creates a new domain");
	Domain *d=(Domain*)safeAlloc(sizeof(Domain));
	d->parent=symTable;
	d->symbols=NULL;
	symTable=d;
	return d;
	}

void delSymbols(Symbol *list);

void delSymbol(Symbol *s){
	printf("\tdeletes the symbol %s\n",s->name);
	if(s->kind==KIND_FN){
		delSymbols(s->args);
		}
	free(s);
	}

void delSymbols(Symbol *list){
	for(Symbol *s1=list,*s2;s1;s1=s2){
		s2=s1->next;
		delSymbol(s1);
		}
	}

void delDomain(){
	puts("deletes the current domain");
	Domain *parent=symTable->parent;
	delSymbols(symTable->symbols);
	free(symTable);
	symTable=parent;
	puts("returns to the parent domain");
	}

Symbol *searchInList(Symbol *list,const char *name){
	for(Symbol *s=list;s;s=s->next){
		if(!strcmp(s->name,name))return s;
		}
	return NULL;
	}

Symbol *searchInCurrentDomain(const char *name){
	return searchInList(symTable->symbols,name);
	}

Symbol *searchSymbol(const char *name){
	for(Domain *d=symTable;d;d=d->parent){
		Symbol *s=searchInList(d->symbols,name);
		if(s)return s;
		}
	return NULL;
	}

Symbol *createSymbol(const char *name,int kind){
	Symbol *s=(Symbol*)safeAlloc(sizeof(Symbol));
	s->name=name;
	s->kind=kind;
	return s;
	}

Symbol *addSymbol(const char *name,int kind){
	printf("\tadds symbol %s\n",name);
	Symbol *s=createSymbol(name,kind);
	s->next=symTable->symbols;
	symTable->symbols=s;
	return s;
	}


	Symbol *addFnArg(Symbol *fn,const char *argName){
	printf("\tadds symbol %s as argument\n",argName);
	Symbol *s=createSymbol(argName,KIND_ARG);
	s->next=NULL;
	if(fn->args){
		Symbol *p;
		for(p=fn->args;p->next;p=p->next){}
		p->next=s;
		}else{
		fn->args=s;
		}
	return s;
	}
