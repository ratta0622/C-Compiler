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

//token type
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


//global variable
extern char* user_input;
extern Token* token;


//function declaration
void error_at(char* location, char* format, ...);
void error(char* format, ...);
bool consume(char* op);
void expect_operator(char* op);
int expect_number();
bool at_eof();
Token* tokenize(char* p);
Node* expr();
void genAssemblyFromNodesOfEBNF(Node* node);