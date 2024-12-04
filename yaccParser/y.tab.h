/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    TOKEN_ID = 258,
    TOKEN_NUM = 259,
    TOKEN_IF = 260,
    TOKEN_ELSE = 261,
    TOKEN_INT = 262,
    TOKEN_FLOAT = 263,
    TOKEN_WHILE = 264,
    TOKEN_PROGRAM = 265,
    TOKEN_THEN = 266,
    TOKEN_RELOP = 267,
    TOKEN_ADDOP = 268,
    TOKEN_MULOP = 269,
    TOKEN_ASSIGN = 270,
    TOKEN_LBRACE = 271,
    TOKEN_RBRACE = 272,
    TOKEN_LPAREN = 273,
    TOKEN_RPAREN = 274,
    TOKEN_LBRACKET = 275,
    TOKEN_RBRACKET = 276,
    TOKEN_SEMICOLON = 277,
    LOWER_THAN_ELSE = 278
  };
#endif
/* Tokens.  */
#define TOKEN_ID 258
#define TOKEN_NUM 259
#define TOKEN_IF 260
#define TOKEN_ELSE 261
#define TOKEN_INT 262
#define TOKEN_FLOAT 263
#define TOKEN_WHILE 264
#define TOKEN_PROGRAM 265
#define TOKEN_THEN 266
#define TOKEN_RELOP 267
#define TOKEN_ADDOP 268
#define TOKEN_MULOP 269
#define TOKEN_ASSIGN 270
#define TOKEN_LBRACE 271
#define TOKEN_RBRACE 272
#define TOKEN_LPAREN 273
#define TOKEN_RPAREN 274
#define TOKEN_LBRACKET 275
#define TOKEN_RBRACKET 276
#define TOKEN_SEMICOLON 277
#define LOWER_THAN_ELSE 278

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 18 "parser.y"

    char* string_val;
    int int_val;

#line 108 "y.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
