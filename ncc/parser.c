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
//               | "return" expr ";"
//               | "if" "(" expr ")" stmt ("else" stmt)?
//               | "while" "(" expr ")" stmt
//               | "for" "(" expr? ";" expr? ";" expr? ")" stmt
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

//  program    = stmt*
void program(){
    int i=0;
    while(!atEof()){
        code[i++] = stmt();
    }
    code[i] = NULL; // end of code
}

//  stmt       = expr ";"
//               | "{" stmt* "}"
//               | "return" expr ";"
//               | "if" "(" expr ")" stmt ("else" stmt)?
//               | "while" "(" expr ")" stmt
//               | "for" "(" expr? ";" expr? ";" expr? ")" stmt
Node* stmt(){
    Node* node;

    if(consumeIf()){ // "if" "(" expr ")" stmt ("else" stmt)?
        node = calloc(1, sizeof(Node));
        node->kind = ND_IF;

        expectOperator("(");
        node->cond = expr();
        expectOperator(")");

        node->stmt = stmt();

        if(consumeElse()){
            node->stmtElse = stmt();
        }

    }else if(consumeWhile()){ // "while" "(" expr ")" stmt
        node = calloc(1, sizeof(Node));
        node->kind = ND_WHILE;

        expectOperator("(");
        node->cond = expr();
        expectOperator(")");

        node->stmt = stmt();

    }else if(consumeFor()){
        node = calloc(1, sizeof(Node)); // "for" "(" expr? ";" expr? ";" expr? ")" stmt
        node->kind = ND_FOR;

        expectOperator("(");
        if(!consumeOperator(";")){
            node->initial = expr();
            consumeOperator(";");
        }
        if(!consumeOperator(";")){
            node->cond = expr();
            consumeOperator(";");
        }
        if(!consumeOperator(";")){
            node->update= expr();
            consumeOperator(";");
        }
        expectOperator(")");

        node->stmt = stmt();

    }else if(consumeReturn()){ // "return" expr ";"
        node = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->rhs = expr();
        expectOperator(";");

    }else if(consumeOperator("{")){
        node = calloc(1, sizeof(Node));
        node->kind = ND_BLOCK;
        Node* cur = node;
        while(!consumeOperator("}")){
            cur->next = stmt();
            cur = cur->next;
        }
        return node;
    }else{ // expr ";"
        node = expr();
        expectOperator(";");
    }

    return node;
}

//  expr       = assign
Node* expr(){
    Node* node = assign();
    return node;
}

//  assign     = equality ("=" assign)?
Node* assign(){
    Node* node = equality();
    if(consumeOperator("=")){
        node = newNodeOperation(ND_ASS, node, assign());
    }
    return node;
}

//  equality   = relational ("==" relational | "!=" relational)*
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

//  relational = add ("<" add | "<=" add | ">" add | ">=" add)*
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

//  add        = mul ("+" mul | "-" mul)*
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

//  mul        = unary ('*' unary | '/' unary)*
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

//  unary      = ('+' | '-')? primary
Node* unary(){
    if(consumeOperator("+")){
        return primary();
    }
    if(consumeOperator("-")){
        return newNodeOperation(ND_SUB, newNodeNumber(0), primary());
    }
    return primary();
}

//  primary    = num | ident | ( '(' expr ')')
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