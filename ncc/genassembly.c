#include "ncc.h"


// Set the address of the variable specified by the node(RBP - node.offset) to RAX
void setAddressToRAX(Node *node){
    if(node->kind != ND_LVAR){
        error("The LHS of the assignment is not local variable\n");
    }else{
        printf("    mov rax, rbp\n");
        printf("    sub rax, %d\n", node->offset);
    }
}


// Output assembly for a single statement devided by ;
void genStatement(Node* node){
    switch (node->kind)
    {
    case ND_NUM: // If node is term of single number
        printf("    push %d\n", node->val);
        return;

    case ND_LVAR: // If node is name of local variable
        setAddressToRAX(node);
        printf("    mov rax, [rax]\n"); // Set the value of the variable to RAX
        printf("    push rax\n"); // This statement is the same as the case of ND_NUM
        return;

    case ND_ASS: // If node is '='
        setAddressToRAX(node->lhs);
        printf("    push rax\n");

        genStatement(node->rhs);

        printf("    pop rdi\n"); // RDI is RHS(value to be assigned)
        printf("    pop rax\n"); // RAX is address of local value(this was pushed by setAddressToRAX())
        printf("    mov [rax], rdi\n"); // e.g.) a = 8
        printf("    push rdi\n"); // In C lang, the return value of an assignment expression is the value of the LHS after the assiginment
        return;

    case ND_RETURN: // If node is "return"
        genStatement(node->rhs); // Push RHS
        printf("    pop rax\n"); // RAX is return value
        // Epilogue that is the same as one of main.c
        printf("    mov rsp, rbp\n");
        printf("    pop rbp\n");
        printf("    ret\n");
        return;

    case ND_IF: // If node is "if"
        genStatement(node->cond); // Push condition
        printf("    pop rax\n");
        printf("    cmp rax, 0\n"); // set RAX(value of condition expression) to Flag Register
        if(node->stmtElse){
            printf("    je .LabelElse\n");
        }
        genStatement(node->stmt); // Condition==true
        printf("    jmp .LabelEnd\n");
        if(node->stmtElse){
            printf(".LabelElse:\n");
            genStatement(node->stmtElse); // Condition==false
        }
        printf(".LabelEnd:\n");
        return;

    case ND_WHILE: // If node is "while"
        printf(".LabelBegin:\n");
        genStatement(node->cond); // Push condition
        printf("    pop rax\n");
        printf("    cmp rax, 0\n"); // set RAX(value of condition expression) to Flag Register
        printf("    je .LabelEnd\n");
        genStatement(node->stmt); // Condition == true
        printf("    jmp .LabelBegin\n");
        printf(".LabelEnd:");
        return;
    }

    // recursive
    genStatement(node->lhs);
    genStatement(node->rhs);

    // If node is + - * / == != <= <
    printf("    pop rdi\n");
    printf("    pop rax\n");
    switch (node->kind)
    {
    case ND_ADD:
        printf("    add rax, rdi\n");
        break;

    case ND_SUB:
        printf("    sub rax, rdi\n");
        break;

    case ND_MUL:
        printf("    imul rax, rdi\n");
        break;

    case ND_DIV:
        printf("    cqo\n");
        printf("    idiv rdi\n");
        break;

    case ND_EQ:
        printf("    cmp rax, rdi\n");
        printf("    sete al\n");
        printf("    movzb rax, al\n");
        break;

    case ND_NEQ:
        printf("    cmp rax, rdi\n");
        printf("    setne al\n");
        printf("    movzb rax, al\n");
        break;

    case ND_LE:
        printf("    cmp rax, rdi\n");
        printf("    setl al\n");
        printf("    movzb rax, al\n");
        break;

    case ND_LEQ:
        printf("    cmp rax, rdi\n");
        printf("    setle al\n");
        printf("    movzb rax, al\n");
        break;
    }
    printf("    push rax\n");
}