#include "ncc.h"

char* userInput;
Token* token;
Lvar* headLocalList;
Lvar* localList;

// Open the file specified by the path, and return the contents of the file as char*
char *readFile(char *filepath){
    // Open the file
    FILE *fp = fopen(filepath, "r");
    if(!fp){
        error("cannnot open %s: %s", filepath, strerror(errno));
    }

    // Get the file size and assign it to "size"
    if(fseek(fp, 0, SEEK_END) == -1){
        error("%s: fseek: %s", filepath, strerror(errno));
    }
    size_t size = ftell(fp);

    // Read the contents of the file, to return it(fileInput)
    char *fileInput = calloc(1, size + 2);
    if(fseek(fp, 0, SEEK_SET) == -1){
        error("%s: fseek: %s", filepath, strerror(errno));
    }
    fread(fileInput, size, 1, fp);

    // Ensure that the file ends with "\n\0"
    if(size == 0 || fileInput[size - 1] != '\n'){
        fileInput[size++] = '\n';
    }
    fileInput[size] = '\0';

    fclose(fp);

    return fileInput;
}

int main(int argc, char* argv[]){
    if(argc != 2){
        fprintf(stderr, "Incorrect number of arguments\n");
        return 1;
    }

    // Save argv[1] to userInput(global val) for error messages.
    userInput = readFile(argv[1]);

    // Tokenize input
    token = tokenize(userInput);

    headLocalList = calloc(1, sizeof(Lvar));
    headLocalList->next = NULL;
    headLocalList->offset = -8;
    localList = headLocalList;

    // Perse the torkenized expression
    // The results are stored in Node* code[100]
    program();

    // Assign offsets to local variables
    int offset = 0;
    for(Lvar* local = headLocalList->next; local != NULL; local = local->next){
        offset += 8;
    }

    // Output the first half of the assembly
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // Prologue
    // Allocate space for 26 variables(a-z)
    printf("    push rbp\n"); // Save the return address(value of RBP) on to the stack, before the function call
    printf("    mov rbp, rsp\n");
    // printf("    sub rsp, 208\n");
    printf("    sub rsp, %d\n", offset);

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