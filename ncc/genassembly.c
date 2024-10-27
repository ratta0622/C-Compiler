#include "ncc.h"


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