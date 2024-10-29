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
    // The results are stored in Node* code[100]
    program();

    // Output the first half of the assembly
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // Prologue
    // Allocate space for 26 variables(a-z)
    printf("    push rbp\n"); // Save the return address(value of RBP) on to the stack, before the function call
    printf("    mov rbp, rsp\n");
    printf("    sub rsp, 208\n");

    // Output the assembly for code consisting of multiple statements
    for(int i=0; code[i] != NULL; ++i){
        genStatement(code[i]);

        // There must be one value remaining on the stack as the result of the expression evaluation,
        // so pop it to prevent stack overflow.
        printf("    pop rax\n");
    }

    // Epilogue
    // The result of the final expression remains in RAX, which becomes the return value
    printf("    mov rsp, rbp\n");
    printf("    pop rbp\n"); // Pop the return address that is before the function call
    printf("    ret\n");

    return 0;
}