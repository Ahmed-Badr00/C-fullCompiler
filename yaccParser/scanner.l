%{
#include "y.tab.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int line_number = 1;
int char_position = 1;
int token_start_pos = 1;
void error_message(char* msg);

// Function to create a copy of token text
char* copy_string() {
    return strdup(yytext);
}
%}

%option noyywrap
%option noinput
%option nounput
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

"Program"       { token_start_pos = char_position; char_position += yyleng; return TOKEN_PROGRAM; }
"if"            { token_start_pos = char_position; char_position += yyleng; return TOKEN_IF; }
"else"          { token_start_pos = char_position; char_position += yyleng; return TOKEN_ELSE; }
"while"         { token_start_pos = char_position; char_position += yyleng; return TOKEN_WHILE; }
"int"           { token_start_pos = char_position; char_position += yyleng; return TOKEN_INT; }
"float"         { token_start_pos = char_position; char_position += yyleng; return TOKEN_FLOAT; }
"then"          { token_start_pos = char_position; char_position += yyleng; return TOKEN_THEN; }

"{"             { token_start_pos = char_position; char_position += yyleng; return TOKEN_LBRACE; }
"}"             { token_start_pos = char_position; char_position += yyleng; return TOKEN_RBRACE; }
"("             { token_start_pos = char_position; char_position += yyleng; return TOKEN_LPAREN; }
")"             { token_start_pos = char_position; char_position += yyleng; return TOKEN_RPAREN; }
"["             { token_start_pos = char_position; char_position += yyleng; return TOKEN_LBRACKET; }
"]"             { token_start_pos = char_position; char_position += yyleng; return TOKEN_RBRACKET; }
";"             { token_start_pos = char_position; char_position += yyleng; return TOKEN_SEMICOLON; }
"="             { token_start_pos = char_position; char_position += yyleng; return TOKEN_ASSIGN; }

"<="            |
">="            |
"=="            |
"!="            |
"<"             |
">"             { token_start_pos = char_position; char_position += yyleng; return TOKEN_RELOP; }

[+\-]           { token_start_pos = char_position; char_position += yyleng; return TOKEN_ADDOP; }
[*/]            { token_start_pos = char_position; char_position += yyleng; return TOKEN_MULOP; }

{ID}            {
    token_start_pos = char_position;
    char_position += yyleng;
    yylval.string_val = copy_string();
    return TOKEN_ID;
}

{INVALID_ID}    {
    token_start_pos = char_position;
    error_message("Error: Invalid identifier format");
    char_position += yyleng;
}

{NUM}           {
    token_start_pos = char_position;
    char_position += yyleng;
    yylval.string_val = copy_string();
    return TOKEN_NUM;
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