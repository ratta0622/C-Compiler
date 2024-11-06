#include "ncc.h"


// Report the error location
void errorAt(char* location, char* format, ...){
    va_list ap;
    va_start(ap, format);

    int pos = location - user_input; // the byte position of the rror in the input

    fprintf(stderr, "\nInput error\n\n");
    // 1st line ... user_input
    fprintf(stderr, "%s\n", user_input);

    // 2nd line
    fprintf(stderr, "%*s", pos, " "); // output pos number of spaces
    fprintf(stderr, "^");
    vfprintf(stderr, format, ap);
    fprintf(stderr, "\n");

    exit(1);
}

// function to report errors
// This takes the same arguments as printf
void error(char *format, ...){
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap); // same as printf     (printf("hoge%d%s", 1, huga))   ------->    format = "hoge%d%s"     ap = 1, huga
    fprintf(stderr, "\n");
    exit(1);
}

// If current token is expected operator(TK_OPE), advance to the next token and return true.
// Otherwise, return false.
bool consumeOperator(char* op){
    if(token->kind == TK_OPE && strncmp(token->str, op, strlen(op))==0){
        token = token->next;
        return true;
    }else{
        return false;
    }
}

// If current token is expected operator(TK_OPE), advance to the next token.
// Otherwise, report errors.
void expectOperator(char* op){
    if(token->kind == TK_OPE && strncmp(token->str, op, strlen(op))==0){
        token = token->next;
    }else{
        errorAt(token->str, "not %s", op);
    }
}

// If current token is expected number(TK_NUM),advance to the next token and return the number.
// Otherwise, report errors.
int expectNumber(){
    if(token->kind == TK_NUM){
        int num = token->val;
        token = token->next;
        return num;
    }else{
        errorAt(token->str, "not a number");
    }
}

// If current token is local variable(TK_IDENT), advance to the next token and return the token.
// Otherwise, return NULL.
Token* consumeIdent(){
    if(token->kind == TK_IDENT){
        Token *tok = token;
        token = tok->next;
        return tok;
    }else{
        return NULL;
    }
}

// Return a boolean value that whether token is EOF(\0)
bool atEof(){
    return token->kind == TK_EOF;
}

// create a new token, and link it to cur
// (use like,     cur = newToken(TK_OPE, cur, p, len)          )
Token* newToken(TokenKind kind, Token* cur, char* str, int len){
    //create a new token as tok
    Token* tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;

    cur->next = tok;
    return tok;
}

// Tokenize the input string p, and return it.
Token* tokenize(char *p){
    Token head;
    head.next = NULL;
    Token* cur = &head;
    // head is blank token

    while(*p){
        // Skip space
        if(isspace(*p)){
            ++p;
            continue;
        }

        // If p is an operator
        if(strncmp(p, "==", 2)==0 || strncmp(p, "!=", 2)==0 || strncmp(p, "<=", 2)==0 || strncmp(p, ">=", 2)==0){
            cur = newToken(TK_OPE, cur, p, 2);
            ++p; ++p;
            continue;
        }else if(*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '<' || *p == '>' || *p == ';' || *p == '='){
            cur = newToken(TK_OPE, cur, p++, 1);
            continue;
        }

        // If p is an identifier(named a-z, A-Z, 0-9, _)
        if(('a' <= *p && *p <= 'z') || ('A' <= *p && *p <= 'Z') || (*p == '_')){
            char *temp = p;
            int count = 0;
            // Count length of the variable name
            while (('a' <= *temp && *temp <= 'z') || ('A' <= *temp && *temp <= 'Z') || (*temp == '_')){
                ++count;
                ++temp;
            }
            cur = newToken(TK_IDENT, cur, p, count);
            p += count;
            continue;
        }

        // If p is a number
        if(isdigit(*p)){
            char * endnum;
            strtol(p, &endnum, 10);
            cur = newToken(TK_NUM, cur, p, endnum-p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error("cannot tokenize");
    }

    newToken(TK_EOF, cur, p, 1);
    return head.next;
}