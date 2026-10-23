#include <stdio.h>
#include <string.h>
#include "TESTscan.c"

extern struct Token Scanout[256];

char token[256], token1[512];
static FILE *fp;

// token/token1 保存当前待处理 Token；调用此函数后推进到下一个 Token。
static int read_next_token(char *type, char *value) {
    if (fscanf(fp, "%255s %511s", type, value) == 2) return 1;
    
    type[0] = '\0';
    value[0] = '\0';
    return 0;
}

static void print_current_token(void) {
    printf("Current token: %s %s\n", token, token1);
}


//program -> {declaration_list statement_list}
int program() {
    int es = 0;
    if (strcmp (token, "{") == 0) {
        read_next_token(token, token1);
        es = declaration_list();
        print_current_token();
        if (es > 0) return es;
        else{
            es = statement_list();
            print_current_token();
            if (es > 0) return es;
            else{
                if (strcmp (token, "}") == 0) {
                    if (read_next_token(token, token1)){
                        return 2;
                    }
                    return 0;
                }
                else return 2;
            }
        }
    }
    else{
        return 2;
    }
    return es;
}


//compound_stat -> {statment_list}
int compound_stat() {
    int es = 0;
    if (strcmp (token, "{") == 0) {
        read_next_token(token, token1);
        es = statement_list();
        print_current_token();
        if (es > 0) return es;
        else{
            if (strcmp (token, "}") == 0) {
                read_next_token(token, token1);
                return es;
            }
            else {
                es = 2;
            }
        }
    }
    else {es = 2;} //e
    return es;
}


//statement -> if_stat | while_stat | for_stat | read_stat | write_stat | compound_stat | expression_stat
int statement() {
    int es = 0;
    if (strcmp (token, "if") == 0) {
        es = if_stat();
    }
    else if (strcmp (token, "while") == 0) {
        es = while_stat();
    }
    else if (strcmp (token, "for") == 0) {
        es = for_stat();
    }
    else if (strcmp (token, "read") == 0) {
        es = read_stat();
    }
    else if (strcmp (token, "write") == 0) {
        es = write_stat();
    }
    else if (strcmp (token, "{") == 0) {
        es = compound_stat();
    }
    else {
        es = expression_stat();
    }
    return es;
}


//expression_stat -> expression ; | ;
int expression_stat() {
    int es = 0;
    if (strcmp (token, ";") == 0) {
        read_next_token(token, token1);
        return es;
    }
    else {
        es = expression();
        if (es > 0) return es;
        else{
            if (strcmp (token, ";") == 0) {
                read_next_token(token, token1);
                return es;
            }
            else es = 3;
        }
    }
    return es;
}


//expression -> ID = bool_expr | bool_expr
//warning here bool_expression may also include ID
int expression() {
    int es = 0;
    int fileadd;
    char token2[256], token3[512];
    if (strcmp (token, "ID") == 0) {
        fileadd = ftell(fp); //store the current file position
        fscanf(fp, "%s %s\n", token2, token3); //read the next two tokens
        printf("Next tokens: %s %s\n", token2, token3); //
        if (strcmp (token2, "=") == 0) {
            fscanf(fp, "%s %s\n", token, token1); //read the next two tokens
            printf("Next tokens: %s %s\n", token, token1); //print the next two tokens
            es = bool_expr();
            print_current_token();
            if (es > 0) return es;
            else {
                return es;
            }
        }
        else {
            fseek(fp, fileadd, 0); //reset the file position to the stored position
            printf("%s %s\n", token, token1); //print the current tokens
            es = bool_expr();
            print_current_token();
            if (es > 0) return es;
            else {
                return es;
            }
        }
    }
    else {
        es = bool_expr();
        print_current_token();
        if (es > 0) return es;
        else {
            return es;
        }
    }
}


//bool_expr -> additive_expr | additive_expr ( >|<|>=|<=|==|!= ) additive_expr
int bool_expr() {
    int es = 0;
    es = additive_expr();
    print_current_token();
    if (es > 0) return es;
    if (strcmp (token, ">") == 0 || strcmp (token, "<") == 0 || strcmp (token, ">=") == 0 || strcmp (token, "<=") == 0 || strcmp (token, "==") == 0 || strcmp (token, "!=") == 0) {
        fscanf(fp, "%s %s\n", token, token1); //read the next two tokens
        printf("Next tokens: %s %s\n", token, token1); //
        es = additive_expr();
        print_current_token();
        if (es > 0) return es;
        else {
            return es;
        }
    }
    else {
        return es;
    }

}


//additive_expr -> term + term | term - term | term
int additive_expr() {
    int es = 0;
    es = term();
    print_current_token();
    if (es > 0) return es;
    if (strcmp (token, "+") == 0 || strcmp (token, "-") == 0) {
        fscanf(fp, "%s %s\n", token, token1); //read the next two tokens
        printf("Next tokens: %s %s\n", token, token1); //
        es = term();
        print_current_token();
        if (es > 0) return es;
        else {
            return es;
        }
    }
    else {
        return es;
    }
}


