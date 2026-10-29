#include <stdio.h>
#include <ctype.h>
#include <string.h>
#define KeywordSum 8

char *keyword[KeywordSum] = {"if", "else", "while", "for", "do", "int", "read", "write"};
char singleword[256] = "+-*(){}:,;";
char doubleword[256] = "<>=!";
int line = 1;

struct Token {
    char type[256];
    char value[256];
};

extern struct Token Scanin[256], Scanout[256];


int check_keyword(char *word) {
    for (int i = 0; i < KeywordSum; i++) {
        if (strcmp(word, keyword[i]) == 0) {
            return i;
        }
    }
    return 1e9;
}

int TESTscan(char *filename){
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Failed to open file: %s\n", filename);
        return 1;
    }
    // Add scanning logic here
    int idx = 0; // 记录当前token的索引
    int cnt = 0; // 记录当前扫描到的token数量
    struct Token token;
    while (!feof(file)){ //
        token.value[idx] = fgetc(file);
        if (token.value[idx] == '\n') {
            line++;
        }
        if (token.value[idx] == EOF) {
            break;
        }
        if (isspace(token.value[idx])) {
            if (idx!=0){
                token.value[idx] = '\0';
                TESTout(token);
                Scanout[cnt++] = token;
            }
            idx = 0;
            continue;
        }
        if (isalpha(token.value[idx])) {
            while (1){
                token.value[++idx] = fgetc(file);
                if (!isalpha(token.value[idx]) && !isdigit(token.value[idx])) {
                    ungetc(token.value[idx], file);
                    break;
                }
            }
            token.value[idx] = '\0';
            if (check_keyword(token.value) != 1e9) {
                strcpy(token.type, keyword[check_keyword(token.value)]); 
                TESTout(token);
                Scanout[cnt++] = token;
                idx = 0;
                continue;
            } else {
                strcpy(token.type, "ID");
                TESTout(token);
                Scanout[cnt++] = token;
                idx = 0;
                continue;
            }
        }
        if (isdigit(token.value[idx])) {
            while (1){
                token.value[++idx] = fgetc(file);
                if (!isdigit(token.value[idx])) {
                    ungetc(token.value[idx], file);
                    break;
                }
            }
            strcpy(token.type, "NUM");
            token.value[idx] = '\0';
            TESTout(token);
            Scanout[cnt++] = token;
            idx = 0;
            continue;
        }
        if  (strchr(singleword, token.value[idx])) {
            token.value[++idx] = '\0';
            strcpy(token.type, token.value);
            TESTout(token);
            Scanout[cnt++] = token;
            idx = 0;
            continue;
        }
        if (strchr(doubleword, token.value[idx])) {
            token.value[++idx] = fgetc(file);
            if (token.value[idx] != '=') {
                ungetc(token.value[idx], file);
                token.value[idx] = '\0';
            } else {
                token.value[++idx] = '\0';
            }
            strcpy(token.type, token.value);
            TESTout(token);
            Scanout[cnt++] = token;
            idx = 0;
            continue;
        }
        if (token.value[idx] == '/') {
            char next = fgetc(file);
            if (next == '*') {
                // Skip the comment
                char prev = 0;
                while (!feof(file)) {
                    char c = fgetc(file);
                    if (prev == '*' && c == '/') {
                        break;
                    }
                    prev = c;
                }
            } else {
                ungetc(next, file);
                token.value[++idx] = '\0';
                strcpy(token.type, token.value);
                TESTout(token);
                Scanout[cnt++] = token;
                continue;
            }
        }
        else{
            ERROR(token.value[idx]);
            cnt=0; idx=0;
            return 1;
        }
    }
    if (cnt<255) {
        strcpy(token.type, "$");
        strcpy(token.value, "$");
        Scanout[cnt++] = token;
        TESTout(token);
    }

    fclose(file);
    return 0;
}


void TESTout(struct Token token) {
    printf("%s\t%s\n", token.type, token.value);
}



void ERROR(char illegalChar) {
    printf("there is one illegal character: '%c' in line %d\n", illegalChar, line); 
}

