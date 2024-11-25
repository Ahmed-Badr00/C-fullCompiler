%{
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Token definitions */
#define ID_TOKEN 1
#define NUM_TOKEN 2
#define IF_TOKEN 3
#define ELSE_TOKEN 4
#define INT_TOKEN 5
#define FLOAT_TOKEN 6
#define WHILE_TOKEN 7
#define PROGRAM_TOKEN 8
#define RELOP_TOKEN 9
#define ADDOP_TOKEN 10
#define MULOP_TOKEN 11
#define THEN_TOKEN 12 

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
INVALID_CHAR    [^A-Za-z0-9.@_ \t\n;+\-*/=<>()[\]{},]

ID              {LETTER}({LETTER}|{DIGIT}|{SPECIAL})*
INVALID_ID      {LETTER}({LETTER}|{DIGIT}|{SPECIAL})*{INVALID_CHAR}+({LETTER}|{DIGIT}|{SPECIAL})*

NUM             {DIGIT}+(\.{DIGIT}+)?({EXPONENT})?
INVALID_NUM     {DIGIT}+(\.{DIGIT}+)?({EXPONENT})?[A-Za-z]+

%%

"/*"            { token_start_pos = char_position; char_position += yyleng; BEGIN(COMMENT); }
<COMMENT>"*/"   { char_position += yyleng; BEGIN(INITIAL); }
<COMMENT>\n     { line_number++; char_position = 1; }
<COMMENT>.      { char_position += yyleng; }
<COMMENT><<EOF>> { error_message("Error: Unclosed comment at end of file"); return 0; }

{WHITESPACE}    { char_position += yyleng; }
\n              { line_number++; char_position = 1; }

"Program"       { token_start_pos = char_position; char_position += yyleng; return PROGRAM_TOKEN; }
"if"            { token_start_pos = char_position; char_position += yyleng; return IF_TOKEN; }
"else"          { token_start_pos = char_position; char_position += yyleng; return ELSE_TOKEN; }
"while"         { token_start_pos = char_position; char_position += yyleng; return WHILE_TOKEN; }
"int"           { token_start_pos = char_position; char_position += yyleng; return INT_TOKEN; }
"float"         { token_start_pos = char_position; char_position += yyleng; return FLOAT_TOKEN; }
"then"          { token_start_pos = char_position; char_position += yyleng; return THEN_TOKEN; } 

"{"             |
"}"             |
"("             |
")"             |
"["             |
"]"             |
";"             |
","             { token_start_pos = char_position; char_position += yyleng; return yytext[0]; }

"<="            |
">="            |
"=="            |
"!="            { token_start_pos = char_position; char_position += yyleng; return RELOP_TOKEN; }

[+\-]           { token_start_pos = char_position; char_position += yyleng; return ADDOP_TOKEN; }
[*/]            { token_start_pos = char_position; char_position += yyleng; return MULOP_TOKEN; }
[=<>]           { token_start_pos = char_position; char_position += yyleng; return yytext[0]; }

{ID}            {
    token_start_pos = char_position;
    char_position += yyleng;
    return ID_TOKEN;
}

{INVALID_ID}    {
    token_start_pos = char_position;
    error_message("Error: Invalid identifier format");
    char_position += yyleng;
}

{NUM}           {
    token_start_pos = char_position;
    char_position += yyleng;
    return NUM_TOKEN;
}

{INVALID_NUM}   {
    token_start_pos = char_position;
    error_message("Error: Invalid number format");
    char_position += yyleng;
}

.               {
    token_start_pos = char_position;
    error_message("Error: Invalid character");
    char_position += yyleng;
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

    }

    fclose(input);
    return 0;
}
