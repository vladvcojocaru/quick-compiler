#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"

#define MAX_INPUT_SIZE 4096

int main(int argc, char* argv[]){
    FILE *fd = fopen("test/test1.q", "r");

    if(fd == NULL){
        printf("File not found");
        return EXIT_FAILURE;
    }

    char input[MAX_INPUT_SIZE];

    size_t bytes_read = fread(input, sizeof(char), MAX_INPUT_SIZE - 1, fd);

    fclose(fd);
    tokenize(input);

    printf("Tokens: \n");
    showTokens();


    return EXIT_SUCCESS;
}