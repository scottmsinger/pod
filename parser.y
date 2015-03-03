    /********************************************************************/
    /* Options Section */
    /********************************************************************/

%require "3.0"
%defines "parser.h"
%define api.pure full
%define parse.error verbose
%locations
%lex-param{void *scanner}
%parse-param{void *scanner}
%parse-param{TipPod::LexerContext *ctx} /* Causes 'ctx' variable to be available in 
                                           semantic actions code below */

%code requires {

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

} /* End %code requires */



    /********************************************************************/
    /* Prologue Section */
    /********************************************************************/
%{
    #include <assert.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <string>
    #include <stdexcept>
    #include <iostream>
    #include <sstream>

    #include "lexer.h"
    #include "LexerContext.h"
    #include "TipPodNode.h"
    #include "TipPodValue.h"
    #include "TipPodBlockPodValue.h"

%}

    /********************************************************************/
    /* Bison declarations */
    /********************************************************************/

%term<_string>  T_IDENTIFIER    "identifier"
%term<_string>  T_STRING        "string"
%term<_float>   T_FLOAT         "float"
%term<_int>     T_INTEGER       "integer"
%term<_int>     T_BOOLCONST     "boolean"
%token<_value>  T_EMBED         "embed tag"
%token<_token>  T_SCOPE         "::"
%token<_token>  T_EQUAL         "="
%token<_token>  T_PERIOD        "."
%token<_token>  T_SEMICOLON     ";"
%token<_token>  T_OPENBRACE     "{"
%token<_token>  T_CLOSEBRACE    "}"
%token<_token>  T_OPENBRACKET   "["
%token<_token>  T_CLOSEBRACKET  "]"
%token END 0 "end of file"  /* Docs say "do this to make a nicer end-of-file error msg" */

%type<_string>      identifier
%type<_string>      variable_name
%type<_string>      type_name
%type<_value>       pod_value
%type<_value>       constant
%type<_value>       block
%type<_node>        pod_node

%start pod_nodes


    /********************************************************************/
    /* Grammar rules */
    /********************************************************************/
%%


pod_nodes: 
        %empty
    |
        pod_nodes pod_node 
;


pod_node: 
        type_name variable_name T_EQUAL pod_value T_SEMICOLON
        {
            TipPod::PodNode* pn = new TipPod::PodNode($2, $1, $4);
            pn->setSource(ctx->sourcefile, yyget_lineno(scanner));
            ctx->current.second.push_back(pn);
            $$ = pn;
        }
    |   
        variable_name T_EQUAL pod_value T_SEMICOLON
        {
            TipPod::PodNode* pn = new TipPod::PodNode($1, "", $3);
            pn->setSource(ctx->sourcefile, yyget_lineno(scanner));
            ctx->current.second.push_back(pn);
            $$ = pn;
        }
    | 
        type_name variable_name T_SEMICOLON
        {
            TipPod::PodNode* pn = new TipPod::PodNode($2, $1);
            pn->setSource(ctx->sourcefile, yyget_lineno(scanner));
            ctx->current.second.push_back(pn);
            $$ = pn;
        }
    | 
        pod_value T_SEMICOLON
        {
            TipPod::PodNode* pn = new TipPod::PodNode("", "", $1);
            pn->setSource(ctx->sourcefile, yyget_lineno(scanner));
            ctx->current.second.push_back(pn);
            $$ = pn;
        }
;


variable_name: 
        identifier
        { 
            $$ = $1;
        }
    |
        identifier T_OPENBRACKET T_INTEGER T_CLOSEBRACKET
        {
            std::cerr << "WARNING: Deprecated syntax '" << $1 
                      << "[" << $3 << "]'" 
                      << "in file '" << ctx->sourcefile << "', line " << yyget_lineno(scanner)
                      << std::endl;
            $$ = $1;
        }
    |
        identifier T_PERIOD identifier
        {
            $1.append("." + $3);
            $$ = $1;
        }
    |
        type_name T_SCOPE identifier
        {
            $1.append("::" + $3);
            $$ = $1;
        }
;


type_name: 
        identifier
        {
            $$ = $1;
        }
    |
        type_name T_SCOPE identifier
        {
            $1.append("::" + $3);
            $$ = $1;
        }
;

identifier: 
        T_IDENTIFIER 
        { 
            $$ = $1;
        }
;


block_begin:
        T_OPENBRACE
        {
            ctx->stack.push(ctx->current);
            ctx->current.first.clear();
            ctx->current.second.clear();
        }
    |
        type_name T_OPENBRACE
        {
            ctx->stack.push(ctx->current);
            ctx->current.first = $1;
            ctx->current.second.clear();
        }
;


block: 
        block_begin pod_nodes T_CLOSEBRACE
        {
            TipPod::PodValue* pv = new TipPod::BlockPodValue(ctx->current.second, ctx->current.first);
            ctx->current = ctx->stack.top();
            ctx->stack.pop();

            $$ = pv;
        }
;


constant:
        T_INTEGER
        {
            TipPod::PodValue* pv = new TipPod::IntPodValue(int($1));
            $$ = pv;
        }
    |
        T_FLOAT
        {
            TipPod::PodValue* pv = new TipPod::FloatPodValue(float($1));
            $$ = pv;
        }
    | 
        T_BOOLCONST
        {
            TipPod::PodValue* pv = new TipPod::BoolPodValue(bool($1));
            $$ = pv;
        }
    | 
        T_STRING
        {
            TipPod::PodValue* pv = new TipPod::StringPodValue($1);
            $$ = pv;
        }
    | 
        T_EMBED
        {
            $$ = $1;
        }
;


pod_value:  
            constant
          | block
            {
                $$ = $1;
            }
          | variable_name
            { 
                TipPod::PodValue* pv = new TipPod::IdentifierPodValue($1);
                $$ = pv;
            }
;


    /********************************************************************/

%%
    /********************************************************************/
    /* Epilogue */
    /********************************************************************/

// *****************************************************************************
void yyerror(YYLTYPE* llocp,
             yyscan_t scanner, 
             TipPod::LexerContext* ctx, 
             const char *errmsg)
{
    std::ostringstream err;

    err << errmsg;
    err << " in file '" << ctx->sourcefile;
    err << "', line " << llocp->first_line;
    err << ", column " << llocp->first_column;
    if (llocp->last_line != llocp->first_line)
    {
        err << " to line " << llocp->last_line;
        err << ", column " << llocp->last_column;
    }
    else
    {
        err << "-" << llocp->last_column;
    }
    err << std::endl;

    // TODO: Figure out how to print the whole line, rather than just
    // the start up to the error.
    err << std::endl << llocp->current_line << std::endl;
    for (size_t i = 0; i < llocp->first_column; ++i) err << " ";
    for (size_t i = llocp->first_column; i < llocp->last_column; ++i) err << "^";

    throw std::runtime_error(err.str().c_str());
}

