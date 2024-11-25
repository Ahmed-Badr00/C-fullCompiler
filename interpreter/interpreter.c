#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h"

// Forward declarations for scanner functions
extern FILE* scannerin;
extern char* scannertext;
extern int scannerlex(void);
extern int line_number;
extern int char_position;

// Symbol table entry structure
typedef struct {
    char* name;
    int type; // TOKEN_INT or TOKEN_FLOAT
    union {
        int int_val;    // For single integer values
        float float_val; // For single float values
        int* int_array; // For integer arrays
        float* float_array; // For float arrays
    } value;
    int is_array;
    int array_size;
    int line_number;
} SymbolEntry;

// Symbol table structure
#define MAX_SYMBOLS 100
typedef struct {
    SymbolEntry entries[MAX_SYMBOLS];
    int count;
} SymbolTable;

// Current token structure
typedef struct {
    int type; 
    char* lexeme;
    int line;
    int position;
} Token;

// Expression value structure
typedef struct {
    int type; 
    union {
        int int_val;
        float float_val;
    } value;
} ExprValue;

// Global variables
Token currentToken;
SymbolTable symbolTable;

// Function declarations
void initSymbolTable();
SymbolEntry* lookupSymbol(const char* name);
void addSymbol(const char* name, int type, int is_array, int array_size);
void semanticError(const char* message);
void match(int expectedType);
void getNextToken();
void program();
void varDeclaration();
void statement();
void assignmentStmt();
void compoundStmt();
void selectionStmt();
void iterationStmt();
void readStmt();
void writeStmt();
ExprValue expression();
ExprValue additiveExpression();
ExprValue term();
ExprValue factor();

// Initialize the symbol table
void initSymbolTable() {
    symbolTable.count = 0;
}

// Look up a symbol in the table
SymbolEntry* lookupSymbol(const char* name) {
    for (int i = 0; i < symbolTable.count; i++) {
        if (strcmp(symbolTable.entries[i].name, name) == 0) {
            return &symbolTable.entries[i];
        }
    }
    return NULL;
}

// Add a new symbol to the table
void addSymbol(const char* name, int type, int is_array, int array_size) {
    if (symbolTable.count >= MAX_SYMBOLS) {
        semanticError("Symbol table full");
    }

    SymbolEntry* entry = &symbolTable.entries[symbolTable.count++];
    entry->name = strdup(name);
    entry->type = type;
    entry->is_array = is_array;
    entry->array_size = array_size;
    entry->line_number = line_number;

    if (is_array) {
        if (type == TOKEN_INT) {
            entry->value.int_array = (int*)calloc(array_size, sizeof(int));
        } else {
            entry->value.float_array = (float*)calloc(array_size, sizeof(float));
        }
    } else {
        if (type == TOKEN_INT) {
            entry->value.int_val = 0;
        } else {
            entry->value.float_val = 0.0f;
        }
    }
}

// Semantic error handler
void semanticError(const char* message) {
    fprintf(stderr, "Semantic Error at line %d: %s\n", line_number, message);
    exit(1);
}

// Match and consume a token
void match(int expectedType) {
    if (currentToken.type == expectedType) {
        // printf("DEBUG: Matched token type %d: %s\n", expectedType, currentToken.lexeme);
        getNextToken();
    } else {
        char error[100];
        sprintf(error, "Expected token type %d but found %d", expectedType, currentToken.type);
        semanticError(error);
    }
}

// Parse a statement
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
        case TOKEN_READ:
            readStmt();
            break;
        case TOKEN_WRITE:
            writeStmt();
            break;
        default:
            semanticError("Invalid statement");
    }
}

