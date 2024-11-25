#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations for scanner functions
extern FILE* yyin;
extern char* yytext;
extern int yylex(void);
extern int line_number;
extern int char_position;

// Remove scanner's main function at link time
#define main scanner_main
#include "lex.yy.c"
#undef main

typedef enum {
    TOKEN_EOF = 0,
    TOKEN_ID = 1,       
    TOKEN_NUM = 2,      
    TOKEN_IF = 3,       
    TOKEN_ELSE = 4,     
    TOKEN_INT = 5,      
    TOKEN_FLOAT = 6,    
    TOKEN_WHILE = 7,    
    TOKEN_PROGRAM = 8,  
    TOKEN_RELOP = 9,    
    TOKEN_ADDOP = 10,   
    TOKEN_MULOP = 11,   
    TOKEN_THEN = 12,    

    TOKEN_LBRACE = 123,      // ASCII value for '{'
    TOKEN_RBRACE = 125,      // ASCII value for '}'
    TOKEN_LPAREN = 40,       // ASCII value for '('
    TOKEN_RPAREN = 41,       // ASCII value for ')'
    TOKEN_LBRACKET = 91,     // ASCII value for '['
    TOKEN_RBRACKET = 93,     // ASCII value for ']'
    TOKEN_SEMICOLON = 59,    // ASCII value for ';'
    TOKEN_ASSIGN = 61,       // ASCII value for '='
    TOKEN_ERROR = 255        // For error handling
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    char* lexeme;
    int line;
    int position;
} Token;

// Global variables
Token currentToken;

// Function to convert scanner token to parser token type
TokenType convertTokenType(int scannerToken) {
    switch(scannerToken) {
        case 0: return TOKEN_EOF;      // EOF
        case '{': return TOKEN_LBRACE;
        case '}': return TOKEN_RBRACE;
        case '(': return TOKEN_LPAREN;
        case ')': return TOKEN_RPAREN;
        case '[': return TOKEN_LBRACKET;
        case ']': return TOKEN_RBRACKET;
        case ';': return TOKEN_SEMICOLON;
        case '=': return TOKEN_ASSIGN;
        case '<': return TOKEN_RELOP;
        case '>': return TOKEN_RELOP;
        default:
            if (scannerToken >= 1 && scannerToken <= 12) {
                return (TokenType)scannerToken;
            }
            return TOKEN_ERROR;
    }
}

// Forward declarations for all parsing functions
void program();
void varDeclaration();
void typeSpecifier();
void statement();
void assignmentStmt();
void var();
void compoundStmt();
void selectionStmt();
void iterationStmt();
void expression();
void additiveExpression();
void term();
void factor();

// Get next token from scanner with debugging
void getNextToken() {
    int scannerToken = yylex();
    currentToken.line = line_number;
    currentToken.position = char_position;
    currentToken.lexeme = strdup(yytext);
    currentToken.type = convertTokenType(scannerToken);
    printf("DEBUG: Token '%s' (type %d) at line %d, position %d\n",
           currentToken.lexeme, currentToken.type, currentToken.line, currentToken.position);
}

// Error reporting
void syntaxError(const char* message) {
    fprintf(stderr, "Syntax Error at line %d, position %d: %s (Got '%s')\n",
            currentToken.line, currentToken.position, message, currentToken.lexeme);
    exit(1);
}

// Match and consume expected token with debugging
void match(TokenType expectedType) {
    printf("DEBUG: Matching expected token type %d with current token type %d ('%s')\n",
           expectedType, currentToken.type, currentToken.lexeme);
    if (currentToken.type == expectedType) {
        getNextToken();
    } else {
        char errorMsg[200];
        sprintf(errorMsg, "Expected token type %d but found '%s'",
                expectedType, currentToken.lexeme);
        syntaxError(errorMsg);
    }
}

// Initialize parser
void initParser(FILE* input) {
    yyin = input;
    getNextToken();
}

