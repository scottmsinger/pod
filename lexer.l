    /********************************************************************/
    /* Options Section */
    /********************************************************************/

%option bison-bridge
%option header-file="lexer.h"
%option reentrant
%option noyywrap
%option bison-locations
%option yylineno
%option extra-type="TipPod::LexerContext *"

    /********************************************************************/
    /* Preamble Section */
    /********************************************************************/
%top{

#include <sstream>
#include <stdexcept>
#include "LexerContext.h"
#include "TipPodNode.h"
#include "TipPodUtils.h"
#include "TipPodValue.h"
#include "parser.h"

    /*
        The YY_USER_ACTION macro lets us manually track the current 
        line and column.  See also 'struct YYLTYPE' in parser.y.
    */
#define YY_USER_ACTION {yylloc->first_line = yylineno;                              \
                        yylloc->first_column = yylloc->current_column;              \
                        yylloc->current_column = yylloc->current_column + yyleng;   \
                        yylloc->last_column = yylloc->current_column;               \
                        yylloc->last_line = yylineno;                               \
                        yylloc->current_line += yytext;}
} 
    /* End %top */


    /********************************************************************/
    /* Definitions Section */
    /********************************************************************/

True              [Tt][Rr][Uu][Ee]
False             [Ff][Aa][Ll][Ss][Ee]
Integer           [-+]?[0-9]+
Float             ([-+]?([0-9]*)?\.[0-9]*([eE][-+]?[0-9]+)?)|([-+]?([0-9]+)(\.[0-9]*)?[eE][-+]?[0-9]+)
Identifier        [A-Za-z_][A-Za-z_0-9]*
Equal             =
NewLine           \r?\n
OpenTag           \<[A-Za-z_][A-Za-z_0-9]*\>
CloseTag          \<\/[A-Za-z_][A-Za-z_0-9]*\>
Whitespace        [ \t\n]
Period            \.


%x EMBED
%x COMMENT
%x STRING

%%
    /********************************************************************/
    /* Rules Section */
    /********************************************************************/

{NewLine}               { yylloc->newline(); }

{Whitespace}            ;    /* Skip whitespace */

{Period}                { return T_PERIOD; }

{True}|{False}          { yylval->_int = (yytext[0] == 't' || yytext[0] == 'T');
                          return T_BOOLCONST; }

{Identifier}            { yylval->_string = yytext;
                          return T_IDENTIFIER; }

{Float}                 { if (TipPod::stringToFloat(yytext, yylval->_float))
                          {
                              return T_FLOAT; 
                          }
                          else
                          {
                              std::ostringstream err;
                              err << "Out of range value: " << yytext;
                              yyerror(yylloc, yyscanner, yyextra, err.str().c_str());
                          }
                        }

{Integer}               { if (TipPod::stringToInt(yytext, yylval->_int))
                          {
                            return T_INTEGER; 
                          }
                          else
                          {
                              std::ostringstream err;
                              err << "Out of range value: " << yytext;
                              yyerror(yylloc, yyscanner, yyextra, err.str().c_str());
                          }
                        }
{Equal}                 { return T_EQUAL; }

"::"                    { return T_SCOPE; }

";"                     { return T_SEMICOLON; }

"{"                     { return T_OPENBRACE; }

"}"                     { return T_CLOSEBRACE; }

"["                     { return T_OPENBRACKET; }

"]"                     { return T_CLOSEBRACKET; }


    /********************************/
    /* Emebedded code (qslang, etc) */
    /********************************/

{OpenTag}               { std::string lang(yytext);
                          lang = lang.substr(1, lang.size() - 2); /* Strip off the angle brackets */
                          yylval->_value = new TipPod::EmbedPodValue("", lang);
                          yylval->_string.clear();
                          BEGIN EMBED; 
                        }

<EMBED>{CloseTag}       { TipPod::EmbedPodValue* ev = dynamic_cast<TipPod::EmbedPodValue*>(yylval->_value);
                          if (yytext == "</" + ev->language() + ">")
                          {
                              /* Set accumulated text, and remove common leading whitespace */
                              ev->setValue(yylval->_string);
                              yylval->_string.clear();
                              ev->dedent();

                              /* We discard the text in the closing tag */
                              BEGIN 0; 
                              return T_EMBED;
                          }
                          else
                          {
                              /* Looked like a close-tag, but didn't match the open tag
                                exactly, so just accumulate it */
                              yylval->_string += yytext;
                          }
                        }

<EMBED>{NewLine}        { /* Accumulate text within the <> and </> tags */
                           yylval->_string += yytext;
                           yylloc->newline();
                        }

<EMBED>.                {  /* Accumulate text within the <> and </> tags */
                           yylval->_string += yytext;
                        }


    /************/
    /* Comments */
    /************/

"//".*{NewLine}         { yylloc->newline(); } /* C++ style comment */
"#".*{NewLine}          { yylloc->newline(); } /* Script style comment */
"/*"                    { BEGIN COMMENT; }     /* Begin C-style block comment */
<COMMENT>{NewLine}      { yylloc->newline(); } 
<COMMENT>.              ;                      /* do nothing in comments */
<COMMENT>"*/"           { BEGIN 0; } ;         /* end C-style block comment */



    /************/
    /* Strings */
    /************/


"\""                    { yylval->_string.clear();
                          BEGIN STRING;           
                        }

<STRING>{NewLine}       { yylval->_string += "\n"; yylloc->newline(); }
<STRING>"\\b"           { yylval->_string += "\b"; }
<STRING>"\\t"           { yylval->_string += "\t"; }
<STRING>"\\n"           { yylval->_string += "\n"; }
<STRING>"\\f"           { yylval->_string += "\f"; }
<STRING>"\\r"           { yylval->_string += "\r"; }
<STRING>"\\\""          { yylval->_string += "\""; }
<STRING>"\'"            { yylval->_string += "'"; }
<STRING>"\\\\"          { yylval->_string += "\\"; }
<STRING>"\""            {
                          BEGIN 0;
                          return T_STRING;
                        }
<STRING>.               { yylval->_string += yytext;}


<<EOF>>                 { yyterminate(); }

.                       { printf("Unknown token: '%s'\n", yytext); yyterminate(); }



%%
    /********************************************************************/
    /* User Code Section */
    /********************************************************************/
