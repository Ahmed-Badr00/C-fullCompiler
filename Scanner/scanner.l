%{
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Token definitions */
#define ID_TOKEN 1
#define NUM_TOKEN 2
#define IF_TOKEN 3
#define THEN_TOKEN 4
#define ELSE_TOKEN 5

int line_number = 1;
int char_position = 1;
int token_start_pos = 1;
void error_message(char* msg);
%}

%option noyywrap
%option caseless

/* States */
%x COMMENT

/* Regular Definitions */
LETTER          [A-Za-z]
DIGIT           [0-9]
WHITESPACE      [ \t\r]
SPECIAL         [.@_]
EXPONENT        [Ee][+-]?{DIGIT}+

ID              {LETTER}({LETTER}|{DIGIT})*({SPECIAL}({LETTER}|{DIGIT})+)?
INVALID_ID      {LETTER}({LETTER}|{DIGIT})*{SPECIAL}?[^A-Za-z0-9.@_\n \t]+({LETTER}|{DIGIT})*
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
    return 0;
}

{WHITESPACE}    {
    char_position += yyleng;
}

\n              {
    line_number++;
    char_position = 1;
}

if              {
    token_start_pos = char_position;
    char_position += yyleng;
    printf("TOKEN: IF at line %d, position %d\n", line_number, token_start_pos);
    return IF_TOKEN;
}

then            {
    token_start_pos = char_position;
    char_position += yyleng;
    printf("TOKEN: THEN at line %d, position %d\n", line_number, token_start_pos);
    return THEN_TOKEN;
}

else            {
    token_start_pos = char_position;
    char_position += yyleng;
    printf("TOKEN: ELSE at line %d, position %d\n", line_number, token_start_pos);
    return ELSE_TOKEN;
}

{ID}            {
    token_start_pos = char_position;
    char_position += yyleng;
    printf("TOKEN: ID (%s) at line %d, position %d\n", yytext, line_number, token_start_pos);
    return ID_TOKEN;
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
    return NUM_TOKEN;
}

{INVALID_NUM}   {
    token_start_pos = char_position;
    error_message("Error: Invalid number format");
    char_position += yyleng;
    /* Continue scanning after the invalid number */
}

[+\-*/=<>!]     {
    token_start_pos = char_position;
    char_position += yyleng;
    printf("TOKEN: OPERATOR (%s) at line %d, position %d\n", yytext, line_number, token_start_pos);
    return yytext[0];
}

.               {
    token_start_pos = char_position;
    error_message("Error: Invalid character");
    char_position += yyleng;
    /* Consume the invalid character */
}

%%

void error_message(char* msg) {
    fprintf(stderr, "%s at line %d, position %d: '%s'\n",
            msg, line_number, token_start_pos, yytext);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s input_file\n", argv[0]);
        return 1;
    }

    FILE* input = fopen(argv[1], "r");
    if (!input) {
        fprintf(stderr, "Error: Cannot open input file '%s'\n", argv[1]);
        return 1;
    }

    yyin = input;

    while (yylex()) {
        // Token processing is handled in the rules above
    }

    fclose(input);
    return 0;
}

