/*
* date: 2021/4/17 
* subject:  lexical analysis 
* descripiton: 
*    1. 词法分析器的作用：对于源码字符串作预处理，以降低语法分析器的复杂程度。
*    2. 词法分析器以源码字符串作为输入，输出标记流（token stream），输出格式为(token, value)。
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <memory.h>
#include <string.h>
#include <math.h>

#define KEYWORDNUM 12

int token;

char keywords[][10] = {
    "struct", "int", "main", "char", "bool", "double",
    "void", "if", "else", "while", "return", "for", 
};


int lines, words, characters;  // The number of lines, words and characters.
                               // words include kerwords and identifiers
void lexer(char *src);

int main(){
    char inFile[20] = "hello.c"; 
    char *src = malloc(500);  // every line of source code 
    FILE *pFile;
    int flag;

    // open file
    // printf("please enter the file's name:\n");
    // scanf("%s", inFile);

    // read the source file
    pFile = fopen(inFile, "r");  
    if(pFile == NULL){
        printf("%s\n", "cannot open the file!");
        return -1;
    }
    else{
        if(src == NULL){
            printf("Fail to allocate the mem!");
            return -1;
        }
        else{
            while (!(feof(pFile)))
            {
                memset(src, 0, sizeof(src));
                fgets(src, 499, pFile);
                lexer(src);  // every time analyze one line
            }

            printf("总行数:%d\t总的字符数:%d\t标识符与关键字总个数:%d\n", lines, characters, words);
        }
    }
    return 0;
}

void lexer(char *src){
    int lastPos;
    char str[100];
    char token_debug;
    int symbol;
    int undifined = 1;  // indiciate lexical errors exits
    lines++;
    while (*src != '\n' && *src != 0)
    {
        
        memset(str, 0, sizeof(str));
        lastPos = src;
        token = *src;
        token_debug = (char)token;
        src++;
        if(token == ' '){
            continue;
        }

        // begin to analyse
        // others symbols
        symbol = token == ';' || token == '{' || token == '}' || 
                token == '(' || token == ')' || token == ',' || 
                token == ':';

        if(token == '#'){
            // skip macro
            undifined = 0;
            while (*src != '\n') {
                src++;
            }
        }
        else if(isalpha(token) || token == '_'){
            undifined = 0; 
            // parse identifier
            words++;
            int flag_key;
            flag_key = 0;  // keyword in default
            while (isalnum(*src) || *src == '_')
            {
                src++;
            }
            for(int i = 0; i < KEYWORDNUM; i++){
                if(memcmp(lastPos, (int)(keywords)+10*i, src-lastPos) == 0 && (strlen(keywords[i]) == src-lastPos)){  // keywords+10*i is wrong
                    if(i == 0){
                        printf("第%d行，定义了结构体", lines);
                    }
                    else{
                        printf("%s\t <---> 关键字", keywords[i]);
                    }
                    flag_key = 1;
                    break;
                }
            }
            if(!flag_key){
                memcpy(str, lastPos, src-lastPos);
                printf("%s\t <---> 标识符", str);
            }
        }

        else if(isdigit(token)){  // consider positive number
            undifined = 0;

            // parse number, include dec, oct(045), hex(0x45)
            int float_num = 0;
            int val = 0, dec_oct;
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
                    printf("第%d行，浮点数，值是%lf", lines, val_float);
                }
                else{  // decimal integer
                    while (isdigit(*src))
                    {
                        val = val * 10 + *src - '0'; 
                        src++;
                    }
                    memcpy(str, lastPos, src-lastPos);
                    printf("%s\t <---> 十进制数，值为%d", str, val);
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
                memcpy(str, lastPos, src-lastPos);
                printf("%s\t <---> 十六进制数，值为%d", str, val);
            }
            else{  // oct
                val = token - '0';
                while (isdigit(*src))
                {
                    val = val * 8 + *src - '0'; 
                    src++;
                }
                memcpy(str, lastPos, src-lastPos);
                printf("%s\t <---> 八进制数，值为%d", str, val);
            }
        }

        else if(token == '"' || token == '\''){
            undifined = 0;
            // parse string literal or character
            while (*src != token)
            {
                src++;
            }
            src++;
            memcpy(str, lastPos, src-lastPos);
            printf("%s\t <---> %s", str, token== '"' ? "字符串" : "字符");
        }
        else if (symbol) {
            undifined = 0;
            // directly return the character as token;
            printf("%c\t <---> 符号", token);
        }
        
        // arithmetic operators
        switch(token){
        case('+'): 
            undifined = 0;
            if(*src == '+'){
                src++;
                printf("++\t <---> 自增运算符");
            }
            else{
                printf("+\t <---> 加号");
            }
            break;
        case('-'):
            undifined = 0;
            if(*src == '-'){
                src++;
                printf("--\t <---> 自减运算符");
            }
            else{
                printf("-\t <---> 减号");
            }
            break;
        case('*'):
            undifined = 0;
            printf("*\t <---> 乘号");
            break;
        case('/'):
            undifined = 0;
            if(*src == '/'){  // comment(this lexer only thinks "//" as comment symbol)
                while (*src != '\n')
                {
                    src++;
                }
                printf("第%d行\t <---> 注释", lines);
            }
            else{
                printf("/\t <---> 除号");
            }
            break;
        case('%'):
            undifined = 0;
            printf("%c\t <---> 求模", '%');
            break;
        default:
            break;
        }

        // recognize array
        int cap = 0;  // capacity
        switch(token){
        case('['):
            cap = *src++ - '0';
            while (isdigit(*src))
            {
                cap = cap * 10 + *src - '0'; 
                src++;
            }
            if(*src == ']'){
                undifined = 0;
                src++;
                printf("第%d行，定义了数组，容量为%d", lines, cap);
            }
        default:
            break;
        }

        // relational operators
        switch(token){
        case('='):
            undifined = 0;
            if(*src == '='){
                src++;
                printf("==\t <---> 相等运算符");
            }
            else{
                printf("=\t <---> 赋值运算符");
            }
            break;
        case('!'):
            undifined = 0;
            if(*src == '='){
                src++;
                printf("!=\t <---> 不相等运算符");
            }
            else{ // logical 
                printf("!\t <---> 逻辑非");
            }
            break;
        case('>'):
            undifined = 0;
            if(*src == '='){
                src++;
                printf(">=\t <---> 大于等于运算符");
            }
            else if(*src == '>'){  // bit 
                src++;
                printf(">>\t <---> 右移");
            }
            else{ 
                printf(">\t <---> 大于运算符");
            }
            break;
        case('<'):
            undifined = 0;
            if(*src == '='){
                src++;
                printf("<=\t <---> 小于等于运算符");
            }
            else if(*src == '<'){  // bit 
                src++;
                printf("<<\t <---> 左移");
            }
            else{ // logic
                printf("<\t <---> 小于运算符");
            }
            break;
        default:
            break;
        }

        // logical operators and bit operators
        switch(token){
        case('&'):
            undifined = 0;
            if(*src == '&'){
                src++;
                printf("&&\t <---> 逻辑与");
            }
            else{ // logical 
                printf("&\t <---> 按位与");
            }
            break;
        case('|'):
            undifined = 0;
            if(*src == '|'){
                src++;
                printf("||\t <---> 逻辑或");
            }
            else{ // logical 
                printf("|\t <---> 按位或");
            }
            break;
        case('^'):
            undifined = 0;
            printf("^\t <---> 异或");
            break;
        case('~'):
            undifined = 0;
            printf("~\t <---> 取反");
            break;
        default:
            break;
        }
        if(undifined){  
            memcpy(str, lastPos, src-lastPos);
            printf("第%d行，存在词法错误,%s未定义", lines, str);
        }
        printf("\n");
        characters += (int)src - lastPos;
    }
    
    return;
}

