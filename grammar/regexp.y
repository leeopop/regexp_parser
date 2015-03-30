%code requires
{
#include "node.h"
#define YYSTYPE Node*
}

%{
#include "regexp_lex.h"
#include <assert.h>
int yyerror(const char* val);

%}

%token CHAR ESCAPED_CHAR

%start including_empty

%%

including_empty:
	regular_expression
	{
		$$ = $1;
		last_accept_node = $$;
	}
	|
	{
		Node* ret = makeNode();
		ret->data = 0;
		ret->type = EMPTY;
		$$ = ret;
		last_accept_node = $$;
	}
	;

vocabulary:
	CHAR
	{
		Node* ret = makeNode();
		ret->data = yytext[0];
		ret->type = VOCABULARY;
		$$ = ret;
	}
	|
	ESCAPED_CHAR
	{
		Node* ret = makeNode();
		assert(yytext[0] == '\\');
		ret->data = yytext[1];
		ret->type = VOCABULARY;
		$$ = ret;
	}
	;

vocabulary_list:
	vocabulary
	{
		$$ = $1;
	}
	|
	vocabulary vocabulary_list
	{
		assert($1->sibling == NULL);
		$1->sibling = $2;
		$$ = $1;
	}
	;

charset:
	'[' vocabulary_list ']'
	{
		Node* ret = makeNode();
		ret->data = 0;
		ret->type = CHARSET;
		ret->child = $2;
		$$ = ret;
	}
	|
	'[' '^' vocabulary_list ']'
	{
		Node* ret = makeNode();
		ret->data = 0;
		ret->type = NEGATIVE_CHARSET;
		ret->child = $3;
		$$ = ret;
	}
	|
	'[' ']'
	{
		Node* ret = makeNode();
		ret->data = 0;
		ret->type = EMPTY;
		$$ = ret;
	}
	|
	'.'
	{
		Node* ret = makeNode();
		ret->data = 0;
		ret->type = EPSILON;
		$$ = ret;
	}
	
regular_primary:
	'(' ')'
	{
		Node* ret = makeNode();
		ret->data = 0;
		ret->type = EMPTY;
		$$ = ret;
	}
	|
	'(' regular_expression ')'
	{
		$$ = $2;
	}
	|
	vocabulary
	{
		$$ = $1;
	}
	|
	charset
	{
		$$ = $1;
	}
	;
	
regular_factor:
	regular_primary
	{
		$$ = $1;
	}
	|
	regular_factor '?'
	{
		Node* ret = makeNode();
		ret->data = 0;
		ret->type = CLOSURE_ONCE;
		ret->child = $1;
		$$ = ret;
	}
	|
	regular_factor '+'
	{
		Node* ret = makeNode();
		ret->data = 0;
		ret->type = CLOSURE_LEAST_ONCE;
		ret->child = $1;
		$$ = ret;
	}
	|
	regular_factor '*'
	{
		Node* ret = makeNode();
		ret->data = 0;
		ret->type = CLOSURE;
		ret->child = $1;
		$$ = ret;
	}
	;
	
regular_term_child:
	regular_factor
	{
		$$ = $1;
	}
	|
	regular_factor regular_term_child
	{
		assert($1->sibling == NULL);
		$1->sibling = $2;
		$$ = $1;
	}
	;

regular_term:
	regular_term_child
	{
		if($1->sibling)
		{
			Node* ret = makeNode();
			ret->data = 0;
			ret->type = CONCAT;
			ret->child = $1;
			$$ = ret;
		}
		else
		{
			$$ = $1;
		}
	}
	;
	
regular_expression_child:
	regular_term
	{
		$$ = $1;
	}
	|
	regular_term '|' regular_expression_child
	{
		assert($1->sibling == NULL);
		$1->sibling = $3;
		$$ = $1;
	}
	;
	
regular_expression:
	regular_expression_child
	{
		if($1->sibling)
		{
			Node* ret = makeNode();
			ret->data = 0;
			ret->type = UNION;
			ret->child = $1;
			$$ = ret;
		}
		else
		{
			$$ = $1;
		}
	}
	;
	
%%


int yyerror(const char* val)
{
	printf("%d ",yyget_lineno());printf("Error occuered: %s, %s\n", val, yytext);
	return 1;
}