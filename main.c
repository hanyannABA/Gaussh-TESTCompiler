#include <stdio.h>
#include <ctype.h>

#include "TESTparse.c"

extern int TESTscan(char *filename);
extern int TESTparse();
struct Token Scanin[256], Scanout[256];

FILE *fin, *fout;

int main(){
    int es = 0;
    char filename[256];
    printf("please input filename: ");
    scanf("%s", filename);
    printf("\nscanning...\n");
    es = TESTscan(filename);
    if (es > 0) {printf("Error occurred during lexical analysis.\n"); return 0;}
    printf("Lexical analysis completed successfully.\n");
    printf("\nparsing...\n");
    es = TESTparse();
    if (es > 0) {printf("Error occurred during syntax analysis.\n"); return 0;}
    printf("Syntax analysis completed successfully.\n");


    return 0;
}
