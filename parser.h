/* A Bison parser, made by GNU Bison 3.0.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

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

#ifndef YY_YY_PARSER_H_INCLUDED
# define YY_YY_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 15 "parser.y" /* yacc.c:1909  */


#include <cstdarg>
#include "LexerContext.h"

typedef void* yyscan_t;

    /* 
       The YYLTYPE struct takes the place of bison's built-in struct of
       the same name.  Doing this allows us to also keep track of the 
       current column in a re-entrant manner (without the use of a 
       global 'int current_column').  See also #define YY_USER_ACTION 
       in lexer.l
    */
struct YYLTYPE
{
    int first_line;
    int last_line;
    int first_column;
    int last_column;
    std::string sourcefile;

    void newline() { current_column = 0; current_line.clear(); }
    int         current_column;
    std::string current_line;
};
#define YYLTYPE_IS_DECLARED 1


    /* 
       The YYSTYPE struct takes the place of the more common '%union'
       declaration.  Doing this allows easier use of non-pod 
       types like std::string at the expense of trivially more memory
       usage.
    */
struct YYSTYPE
{
    YYSTYPE() : _token(0), _int(0), _float(0.0f), _string(), _node(NULL), _value(NULL) {}

    int                     _token;
    int                     _int;
    float                   _float;
    std::string             _string;
    TipPod::PodNode*        _node;
    TipPod::PodValue*       _value;
};

void yyerror(YYLTYPE* llocp,
             yyscan_t scanner, 
             TipPod::LexerContext* ctx, 
             const char *errmsg);


#line 98 "parser.h" /* yacc.c:1909  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    END = 0,
    T_IDENTIFIER = 258,
    T_STRING = 259,
    T_FLOAT = 260,
    T_INTEGER = 261,
    T_BOOLCONST = 262,
    T_EMBED = 263,
    T_SCOPE = 264,
    T_EQUAL = 265,
    T_PERIOD = 266,
    T_SEMICOLON = 267,
    T_OPENBRACE = 268,
    T_CLOSEBRACE = 269,
    T_OPENBRACKET = 270,
    T_CLOSEBRACKET = 271
  };
#endif

/* Value type.  */

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



int yyparse (void *scanner, TipPod::LexerContext *ctx);

#endif /* !YY_YY_PARSER_H_INCLUDED  */