void assignmentStmt() {
    char* name = strdup(currentToken.lexeme);
    SymbolEntry* symbol = lookupSymbol(name);

    if (!symbol) {
        semanticError("Undefined variable");
    }

    match(TOKEN_ID);

    int is_indexed = 0;
    int index = -1;

    if (currentToken.type == TOKEN_LBRACKET) {
        is_indexed = 1;
        match(TOKEN_LBRACKET);

        if (currentToken.type == TOKEN_NUM) {
            index = atoi(currentToken.lexeme);
            match(TOKEN_NUM);
        } else {
            semanticError("Expected integer index for array");
        }

        match(TOKEN_RBRACKET);

        if (!symbol->is_array) {
            semanticError("Variable is not an array");
        }

        if (index < 0 || index >= symbol->array_size) {
            semanticError("Array index out of bounds");
        }
    }

    match(TOKEN_ASSIGN);

    ExprValue expr_val = expression();
    match(TOKEN_SEMICOLON);

    if (is_indexed) {
        if (symbol->type == TOKEN_INT) {
            symbol->value.int_array[index] = expr_val.value.int_val;
        } else {
            symbol->value.float_array[index] = expr_val.value.float_val;
        }
    } else {
        if (symbol->type == TOKEN_INT && expr_val.type == TOKEN_INT) {
            symbol->value.int_val = expr_val.value.int_val;
        } else if (symbol->type == TOKEN_FLOAT && expr_val.type == TOKEN_FLOAT) {
            symbol->value.float_val = expr_val.value.float_val;
        } else if (symbol->type == TOKEN_FLOAT && expr_val.type == TOKEN_INT) {
            symbol->value.float_val = (float)expr_val.value.int_val;
        } else {
            semanticError("Type mismatch in assignment");
        }
    }

    free(name);
}

void compoundStmt() {
    match(TOKEN_LBRACE); // Match '{'
    while (currentToken.type != TOKEN_RBRACE && currentToken.type != TOKEN_EOF) {
        if (currentToken.type == TOKEN_INT || currentToken.type == TOKEN_FLOAT) {
            varDeclaration();
        } else {
            statement();
        }
    }
    if (currentToken.type == TOKEN_RBRACE) {
        match(TOKEN_RBRACE); // Match '}'
    } else {
        semanticError("Unmatched '{' in compound statement, expected '}'");
    }
}

void selectionStmt() {
    match(TOKEN_IF);
    match(TOKEN_LPAREN);
    
    // Evaluate the condition
    ExprValue condition = expression();
    match(TOKEN_RPAREN);
    match(TOKEN_THEN);
    
    if (condition.value.int_val) {
        // Execute the 'then' branch
        if (currentToken.type == TOKEN_LBRACE) {
            compoundStmt();
        } else {
            statement();
        }
        
        // Skip the else part if it exists
        if (currentToken.type == TOKEN_ELSE) {
            match(TOKEN_ELSE);
            // Skip the else block
            if (currentToken.type == TOKEN_LBRACE) {
                int brace_count = 1;
                match(TOKEN_LBRACE);
                while (brace_count > 0 && currentToken.type != TOKEN_EOF) {
                    if (currentToken.type == TOKEN_LBRACE) brace_count++;
                    if (currentToken.type == TOKEN_RBRACE) brace_count--;
                    getNextToken();
                }
            } else {
                // Skip single statement
                while (currentToken.type != TOKEN_SEMICOLON && 
                       currentToken.type != TOKEN_EOF) {
                    getNextToken();
                }
                if (currentToken.type == TOKEN_SEMICOLON) {
                    match(TOKEN_SEMICOLON);
                }
            }
        }
    } else {
        // Skip the 'then' branch
        if (currentToken.type == TOKEN_LBRACE) {
            int brace_count = 1;
            match(TOKEN_LBRACE);
            while (brace_count > 0 && currentToken.type != TOKEN_EOF) {
                if (currentToken.type == TOKEN_LBRACE) brace_count++;
                if (currentToken.type == TOKEN_RBRACE) brace_count--;
                getNextToken();
            }
        } else {
            // Skip single statement
            while (currentToken.type != TOKEN_SEMICOLON && 
                   currentToken.type != TOKEN_EOF) {
                getNextToken();
            }
            if (currentToken.type == TOKEN_SEMICOLON) {
                match(TOKEN_SEMICOLON);
            }
        }
        
        // Process the else branch if it exists
        if (currentToken.type == TOKEN_ELSE) {
            match(TOKEN_ELSE);
            if (currentToken.type == TOKEN_LBRACE) {
                compoundStmt();
            } else {
                statement();
            }
        }
    }
}

