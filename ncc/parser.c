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


//EBNF(abstract syntax tree)
// expr       = equality
// equality   = relational ("==" relational | "!=" relational)*
// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
// add        = mul ("+" mul | "-" mul)*
// mul        = unary ('*' unary | '/' unary)*
// unary      = ('+' | '-')? primary
// primary    = num | ( '(' expr ')')
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