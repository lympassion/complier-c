/*
* date: 2021/5/12 
* subject:  syntantic analysis 
* descripiton: 
*    1. 语法分析器，采用递归下降的方式（容易实现，方便手工编码）
*    2. 需要消除左递归
*    3. 将exp1的词法分析部分进行改进
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <memory.h>
#include <string.h>
#include <math.h>

#include "tree.h"

#define PIDMEM 1024
#define KEYWORDMEM 256

int token;

int line = 1, words, characters;  // The number of line, words and characters.
                               // words include kerwords and identifiers
char *src, *init_src;  
int undefined;  // variable is undefined

// tokens and classes (operators last and in precedence order)
// Gvar----global variable, Lvar----local variable
enum {
  Num = 128, Fun, Sys, Gvar, Lvar, 
  Id, Char, Else, Struct, If, Int, Return, While,
  Assign, LOr, LAnd, Or, Xor, And, Eq, Ne, Not, LNot, 
  Lt, Gt, LOrE, GOrE, ShL, ShR, Add, Sub, Mul, Div, Mod, Inc, Dec, Brak
};
int operator;

// types of variable/function
enum { CHAR, INT, PTR };
int type;  // variable type

// tell the different identifiers:
struct Identifier{
    int class;  // Num = 128, Fun, Sys, Gvar, Lvar
    int token;  // Id, Char, Else, Struct, If, Int, Return, While,
    int type;  // CHAR INT PTR
    int hash;
    char id[20];
};
typedef struct Identifier Iden;
Iden *pid, *pid_curr;
int id_ind;


int flag;  // just use to judge bool
char str[100];  // to store string
int val;  // if token==Num, this is its value


void lexer();
void compiler();
void match(int tk);
void struct_body();
void global_declare();
void function_declare();
void function_parameter();
void function_body();
int expr(treeNode* TN);
void statement();
void undef();

treeNode *TState;  // root

int main()
{
    pid = malloc(PIDMEM);
    memset(pid, 0, PIDMEM);

    // attention, I use the vscode for coding, and the path of the inFile is very confusing.
    // So I decide to create a file named hello.c in the root directory, which is in the 
    // same directory with exp1. And sometimes it needs you to open under exp2 directory.
    // so it's very confusing.
    char inFile[20] = "tHello.c"; 
    // char inFile[20] = "hello.c"; 
    FILE *pFile;
    
    pFile = fopen(inFile, "rb");  
    if(pFile == NULL){
        printf("%s\n", "cannot open the file!");
        return -1;
    }
    else{
        fseek (pFile , 0 , SEEK_END);
        int size = ftell (pFile) + KEYWORDMEM;
        rewind (pFile);

        src = malloc(size);
        init_src = src;
        if(src == NULL){
            printf("Fail to allocate the memory!");
            return -1;
        }
        else{
            memcpy(src, "char else struct if int return while", KEYWORDMEM);
            // add keywords to symbol table
            int i = Char;
            while (i <= While) {
                lexer();
                pid_curr[id_ind].token = i++;
            }
            for(int i = 0; i <= id_ind; i++){
                printf("hash:%d ", pid[i].hash);
            }
            printf("\n");
            
            if((fread(src, 1, size, pFile)) <= 0){
                // cause in previous keywords' operation, src++, so 
                // the previous memory will not be covered.
                printf("read failed!\n");
                exit(-1);
            }
            compiler();
            print(TState, 0);
            fclose(pFile);
            printf("总行数:%d\t总的字符数:%d\t标识符与关键字总个数:%d\n", line, characters, words);
        }
    }

    // attention here you cannot free(src) directly,
    // cause src++ till src point to the source code end.
    printf("hash:%d\n", pid[2].hash);
    free(init_src); 
    free(pid);
    printf("end\n");
    printf("src:%d  pid%d, \n", init_src, pid[2].hash);
    return 0;
}

void compiler(){
    lexer();  // call the lexer and get token
    while(token != NULL){
        global_declare();
    }
}

void match(int tk){  
    if(token == tk){
        lexer();
    }
    else{
        flag = 1;
        printf("line:%d error: expected token(%d):\'%c\'\n", line, tk, (char)tk);
        return;
    }
}

void undef(){
    if(undefined){
        printf("第%d行，存在词法错误,有关变量未定义\n", line);
    }
}


/*************** lexical analysis ***************/
void lexer(){
    // this funciton is to get token, and will end(return) 
    // once it get a token
    int lastPos;
    
    int symbol;
    while (token = *src)
    {
        undefined = 1;  // indiciate lexical errors exits
        memset(str, 0, sizeof(str));
        lastPos = src;
        src++;
        if(token == '\n'){
            undefined = 0;
            line++;
        }
        // else if(token == '\r'){
        //     continue;
        // }
        else if(token == ' ' || token == '\r' || token == ']'){ 
            // cause in windows, when you enter "enter"
            // there will be "\r\n"
            continue;
            // undefined = 0;
        }

        // begin to analyse
        symbol = token == ';' || token == '{' || token == '}' || 
                token == '(' || token == ')' || token == ',' || 
                token == ':';

        if(token == '#'){
            // skip macro
            undefined = 0;
            while (*src != '\n') {
                src++;
            }
        }
        else if(isalpha(token) || token == '_'){
            undefined = 0; 
            // parse identifier
            int hash = token;
            words++;

            while (isalnum(*src) || *src == '_')
            {
                hash = hash * 147 + *src;
                src++;
            }

            // find the existing identifier if we need to store a new identifier
            pid_curr = pid;
            id_ind = 0;
            while(pid_curr[id_ind].token != 0){
                if(pid_curr[id_ind].hash == hash){
                    // token = Id;  // cause for the preserved kerwords their token is not Id
                    token = pid_curr[id_ind].token;
                    words--;
                    return;
                }
                id_ind++;
            }
            pid_curr[id_ind].token = Id;
            pid_curr[id_ind].hash = hash;
            memcpy(pid_curr[id_ind].id, lastPos, src-lastPos); 
            token = Id;

            // id_ind++; fatal error
            return;
        }

        else if(isdigit(token)){  // consider positive number
            undefined = 0;

            // parse number, include dec, oct(045), hex(0x45)
            int float_num = 0;
            val = 0;
            if(token != '0'){  // dec
                val = token - '0';
                
                if(*src == '.'){
                    // here, we are able to recognize floating point numbers in exponential form
                    // and the form must be standard.
                    double frac = 0.1, val_float; 
                    double base, exp;
                    
                    float_num = 1;
                    base = token-'0';
                    src++;
                    while (*src != 'e' && *src != 'E')
                    {
                        base += (*src - '0') * frac;
                        frac /= 10;
                        src++;
                    }
                    src++;
                    if(*src == '-'){
                        src++;
                        exp = -(*src++ - '0');
                        while (isdigit(*src))
                        {
                            exp = exp * 10 - (*src - '0'); 
                            src++;
                        }
                    }
                    else{
                        exp = *src++ - '0';
                        while (isdigit(*src))
                        {
                            exp = exp * 10 + (*src - '0'); 
                            src++;
                        }
                    }
                    val_float = base * pow(10, exp);
                }
                else{  // decimal integer
                    while (isdigit(*src))
                    {
                        val = val * 10 + *src - '0'; 
                        src++;
                    }
                }
            }
            else if(*src == 'x'){  // hex
                // 'A'\t <---> 0x41
                // 'a'\t <---> 0x61
                val = (*(++src) & 15) + (*src >= 'A' ? 9 : 0);
                src++;
                while (isdigit(*src) || ((*src)>='A' && (*src)<='F') || ((*src)>='a' && (*src)<='f'))
                {
                    val = val*16 + (*(src) & 15) + (*src >= 'A' ? 9 : 0);
                    src++;
                }
            }
            else{  // oct
                val = token - '0';
                while (isdigit(*src))
                {
                    val = val * 8 + *src - '0'; 
                    src++;
                }
            }
            token = Num;
            return;
        }

        else if(token == '"' || token == '\''){
            undefined = 0;
            // parse string literal or character
            while (*src != token)
            {
                src++;
            }
            src++;
            if(token == '\''){
                token = Char;
            }
            return;
        }
        else if (symbol) {
            undefined = 0;
            return;
        }
        
        // arithmetic operators
        switch(token){
        case('+'): 
            undefined = 0;
            if(*src == '+'){
                src++;
                token = Inc;
            }
            else{
                token = Add;
            }
            return;
        case('-'):
            undefined = 0;
            if(*src == '-'){
                src++;
                token = Dec;
            }
            else{
                token = Sub;
            }
            return;
        case('*'):
            undefined = 0;
            token = Mul;
            return;
        case('/'):
            undefined = 0;
            if(*src == '/'){  // comment(this lexer only thinks "//" as comment symbol)
                while (*src != '\n')
                {
                    src++;
                }
            }
            else{
                token = Div;
                return;
            }
            break;
        case('%'):
            undefined = 0;
            token = Mod;
            return;
        default:
            break;
        }

        // recognize array
        int cap = 0;  // capacity
        if(token == '['){
            cap = *src++ - '0';
            while (isdigit(*src))
            {
                cap = cap * 10 + *src - '0'; 
                src++;
            }
            if(*src == ']'){
                undefined = 0;
                // src++;
                token = Brak;
                return;
                // printf("第%d行，定义了数组，容量为%d", line, cap);
            }
        }

        // relational operators
        switch(token){
        case('='):
            undefined = 0;
            if(*src == '='){
                src++;
                token = Eq;
            }
            else{
                token = Assign;
            }
            return;
        case('!'):
            undefined = 0;
            if(*src == '='){
                src++;
                token = Ne;
            }
            else{ // logical 
                token = LNot;
            }
            return;
        case('>'):
            undefined = 0;
            if(*src == '='){
                src++;
                token = GOrE;
            }
            else if(*src == '>'){  // bit 
                src++;
                token = ShR;
            }
            else{ 
                token = Gt;
            }
            return;
        case('<'):
            undefined = 0;
            if(*src == '='){
                src++;
                token = LOrE;
            }
            else if(*src == '<'){  // bit 
                src++;
                token = ShL;
            }
            else{ // logic
                token = Lt;
            }
            return;
        default:
            break;
        }

        // logical operators and bit operators
        switch(token){
        case('&'):
            undefined = 0;
            if(*src == '&'){
                src++;
                token = LAnd;
            }
            else{ // logical 
                token = And;
            }
            return;
        case('|'):
            undefined = 0;
            if(*src == '|'){
                src++;
                token = LOr;
            }
            else{ // logical 
                token = Or;
            }
            return;
        case('^'):
            undefined = 0;
            token = Xor;
            return;
        case('~'):
            undefined = 0;
            token = Not;
            return;
        default:
            break;
        }
        undef();
        characters += (int)src - lastPos;
    }
    
    return;
}


