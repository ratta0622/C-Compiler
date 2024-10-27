#include "ncc.h"


//Report the error location
void error_at(char* location, char* format, ...){
    va_list ap;
    va_start(ap, format);

    int pos = location - user_input; //the byte position of the rror in the input

    fprintf(stderr, "\nInput error\n\n");
    //1st line ... user_input
    fprintf(stderr, "%s\n", user_input);

    //2nd line
    fprintf(stderr, "%*s", pos, " "); //output pos number of spaces
    fprintf(stderr, "^");
    vfprintf(stderr, format, ap);
    fprintf(stderr, "\n");

    exit(1);
}

//function to report errors
//This takes the same arguments as printf
void error(char *format, ...){
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap); //same as printf     (printf("hoge%d%s", 1, huga))   ------->    format = "hoge%d%s"     ap = 1, huga
    fprintf(stderr, "\n");
    exit(1);
}

//If current token is expected operator(TK_OPE), advance to the next token and return true.
//Otherwise, return false.
bool consume_operator(char* op){
    if(token->kind == TK_OPE && strncmp(token->str, op, token->len)==0){
        token = token->next;
        return true;
    }else{
        return false;
    }
}

//If current token is expected operator(TK_OPE), advance to the next token.
//Otherwise, report errors.
void expect_operator(char* op){
    if(token->kind == TK_OPE && strncmp(token->str, op, token->len)==0){
        token = token->next;
    }else{
        error_at(token->str, "not %s", op);
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
        error_at(token->str, "not a number");
    }
}

char consume_ident(){
    if(token->kind == TK_IDENT){
        token = token->next;
        return token->str[0];
    }else{
        return '!';
    }
}

//Return a boolean value that whether token is EOF(\0)
bool at_eof(){
    return token->kind == TK_EOF;
}

//create a new token, and link it to cur
//(use like,     cur = new_token(TK_OPE, cur, p)          )
Token* new_token(TokenKind kind, Token* cur, char* str, int len){
    //create a new token as tok
    Token* tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;

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
        if(strncmp(p, "==", 2)==0 || strncmp(p, "!=", 2)==0 || strncmp(p, "<=", 2)==0 || strncmp(p, ">=", 2)==0){
            cur = new_token(TK_OPE, cur, p, 2);
            ++p; ++p;
            continue;
        }else if(*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '<' || *p == '>' || *p == ';'){
            cur = new_token(TK_OPE, cur, p++, 1);
            continue;
        }

        //If p is an identifier (a-z)
        if('a' <= *p && *p <= 'z'){
            cur = new_token(TK_IDENT, cur, p++, 1);
            continue;
        }

        //If p is a number
        if(isdigit(*p)){
            char * endnum;
            strtol(p, &endnum, 10);
            cur = new_token(TK_NUM, cur, p, endnum-p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error("cannot tokenize");
    }

    new_token(TK_EOF, cur, p, 1);
    return head.next;
}