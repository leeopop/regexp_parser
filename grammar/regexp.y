
%{
#include "regexp_lex.h"
int yyerror(const char* val);
%}

%token CHAR ESCAPED_CHAR

%start program

%%
program:
	;
	
%%


int yyerror(const char* val)
{
	printf("%d ",yyget_lineno());printf("Error occuered: %s, %s\n", val, yytext);
	return 1;
}