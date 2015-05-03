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

#include "RegularParseTree.hpp"
#include "Automata.hpp"

int main()
{
	while(1)
	{
		yyrestart(stdin);
		bool stop = false;
		if(yyparse() == 0)
		{
			std::set<char> vocab = {
					'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
					'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F',
					'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
					'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
					//,' ', '|', '+', '.', '*', '(', ')', '[', ']', '\"', '\'', '?', '^', '\\'
			};

			stop = last_accept_node->type == NodeType::EMPTY;
			RegularParseTree tree(last_accept_node);
			tree.printParseTree(std::cout);

			Automata rough_automata = Automata::createEpsilonAutomata(vocab, last_accept_node);
			rough_automata.printAutomata("rough_automata", std::cout);

			Automata normal_NFA = Automata::removeAmbiguity(rough_automata);
			normal_NFA.printAutomata("normal_NFA", std::cout);

			Automata DFA = Automata::removeEpsilon(normal_NFA, true);
			DFA.printAutomata("DFA", std::cout);
		}
		clearNode();
		yylex_destroy();
		if(stop)
			break;
	}
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
