#include "ncc.h"


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
//Create a new node(kind is identifier)
Node* new_node_ident(char tok){
    Node* node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    node->offset = (tok - 'a' +1)*8; // offset of a single local variable is 8. In addition, local variables are arranged in order as a, b, c, d, ... , z
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
    while(!at_eof()){
        code[i++] = stmt();
    }
    code[i] = NULL; // end of code
}

Node* stmt(){
    Node* node = expr();
    expect_operator(";"); // if there is not ';', error
    return node;
}

Node* expr(){
    Node* node = assign();
    return node;
}

Node* assign(){
    Node* node = equality();
    if(consume_operator("=")){
        node = new_node_op(ND_ASS, node, assign());
    }
    return node;
}

Node* equality(){
    Node* node = relational();

    while(1){
        if(consume_operator("==")){
            node = new_node_op(ND_EQ, node, relational());
        }else if(consume_operator("!=")){
            node = new_node_op(ND_NEQ, node, relational());
        }else{
            return node;
        }
    }
}

Node* relational(){
    Node* node = add();

    while(1){
        if(consume_operator(">=")){
            node = new_node_op(ND_GEQ, add(), node);
        }else if(consume_operator(">")){
            node = new_node_op(ND_GE, add(), node);
        }else if(consume_operator("<=")){
            node = new_node_op(ND_LEQ, node, add());
        }else if(consume_operator("<")){
            node = new_node_op(ND_LE, node, add());
        }else{
            return node;
        }
    }
}

Node* add(){
    Node* node = mul();

    while(1){
        if(consume_operator("+")){
            node = new_node_op(ND_ADD, node, mul());
        }else if(consume_operator("-")){
            node = new_node_op(ND_SUB, node, mul());
        }else{
            return node;
        }
    }
}

Node* mul(){
    Node* node = unary();

    while(1){
        if(consume_operator("*")){
            node = new_node_op(ND_MUL, node, unary());
        }else if(consume_operator("/")){
            node = new_node_op(ND_DIV, node, unary());
        }else{
            return node;
        }
    }
}

Node* unary(){
    if(consume_operator("+")){
        return primary();
    }
    if(consume_operator("-")){
        return new_node_op(ND_SUB, new_node_num(0), primary());
    }
    return primary();
}

Node* primary(){
    char tok = consume_ident();
    if(tok != '!'){

    }else if(consume_operator("(")){
        Node* node = expr();
        expect_operator(")");
        return node;
    }else{
        return new_node_num(expect_number());
    }
}