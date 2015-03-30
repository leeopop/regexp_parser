%{
#include "regexp_yacc.h"
%}

BACKSLASH [\\]
OPERATOR [\|\+\.\*\(\)\[\]\"\'\?\^]
SPECIAL [abfnrtv\\]
NORMAL_WORD [A-Za-z0-9 ]

%%

{BACKSLASH}{SPECIAL} { return ESCAPED_CHAR; }
{BACKSLASH}{OPERATOR} { return ESCAPED_CHAR; }
{BACKSLASH}{NORMAL_WORD} { YY_FATAL_ERROR("invalid character after backslash"); }
{OPERATOR} { return yytext[0]; }
{NORMAL_WORD} { return CHAR; }
"\n" { yyterminate(); }

%%

int yywrap()
{
	return 1;
}