/*************** syntactic analysis ***************/
void global_declare() {
    flag = 0;

    // parse struct
    // attention, we can use the below way to judge
    // cause in main function, we have this mothord to fill
    // the pid
    // i = Char;
    // while (i <= While) {
    //     next();
    //     current_id[Token] = i++;
    // }
    if(token == Struct){
        match(Struct);
        if(token == Id){
            match(Id);
        }

        if(token != '{'){
            flag = 1;
        }
        else{
            match('{');
            struct_body();
            match('}');
            match(';');
        }
        if(flag){
            printf("Structure declaration error\n");
        }
    }
    // There are only three possible types of variables
    // they are INT, CHAR, PTR
    // int [*]id [; | (...) {...}]
    // parse type information
    if (token == Int) {
        match(Int);
        type = INT;
    }
    else if (token == Char) {
        match(Char);
        type = CHAR;
    }

    // parse the comma seperated variable declaration.
    while (token != ';' && token != '}') {
        // parse pointer type, note that there may exist `int ****x;`
        while (token == Mul) {
            match(Mul);
            type = type<2 ? PTR : type+1;
        }

        if (token != Id) {
            printf("%d: something wrong with variable declaration\n", line);
            exit(-1);
        }
        if (pid_curr[id_ind].hash && id_ind > While) {  // identifier exists
            printf("id_ind:%d\n", id_ind);
            printf("%d: duplicate global declaration\n", line); // TODO
            exit(-1);
        }
        match(Id);
        pid_curr[id_ind].type = type;

        if (token == '(') {
            pid_curr[id_ind].class = Fun;
            function_declare();
        } else {
            // variable declaration
            pid_curr[id_ind].class = Gvar; // global variable
        }

        if (token == ',') {
            match(',');
        }
    }
    lexer();
}

