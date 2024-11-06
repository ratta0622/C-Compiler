#include "ncc.h"

// Search for a variable by name.
// If not found, return NULL.
Lvar* searchLvar(Token* tok){
    for(Lvar* local = headLocalList; local != NULL; local = local->next){
        if(local->len == tok->len && strncmp(local->name, tok->str, local->len)==0){
            return local;
        }
    }
    return NULL;
}

// Create a new node from a token(kind is operator)
Node* newNodeOperation(NodeKind kind, Node* lhs, Node* rhs){
    Node* node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}
// Create a new node from a token(kind is number)
Node* newNodeNumber(int val){
    Node* node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}
// Create a new node from a token(kind is identifier)
Node* newNodeIdent(Token* tok){
    Node* node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;

    Lvar *local = searchLvar(tok);
    if(local != NULL){
        node->offset = local->offset;
    }else{
        local = calloc(1, sizeof(Lvar));
        localList->next = local;                    // add to the end of the list
        local->len = tok->len;                      // local.len
        local->name = (char*)malloc(sizeof(char) * (local->len + 1));
        strncpy(local->name, tok->str, local->len); // local.name
        local->name[local->len] = '\0';             // Same as above
        local->offset = localList->offset + 8;      // local.offset
        node->offset = local->offset;               // node.offset
        localList = local;                          // Update the current element of the list
    }
    return node;
}

// EBNF(abstract syntax tree)
//  program    = stmt*
//  stmt       = expr ";"
//  expr       = assign
//  assign     = equality ("=" assign)?
//  equality   = relational ("==" relational | "!=" relational)*
//  relational = add ("<" add | "<=" add | ">" add | ">=" add)*
//  add        = mul ("+" mul | "-" mul)*
//  mul        = unary ('*' unary | '/' unary)*
//  unary      = ('+' | '-')? primary
//  primary    = num | ident | ( '(' expr ')')
Node* code[100]; // store nodes of program (extern declaration in ncc.h)
void program();
Node* stmt();
Node* expr();
Node* assign();
Node* equality();
Node* relational();
Node* add();
Node* mul();
Node* unary();
Node* primary();

void program(){
    int i=0;
    while(!atEof()){
        code[i++] = stmt();
    }
    code[i] = NULL; // end of code
}

Node* stmt(){
    Node* node = expr();
    expectOperator(";"); // if there is not ';', error
    return node;
}

Node* expr(){
    Node* node = assign();
    return node;
}

Node* assign(){
    Node* node = equality();
    if(consumeOperator("=")){
        node = newNodeOperation(ND_ASS, node, assign());
    }
    return node;
}

Node* equality(){
    Node* node = relational();

    while(1){
        if(consumeOperator("==")){
            node = newNodeOperation(ND_EQ, node, relational());
        }else if(consumeOperator("!=")){
            node = newNodeOperation(ND_NEQ, node, relational());
        }else{
            return node;
        }
    }
}

Node* relational(){
    Node* node = add();

    while(1){
        if(consumeOperator(">=")){
            node = newNodeOperation(ND_GEQ, add(), node);
        }else if(consumeOperator(">")){
            node = newNodeOperation(ND_GE, add(), node);
        }else if(consumeOperator("<=")){
            node = newNodeOperation(ND_LEQ, node, add());
        }else if(consumeOperator("<")){
            node = newNodeOperation(ND_LE, node, add());
        }else{
            return node;
        }
    }
}

Node* add(){
    Node* node = mul();

    while(1){
        if(consumeOperator("+")){
            node = newNodeOperation(ND_ADD, node, mul());
        }else if(consumeOperator("-")){
            node = newNodeOperation(ND_SUB, node, mul());
        }else{
            return node;
        }
    }
}

Node* mul(){
    Node* node = unary();

    while(1){
        if(consumeOperator("*")){
            node = newNodeOperation(ND_MUL, node, unary());
        }else if(consumeOperator("/")){
            node = newNodeOperation(ND_DIV, node, unary());
        }else{
            return node;
        }
    }
}

Node* unary(){
    if(consumeOperator("+")){
        return primary();
    }
    if(consumeOperator("-")){
        return newNodeOperation(ND_SUB, newNodeNumber(0), primary());
    }
    return primary();
}

Node* primary(){
    Token* tok = consumeIdent();
    if(tok != NULL){
        return newNodeIdent(tok);
    }else if(consumeOperator("(")){
        Node* node = expr();
        expectOperator(")");
        return node;
    }else{
        return newNodeNumber(expectNumber());
    }
}