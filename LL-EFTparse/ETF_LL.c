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
    char *symbol;
    struct tree *left;
    struct tree *middle;
    struct tree * right;
};

struct tree *createNode(char *symbol) {
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


// char includes ID, NUM, +, *, (, ), $
char *stack[1024];

// E, E', T, T', F, +, *, (, ), ID, NUM, $
int LLparsing(int es){
    int cnt = 0;
    int top = 0;
    stack[top]="$";
    stack[++top]="E";
    struct tree *root = createNode("E");
    struct tree *p = root;
    while (!es){
        char *chartype= Scanout[cnt].type;
        char *symbol = stack[top];
        if (strcmp(symbol, "E")==0){
            if (strcmp(chartype, "+")==0){
                es=1;
            }
            else if (strcmp(chartype, "*")==0){
                es=1;
            }
            else if (strcmp(chartype, "ID")==0 || strcmp(chartype, "NUM")==0){
                stack[top]="E\'";
                stack[++top]="T";
                p->left = createNode("T");
                p->right = createNode("E\'");
                p = p->left;
            }
            else if (strcmp(chartype, "(")==0){
                stack[top]="E\'";
                stack[++top]="T";
                p->left = createNode("T");
                p->right = createNode("E\'");
                p = p->left;
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
                stack[++top]="T";
                stack[++top]="+";
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
                top--;
            }
            else if (strcmp(chartype, "$")==0){
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
                stack[top]="T\'";
                stack[++top]="F";
            }
            else if (strcmp(chartype, "(")==0){
                stack[top]="T\'";
                stack[++top]="F";
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
                top--;
            }
            else if (strcmp(chartype, "*")==0){
                stack[++top]="F";
                stack[++top]="*";
            }
            else if (strcmp(chartype, "ID")==0){
                es=1;
            }
            else if (strcmp(chartype, "(")==0){
                es=1;
            }
            else if (strcmp(chartype, ")")==0){
                top--;
            }
            else if (strcmp(chartype, "$")==0){
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
                stack[top]="ID";
            }
            else if (strcmp(chartype, "NUM")==0){
                stack[top]="NUM";
            }
            else if (strcmp(chartype, "(")==0){
                stack[top]=")";
                stack[++top]="E";
                stack[++top]="(";
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
        else{//缁堢粨绗?
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

        //鏄剧ず杈撳嚭鏍堝鐞嗚繃绋?
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
    printf("please input filename: ");
    scanf("%s", filename);
    printf("\nscanning...\n");
    es = TESTscan(filename);
    printf("\nparsing...\n");
    
    es = LLparsing(es);
    if (es>0){
        printf("Syntax error!!!\n");
        return 0;
    }
    printf("Parsing succeeded!!!\n");
    

    return 0;
}