void struct_body() {
    // struct [id]{int a, int b}
    while(token != '}'){
        if (token == Int || token == Char) {
            lexer();
            if(token == Id){
                lexer();
                if(token == ',' || token == Brak){
                    lexer();
                }
            }
            else{
                flag = 1;
                return;
            }
        }
        else if(token == ';'){
            // because the loop condition is '}', so if
            // ';' exists in struct, use lexer() to reach next token(not ' ')
            lexer();
        }
        else{
            flag = 1;
            return;
        }
    }
}

void function_declare() {
    // type func_name (...) {...}
    //               | this part

    match('(');
    function_parameter();
    match(')');
    match('{');
    function_body();
    //match('}');
}

void function_parameter() {
    int type;
    int params;
    params = 0;
    while (token != ')') {
        // int name, ...
        type = INT;
        if (token == Int) {
            match(Int);
        } else if (token == Char) {
            type = CHAR;
            match(Char);
        }

        // pointer type
        while (token == Mul) {
            match(Mul);
            type = type<2 ? PTR : type+1;
        }

        // parameter name
        if (token != Id) {
            printf("%d: error: parameter name omitted\n", line);
            exit(-1);
        }
        match(Id);

        if (token == ',') {
            match(',');
        }
    }
}

void function_body() {
    // type func_name (...) {...}
    //                   -->|   |<--
    // 1. local declarations
    // 2. statements
    // attention we may cannot declare a variable and assign it together
    // Instead, our source code may look like this: int a; a = 5;
    while (token == Int || token == Char) {
        // local variable declaration, just like global ones.
        type = (token == Int) ? INT : CHAR;  // TODO
        match(token);

        while (token != ';') {
            while (token == Mul) {
                match(Mul);
                type = type<2 ? PTR : type+1;
            }
            if (token == ',' || token == Brak) {
                lexer();
            }
            if(token == ';'){
                break;
            }
            if (token != Id) {
                // invalid declaration
                printf("%d: error: wrong variable declaration\n", line);
                exit(-1);
            }
            if (pid_curr[id_ind].class == Lvar) {
                // identifier exists
                printf("%d: duplicate local declaration\n", line);
                exit(-1);
            }
            match(Id);
            pid_curr[id_ind].class = Lvar;
            pid_curr[id_ind].type = type;

            
        }
        match(';');
    }

    // statements
    while (token != '}') {
        statement();
    }
}

