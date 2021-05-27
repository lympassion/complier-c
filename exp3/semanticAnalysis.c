/*
* date: 2021/5/25 
* subject:  semantic analysis 
* descripiton: 
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <memory.h>
#include <string.h>
#include <math.h>

#include "tree.h"

#define PIDMEM 1024
#define KEYWORDNUM 256

int token;

int line = 1, words, characters;  // The number of line, words and characters.
                               // words include kerwords and identifiers
char *src, *init_src;  
int undefined;  // variable is undefined

// tokens and classes (operators last and in precedence order)
// Gvar----global variable, Lvar----local variable
enum {
  Num = 128, Fun, Sys, Gvar, Lvar, Strc,
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
    int class;  // Num = 128, Fun, Sys, Gvar, Lvar, Strc
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
int cntState;  // count construct statement stree
int idLen;  // the length of identifier
int flagOperator;  //  the flag indicate whether the operator is lost


void lexer();
void parser();
void match(int tk);
int struct_body(treeNode *TN);
void global_declare(treeNode *TN);
void function_declare(treeNode *TN);
void function_parameter();
void function_body(treeNode *TN);
int expr(treeNode* TN);
void statement(treeNode *TN);
void undef();

treeNode *TParser;  // root(in the first level)
int cntInit;  // number of second-level nodes 


int main()
{
    pid = malloc(PIDMEM);
    memset(pid, 0, PIDMEM);

    char inFile[20] = "hello.c"; 
    FILE *pFile;
    
    pFile = fopen(inFile, "rb");  
    if(pFile == NULL){
        printf("%s\n", "cannot open the file!");
        return -1;
    }
    else{
        fseek (pFile , 0 , SEEK_END);
        int size = ftell (pFile) + KEYWORDNUM;
        rewind (pFile);

        src = malloc(size);
        memset(src, 0, size);
        init_src = src;
        if(src == NULL){
            printf("Fail to allocate the memory!");
            return -1;
        }
        else{
            memcpy(src, "char else struct if int return while", KEYWORDNUM);
            // add keywords to symbol table
            int i = Char;
            while (i <= While) {
                lexer();
                pid_curr[id_ind].token = i++;
            }
            
            // when hello.c contines only a few code, fread() will read
            // not only hello.c but also some last charactes of the printf 
            // next, and I don't know the reason now
            if((fread(src, 1, size - KEYWORDNUM, pFile)) <= 0){
                // cause in previous keywords' operation, src++, so 
                // the previous memory will not be covered.
                printf("read failed!\n");
                exit(-1);
            }
            parser();
            printf("begin to print parse tree\n");
            print(TParser, 0);
            printf("\nend\n");
            fclose(pFile);
            printf("总行数:%d\t总的字符数:%d\t标识符与关键字总个数:%d\n", line, characters, words);
        }
    }

    // attention here you cannot free(src) directly,
    // cause src++ till src point to the source code end.
    free(init_src); 
    free(pid);
    

    return 0;
}

void parser(){
    lexer();  // call the lexer and get token
    TParser = malloc(sizeof(treeNode));
    construct(TParser, "begin");
    while(token != NULL){
        global_declare(TParser);
    }
}

void match(int tk){  
    if(token == tk){
        lexer();
    }
    else{
        flag = 1;
        printf("第%d行： 出错: 期望出现字符(%d):\'%c\'\n", line, tk, (char)tk);
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
            idLen = src-lastPos;
            // find the existing identifier if we need to store a new identifier
            pid_curr = pid;
            id_ind = 0;
            while(pid_curr[id_ind].token != 0){

                // we cannot use the methord below to judge if the variable is
                // deplicate, or it will simply think using variable as the
                // repeatation of variable's declare.
                // if(pid_curr[id_ind].token == Id && pid_curr[id_ind].hash == hash){
                //     printf("第%d行: 变量%s重复定义\n", line, pid_curr[id_ind].id);
                // }
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
            
            memcpy(pid_curr[id_ind].id, lastPos, idLen); 
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

treeNode *var_type(treeNode *TN, int ind, char *type){
    treeNode *TVarDeclare = malloc(sizeof(treeNode));
    construct(TVarDeclare, "<variable declare>");
    TN->child[ind] = TVarDeclare;

    treeNode *TType = malloc(sizeof(treeNode));
    construct(TType, type);
    TVarDeclare->child[0] = TType;
    return TVarDeclare;
}

void id(treeNode *TN, int indIdDecalre){
    treeNode *TIdDecalre = malloc(sizeof(treeNode));
    construct(TIdDecalre, "<id>");
    TN->child[indIdDecalre] = TIdDecalre;

    treeNode *TId = malloc(sizeof(treeNode));
    construct(TId, pid_curr[id_ind].id);
    TIdDecalre->child[0] = TId;
}

treeNode *punction(treeNode *TN, int ind, char *punc){
    treeNode *TVarDeclare = malloc(sizeof(treeNode));
    construct(TVarDeclare, punc);
    TN->child[ind] = TVarDeclare;
    return TVarDeclare;
}

void global_declare(treeNode *TN) {
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
    while(token == Struct){
        treeNode *TStructDeclare = malloc(sizeof(treeNode));
        construct(TStructDeclare, "<declare struct>");
        // treeNode *T = malloc(sizeof(treeNode));
        // T->child[cntTN] = TStructDeclare;
        TN->child[cntInit] = TStructDeclare;
        cntInit++;
        match(Struct);
        
        if(token == Id){
            id(TStructDeclare, 0);
            if(pid_curr[id_ind].class == Strc){
                printf("第%d行: 结构体%s重复定义\n", line, pid_curr[id_ind].id);
                // exit(-1);
            }
            pid_curr[id_ind].class = Strc;
            match(Id);

            if(token != '{'){
                flag = 1;
            }
            else{
                treeNode *TOpenBrace = punction(TStructDeclare, 1, "{");
                TStructDeclare->child[1] = TOpenBrace;
                match('{');

                treeNode *TStructBody = malloc(sizeof(treeNode));
                construct(TStructBody, "<struct body>");
                TStructDeclare->child[2] = TStructBody;
                int cnt = struct_body(TStructBody);

                // treeNode *TCloseBrace = malloc(sizeof(treeNode));
                // construct(TCloseBrace, "{");
                // TStructDeclare->child[3] = TCloseBrace;
                // punction(TStructDeclare, cnt, "}");
                punction(TStructDeclare, 3, "}");
                cnt++;
                match('}');
                punction(TStructDeclare, cnt, ";");
                match(';');
                treeNode *TStructDeclareEnd = malloc(sizeof(treeNode));
                construct(TStructDeclareEnd, "<struct's declaration completed>");
                TN->child[cntInit] = TStructDeclareEnd;
                cntInit++;
                
            }
        }
        
        if(flag){
            printf("Structure declaration error\n");
        }
        // print(TParser, 0);
    }

    // There are only three possible types of variables
    // they are INT, CHAR, PTR
    // int [*]id [; | (...) {...}]
    // parse type information
    if (token == Int) {
        punction(TN, cntInit, pid_curr[id_ind].id);
        cntInit++;
        match(Int);
        type = INT;
    }
    else if (token == Char) {
        punction(TN, cntInit, pid_curr[id_ind].id);
        cntInit++;
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
            printf("line%d: 变量定义出错\n", line);
            // while(token != ';'){
            //     lexer();
            // }
            // exit(-1);
        }
        // if (pid_curr[id_ind].hash && pid_curr[id_ind].token > While) {  // identifier exists
        //     printf("id_ind:%d\n", id_ind);
        //     printf("%d: duplicate global declaration\n", line); // TODO
        //     exit(-1);
        // }
        id(TN, cntInit);
        cntInit++;
        match(Id);
        pid_curr[id_ind].type = type;

        if (token == '(') {
            if(pid_curr[id_ind].class == Fun){
                printf("第%d行: 函数%s重复定义\n", line, pid_curr[id_ind].id);
                // exit(-1);
            }
            pid_curr[id_ind].class = Fun;

            treeNode *TOpenParen = punction(TN, cntInit, "(");
            cntInit++;
            treeNode *TFunDeclare = malloc(sizeof(treeNode));
            construct(TFunDeclare, "<declare function>");
            TN->child[cntInit] = TFunDeclare;
            cntInit++;
            function_declare(TFunDeclare);
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

int struct_body(treeNode *TN) {
    // struct [id]{int a, int b}
    int cntTN = 0;
    treeNode *varID;
    while(token != '}'){
        if (token == Int || token == Char) {
            varID = var_type(TN, cntTN, pid_curr[id_ind].id);
            cntTN++;
            lexer();

            if(token == Id){
                id(TN, cntTN);
                cntTN++;
                lexer();
                if(token == ',' || token == Brak){
                    if(token == ','){
                        varID = punction(TN, cntTN, ",");
                        cntTN++;
                    }
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
            varID = punction(TN, cntTN, ";");
            cntTN++;
            lexer();
        }
        else{
            flag = 1;
            return;
        }
    }
    return cntTN;
}

void function_declare(treeNode *TN) {
    // type func_name (...) {...}
    //               | this part
    match('(');
    function_parameter();
    match(')');
    match('{');
    punction(TN, 0, "{");

    treeNode *TFunBody = malloc(sizeof(treeNode));
    construct(TFunBody, "<function body>");
    TN->child[1] = TFunBody;
    function_body(TFunBody);
    punction(TN, 2, "}");
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

void function_body(treeNode *TN) {
    // type func_name (...) {...}
    //                   -->|   |<--
    // 1. local declarations
    // 2. statements
    // attention we may cannot declare a variable and assign it together
    // Instead, our source code may look like this: int a; a = 5;

    int cntTN = 0;
    while (token == Int || token == Char) {
        // local variable declaration, just like global ones.
        type = (token == Int) ? INT : CHAR;  
        var_type(TN, cntTN, pid_curr[id_ind].id);
        cntTN++;
        match(token);

        while (token != ';') {
            while (token == Mul) {
                punction(TN, cntTN, ";");
                cntTN++;
                match(Mul);
                type = type<2 ? PTR : type+1;
            }
            if (token == ',' || token == Brak) {
                if(token == ','){
                    punction(TN, cntTN, ";");
                    cntTN++;
                }
                lexer();
            }
            if(token == ';'){
                punction(TN, cntTN, ";");
                cntTN++;
                break;
            }
            if (token != Id) {
                // invalid declaration
                printf("第%d行: 定义变量出错（可能原因：函数嵌套定义）\n", line);
                while(token != '}'){
                   lexer(); 
                }
                lexer();
                break;
                // exit(-1);
            }
            if (pid_curr[id_ind].class == Lvar) {
                // identifier exists
                printf("第%d行: 变量%s重复定义\n", line, pid_curr[id_ind].id);
                // exit(-1);
            }
            id(TN, cntTN);
            cntTN++;
            match(Id);
            // print(TParser, 0);
            pid_curr[id_ind].class = Lvar;
            pid_curr[id_ind].type = type;

            
        }
        punction(TN, cntTN, ";");
        cntTN++;
        match(';');
    }

    // statements
    while (token != '}') {
        // wtf, here I make a fatal mistake, and the most terrible thing
        // is the terminal suggests that not this malloc wrong but next
        // so it's time-consuming
        // treeNode *TState = malloc(sizeof(TState));  
        treeNode *TState = malloc(sizeof(treeNode));
        construct(TState, "<statement>");
        TN->child[cntTN] = TState;
        cntTN++;
        statement(TState);
    }
}

int factor(treeNode *TN) {
    int value = 0;
    if (token == '(') {
        treeNode *TOpenParen = malloc(sizeof(treeNode));
        construct(TOpenParen, "(");
        TN->child[0] = TOpenParen;
        match('(');

        treeNode *TExpr = malloc(sizeof(treeNode));
        construct(TExpr, "<expression>");
        TN->child[1] = TExpr;
        value = expr(TExpr);
        if(!flagOperator){
            printf("第%d行：丢失运算符\n", line);
        }

        if(token != ')'){
            printf("第%d行：丢失右括号\n", line);
        }
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
    else if(token == Num){
        itoa(val, str, 10);
        treeNode *TNum = malloc(sizeof(treeNode));
        construct(TNum, str);
        TN->child[0] = TNum;
        match(Num);
    }
    else{
        if(pid_curr[id_ind].type == CHAR){
            printf("第%d行：仅整形变量才可以参与算术运算\n", line);
        }
        else{
            printf("第%d行：丢失操作数\n", line);
        }
    }
    return value;
}

int term_tail(int lvalue, treeNode *TN) {
    if (token == Mul) {
        flagOperator = 1;
        treeNode *TMul = malloc(sizeof(treeNode));
        construct(TMul, "*");
        TN->child[0] = TMul;
        match(token);
        
        treeNode *TFac = malloc(sizeof(treeNode));
        construct(TFac, "<factor>");
        TN->child[1] = TFac;
        int value = lvalue * factor(TFac);

        treeNode *TTermTail = malloc(sizeof(treeNode));
        construct(TTermTail, "<term_tail>");
        TN->child[2] = TTermTail;
        return term_tail(value, TTermTail);
    } 
    else if (token == Div) {
        flagOperator = 1;
        treeNode *TDiv = malloc(sizeof(treeNode));
        construct(TDiv, "/");
        TN->child[0] = TDiv;
        match(token);

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
        // printf("第%d行：丢失操作数\n", line);
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
    if (token == Add) {
        flagOperator = 1;
        treeNode *TAdd = malloc(sizeof(treeNode));
        construct(TAdd, "+");
        TN->child[0] = TAdd;
        match(token);

        treeNode *TTerm = malloc(sizeof(treeNode));
        construct(TTerm, "<term>");
        TN->child[1] = TTerm;
        int value = lvalue + term(TTerm);

        treeNode *TExprTail = malloc(sizeof(treeNode));
        construct(TExprTail, "<expr_tail>");
        TN->child[2] = TExprTail;
        return expr_tail(value, TExprTail);
    } 
    else if (token == Sub) {
        flagOperator = 1;
        treeNode *TSub = malloc(sizeof(treeNode));
        construct(TSub, "-");
        TN->child[0] = TSub;
        match(token);

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
        // printf("第%d行：丢失操作数(+ -)\n", line);
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

    flagOperator = 0;
    if (token == Id) {
        
        if (pid_curr[id_ind].class == Fun) {  // function call
            treeNode *TFun = malloc(sizeof(treeNode));
            construct(TFun, "<function call>");
            TN->child[0] = TFun;
            match(Id);
            if (token == '(') {
                // function call
                printf("第%d行：调用函数%s()\n", line, pid_curr[id_ind].id);  // TODO
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
            // exit(-1);
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
    else if(token == '('){
        // like (1 + 2)
        treeNode *TFac = malloc(sizeof(treeNode));
        construct(TFac, "<factor>");
        TN->child[0] = TFac;
        int lvalue = factor(TFac);
    }
    else{
        printf("line:%d error: bad expression\n", line);
    }
    
}

void statement(treeNode *TN) {
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
        statement(TN);         // parse statement
        if (token == Else) { // parse else
            match(Else);
            statement(TN);
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
            statement(TN);
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
            // memcpy(str, pid_curr[id_ind].id, sizeof(pid_curr[id_ind].id));
            match(token);
            if(token == Assign){
                lexer();
                treeNode *TExpr = malloc(sizeof(treeNode));
                construct(TExpr, "<expression>");
                TN->child[cntState] = TExpr;
                cntState++;
                // printf("\nexpression\n");
                // print(TParser, 0);
                expr(TExpr);
                if(!flagOperator){
                    printf("第%d行：丢失运算符\n", line);
                }
            }
            else if(token >= LOr && token <= Dec){  // TODO
                lexer();
            }
            else{
                statement(TN);
            }
        }
        else{
            // printf("第%d行：缺一个标识符\n", line);
            lexer();
        }
    }
}