//term -> factor * factor | factor / factor | factor
int term() {
    int es = 0;
    es = factor();
    print_current_token();
    if (es > 0) return es;
    if (strcmp (token, "*") == 0 || strcmp (token, "/") == 0) {
        fscanf(fp, "%s %s\n", token, token1); //read the next two tokens
        printf("Next tokens: %s %s\n", token, token1); //
        es = factor();
        print_current_token();
        if (es > 0) return es;
        else {
            return es;
        }
    }
    else {
        return es;
    }
}

//factor -> ( expression ) | ID | NUM
int factor() {
    int es = 0;
    if (strcmp (token, "(") == 0) {
        fscanf(fp, "%s %s\n", token, token1); //read the next two tokens
        printf("Next tokens: %s %s\n", token, token1); //
        es = expression();
        if (es > 0) return es;
        else {
            fscanf(fp, "%s %s\n", token, token1); //read the next two tokens
            printf("Next tokens: %s %s\n", token, token1); //print
            return es;
        }
    }
    else if (strcmp (token, "ID") == 0) {
        fscanf(fp, "%s %s\n", token, token1); //read the next two tokens
        printf("Next tokens: %s %s\n", token, token1); //
        return es;
    }
    else if (strcmp (token, "NUM") == 0) {
        fscanf(fp, "%s %s\n", token, token1); //read the next two tokens
        printf("Next tokens: %s %s\n", token, token1); //
        return es;
    }
    else {
        es = 4; // error in expression
        return es;
    }
}


//if_stat -> if ( expression ) statment | if ( expression ) statement else statement
int if_stat() {
    int es = 0;
    if (strcmp (token, "if") == 0) {
        fscanf(fp, "%s %s\n", token, token1); //read the next two tokens
        printf("Next tokens: %s %s\n", token, token1); //
        if (strcmp (token, "(") == 0) {
            fscanf(fp, "%s %s\n", token, token1); //read the next two tokens
            printf("Next tokens: %s %s\n", token, token1); //
            es = expression();
            if (es > 0) {
                return es;
            }
            else {
                if (strcmp (token, ")") == 0) {
                    fscanf(fp, "%s %s\n", token, token1); //read the next two tokens
                    printf("Next tokens: %s %s\n", token, token1); //
                    es = statement();
                    print_current_token();
                    if (es > 0) return es;
                    if (strcmp (token, "else") == 0) {
                        fscanf(fp, "%s %s\n", token, token1); //read the next two tokens
                        printf("Next tokens: %s %s\n", token, token1); //
                        es = statement();
                        print_current_token();
                        if (es > 0) return es;
                    }
                    else {
                        return es;
                    }
                }
                else {
                    es = 6; //unpaired '()'
                    return es;
                }
            }
        }
        else {
            es = 5; //no paired '()'
            return es;
        }
    }
}


//while_stat -> while ( expression ) statment
int while_stat() {
    int es = 0;
    if (strcmp (token, "while") == 0) {
        fscanf(fp, "%s %s\n", token, token1); //read the next two tokens
        printf("Next tokens: %s %s\n", token, token1); //
        if (strcmp (token, "(") == 0) {
            fscanf(fp, "%s %s\n", token, token1); //read the next two tokens
            printf("Next tokens: %s %s\n", token, token1); //
            es = expression();
            if (es > 0) return es;
            else if (strcmp (token, ")") == 0) {
                fscanf(fp, "%s %s\n", token, token1); //read the next two tokens
                printf("Next tokens: %s %s\n", token, token1); //
                es = statement();
                print_current_token();
                if (es > 0) return es;
                return es;
            }
            else {
                es = 6; //unpaired()
                return es;
            }
        }
        else {
            es = 5; //no ( after while
            return es;
        }
    }
    else {
        es = 1; //error not identified(not exist)
        return es;
    }
}


// for_stat ->for ( expression ; expression ; expression) statement
int for_stat() {
    int es = 0;
    if (strcmp(token, "for") == 0) {
        fscanf(fp, "%s %s\n", token, token1); //read the next two tokens
        printf("Next tokens: %s %s\n", token, token1); //
        if (strcmp(token, "(") == 0) {
            fscanf(fp, "%s %s\n", token, token1); //read the next two tokens
            printf("Next tokens: %s %s\n", token, token1); //
            es = expression();
            if (es > 0) return es;
            if (strcmp(token, ";") == 0) {
                fscanf(fp, "%s %s\n", token, token1); //read the next two tokens
                printf("Next tokens: %s %s\n", token, token1); //
                es = expression();
                if (es > 0) return es;
                if (strcmp(token, ";") == 0) {
                    fscanf(fp, "%s %s\n", token, token1); //read the next two tokens
                    printf("Next tokens: %s %s\n", token, token1); //
                    es = expression();
                    if (es > 0) return es;
                    if (strcmp(token, ")") == 0) {
                        fscanf(fp, "%s %s\n", token, token1); //read the next two tokens
                        printf("Next tokens: %s %s\n", token, token1); //
                        es = statement();
                        print_current_token();
                        if (es > 0) return es;
                        return es;
                    }
                    else {
                        es = 6; // unpaired ( )
                        return es;
                    }
                }
                else {
                    es = 7; //lack ; in the if statement
                    return es;
                }
            }
            else {
                es = 7; //lack ; in the if statement
                return es;
            }
        }
        else {
            es = 5; // no ( after if
            return es;
        }
    }
    else {
        es = 1; //error not identified(not exist)
        return es;
    }
}