ExprValue parseCondition() {
    // Save current token to restore after parsing
    Token savedToken = currentToken;

    ExprValue result = expression();

    // Restore current token
    currentToken = savedToken;

    return result;
}

void parseCompoundStmt() {
    match(TOKEN_LBRACE);
    while (currentToken.type != TOKEN_RBRACE && currentToken.type != TOKEN_EOF) {
        if (currentToken.type == TOKEN_INT || currentToken.type == TOKEN_FLOAT) {
            varDeclaration();
        } else {
            statement();
        }
    }
    match(TOKEN_RBRACE);
}

void parseStatement() {
    statement();
}

void iterationStmt() {

    // match(TOKEN_WHILE);  // Match 'while'
    // match(TOKEN_LPAREN); // Match '('

    // ExprValue condition = expression(); // Parse the condition
    // match(TOKEN_RPAREN); // Match ')'

    // if (currentToken.type == TOKEN_LBRACE) {
    //     // Parse a compound statement
    //     compoundStmt();
    // } else {
    //     // Parse a single statement
    //     statement();
    // }
    
    match(TOKEN_WHILE);  // Match 'while'
    match(TOKEN_LPAREN); // Match '('
    
    ExprValue condition = expression(); // Parse the condition
    match(TOKEN_RPAREN); // Match ')'

    if (condition.value.int_val) {
        // If condition is true, execute the loop body
        if (currentToken.type == TOKEN_LBRACE) {
            compoundStmt();
        } else {
            statement();
        }
        
        // After executing the body, rewind to evaluate the condition again
        fseek(scannerin, -strlen(currentToken.lexeme), SEEK_CUR);
        getNextToken();
        iterationStmt(); // Recursive call to handle next iteration
    } 
    
    else {
        // If condition is false, skip the loop body
        if (currentToken.type == TOKEN_LBRACE) {
            compoundStmt();
        } else {
            statement();
        }
    }
}
void readStmt() {
    match(TOKEN_READ);

    do {
        char* name = strdup(currentToken.lexeme);
        SymbolEntry* symbol = lookupSymbol(name);

        if (!symbol) {
            semanticError("Undefined variable in read statement");
        }

        if (symbol->type == TOKEN_INT) {
            printf("Enter integer value for %s: ", name);
            scanf("%d", &symbol->value.int_val);
        } else {
            printf("Enter float value for %s: ", name);
            scanf("%f", &symbol->value.float_val);
        }

        free(name);
        match(TOKEN_ID);

        if (currentToken.type == TOKEN_COMMA) {
            match(TOKEN_COMMA);
        } else {
            break;
        }
    } while (1);

    match(TOKEN_SEMICOLON);
}

void writeStmt() {
    match(TOKEN_WRITE);

    do {
        char* name = strdup(currentToken.lexeme);
        SymbolEntry* symbol = lookupSymbol(name);

        if (!symbol) {
            semanticError("Undefined variable in write statement");
        }

        match(TOKEN_ID);

        if (currentToken.type == TOKEN_LBRACKET) {
            // Handle array element
            match(TOKEN_LBRACKET);

            if (currentToken.type != TOKEN_NUM) {
                semanticError("Expected integer index for array");
            }

            int index = atoi(currentToken.lexeme);
            match(TOKEN_NUM);
            match(TOKEN_RBRACKET);

            if (!symbol->is_array) {
                semanticError("Variable is not an array");
            }

            if (index < 0 || index >= symbol->array_size) {
                semanticError("Array index out of bounds");
            }

            if (symbol->type == TOKEN_INT) {
                printf("%s[%d] = %d\n", name, index, symbol->value.int_array[index]);
            } else {
                printf("%s[%d] = %f\n", name, index, symbol->value.float_array[index]);
            }
        } else {
            // Handle scalar variable
            if (symbol->is_array) {
                semanticError("Cannot write entire array, specify an index");
            }

            if (symbol->type == TOKEN_INT) {
                printf("%s = %d\n", name, symbol->value.int_val);
            } else {
                printf("%s = %f\n", name, symbol->value.float_val);
            }
        }

        free(name);

        if (currentToken.type == TOKEN_COMMA) {
            match(TOKEN_COMMA);
        } else {
            break;
        }
    } while (1);

    match(TOKEN_SEMICOLON);
}

