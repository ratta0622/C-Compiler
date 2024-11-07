#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <errno.h>

//kinds of tokens
typedef enum{
    TK_OPE,    // token of operator(+, -, *, /, ...)
    TK_IDENT,  // identifier
    TK_NUM,    // token of number
    TK_RETURN, // token of "return"
    TK_IF,     // token of "if"
    TK_ELSE,   // token of "else"
    TK_WHILE,  // token of "while"
    TK_FOR,    // token of "for"
    TK_EOF,    // token of End Of File(\0)
} TokenKind;

// token type
// (list of token <=> input statements)
typedef struct Token Token;

struct Token{
    TokenKind kind; // type of a token
    Token *next;    // next token
    int val;        // number of token(if token = TK_NUM)
    char *str;      // Token string
    int len;        // Token length
};

// local variable type
// (list representing local variables)
typedef struct Lvar Lvar;

struct Lvar{
    Lvar *next; // next local variable
    char *name; // name of local variable
    int len;    // length of name
    int offset; // offset from RBP
};

// list of local variables
extern Lvar* headLocalList;
extern Lvar* localList;

// kinds of nodes in the abstract syntax tree
typedef enum{
    ND_ADD,    // +
    ND_SUB,    // -
    ND_MUL,    // *
    ND_DIV,    // /
    ND_MOD,    // %
    ND_EQ,     // ==
    ND_NEQ,    // !=
    ND_GE,     // >
    ND_GEQ,    // >=
    ND_LE,     // <
    ND_LEQ,    // <=
    ND_ASS,    // =
    ND_LVAR,   // local variables
    ND_NUM,    // integer
    ND_IF,     // "if"
    ND_WHILE,  // "while"
    ND_FOR,    // "for"
    ND_BLOCK,  // "block {}"
    ND_RETURN, // "return"
} NodeKind;

// type of nodes in the abstract syntax tree
typedef struct Node Node;

struct Node{
    NodeKind kind;   // type of a node
    Node *lhs;       // left-hand side
    Node *rhs;       // right-hand side
    Node *cond;      // condition expression (if kind = if, while, for)
    Node *stmt;      // statements executed if condition==true (if kind = if, while, for)
    Node *stmtElse;  // statements executed if condition==false (if kind = if)
    Node *initial;   // initialization (if kind = for)
    Node *update;    // update such as increment (if kind = for)
    Node *next;      // next statement (if kind = block{})
    int val;         // number of a node (if kind = ND_NUM)
    int offset;      // offset of local variable (if kind = ND_LVAR)
};


//global variable
extern char* userInput;
extern Token* token;
extern Node* code[100];
extern long labelNumber;


//function declaration
void errorAt(char* location, char* format, ...);
void error(char* format, ...);
bool consumeOperator(char* op);
void expectOperator(char* op);
int expectNumber();
Token* consumeIdent();
bool consumeReturn();
bool consumeIf();
bool consumeElse();
bool consumeWhile();
bool consumeFor();
bool atEof();
Token* tokenize(char* p);
void program();
void genStatement(Node* node);