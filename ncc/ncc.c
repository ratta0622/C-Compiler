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

//type of tokens
//(list of token <=> input equation)
typedef struct Token Token;

struct Token{
    TokenKind kind; //type of a token
    Token *next;    //next token
    int val;        //number of token(if token = TK_NUM)
    char *str;      //Token string
    int len;        //Token length
};

//kinds of nodes in the abstract syntax tree
typedef enum{
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_EQ,  // ==
    ND_NEQ, // !=
    ND_GE,  // >
    ND_GEQ, // >=
    ND_LE,  // <
    ND_LEQ, // <=
    ND_NUM, // integer
} NodeKind;

//type of nodes in the abstract syntax tree
typedef struct Node Node;

struct Node{
    NodeKind kind; // type of a node
    Node *lhs;     // left-hand side
    Node *rhs;     // right-hand side
    int val;       // number of a node (if kind = ND_NUM)
};


//the current token being focused on
Token *token;

//input
char* user_input;

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
bool consume(char* op){
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
        }else if(*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '<' || *p == '>'){
            cur = new_token(TK_OPE, cur, p++, 1);
            continue;
        }

        //If p is a number
        if(isdigit(*p)){
            cur = new_token(TK_NUM, cur, p, 1);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error("cannot tokenize");
    }

    new_token(TK_EOF, cur, p, 1);
    return head.next;
}


void debug_token(){
    while(!at_eof()){
        printf("%s\n",token->str);
        token = token->next;
    }
    error("debug function\n");
}


//Create a new node (kind is operator)
Node* new_node_op(NodeKind kind, Node* lhs, Node* rhs){
    Node* node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}
//Create a new node(kind is number)
Node* new_node_num(int val){
    Node* node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

//EBNF(abstract syntax tree)
// expr       = equality
// equality   = relational ("==" relational | "!=" relational)*
// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
// add        = mul ("+" mul | "-" mul)*
// mul        = unary ('*' unary | '/' unary)*
// unary      = ('+' | '-')? primary
// primary    = num | ( '(' expr ')')
Node* expr();
Node* relational();
Node* add();
Node* mul();
Node* unary();
Node* primary();

Node* expr(){
    Node* node = relational();

    while(1){
        if(consume("==")){
            node = new_node_op(ND_EQ, node, relational());
        }else if(consume("!=")){
            node = new_node_op(ND_NEQ, node, relational());
        }else{
            return node;
        }
    }
}

Node* relational(){
    Node* node = add();

    while(1){
        if(consume(">=")){
            node = new_node_op(ND_GEQ, add(), node);
        }else if(consume(">")){
            node = new_node_op(ND_GE, add(), node);
        }else if(consume("<=")){
            node = new_node_op(ND_LEQ, node, add());
        }else if(consume("<")){
            node = new_node_op(ND_LE, node, add());
        }else{
            return node;
        }
    }
}

Node* add(){
    Node* node = mul();

    while(1){
        if(consume("+")){
            node = new_node_op(ND_ADD, node, mul());
        }else if(consume("-")){
            node = new_node_op(ND_SUB, node, mul());
        }else{
            return node;
        }
    }
}

Node* mul(){
    Node* node = unary();

    while(1){
        if(consume("*")){
            node = new_node_op(ND_MUL, node, unary());
        }else if(consume("/")){
            node = new_node_op(ND_DIV, node, unary());
        }else{
            return node;
        }
    }
}

Node* unary(){
    if(consume("+")){
        return primary();
    }
    if(consume("-")){
        return new_node_op(ND_SUB, new_node_num(0), primary());
    }
    return primary();
}

Node* primary(){
    if(consume("(")){
        Node* node = expr();
        expect_operator(")");
        return node;
    }else{
        return new_node_num(expect_number());
    }
}

//Output assembly for calculating expressions on integers represented by the nodes of the abstract syntax tree.
void genAssemblyFromNodesOfEBNF(Node* node){
    //if node is term of single number
    if(node->kind == ND_NUM){
        printf("    push %d\n", node->val);
        return;
    }else{
        genAssemblyFromNodesOfEBNF(node->lhs);
        genAssemblyFromNodesOfEBNF(node->rhs);

        switch (node->kind)
        {
        case ND_ADD:
            printf("    pop rdi\n");
            printf("    pop rax\n");
            printf("    add rax, rdi\n");
            printf("    push rax\n");
            break;

        case ND_SUB:
            printf("    pop rdi\n");
            printf("    pop rax\n");
            printf("    sub rax, rdi\n");
            printf("    push rax\n");
            break;

        case ND_MUL:
            printf("    pop rdi\n");
            printf("    pop rax\n");
            printf("    imul rax, rdi\n");
            printf("    push rax\n");
            break;

        case ND_DIV:
            printf("    pop rdi\n");
            printf("    pop rax\n");
            printf("    cqo\n");
            printf("    idiv rdi\n");
            printf("    push rax\n");
            break;

        case ND_EQ:
            printf("    pop rdi\n");
            printf("    pop rax\n");
            printf("    cmp rax, rdi\n");
            printf("    sete al\n");
            printf("    movzb rax, al\n");
            printf("    push rax\n");
            break;

        case ND_NEQ:
            printf("    pop rdi\n");
            printf("    pop rax\n");
            printf("    cmp rax, rdi\n");
            printf("    setne al\n");
            printf("    movzb rax, al\n");
            printf("    push rax\n");
            break;

        case ND_LE:
            printf("    pop rdi\n");
            printf("    pop rax\n");
            printf("    cmp rax, rdi\n");
            printf("    setl al\n");
            printf("    movzb rax, al\n");
            printf("    push rax\n");
            break;

        case ND_LEQ:
            printf("    pop rdi\n");
            printf("    pop rax\n");
            printf("    cmp rax, rdi\n");
            printf("    setle al\n");
            printf("    movzb rax, al\n");
            printf("    push rax\n");
            break;
        }
    }
}

int main(int argc, char *argv[]){
    if(argc != 2){
        fprintf(stderr, "Incorrect number of arguments\n");
        return 1;
    }

    // Save argv[1] to user_input(global val) for error messages.
    user_input = argv[1];

    // Tokenize input
    token = tokenize(user_input);

    // Perse the torkenized expression
    Node* node = expr();

    // Output the first half of the assembly
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // Output the assembly for calculations
    genAssemblyFromNodesOfEBNF(node);

    // Pop the calculation result from the stack, and return
    printf("    pop rax\n");
    printf("    ret\n");

    return 0;
}