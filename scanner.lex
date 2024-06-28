%{
/* Declaration section */
#include <stdio.h>
#include "parser.tab.hpp"
#include "output.hpp"
using namespace output;
%}
%option noyywrap
%option yylineno
digit ([0-9])
positive ([1-9])
whitespace ([\t\n\r ])
letter ([a-zA-Z])

%%
int                                         return INT;
byte                                        return BYTE;
b                                           return B;
bool                                        return BOOL;
and                                         return AND;
or                                          return OR;
not                                         return NOT;
true                                        return TRUE;
false                                       return FALSE; 
return                                      return RETURN; 
if                                          return IF;
else                                        return ELSE; 
while                                       return WHILE; 
break                                       return BREAK;
continue                                    return CONTINUE; 
;                                           return SC;
\(                                          return LPAREN;
\)                                          return RPAREN;
\{                                          return LBRACE;
\}                                          return RBRACE;
=                                           return ASSIGN;
==|!=                                       return equality;
(<|>|<=|>=)                                   return relational;
(\+|\-)                                         return additive;
(\*|\/)                                         return multiplicative;
{letter}({letter}|{digit})*                 return ID;
({positive}{digit}*|0)                      return NUM;
\"([^\n\r\"\\]|\\[rnt"\\])+\"               return STRING;
\/\/[^\r\n]*[\r|\n|\r\n]?                    ;
{whitespace}                                 ;
.                                           {errorLex(yylineno); exit(0);}
