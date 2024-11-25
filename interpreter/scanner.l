%{
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "tokens.h"

/* Token definitions to match interpreter.c */

int line_number = 1;
int char_position = 1;
int token_start_pos = 1;
void error_message(char* msg);
%}

%option noyywrap
%option caseless
%option nomain

/* States */
%x COMMENT

/* Regular Definitions */
LETTER          [A-Za-z]
DIGIT           [0-9]
WHITESPACE      [ \t\r]
SPECIAL         [.@_]
EXPONENT        [Ee][+-]?{DIGIT}+

ID              {LETTER}({LETTER}|{DIGIT})*
INVALID_ID      {DIGIT}+{LETTER}+
NUM             {DIGIT}+(\.{DIGIT}+)?({EXPONENT})?
INVALID_NUM     {DIGIT}+(\.{DIGIT}+)?({EXPONENT})?[A-Za-z]+

%%

"/*"            {
    token_start_pos = char_position;
    char_position += yyleng;
    BEGIN(COMMENT);
}

<COMMENT>"*/"   {
    char_position += yyleng;
    BEGIN(INITIAL);
}

<COMMENT>\n     {
    line_number++;
    char_position = 1;
}

<COMMENT>.      {
    char_position += yyleng;
}

<COMMENT><<EOF>> {
    error_message("Error: Unclosed comment at end of file");
    return TOKEN_EOF;
}

{WHITESPACE}    {
    char_position += yyleng;
}

\n              {
    line_number++;
    char_position = 1;
}

"program"       {
    token_start_pos = char_position;
    char_position += yyleng;
    printf("TOKEN: PROGRAM at line %d, position %d\n", line_number, token_start_pos);
    return TOKEN_PROGRAM;
}

"if"            {
    token_start_pos = char_position;
    char_position += yyleng;
    printf("TOKEN: IF at line %d, position %d\n", line_number, token_start_pos);
    return TOKEN_IF;
}

"then"          {
    token_start_pos = char_position;
    char_position += yyleng;
    printf("TOKEN: THEN at line %d, position %d\n", line_number, token_start_pos);
    return TOKEN_THEN;
}

"else"          {
    token_start_pos = char_position;
    char_position += yyleng;
    printf("TOKEN: ELSE at line %d, position %d\n", line_number, token_start_pos);
    return TOKEN_ELSE;
}

"while"         {
    token_start_pos = char_position;
    char_position += yyleng;
    printf("TOKEN: WHILE at line %d, position %d\n", line_number, token_start_pos);
    return TOKEN_WHILE;
}

"do"    { return TOKEN_DO; }

"int"           {
    token_start_pos = char_position;
    char_position += yyleng;
    printf("TOKEN: INT at line %d, position %d\n", line_number, token_start_pos);
    return TOKEN_INT;
}

"float"         {
    token_start_pos = char_position;
    char_position += yyleng;
    printf("TOKEN: FLOAT at line %d, position %d\n", line_number, token_start_pos);
    return TOKEN_FLOAT;
}

"read"          {
    token_start_pos = char_position;
    char_position += yyleng;
    printf("TOKEN: READ at line %d, position %d\n", line_number, token_start_pos);
    return TOKEN_READ;
}

"write"         {
    token_start_pos = char_position;
    char_position += yyleng;
    printf("TOKEN: WRITE at line %d, position %d\n", line_number, token_start_pos);
    return TOKEN_WRITE;
}

{ID}            {
    token_start_pos = char_position;
    char_position += yyleng;
    printf("TOKEN: ID (%s) at line %d, position %d\n", yytext, line_number, token_start_pos);
    return TOKEN_ID;
}

{INVALID_ID}    {
    token_start_pos = char_position;
    error_message("Error: Invalid identifier format");
    char_position += yyleng;
    /* Continue scanning after the invalid identifier */
}

{NUM}           {
    token_start_pos = char_position;
    char_position += yyleng;
    printf("TOKEN: NUM (%s) at line %d, position %d\n", yytext, line_number, token_start_pos);
    return TOKEN_NUM;
}

{INVALID_NUM}   {
    token_start_pos = char_position;
    error_message("Error: Invalid number format");
    char_position += yyleng;
    /* Continue scanning after the invalid number */
}



"{"             {
    token_start_pos = char_position;
    char_position += yyleng;
    printf("TOKEN: LBRACE at line %d, position %d\n", line_number, token_start_pos);
    return TOKEN_LBRACE;
}

"}"             {
    token_start_pos = char_position;
    char_position += yyleng;
    printf("TOKEN: RBRACE at line %d, position %d\n", line_number, token_start_pos);
    return TOKEN_RBRACE;
}

";"             {
    token_start_pos = char_position;
    char_position += yyleng;
    printf("TOKEN: SEMICOLON at line %d, position %d\n", line_number, token_start_pos);
    return TOKEN_SEMICOLON;
}

","             {
    token_start_pos = char_position;
    char_position += yyleng;
    printf("TOKEN: COMMA at line %d, position %d\n", line_number, token_start_pos);
    return TOKEN_COMMA;
}

"="             {
    token_start_pos = char_position;
    char_position += yyleng;
    printf("TOKEN: ASSIGN at line %d, position %d\n", line_number, token_start_pos);
    return TOKEN_ASSIGN;
}

"+"             |
"-"             {
    token_start_pos = char_position;
    char_position += yyleng;
    printf("TOKEN: ADDOP (%s) at line %d, position %d\n", yytext, line_number, token_start_pos);
    return TOKEN_ADDOP;
}

"*"             |
"/"             {
    token_start_pos = char_position;
    char_position += yyleng;
    printf("TOKEN: MULOP (%s) at line %d, position %d\n", yytext, line_number, token_start_pos);
    return TOKEN_MULOP;
}

"<"             |
">"             |
"<="            |
">="            |
"=="            |
"!="            {
    token_start_pos = char_position;
    char_position += yyleng;
    printf("TOKEN: RELOP (%s) at line %d, position %d\n", yytext, line_number, token_start_pos);
    return TOKEN_RELOP;
}

"("             {
    token_start_pos = char_position;
    char_position += yyleng;
    printf("TOKEN: LPAREN at line %d, position %d\n", line_number, token_start_pos);
    return TOKEN_LPAREN;
}

")"             {
    token_start_pos = char_position;
    char_position += yyleng;
    printf("TOKEN: RPAREN at line %d, position %d\n", line_number, token_start_pos);
    return TOKEN_RPAREN;
}

"["             {
    token_start_pos = char_position;
    char_position += yyleng;
    printf("TOKEN: LBRACKET at line %d, position %d\n", line_number, token_start_pos);
    return TOKEN_LBRACKET;
}

"]"             {
    token_start_pos = char_position;
    char_position += yyleng;
    printf("TOKEN: RBRACKET at line %d, position %d\n", line_number, token_start_pos);
    return TOKEN_RBRACKET;
}

.               {
    token_start_pos = char_position;
    error_message("Error: Invalid character");
    char_position += yyleng;
    return TOKEN_ERROR;
}

%%

void error_message(char* msg) {
    fprintf(stderr, "%s at line %d, position %d: '%s'\n",
            msg, line_number, token_start_pos, yytext);
}