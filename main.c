#include<stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "./lexer/lexer.h"
#include "./parser/parser.h"
#include "./utils/utils.h"


int main()
{   /*const char *inbuf=loadFile("tests/testlex.c");

    Token *tokens=tokenize(inbuf);

    FILE *out=fopen("tests/lista-de-atomi.txt","w");
    if(!out)
    {
        perror("Failed to open output file");
        exit(EXIT_FAILURE);
    }
    showTokens(tokens,out);
    fclose(out);
    printf("Tokens written to tests/lista-de-atomi.txt\n");*/ //Assigment 2 with lexer


    /*const char *inbuf = loadFile("tests/testparser.c");
    Token *tokens=tokenize(inbuf);
    if (!inbuf) {
        perror("Failed to open test file");
        exit(EXIT_FAILURE);
    }

    // Apelează parserul pentru a valida sintactic codul sursă
    //showTokens(tokens, stdout);
    parse(tokens); //Assigment 3 with parser*/

    const char *inbuf = loadFile("tests/testad.c");
    if (!inbuf) {
        perror("Failed to open testad.c file");
        exit(EXIT_FAILURE);
    }
    
    // Tokenize the input
    Token *tokens = tokenize(inbuf);
    
    // showTokens(tokens, stdout);
    parse(tokens);
    

    return 0;
}