// Parsing functions
void program() {
    match(TOKEN_PROGRAM);
    match(TOKEN_ID);
    match(TOKEN_LBRACE);
    // Handle declarations and statements
    while (currentToken.type != TOKEN_RBRACE && currentToken.type != TOKEN_EOF) {
        if (currentToken.type == TOKEN_INT || currentToken.type == TOKEN_FLOAT) {
            varDeclaration();
        } else {
            statement();
        }
    }
    match(TOKEN_RBRACE);
}

void varDeclaration() {
    typeSpecifier();
    match(TOKEN_ID);
    if (currentToken.type == TOKEN_LBRACKET) {
        match(TOKEN_LBRACKET);
        match(TOKEN_NUM);
        match(TOKEN_RBRACKET);
    }
    match(TOKEN_SEMICOLON);
}

void typeSpecifier() {
    if (currentToken.type == TOKEN_INT) {
        match(TOKEN_INT);
    } else if (currentToken.type == TOKEN_FLOAT) {
        match(TOKEN_FLOAT);
    } else {
        syntaxError("Expected type specifier (int or float)");
    }
}

void statement() {
    switch (currentToken.type) {
        case TOKEN_ID:
            assignmentStmt();
            break;
        case TOKEN_LBRACE:
            compoundStmt();
            break;
        case TOKEN_IF:
            selectionStmt();
            break;
        case TOKEN_WHILE:
            iterationStmt();
            break;
        default:
            syntaxError("Invalid statement");
    }
}

void assignmentStmt() {
    var();
    match(TOKEN_ASSIGN);
    expression();
    match(TOKEN_SEMICOLON);  
}

void var() {
    match(TOKEN_ID);
    if (currentToken.type == TOKEN_LBRACKET) {
        match(TOKEN_LBRACKET);
        expression();
        match(TOKEN_RBRACKET);
    }
}

void compoundStmt() {
    match(TOKEN_LBRACE);
    // Handle declarations and statements in any order
    while (currentToken.type != TOKEN_RBRACE && currentToken.type != TOKEN_EOF) {
        if (currentToken.type == TOKEN_INT || currentToken.type == TOKEN_FLOAT) {
            varDeclaration();
        } else {
            statement();
        }
    }
    match(TOKEN_RBRACE);
}

void selectionStmt() {
    match(TOKEN_IF);
    match(TOKEN_LPAREN);
    expression();
    match(TOKEN_RPAREN);
    match(TOKEN_THEN);  
    statement();
    if (currentToken.type == TOKEN_ELSE) {
        match(TOKEN_ELSE);
        statement();
    }
}

void iterationStmt() {
    match(TOKEN_WHILE);
    match(TOKEN_LPAREN);
    expression();
    match(TOKEN_RPAREN);
    statement();
}

void expression() {
    additiveExpression();
    if (currentToken.type == TOKEN_RELOP) {
        match(TOKEN_RELOP);
        additiveExpression();
    }
}

void additiveExpression() {
    term();
    while (currentToken.type == TOKEN_ADDOP) {
        match(TOKEN_ADDOP);
        term();
    }
}

void term() {
    factor();
    while (currentToken.type == TOKEN_MULOP) {
        match(TOKEN_MULOP);
        factor();
    }
}

void factor() {
    switch (currentToken.type) {
        case TOKEN_LPAREN:
            match(TOKEN_LPAREN);
            expression();
            match(TOKEN_RPAREN);
            break;
        case TOKEN_ID:
            var();
            break;
        case TOKEN_NUM:
            match(TOKEN_NUM);
            break;
        default:
            syntaxError("Invalid factor");
    }
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

    initParser(input);
    program();

    if (currentToken.type == TOKEN_EOF) {
        printf("Parsing completed successfully.\n");
    } else {
        syntaxError("Unexpected tokens after end of program");
    }

    fclose(input);
    return 0;
}