int factor(treeNode *TN) {
    int value = 0;
    if (token == '(') {
        treeNode *TLeftPare = malloc(sizeof(treeNode));
        construct(TLeftPare, "(");
        TN->child[0] = TLeftPare;
        match('(');

        treeNode *TExpr = malloc(sizeof(treeNode));
        construct(TExpr, "<expression>");
        TN->child[1] = TExpr;
        value = expr(TExpr);

        treeNode *TRightPare = malloc(sizeof(treeNode));
        construct(TRightPare, ")");
        TN->child[2] = TRightPare;
        match(')');
    } 
    else if(token == Id && pid_curr[id_ind].type == INT){
        treeNode *TId = malloc(sizeof(treeNode));
        construct(TId, pid_curr[id_ind].id);
        TN->child[0] = TId;
        match(Id);
    }
    else {
        itoa(val, str, 10);
        treeNode *TNum = malloc(sizeof(treeNode));
        construct(TNum, str);
        TN->child[0] = TNum;
        match(Num);
    }
    return value;
}

int term_tail(int lvalue, treeNode *TN) {
    if (token == '*') {
        treeNode *TMul = malloc(sizeof(treeNode));
        construct(TMul, "*");
        TN->child[0] = TMul;
        match('*');
        
        treeNode *TFac = malloc(sizeof(treeNode));
        construct(TFac, "<factor>");
        TN->child[1] = TFac;
        int value = lvalue * factor(TFac);

        treeNode *TTermTail = malloc(sizeof(treeNode));
        construct(TTermTail, "<term_tail>");
        TN->child[2] = TTermTail;
        return term_tail(value, TTermTail);
    } 
    else if (token == '/') {
        treeNode *TDiv = malloc(sizeof(treeNode));
        construct(TDiv, "/");
        TN->child[0] = TDiv;
        match('/');

        treeNode *TFac = malloc(sizeof(treeNode));
        construct(TFac, "<factor>");
        TN->child[1] = TFac;
        int value = lvalue / factor(TFac);

        treeNode *TTermTail = malloc(sizeof(treeNode));
        construct(TTermTail, "<term_tail>");
        TN->child[2] = TTermTail;
        return term_tail(value, TTermTail);
    } 
    else {
        return lvalue;
    }
}

int term(treeNode *TN) {
    treeNode *TFac = malloc(sizeof(treeNode));
    construct(TFac, "<factor>");
    TN->child[0] = TFac;
    int lvalue = factor(TFac);

    treeNode *TTermTail = malloc(sizeof(treeNode));
    construct(TTermTail, "<term_tail>");
    TN->child[1] = TTermTail;
    return term_tail(lvalue, TTermTail);
}

