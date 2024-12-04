%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylex();
extern int yyparse();
extern FILE* yyin;
extern int line_number;
extern int char_position;
extern char* yytext;

void yyerror(const char* message);

// Define YYSTYPE to handle both string and numeric values
%}

%union {
    char* string_val;
    int int_val;
}

/* Token Declarations with types */
%token <string_val> TOKEN_ID
%token <string_val> TOKEN_NUM
%token TOKEN_IF TOKEN_ELSE TOKEN_INT TOKEN_FLOAT TOKEN_WHILE TOKEN_PROGRAM TOKEN_THEN
%token TOKEN_RELOP TOKEN_ADDOP TOKEN_MULOP
%token TOKEN_ASSIGN
%token TOKEN_LBRACE TOKEN_RBRACE
%token TOKEN_LPAREN TOKEN_RPAREN
%token TOKEN_LBRACKET TOKEN_RBRACKET
%token TOKEN_SEMICOLON

/* Precedence and Associativity Rules */
%nonassoc LOWER_THAN_ELSE
%nonassoc TOKEN_ELSE
%left TOKEN_RELOP
%left TOKEN_ADDOP
%left TOKEN_MULOP

/* Start Symbol */
%start program

%%

program
    : TOKEN_PROGRAM TOKEN_ID TOKEN_LBRACE 
        { printf("\nParsing program: %s\n", $2); printf("=====================================\n"); }
        declarations_and_statements TOKEN_RBRACE
        { printf("\nProgram parsing completed successfully.\n"); }
    ;

declarations_and_statements
    : /* empty */
    | declarations_and_statements var_declaration
    | declarations_and_statements statement
    ;

var_declaration
    : type_specifier TOKEN_ID TOKEN_SEMICOLON
        { printf("Variable Declaration: %s\n", $2); }
    | type_specifier TOKEN_ID TOKEN_LBRACKET TOKEN_NUM TOKEN_RBRACKET TOKEN_SEMICOLON
        { printf("Array Declaration  : %s[%s]\n", $2, $4); }
    ;

type_specifier
    : TOKEN_INT
        { printf("Type Specifier    : int\n"); }
    | TOKEN_FLOAT
        { printf("Type Specifier    : float\n"); }
    ;

statement
    : assignment_stmt
    | compound_stmt
    | selection_stmt
    | iteration_stmt
    ;

assignment_stmt
    : var TOKEN_ASSIGN expression TOKEN_SEMICOLON
        { printf("Assignment Statement\n"); }
    ;

var
    : TOKEN_ID
        { printf("Variable          : %s\n", $1); }
    | TOKEN_ID TOKEN_LBRACKET expression TOKEN_RBRACKET
        { printf("Array Access      : %s[]\n", $1); }
    ;

compound_stmt
    : TOKEN_LBRACE
        { printf("Begin Compound Statement\n"); }
        declarations_and_statements TOKEN_RBRACE
        { printf("End Compound Statement\n"); }
    ;

selection_stmt
    : if_part statement %prec LOWER_THAN_ELSE
        { printf("End If Statement\n"); }
    | if_part statement TOKEN_ELSE 
        { printf("Begin Else Statement\n"); }
        statement
        { printf("End If-Else Statement\n"); }
    ;

if_part
    : TOKEN_IF TOKEN_LPAREN expression TOKEN_RPAREN TOKEN_THEN
        { printf("Begin If Statement\n"); }
    ;

iteration_stmt
    : TOKEN_WHILE TOKEN_LPAREN
        { printf("Begin While Statement\n"); }
        expression TOKEN_RPAREN
        statement
        { printf("End While Statement\n"); }
    ;

expression
    : additive_expression
    | additive_expression TOKEN_RELOP additive_expression
        { printf("Relational Expression\n"); }
    ;

additive_expression
    : term
    | additive_expression TOKEN_ADDOP term
        { printf("Additive Expression\n"); }
    ;

term
    : factor
    | term TOKEN_MULOP factor
        { printf("Multiplicative Expression\n"); }
    ;

factor
    : TOKEN_LPAREN expression TOKEN_RPAREN
    | var
    | TOKEN_NUM
        { printf("Number           : %s\n", $1); }
    ;

%%

void yyerror(const char* message) {
    fprintf(stderr, "Syntax Error at line %d, position %d: %s (near '%s')\n",
            line_number, char_position, message, yytext);
    exit(1);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE* input = fopen(argv[1], "r");
    if (!input) {
        fprintf(stderr, "Error: Cannot open input file '%s'\n", argv[1]);
        return 1;
    }

    printf("Starting C- Parser\n");
    printf("=====================================\n");

    yyin = input;
    int result = yyparse();

    if (result == 0) {
        printf("=====================================\n");
        printf("Parsing completed without errors.\n");
    }

    fclose(input);
    return result;
}