// Token handling
void getNextToken() {
    int scannerToken = scannerlex();
    currentToken.line = line_number;
    currentToken.position = char_position;
    currentToken.lexeme = strdup(scannertext);
    currentToken.type = scannerToken;
        // printf("DEBUG: Token Type: %d, Lexeme: %s, Line: %d, Pos: %d\n",
        //    scannerToken, scannertext, line_number, char_position);
           }

// Parse a program
void program() {
    initSymbolTable();

    match(TOKEN_PROGRAM);
    match(TOKEN_ID);
    match(TOKEN_LBRACE);

    while (currentToken.type != TOKEN_RBRACE && currentToken.type != TOKEN_EOF) {
        if (currentToken.type == TOKEN_INT || currentToken.type == TOKEN_FLOAT) {
            varDeclaration();
        } else {
            statement();
        }
    }

    if (currentToken.type == TOKEN_RBRACE) {
        match(TOKEN_RBRACE);
    } else {
        semanticError("Unmatched '{' in program, expected '}' before EOF");
    }

    // Check if EOF is reached properly
    if (currentToken.type != TOKEN_EOF) {
        semanticError("Unexpected tokens after program block");
    } else {
        printf("DEBUG: Reached EOF successfully.\n");
    }

    // Print final symbol table
    printf("\nFinal Symbol Table:\n");
    printf("Name\tType\tValue\tLine\n");
    printf("-----------------------------\n");
    for (int i = 0; i < symbolTable.count; i++) {
        SymbolEntry* entry = &symbolTable.entries[i];
        printf("%s\t%s\t", entry->name, entry->type == TOKEN_INT ? "int" : "float");

        if (entry->is_array) {
            printf("[array]\t");
        } else if (entry->type == TOKEN_INT) {
            printf("%d\t", entry->value.int_val);
        } else {
            printf("%f\t", entry->value.float_val);
        }

        printf("%d\n", entry->line_number);
    }
}

// Parse a variable declaration
void varDeclaration() {
    int type = currentToken.type;
    match(type);

    do {
        if (currentToken.type != TOKEN_ID) {
            semanticError("Expected identifier");
        }

        char* name = strdup(currentToken.lexeme);
        match(TOKEN_ID);

        int is_array = 0, array_size = 0;
        if (currentToken.type == TOKEN_LBRACKET) {
            is_array = 1;
            match(TOKEN_LBRACKET);
            if (currentToken.type != TOKEN_NUM) {
                semanticError("Array size must be a number");
            }
            array_size = atoi(currentToken.lexeme);
            match(TOKEN_NUM);
            match(TOKEN_RBRACKET);
        }

        if (lookupSymbol(name)) {
            semanticError("Variable already declared");
        }

        addSymbol(name, type, is_array, array_size);
        free(name);

        // Check for comma to declare more variables
        if (currentToken.type == TOKEN_COMMA) {
            match(TOKEN_COMMA);
        } else {
            break;
        }
    } while (1);

    match(TOKEN_SEMICOLON);
}

