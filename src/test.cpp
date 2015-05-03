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
			stop = last_accept_node->type == NodeType::EMPTY;
			RegularParseTree tree(last_accept_node);
			tree.printParseTree(std::cout);

			Automata rough_automata = Automata::createEpsilonAutomata(last_accept_node);
			rough_automata.printAutomata("rough_automata", std::cout);

			Automata normal_NFA = Automata::removeAmbiguity(rough_automata);

			if(false)
			{ // works with 1*(01*0)*1*
				auto vocab = normal_NFA.getVocab();
				auto all_state = normal_NFA.getStates();
				auto start = normal_NFA.getStartState();
				auto final = normal_NFA.getFinalStates();
				auto rule = normal_NFA.getRules();

				rule.insert(make_pair(std::pair<std::string, char>("State_0_Concat_start", 'x'), "Dummy1"));
				rule.insert(make_pair(std::pair<std::string, char>("State_2_Closure_start", 'x'), "Dummy2"));
				rule.insert(make_pair(std::pair<std::string, char>("State_3_Concat_mid_0", 'x'), "Dummy3"));

				all_state.insert("Dummy1");
				all_state.insert("Dummy2");
				all_state.insert("Dummy3");
				rule.insert(make_pair(std::pair<std::string, char>("Dummy1", 'y'), "Dummy2"));
				rule.insert(make_pair(std::pair<std::string, char>("Dummy2", 'z'), "Dummy3"));

				normal_NFA = Automata(vocab, all_state, start, final, rule);
			}

			normal_NFA.printAutomata("normal_NFA", std::cout);

			Automata DFA = Automata::removeEpsilon(normal_NFA, true);
			DFA.printAutomata("DFA", std::cout);


			Automata mDFA = Automata::minimalize(DFA);
			mDFA.printAutomata("mDFA", std::cout);
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
