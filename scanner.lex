%{
#include "source.hpp"
#include "parser.tab.hpp"
#include "hw3_output.hpp"
int parenthesis_depth = 0;
%}

%option noyywrap
%option yylineno

digit   ([0-9])
letter  ([a-zA-Z])
whitespace ([ \t\n\r])

%%
int         return INT;
byte        return BYTE;
b           return B;
bool        return BOOL;
and         return AND;
or          return OR;
not         return NOT;
true        { yylval = new BoolLex("BOOL", true); return TRUE; }
false       { yylval = new BoolLex("BOOL", false); return FALSE; }
return      return RETURN;
if          return IF;
else        return ELSE;
while       return WHILE;
break       return BREAK;
continue    return CONTINUE;
;           return SC;
"("             { parenthesis_depth++; return LPAREN; }
")"             { if (parenthesis_depth > 0) parenthesis_depth--; return RPAREN; }
","             { if (parenthesis_depth > 0) { output::errorLex(yylineno); exit(0); } 
                  else return COMMA; }
\{          return LBRACE;
\}          return RBRACE;
=           return ASSIGN;
==|!=                          { yylval = new Relop("RELOP", yytext); return equality;}
(<|>|<=|>=)                    { yylval = new Relop("RELOP", yytext); return relational;}
(\+|\-)                        { yylval = new Binop("BINOP", yytext); return additive; }
(\*|\/)                        { yylval = new Binop("BINOP", yytext); return multiplicative;}
{letter}({letter}|{digit})*    { yylval = new IdentifierStr("ID", yytext); return ID; }
0|[1-9]{digit}* {
    if (strlen(yytext) > 10 || (strlen(yytext) == 10 && strcmp(yytext, "2147483647") > 0)) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    yylval = new Num("NUM", stoi(yytext));
    return NUM;
}
\"([^\n\r\"\\]|\\[rnt"\\])+\"  { yylval = new StringLex("STRING", yytext); return STRING; }
\/\/[^\r\n]*[\r\n|\n]?         ; /* Ignore comments */
{whitespace}                   ; /* Ignore whitespace */
.                              { output::errorLex(yylineno); exit(0); }

%%