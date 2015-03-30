/*
 * test.cpp
 *
 *  Created on: 2015. 3. 30.
 *      Author: 근홍
 */

extern "C"
{
#include "regexp_lex.h"
#include "regexp_yacc.h"
}


int main()
{
	printf("%d\n", yyparse());
	/*
	while(true)
	{
		int ret = yylex();
		if(ret == 0)
		{
			fflush(0);
			yyrestart(stdin);
			continue;
		}
		if(ret == -1)
		{
			printf("Error:\n");
			break;
		}
		if(ret == yytokentype::CHAR)
		{
			printf("CHAR:%s\n", yytext);
		}
		else if(ret == yytokentype::ESCAPED_CHAR)
		{
			printf("ESCAPED_CHAR:%s\n", yytext);
		}
		else
		{
			printf("ELSE:%s\n", yytext);
		}
	}
	*/
	return 0;
}
