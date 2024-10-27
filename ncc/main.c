#include "ncc.h"

char* user_input;
Token* token;

int main(int argc, char* argv[]){
    if(argc != 2){
        fprintf(stderr, "Incorrect number of arguments\n");
        return 1;
    }

    // Save argv[1] to user_input(global val) for error messages.
    user_input = argv[1];

    // Tokenize input
    token = tokenize(user_input);

    // Perse the torkenized expression
    program();

    // Output the first half of the assembly
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // Output the assembly for calculations
    for(int i=0; code[i] != NULL; ++i){
        genAssemblyFromNodesOfEBNF(code[i]);
    }

    // Pop the calculation result from the stack, and return
    printf("    pop rax\n");
    printf("    ret\n");

    return 0;
}