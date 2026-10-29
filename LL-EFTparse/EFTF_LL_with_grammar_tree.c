#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "TESTscan.c"

extern int TESTscan(char *filename);
extern int TESTparse();
struct Token Scanin[256], Scanout[256];
FILE *fin, *fout;


struct tree{
    const char *symbol;
    struct tree *left;
    struct tree *middle;
    struct tree *right;
};

struct tree *createNode(const char *symbol) {
    struct tree *node = malloc(sizeof(struct tree));

    if (node == NULL) {
        return NULL;
    }

    node->symbol = symbol;
    node->left = NULL;
    node->middle = NULL;
    node->right = NULL;

    return node;
}

void printTree(const struct tree *node, int depth) {
    if (node == NULL) {
        return;
    }

    for (int i = 0; i < depth; i++) {
        printf("    ");
    }
    printf("|-- %s\n", node->symbol);

    printTree(node->left, depth + 1);
    printTree(node->middle, depth + 1);
    printTree(node->right, depth + 1);
}

void freeTree(struct tree *node) {
    if (node == NULL) {
        return;
    }

    freeTree(node->left);
    freeTree(node->middle);
    freeTree(node->right);
    free(node);
}


// char includes ID, NUM, +, *, (, ), $
char *stack[1024];
struct tree *treeStack[1024];

