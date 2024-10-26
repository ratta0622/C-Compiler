#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>

//kinds of tokens
typedef enum{
    TK_OPE, //token of operator(+, -)
    TK_NUM, //token of number
    TK_EOF, //token of End Of File(\0)
} TokenKind;

//type of token
//(list of token <=> input equation)
typedef struct Token Token;

struct Token{
    TokenKind kind; //type of a token
    Token *next;    //next token
    int val;        //number of token(if token = TK_NUM)
    char *str;      //Token string
};

//the current token being focused on
Token *token;

//function to report errors
//This takes the same arguments as printf
void error(char *format, ...){
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap); //same as printf
    fprintf(stderr, "\n");
    exit(1);
}

//If current token is expected operator(TK_OPE), advance to the next token and return true.
//Otherwise, return false.
//[char op] is shuch as + or -
bool consume(char op){
    if(token->kind == TK_OPE && token->str[0] == op){
        token = token->next;
        return true;
    }else{
        return false;
    }
}

//If current token is expected operator(TK_OPE), advance to the next token.
//Otherwise, report errors.
void expect_operator(char op){
    if(token->kind == TK_OPE && token->str[0] == op){
        token = token->next;
    }else{
        error("not '%c'", op);
    }
}

//If current token is expected number(TK_NUM),advance to the next token and return the number.
//Otherwise, report errors.
int expect_number(){
    if(token->kind == TK_NUM){
        int num = token->val;
        token = token->next;
        return num;
    }else{
        error("not a number");
    }
}

//Return a boolean value that whether token is EOF(\0)
bool at_eof(){
    return token->kind == TK_EOF;
}

//create new token, and link it to cur
//(use like,     cur = new_token(TK_OPE, cur, p)          )
Token* new_token(TokenKind kind, Token* cur, char* str){
    Token* tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

//Tokenize the input string p, and return it.
Token* tokenize(char *p){
    Token head;
    head.next = NULL;
    Token* cur = &head;
    //head is blank token

    while(*p){
        //Skip space
        if(isspace(*p)){
            ++p;
            continue;
        }

        //If p is an operator
        if(*p == '+' || *p == '-'){
            cur = new_token(TK_OPE, cur, p++);
            continue;
        }

        //If p is a number
        if(isdigit(*p)){
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error("cannot tokenize");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}


void debug_token(){
    while(!at_eof()){
        printf("%s\n",token->str);
        token = token->next;
    }
    error("debug function\n");
}


int main(int argc, char *argv[]){
    if(argc != 2){
        fprintf(stderr, "Incorrect number of arguments\n");
        return 1;
    }

    //Tokenize input
    token = tokenize(argv[1]);

    //Output the first half of the assembly
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    //Check whether the expression starts with a number (expect_number())
    //and output the first mov instruction
    printf("    mov rax, %d\n", expect_number());

    while(!at_eof()){
        if(consume('+')){
            printf("    add rax, %d\n", expect_number());
        }else{
            expect_operator('-');
            printf("    sub rax, %d\n", expect_number());
        }
    }

    printf("    ret\n");

    return 0;
}