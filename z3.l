%{
#include <stdlib.h>
#include <string.h>
#include "def.tab.hh"
extern int yylineno;
int yyerror(const char *msg,...);
%}
%%
"If"					  			{return IF;}
"Else"								{return ELSE;}
"While"								{return WHILE;}
"Read"							{return INPUT;}
"Print"							{return PRINT;}
"Int"									{return INT;}
"Flt"									{return FLT;}
"Str"									{return STR;}
"while"								{return WHILE;}
\<\=								{return LEQ;}
\=\>								{return GEQ;}
\=\=								{return EQ;}
\!\=								{return NEQ;}
\;								{return ';';}
\<								{return '<';}
\>								{return '>';}
\+								{return '+';}
\=                            					{return '=';}
\*								{return '*';}
\(								{return '(';}
\{								{return '{';}
\}								{return '}';}
\)								{return ')';}
\[								{return '[';}
\]								{return ']';}
\/								{return '/';}
\-								{return '-';}
\^								{return '^';}

(([1-9][0-9]*)|(0))[.][0-9]*     {yylval.fval = atof(yytext);
									return LZ;}
[1-9][0-9]*|0							{yylval.ival = atoi(yytext);
									return LC;}

\".*\"				{yylval.text = strdup(yytext); return TEXT;}
[#][a-zA-Z_][a-zA-Z_0-9]*				{yylval.text = strdup(yytext);
									return ID;}
\ |\t								{;}
\n									{yylineno++ ;}
.									{yyerror("Blad leksykalny\n");}
%%
int yyerror(const char *msg,...)
{
	printf("%d: %s", yylineno, msg);
	printf("near: %s", yytext);
	exit(1);
}
