#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){
    if(argc != 2){
        fprintf(stderr, "Incorrect number of arguments\n");
        return 1;
    }

    char *inp = argv[1];

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    printf("    mov rax, %ld\n", strtol(inp, &inp, 10));

    while(*inp != '\0'){
        if(*inp == '+'){
            ++inp;
            printf("    add rax, %ld\n", strtol(inp, &inp, 10));
        }else if(*inp =='-'){
            ++inp;
            printf("    sub rax, %ld\n", strtol(inp, &inp, 10));
        }else{
            fprintf(stderr, "unexpected input character: %c\n",*inp);
            exit(1);
        }
    }
printf("    ret\n");
    return 0;
}