// E, E', T, T', F, +, *, (, ), ID, NUM, $
int LLparsing(int es, struct tree **rootOut){
    int cnt = 0;
    int top = 0;
    stack[top]="$";
    treeStack[top]=NULL;

    struct tree *root = createNode("E");
    if (root == NULL) {
        return 1;
    }
    *rootOut = root;

    stack[++top]="E";
    treeStack[top]=root;

    while (!es){
        char *chartype= Scanout[cnt].type;
        char *symbol = stack[top];
        struct tree *p = treeStack[top];
        if (strcmp(symbol, "E")==0){
            if (strcmp(chartype, "+")==0){
                es=1;
            }
            else if (strcmp(chartype, "*")==0){
                es=1;
            }
            else if (strcmp(chartype, "ID")==0 || strcmp(chartype, "NUM")==0){
                struct tree *tNode = createNode("T");
                struct tree *ePrimeNode = createNode("E'");
                if (tNode == NULL || ePrimeNode == NULL) {
                    es=1;
                    break;
                }
                p->left = tNode;
                p->right = ePrimeNode;

                stack[top]="E\'";
                treeStack[top]=ePrimeNode;
                stack[++top]="T";
                treeStack[top]=tNode;
            }
            else if (strcmp(chartype, "(")==0){
                struct tree *tNode = createNode("T");
                struct tree *ePrimeNode = createNode("E'");
                if (tNode == NULL || ePrimeNode == NULL) {
                    es=1;
                    break;
                }
                p->left = tNode;
                p->right = ePrimeNode;

                stack[top]="E\'";
                treeStack[top]=ePrimeNode;
                stack[++top]="T";
                treeStack[top]=tNode;
            }
            else if (strcmp(chartype, ")")==0){
                es=1;
            }
            else if (strcmp(chartype, "$")==0){
                es=1;
            }
            else{
                es=1;
            }
        }
        else if (strcmp(symbol, "E\'")==0){
            if (strcmp(chartype, "+")==0){
                struct tree *plusNode = createNode("+");
                struct tree *tNode = createNode("T");
                struct tree *ePrimeNode = createNode("E'");
                if (plusNode == NULL || tNode == NULL || ePrimeNode == NULL) {
                    es=1;
                    break;
                }
                p->left = plusNode;
                p->middle = tNode;
                p->right = ePrimeNode;

                stack[top]="E'";
                treeStack[top]=ePrimeNode;
                stack[++top]="T";
                treeStack[top]=tNode;
                stack[++top]="+";
                treeStack[top]=plusNode;
            }
            else if (strcmp(chartype, "*")==0){
                es=1;
            }
            else if (strcmp(chartype, "ID")==0){
                es=1;
            }
            else if (strcmp(chartype, "(")==0){
                es=1;
            }
            else if (strcmp(chartype, ")")==0){
                p->left=createNode("epsilon");
                top--;
            }
            else if (strcmp(chartype, "$")==0){
                p->left=createNode("epsilon");
                top--;
            }
            else{
                es=1;
            }
        }
        else if (strcmp(symbol, "T")==0){
            if (strcmp(chartype, "+")==0){
                es=1;
            }
            else if (strcmp(chartype, "*")==0){
                es=1;
            }
            else if (strcmp(chartype, "ID")==0 || strcmp(chartype, "NUM")==0){
                struct tree *fNode = createNode("F");
                struct tree *tPrimeNode = createNode("T'");
                if (fNode == NULL || tPrimeNode == NULL) {
                    es=1;
                    break;
                }
                p->left = fNode;
                p->right = tPrimeNode;

                stack[top]="T\'";
                treeStack[top]=tPrimeNode;
                stack[++top]="F";
                treeStack[top]=fNode;
            }
            else if (strcmp(chartype, "(")==0){
                struct tree *fNode = createNode("F");
                struct tree *tPrimeNode = createNode("T'");
                if (fNode == NULL || tPrimeNode == NULL) {
                    es=1;
                    break;
                }
                p->left = fNode;
                p->right = tPrimeNode;

                stack[top]="T\'";
                treeStack[top]=tPrimeNode;
                stack[++top]="F";
                treeStack[top]=fNode;
            }
            else if (strcmp(chartype, ")")==0){
                es=1;
            }
            else if (strcmp(chartype, "$")==0){
                es=1;
            }
            else{
                es=1;
            }
        }
        else if (strcmp(symbol, "T\'")==0){
            if (strcmp(chartype, "+")==0){
                p->left=createNode("epsilon");
                top--;
            }
            else if (strcmp(chartype, "*")==0){
                struct tree *starNode = createNode("*");
                struct tree *fNode = createNode("F");
                struct tree *tPrimeNode = createNode("T'");
                if (starNode == NULL || fNode == NULL || tPrimeNode == NULL) {
                    es=1;
                    break;
                }
                p->left = starNode;
                p->middle = fNode;
                p->right = tPrimeNode;

                stack[top]="T'";
                treeStack[top]=tPrimeNode;
                stack[++top]="F";
                treeStack[top]=fNode;
                stack[++top]="*";
                treeStack[top]=starNode;
            }
            else if (strcmp(chartype, "ID")==0){
                es=1;
            }
            else if (strcmp(chartype, "(")==0){
                es=1;
            }
            else if (strcmp(chartype, ")")==0){
                p->left=createNode("epsilon");
                top--;
            }
            else if (strcmp(chartype, "$")==0){
                p->left=createNode("epsilon");
                top--;
            }
            else{
                es=1;
            }   
        }
        else if (strcmp(symbol, "F")==0){
            if (strcmp(chartype, "+")==0){
                es=1;
            }
            else if (strcmp(chartype, "*")==0){
                es=1;
            }
            else if (strcmp(chartype, "ID")==0){
                struct tree *idNode = createNode("ID");
                if (idNode == NULL) {
                    es=1;
                    break;
                }
                p->left=idNode;
                stack[top]="ID";
                treeStack[top]=idNode;
            }
            else if (strcmp(chartype, "NUM")==0){
                struct tree *numNode = createNode("NUM");
                if (numNode == NULL) {
                    es=1;
                    break;
                }
                p->left=numNode;
                stack[top]="NUM";
                treeStack[top]=numNode;
            }
            else if (strcmp(chartype, "(")==0){
                struct tree *leftParenNode = createNode("(");
                struct tree *eNode = createNode("E");
                struct tree *rightParenNode = createNode(")");
                if (leftParenNode == NULL || eNode == NULL || rightParenNode == NULL) {
                    es=1;
                    break;
                }
                p->left=leftParenNode;
                p->middle=eNode;
                p->right=rightParenNode;

                stack[top]=")";
                treeStack[top]=rightParenNode;
                stack[++top]="E";
                treeStack[top]=eNode;
                stack[++top]="(";
                treeStack[top]=leftParenNode;
            }
            else if (strcmp(chartype, ")")==0){
                es=1;
            }
            else if (strcmp(chartype, "$")==0){
                es=1;
            }
            else{
                es=1;
            }
        }
        else if (strcmp(symbol, "$")==0){
            if (strcmp(chartype, "$")!=0){
                es=1;
            }
            break;
        }
        else{//终结符
            if (strcmp(symbol, chartype)==0){
                top--;
                cnt++;
            }
            else{
                es=1;
            }
        }
        if (es==1){
            break;
        }

        //显示输出栈处理过程
        for (int i=0; i<=top; i++){
            printf("%s ",stack[i]);
        }
        printf("\n");

    }
    return es;
}




int main(){
    int es = 0;
    char filename[256];
    struct tree *root = NULL;
    printf("please input filename: ");
    scanf("%s", filename);
    printf("\nscanning...\n");
    es = TESTscan(filename);
    printf("\nparsing...\n");
    
    es = LLparsing(es, &root);
    if (es>0){
        printf("Syntax error!!!\n");
        freeTree(root);
        return 0;
    }
    printf("Parsing succeeded!!!\n");
    printf("\nParse tree:\n");
    printTree(root, 0);
    freeTree(root);

    return 0;
}