//write_stat -> write expression ;
int write_stat() {
    int es = 0;
    if (strcmp(token, "write") == 0) {
        fscanf(fp, "%s %s\n", token, token1); //read the next two tokens
        printf("Next tokens: %s %s\n", token, token1); //
        es = expression();
        if (es > 0) return es;
        if (strcmp(token, ";") == 0) {
            fscanf(fp, "%s %s\n", token, token1); //read the next two tokens
            printf("Next tokens: %s %s\n", token, token1); //
            return es;
        }
        else {
            es = 3; //lacking ;
            return es;
        }
    }
    else {
        es = 1; //error not identified (not exist)
        return es;
    }
}


//read_stat -> read ID ;
int read_stat() {
    int es = 0;
    if (strcmp(token, "read") == 0) {
        fscanf(fp, "%s %s\n", token, token1); //read the next two tokens
        printf("Next tokens: %s %s\n", token, token1); //
        if (strcmp(token, "ID") == 0) {
            fscanf(fp, "%s %s\n", token, token1); //read the next two tokens
            printf("Next tokens: %s %s\n", token, token1); //
            if (strcmp(token, ";") == 0) {
                fscanf(fp, "%s %s\n", token, token1); //read the next two tokens
                printf("Next tokens: %s %s\n", token, token1); //
                return es;
            }
            else {
                es = 3; //lacking ;
                return es;
            }
        }
        else {
            es = 8; // confilict with any keyword
            return es;
        }
    }
    else {
        es = 1; //error not identified
        return es;
    }
}


//declaration_stat -> int ID;
int declaration_stat() {
    int es = 0;
    if (strcmp(token, "int") == 0) {
        fscanf(fp, "%s %s\n", token, token1); //read the next two tokens
        printf("Next tokens: %s %s\n", token, token1); //
        if (strcmp(token, "ID") == 0) {
            fscanf(fp, "%s %s\n", token, token1); //read the next two tokens
            printf("Next tokens: %s %s\n", token, token1); //
            if (strcmp(token, ";") == 0) {
                fscanf(fp, "%s %s\n", token, token1); //read the next two tokens
                printf("Next tokens: %s %s\n", token, token1); //
                return es;
            }
            else {
                es = 3; //lacking ;
                return es;
            }
        }
        else {
            es = 8; //keyword conflict
            return es;
        }
    }
    else {
        es = 1; //error not identified(not exist)
        return es;
    }
}


//declaration_list -> declaration_list declaration_stat | ε
int declaration_list() {
    if (strcmp(token, "int") != 0) {
        return 0; // ε
    }
    int es = 0;
    es = declaration_stat();
        print_current_token();
    if (es > 0) return es;
    else {
        es = declaration_list();
        if (es > 0) return es;
        else {
            return es;
        }
    }
}


//statement_list -> statement_list statement | ε
int statement_list() {
    if (token[0] == '\0') {
        return 2;
    }
    if (strcmp(token, "}") == 0) {
        return 0; // ε
    }
    int es = 0;
    es = statement();
    print_current_token();
    if (es > 0) return es;
    else {
        es = statement_list();
        if (es > 0) return es;
        else {
            return es;
        }
    }
    
}


int TESTparse() {
    int es = 0;
    int i = 0;
    fp = tmpfile();
    if (fp == NULL) {
        es = 10;
    } else {
        while (i < 256 && Scanout[i].type[0] != '\0') {
            fprintf(fp, "%s %s\n", Scanout[i].type, Scanout[i].value);
            ++i;
        }
        rewind(fp);
        if (!read_next_token(token, token1)) {
            es = 1;
        }
    }
    if (es == 0) {
        es = program();
    }
    printf( "========syntax analysis result========\n");
    switch(es){
        case 0: printf("parse successful!!!\n"); break;
        case 10: printf("failed to create token input stream!!!\n"); break;
        case 1: printf("syntax error!!!\n"); break;
        case 2: printf("lacking '{' or '}' in the program!!!\n" ); break;
        case 3: printf("lacking ';' at the end of the statement!!!\n"); break;
        case 4: printf("expected expression error!!!\n"); break;
        case 5: printf("no '(' after if/while/for statement!!!\n"); break;
        case 6: printf("there is no ')' paired with former '('!!!\n"); break;
        case 7: printf("lacking ';' in if statement!!!\n"); break;
        case 8: printf("keyword conflict!!!\n"); break;
        case 9: ; break;
        case 11: ; break;
        case 12: ; break;
        case 13: ; break;
        case 14: ; break;
    }
    if (fp != NULL) fclose(fp);
    return (es);
}

