%option noyywrap
%option yylineno

%{
#include "minako.h"
#include <stdlib.h>
%}

LETTER      [a-zA-Z]
DIGIT       [0-9]
INTEGER     {DIGIT}+
FLOAT       {INTEGER}"."{INTEGER}|"."{INTEGER}
WHITESPACE  [ \t\n\r]

%x COMMENT_TYPE1
%x COMMENT_TYPE2
%x BEGIN_STRING
%x END_STRING

%%

"/*"                    { BEGIN (COMMENT_TYPE1); }
<COMMENT_TYPE1>.|\n     { }
<COMMENT_TYPE1>"*/"     { BEGIN (INITIAL); }
"//"                    { BEGIN (COMMENT_TYPE2); }
<COMMENT_TYPE2>.*       { }
<COMMENT_TYPE2>\n       { BEGIN (INITIAL); }

"bool"                  { return KW_BOOLEAN; }
"do"                    { return KW_DO; }
"else"                  { return KW_ELSE; }
"float"                 { return KW_FLOAT; }
"for"                   { return KW_FOR; }
"if"                    { return KW_IF; }
"int"                   { return KW_INT; }
"printf"                { return KW_PRINTF; }
"return"                { return KW_RETURN; }
"void"                  { return KW_VOID; }
"while"                 { return KW_WHILE; }

"=="                    { return EQ; }
"!="                    { return NEQ; }
"<="                    { return LEQ; }
">="                    { return GEQ; }
"&&"                    { return AND; }
"||"                    { return OR; }

"<"|">"|"+"|"-"|"*"|"/"|"="|","|";"|"("|")"|"{"|"}"             {return *yytext;}

{WHITESPACE}+                                                   {}

{FLOAT}([eE]([-+])?{INTEGER})?|{INTEGER}[eE]([-+])?{INTEGER}    {
                                                                    yylval.floatValue = atof(yytext);
                                                                    return CONST_FLOAT;
                                                                }

{INTEGER}                                                       {
                                                                    yylval.intValue = atoi(yytext);
                                                                    return CONST_INT;
                                                                }

"true"|"false"                                                  {
                                                                    if(*yytext == 't')
                                                                    {
                                                                        yylval.intValue = 1;
                                                                    }
                                                                    else
                                                                    {
                                                                        yylval.intValue = 0;
                                                                    }

                                                                    return CONST_BOOLEAN;
                                                                }

({LETTER})+({DIGIT}|{LETTER})*                                  {
                                                                    yylval.string = yytext;
                                                                    return ID;
                                                                }

\"/[^\n\"]*\"                                                   { BEGIN(BEGIN_STRING); }

<BEGIN_STRING>[^\n\"]*/\"                                       {
                                                                    BEGIN(END_STRING);
                                                                    yylval.string = yytext;
                                                                    return CONST_STRING;
                                                                }
                                                                
<END_STRING>"\""                                                { BEGIN(INITIAL); }

<<EOF>>                                                         {return EOF;}

.                                                               {printf("Unknown token: %s\n", yytext);exit(-1);}
%%