int expr_tail(int lvalue, treeNode *TN) {
    if (token == '+') {
        treeNode *TAdd = malloc(sizeof(treeNode));
        construct(TAdd, "+");
        TN->child[0] = TAdd;
        match('+');

        treeNode *TTerm = malloc(sizeof(treeNode));
        construct(TTerm, "<term>");
        TN->child[1] = TTerm;
        int value = lvalue + term(TTerm);

        treeNode *TExprTail = malloc(sizeof(treeNode));
        construct(TExprTail, "<expr_tail>");
        TN->child[2] = TExprTail;
        return expr_tail(value, TExprTail);
    } 
    else if (token == '-') {
        treeNode *TSub = malloc(sizeof(treeNode));
        construct(TSub, "-");
        TN->child[0] = TSub;
        match('-');

        treeNode *TTerm = malloc(sizeof(treeNode));
        construct(TTerm, "<term>");
        TN->child[1] = TTerm;
        int value = lvalue - term(TTerm);

        treeNode *TExprTail = malloc(sizeof(treeNode));
        construct(TExprTail, "<expr_tail>");
        TN->child[2] = TExprTail;
        return expr_tail(value, TExprTail);
    } 
    else {
        return lvalue;
    }
}

int expr(treeNode* TN) {
    // Eliminate left recursion
    // recursion down
    // 1. identify function call
    // 2. arthmetic

    // attention, here we just care right part of the expression
    // and left part will be handled before enter this function

    if (token == Id) {
        
        if (pid_curr[id_ind].class == Fun) {  // function call
            treeNode *TFun = malloc(sizeof(treeNode));
            construct(TFun, "function call");
            TN->child[0] = TFun;
            match(Id);
            if (token == '(') {
                // function call
                printf("line: %d, call function %s()\n", line, pid_curr[id_ind].id);
                match('(');
                while (token != ')') {
                    lexer();
                }
                match(')');
            }
            else {
                printf("%d: bad function call\n", line);
                exit(-1);
            }
        }
        // variable
        else if (pid_curr[id_ind].class == Lvar || pid_curr[id_ind].class == Gvar) {
            // like a + b * 2
            treeNode *TTerm = malloc(sizeof(treeNode));
            construct(TTerm, "<term>");
            TN->child[0] = TTerm;
            int lvalue = term(TTerm);
            
            treeNode *TExprTail = malloc(sizeof(treeNode));
            construct(TExprTail, "<expr_tail>");
            TN->child[1] = TExprTail;
            return expr_tail(lvalue, TExprTail);
        }
        else {
            printf("%d: undefined variable\n", line);
            exit(-1);
        }
    }
    else if(token == Num){
        // like 1 + 2
        treeNode *TTerm = malloc(sizeof(treeNode));
        construct(TTerm, "<term>");
        TN->child[0] = TTerm;
        int lvalue = term(TTerm);
        
        treeNode *TExprTail = malloc(sizeof(treeNode));
        construct(TExprTail, "<expr_tail>");
        TN->child[1] = TExprTail;
        return expr_tail(lvalue, TExprTail);
    }
    else{
        printf("line:%d error: bad expression", line);
        exit(-1);
    }
}

void statement() {
    undef();
    // there are 8 kinds of statements here:
    // 1. if (...) <statement> [else <statement>]
    // 2. while (...) <statement>
    // 3. { <statement> }
    // 4. return xxx;
    // 5. <empty statement>;
    // 6. expression; (expression end with semicolon)

    int *a, *b; // bess for branch control

    if (token == If) {
        match(If);
        match('(');
        while(token != ')'){
            match(token);
        }
        match(')');
        statement();         // parse statement
        if (token == Else) { // parse else
            match(Else);
            statement();
        }
    }
    else if (token == While) {
        match(While);
        match('(');
        while(token != ')'){
            match(token);
        }
        match(')');
    }
    else if (token == '{') {
        // { <statement> ... }
        match('{');
        while (token != '}') {
            statement();
        }
        match('}');
    }
    else if (token == Return) {
        // return [expression];
        match(Return);

        if (token != ';') {
            match(token);
        }

        match(';');
    }
    else if (token == ';') {
        // empty statement
        match(';');
    }
    else {
        // function call or arthmetic like a = 1 + 2;

        if(token == Id){
            memcpy(str, pid_curr[id_ind].id, sizeof(pid_curr[id_ind].id));
            match(token);
            if(token == Assign){
                lexer();
                TState = malloc(sizeof(treeNode));
                construct(TState, "<statement>");
                treeNode *TExpr = malloc(sizeof(treeNode));
                construct(TExpr, "<expression>");
                TState->child[0] = TExpr;
                expr(TExpr);
            }
            else if(token >= LOr && token <= Dec){  // TODO
                lexer();
            }
            else{
                statement();
            }
        }
        else{
            printf("line:%d error: here is lack of an identifier\n", line);
            exit(-1);
        }
    }
}