// Parse a factor (numbers or identifiers)
ExprValue factor() {
    ExprValue result;

    if (currentToken.type == TOKEN_NUM) {
        if (strchr(currentToken.lexeme, '.')) {
            result.type = TOKEN_FLOAT;
            result.value.float_val = atof(currentToken.lexeme);
        } else {
            result.type = TOKEN_INT;
            result.value.int_val = atoi(currentToken.lexeme);
        }
        match(TOKEN_NUM);
    } else if (currentToken.type == TOKEN_ID) {
        char* name = strdup(currentToken.lexeme);
        SymbolEntry* symbol = lookupSymbol(name);
        if (!symbol) {
            semanticError("Undefined variable");
        }
        match(TOKEN_ID);

        if (currentToken.type == TOKEN_LBRACKET) {
            match(TOKEN_LBRACKET);

            if (currentToken.type != TOKEN_NUM) {
                semanticError("Expected integer index for array");
            }
            int index = atoi(currentToken.lexeme);
            match(TOKEN_NUM);
            match(TOKEN_RBRACKET);

            if (!symbol->is_array) {
                semanticError("Variable is not an array");
            }

            if (index < 0 || index >= symbol->array_size) {
                semanticError("Array index out of bounds");
            }

            result.type = symbol->type;
            if (symbol->type == TOKEN_INT) {
                result.value.int_val = symbol->value.int_array[index];
            } else {
                result.value.float_val = symbol->value.float_array[index];
            }
        } else {
            result.type = symbol->type;
            if (symbol->type == TOKEN_INT) {
                result.value.int_val = symbol->value.int_val;
            } else {
                result.value.float_val = symbol->value.float_val;
            }
        }

        free(name);
    } else if (currentToken.type == TOKEN_LPAREN) {
        match(TOKEN_LPAREN);
        result = expression();
        match(TOKEN_RPAREN);
    } else {
        semanticError("Invalid factor");
    }

    return result;
}

// Parse a term (handles * and /)
ExprValue term() {
    ExprValue left = factor();

    while (currentToken.type == TOKEN_MULOP) {
        char op = currentToken.lexeme[0];
        match(TOKEN_MULOP);

        ExprValue right = factor();

        if (left.type != right.type) {
            semanticError("Type mismatch in multiplication/division");
        }

        if (left.type == TOKEN_INT) {
            if (op == '*') {
                left.value.int_val *= right.value.int_val;
            } else {
                if (right.value.int_val == 0) {
                    semanticError("Division by zero");
                }
                left.value.int_val /= right.value.int_val;
            }
        } else {
            if (op == '*') {
                left.value.float_val *= right.value.float_val;
            } else {
                if (right.value.float_val == 0.0f) {
                    semanticError("Division by zero");
                }
                left.value.float_val /= right.value.float_val;
            }
        }
    }

    return left;
}

// Parse an additive expression (handles + and -)
ExprValue additiveExpression() {
    ExprValue left = term();

    while (currentToken.type == TOKEN_ADDOP) {
        char op = currentToken.lexeme[0];
        match(TOKEN_ADDOP);

        ExprValue right = term();

        if (left.type != right.type) {
            semanticError("Type mismatch in addition/subtraction");
        }

        if (left.type == TOKEN_INT) {
            left.value.int_val = (op == '+') ? left.value.int_val + right.value.int_val
                                             : left.value.int_val - right.value.int_val;
        } else {
            left.value.float_val = (op == '+') ? left.value.float_val + right.value.float_val
                                               : left.value.float_val - right.value.float_val;
        }
    }

    return left;
}

ExprValue expression() {
    ExprValue left = additiveExpression();

    // Handle relational operators
    if (currentToken.type == TOKEN_RELOP) {
        char op = currentToken.lexeme[0];
        match(TOKEN_RELOP);            // Consumes relational operator
        ExprValue right = additiveExpression(); // Parse the right-hand side
            // If no TOKEN_RELOP is found, just return the left-hand side
        if (currentToken.type != TOKEN_RELOP) {
            return left;
        }


        if (left.type != right.type) {
            semanticError("Type mismatch in comparison");
        }

        ExprValue result;
        result.type = TOKEN_INT; 

        if (left.type == TOKEN_INT) {
            if (op == '<') {
                result.value.int_val = left.value.int_val < right.value.int_val;
            } else if (op == '>') {
                result.value.int_val = left.value.int_val > right.value.int_val;
            } else if (op == '=') {
                result.value.int_val = left.value.int_val == right.value.int_val;
            }
        } else {
            if (op == '<') {
                result.value.int_val = left.value.float_val < right.value.float_val;
            } else if (op == '>') {
                result.value.int_val = left.value.float_val > right.value.float_val;
            } else if (op == '=') {
                result.value.int_val = left.value.float_val == right.value.float_val;
            }
        }

        return result;
    }

    return left;
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

    scannerin = input;
    getNextToken();

    program();

    if (currentToken.type != TOKEN_EOF) {
        semanticError("Unexpected tokens after end of program");
    }

    fclose(input);
    return 0;
}
