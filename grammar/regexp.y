
%{
#include "regexp_lex.h"
%}

%token CHAR ESCAPED_CHAR

%start program

%%
program:
	;