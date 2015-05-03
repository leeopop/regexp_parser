/*
 * Automata.hpp
 *
 *  Created on: 2015. 5. 2.
 *      Author: 근홍
 */

#ifndef SRC_AUTOMATA_HPP_
#define SRC_AUTOMATA_HPP_

#include "node.h"
#include <cstring>
#include <string>
#include <list>
#include <cassert>
#include <vector>
#include <set>
#include <map>
#include <list>

class Automata
{
private:
	std::set<char> vocab;
	std::set<std::string> states;
	std::string start_state;
	std::set<std::string> final_states;
	std::multimap<std::pair<std::string, char>, std::string> rules;

	std::string current_state;
public:

	typedef std::multimap<std::pair<std::string, char>, std::string> Rule;
	Automata(std::set<std::string> states,
			std::string start_state,
			std::set<std::string> final_states,
			std::multimap<std::pair<std::string, char>, std::string> rules)
	{
		this->vocab = {
				'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
				'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F',
				'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
				'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
				' ', '|', '+', '.', '*', '(', ')', '[', ']', '\"', '\'', '?', '^', '\\'
		};
		this->states = states;
		this->start_state = start_state;
		this->final_states = final_states;
		this->rules = rules;
	}
	Automata(const std::set<char> &vocab,
			std::set<std::string> states,
			std::string start_state,
			std::set<std::string> final_states,
			std::multimap<std::pair<std::string, char>, std::string> rules)
	{
		this->vocab = vocab;
		this->states = states;
		this->start_state = start_state;
		this->final_states = final_states;
		this->rules = rules;
	}

	void printAutomata(const std::string& name, std::ostream& out)
	{
		out<<"digraph "<<name<<" {"<<std::endl;
		out<<start_state<<"\t[label=\""<<start_state<<"\", fillcolor=\"blue\"];"<<std::endl;
		for(auto node : final_states)
		{
			out<<node<<"\t[label=\""<<node<<"\", fillcolor=\"green\"];"<<std::endl;
		}
		for(auto node : states)
		{
			if(node == start_state)
				continue;
			if(final_states.find(node) != final_states.end())
				continue;
			out<<node<<"\t[label=\""<<node<<"\", fillcolor=\"black\"];"<<std::endl;
		}
		//int uniq_edge = 0;
		for(auto edge : rules)
		{
			const std::string& from = edge.first.first;
			std::string symbol(1, edge.first.second);
			const std::string& to = edge.second;
			std::string color = "black";

			if(edge.first.second == 0)
			{
				symbol = "_Epsilon";
				color = "orange";
			}

			out<<from<<"\t->\t"<<to<<"\t[label=\""<<symbol<</*"_["<<uniq_edge++<<"]"<<*/"\", color=\""<<color<<"\"];"<<std::endl;
		}
		out<<"}"<<std::endl;
	}

	const std::set<std::string>& getFinalStates() const {
		return final_states;
	}

	const std::multimap<std::pair<std::string, char>, std::string>& getRules() const {
		return rules;
	}

	const std::string& getStartState() const {
		return start_state;
	}

	const std::set<std::string>& getStates() const {
		return states;
	}

	const std::set<char>& getVocab() const {
		return vocab;
	}

	bool evaluate(std::string input)
	{
		return false;
	}

	static Automata createEpsilonAutomata(std::set<char> vocab, const Node* currentNode)
	{
		int uniq_index = 0;
		return generateAutomata(vocab, currentNode, uniq_index);
	}
	static Automata createEpsilonAutomata(const Node* currentNode)
	{
		int uniq_index = 0;
		std::set<char> vocab = {
				'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
				'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F',
				'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
				'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
				' ', '|', '+', '.', '*', '(', ')', '[', ']', '\"', '\'', '?', '^', '\\'
		};
		return generateAutomata(vocab, currentNode, uniq_index);
	}

	static Automata removeAmbiguity(const Automata& orig)
	{
		char buf[4096];
		std::set<std::string> all_state;
		std::set<std::string> final_state;
		std::string start_state;
		Rule rules;
		auto vocab = orig.getVocab();
		auto orig_rules = orig.getRules();

		start_state = orig.getStartState();
		for(auto finals : orig.getFinalStates())
			final_state.insert(finals);
		for(auto state : orig.getStates())
		{
			all_state.insert(state);
			for(auto voc : vocab)
			{
				auto range = orig_rules.equal_range(make_pair(state, voc));
				std::list<std::string> ambiguous;
				auto iter = range.first;
				while(iter != range.second)
				{
					ambiguous.push_back(iter->second);
					iter++;
				}
				if(ambiguous.size() > 1)
				{
					snprintf(buf, sizeof(buf), "%s_remove_ambiguity_with_%c", state.c_str(), voc);
					std::string temp_state_name = std::string(buf);
					rules.insert(make_pair(make_pair(state, voc), temp_state_name));
					for(auto ambiguous_state : ambiguous)
						rules.insert(make_pair(make_pair(temp_state_name, 0), ambiguous_state));
				}
				else if(ambiguous.size() == 1)
				{
					rules.insert(make_pair(make_pair(state, voc), ambiguous.front()));
				}
			}
			auto range = orig_rules.equal_range(make_pair(state, 0));
			auto iter = range.first;
			while(iter != range.second)
			{
				rules.insert(*iter);
				iter++;
			}
		}

		return Automata(vocab, all_state, start_state, final_state, rules);
	}
private:
	bool evaluate(std::string current_state, std::string input)
	{
		if(input.length() == 0)
			if(final_states.find(current_state) != final_states.end())
				return true;

		//e-transitions
		auto range = rules.equal_range(make_pair(current_state, 0));
		auto iter = range.first;
		while(iter != rules.end())
		{
			std::string next_state = iter->second;
			bool ret = evaluate(next_state, input);
			if(ret == true)
				return true;
			iter++;
		}

		char first_char = input[0];
		std::string remaining = input.substr(1);
		range = rules.equal_range(make_pair(current_state, first_char));
		iter = range.first;
		while(iter != range.second)
		{
			std::string next_state = iter->second;
			bool ret = evaluate(next_state, remaining);
			if(ret == true)
				return true;
			iter++;
		}

		return false;
	}

	static Automata generateAutomata(std::set<char> vocab, const Node* currentNode, int& uniq_index)
	{
		char buf[4096];
		switch(currentNode->type)
		{
		case VOCABULARY:
		{
			snprintf(buf, sizeof(buf), "State_%d_Verb_%c_start", uniq_index, currentNode->data);
			std::string start_state_name = std::string(buf);
			snprintf(buf, sizeof(buf), "State_%d_Verb_%c_final", uniq_index, currentNode->data);
			std::string final_state_name = std::string(buf);
			uniq_index++;

			std::multimap<std::pair<std::string, char>, std::string> rules;
			rules.insert(make_pair(make_pair(start_state_name, currentNode->data), final_state_name));

			std::set<std::string> states;
			states.insert(start_state_name);
			states.insert(final_state_name);

			std::set<std::string> final_states;
			final_states.insert(final_state_name);
			return Automata(vocab,states, start_state_name, final_states, rules);
		}
		case CONCAT:
		{
			snprintf(buf, sizeof(buf), "State_%d_Concat_start", uniq_index);
			std::string start_state_name = std::string(buf);
			snprintf(buf, sizeof(buf), "State_%d_Concat_final", uniq_index);
			std::string final_state_name = std::string(buf);
			int current_uniq = uniq_index;
			uniq_index++;


			std::set<std::string> states;
			states.insert(start_state_name);
			states.insert(final_state_name);

			std::multimap<std::pair<std::string, char>, std::string> rules;
			const Node* currentChild = currentNode->child;

			int mid_index = 0;
			std::string mid_state_name = start_state_name;
			while(currentChild != nullptr)
			{
				Automata currentAutomata = generateAutomata(vocab, currentChild, uniq_index);
				rules.insert(make_pair(make_pair(mid_state_name, 0), currentAutomata.getStartState()));

				if(currentChild->sibling != nullptr)
				{
					snprintf(buf, sizeof(buf), "State_%d_Concat_mid_%d", current_uniq, mid_index);
					mid_state_name = std::string(buf);
				}
				else
				{
					mid_state_name = final_state_name;
				}
				states.insert(mid_state_name);

				for(auto finals : currentAutomata.final_states)
					rules.insert(make_pair(make_pair(finals, 0), mid_state_name));
				for(auto child_rule : currentAutomata.getRules())
					rules.insert(child_rule);
				for(auto child_state : currentAutomata.getStates())
					states.insert(child_state);

				mid_index++;
				currentChild = currentChild->sibling;
			}

			std::set<std::string> final_states;
			final_states.insert(final_state_name);
			return Automata(vocab,states, start_state_name, final_states, rules);
		}
		case UNION:
		{
			snprintf(buf, sizeof(buf), "State_%d_Concat_start", uniq_index);
			std::string start_state_name = std::string(buf);
			snprintf(buf, sizeof(buf), "State_%d_Concat_final", uniq_index);
			std::string final_state_name = std::string(buf);
			uniq_index++;


			std::set<std::string> states;
			states.insert(start_state_name);
			states.insert(final_state_name);

			std::multimap<std::pair<std::string, char>, std::string> rules;
			const Node* currentChild = currentNode->child;

			std::string mid_state_name = start_state_name;
			while(currentChild != nullptr)
			{
				Automata currentAutomata = generateAutomata(vocab, currentChild, uniq_index);
				rules.insert(make_pair(make_pair(start_state_name, 0), currentAutomata.getStartState()));

				for(auto finals : currentAutomata.final_states)
					rules.insert(make_pair(make_pair(finals, 0), final_state_name));
				for(auto child_rule : currentAutomata.getRules())
					rules.insert(child_rule);
				for(auto child_state : currentAutomata.getStates())
					states.insert(child_state);

				currentChild = currentChild->sibling;
			}

			std::set<std::string> final_states;
			final_states.insert(final_state_name);
			return Automata(vocab,states, start_state_name, final_states, rules);
		}
		case CLOSURE:
		{
			//name = std::string("<CLOSURE[*]>");
			Automata childAutomata = generateAutomata(vocab, currentNode->child, uniq_index);

			snprintf(buf, sizeof(buf), "State_%d_Closure_start", uniq_index);
			std::string start_state_name = std::string(buf);
			snprintf(buf, sizeof(buf), "State_%d_Closure_final", uniq_index);
			std::string final_state_name = std::string(buf);
			uniq_index++;

			std::string child_start_state = childAutomata.getStartState();

			std::set<std::string> states;
			states.insert(start_state_name);
			states.insert(final_state_name);
			for(auto child_state : childAutomata.getStates())
				states.insert(child_state);

			std::multimap<std::pair<std::string, char>, std::string> rules;

			//0 times
			rules.insert(make_pair(make_pair(start_state_name, 0), final_state_name));

			{//repeat
				rules.insert(make_pair(make_pair(start_state_name, 0), child_start_state));
				for(auto states : childAutomata.getFinalStates())
				{
					rules.insert(make_pair(make_pair(states, 0), final_state_name));
					rules.insert(make_pair(make_pair(states, 0), child_start_state));
				}
			}

			for(auto sub_rules : childAutomata.getRules())
				rules.insert(sub_rules);
			for(auto child_state : childAutomata.getStates())
				states.insert(child_state);

			std::set<std::string> final_states;
			final_states.insert(final_state_name);
			return Automata(vocab,states, start_state_name, final_states, rules);
		}
		case CLOSURE_LEAST_ONCE:
		{
			//name = std::string("<CLOSURE[+]>");
			Automata childAutomata = generateAutomata(vocab, currentNode->child, uniq_index);

			snprintf(buf, sizeof(buf), "State_%d_Closure_start", uniq_index);
			std::string start_state_name = std::string(buf);
			snprintf(buf, sizeof(buf), "State_%d_Closure_final", uniq_index);
			std::string final_state_name = std::string(buf);
			uniq_index++;

			std::string child_start_state = childAutomata.getStartState();

			std::set<std::string> states;
			states.insert(start_state_name);
			states.insert(final_state_name);
			for(auto child_state : childAutomata.getStates())
				states.insert(child_state);

			std::multimap<std::pair<std::string, char>, std::string> rules;

			//0 times
			//rules.insert(make_pair(make_pair(start_state_name, 0), final_state_name));

			{//repeat
				rules.insert(make_pair(make_pair(start_state_name, 0), child_start_state));
				for(auto states : childAutomata.getFinalStates())
				{
					rules.insert(make_pair(make_pair(states, 0), final_state_name));
					rules.insert(make_pair(make_pair(states, 0), child_start_state));
				}
			}

			for(auto sub_rules : childAutomata.getRules())
				rules.insert(sub_rules);
			for(auto child_state : childAutomata.getStates())
				states.insert(child_state);

			std::set<std::string> final_states;
			final_states.insert(final_state_name);
			return Automata(vocab,states, start_state_name, final_states, rules);
		}
		case CLOSURE_ONCE:
		{
			//name = std::string("<CLOSURE[?]>");
			Automata childAutomata = generateAutomata(vocab, currentNode->child, uniq_index);

			snprintf(buf, sizeof(buf), "State_%d_Closure_start", uniq_index);
			std::string start_state_name = std::string(buf);
			snprintf(buf, sizeof(buf), "State_%d_Closure_final", uniq_index);
			std::string final_state_name = std::string(buf);
			uniq_index++;

			std::string child_start_state = childAutomata.getStartState();

			std::set<std::string> states;
			states.insert(start_state_name);
			states.insert(final_state_name);
			for(auto child_state : childAutomata.getStates())
				states.insert(child_state);

			std::multimap<std::pair<std::string, char>, std::string> rules;

			//0 times
			rules.insert(make_pair(make_pair(start_state_name, 0), final_state_name));

			{//repeat
				rules.insert(make_pair(make_pair(start_state_name, 0), child_start_state));
				for(auto states : childAutomata.getFinalStates())
				{
					rules.insert(make_pair(make_pair(states, 0), final_state_name));
					//rules.insert(make_pair(make_pair(states, 0), child_start_state));
				}
			}

			for(auto sub_rules : childAutomata.getRules())
				rules.insert(sub_rules);
			for(auto child_state : childAutomata.getStates())
				states.insert(child_state);

			std::set<std::string> final_states;
			final_states.insert(final_state_name);
			return Automata(vocab,states, start_state_name, final_states, rules);
		}
		case EPSILON:
		{
			//name = std::string("[EPSILON]");

			snprintf(buf, sizeof(buf), "State_%d_Epsilon", uniq_index);
			std::string e_state_name = std::string(buf);
			uniq_index++;

			std::set<std::string> states;
			states.insert(e_state_name);

			std::multimap<std::pair<std::string, char>, std::string> rules;

			std::set<std::string> final_states;
			final_states.insert(e_state_name);
			return Automata(vocab,states, e_state_name, final_states, rules);
		}
		case CHARACTER_CLASS:
		{
			snprintf(buf, sizeof(buf), "State_%d_Character_class[%c]_start", uniq_index, currentNode->data);
			std::string start_state_name = std::string(buf);
			snprintf(buf, sizeof(buf), "State_%d_Character_class[%c]_final", uniq_index, currentNode->data);
			std::string final_state_name = std::string(buf);
			uniq_index++;

			std::multimap<std::pair<std::string, char>, std::string> rules;
			switch(currentNode->data)
			{
			case '.':
			{
				for(char vocabulary : vocab)
					rules.insert(make_pair(make_pair(start_state_name, vocabulary), final_state_name));
				break;
			}
			default:
				assert(0); //not implemented
			}
			std::set<std::string> states;
			states.insert(start_state_name);
			states.insert(final_state_name);

			std::set<std::string> final_states;
			final_states.insert(final_state_name);
			return Automata(vocab,states, start_state_name, final_states, rules);
		}
		case CHARSET:
		{
			//name = std::string("[CHARSET]");
			snprintf(buf, sizeof(buf), "State_%d_CHARSET_start", uniq_index);
			std::string start_state_name = std::string(buf);
			snprintf(buf, sizeof(buf), "State_%d_CHARSET_final", uniq_index);
			std::string final_state_name = std::string(buf);
			uniq_index++;

			std::multimap<std::pair<std::string, char>, std::string> rules;
			auto child_node = currentNode->child;
			while(child_node != nullptr)
			{
				assert(child_node->type == VOCABULARY);
				rules.insert(make_pair(make_pair(start_state_name, child_node->data), final_state_name));
				child_node = child_node->sibling;
			}

			std::set<std::string> states;
			states.insert(start_state_name);
			states.insert(final_state_name);

			std::set<std::string> final_states;
			final_states.insert(final_state_name);
			return Automata(vocab,states, start_state_name, final_states, rules);
		}
		case NEGATIVE_CHARSET:
		{
			//name = std::string("[^CHARSET]");
			snprintf(buf, sizeof(buf), "State_%d_NEGATIVE_CHARSET_start", uniq_index);
			std::string start_state_name = std::string(buf);
			snprintf(buf, sizeof(buf), "State_%d_NEGATIVE_CHARSET_final", uniq_index);
			std::string final_state_name = std::string(buf);
			uniq_index++;

			std::multimap<std::pair<std::string, char>, std::string> rules;
			std::set<char> allVoca = vocab;
			auto child_node = currentNode->child;
			while(child_node != nullptr)
			{
				assert(child_node->type == VOCABULARY);
				vocab.erase(child_node->data);
				child_node = child_node->sibling;
			}
			for(auto voc : allVoca)
				rules.insert(make_pair(make_pair(start_state_name, voc), final_state_name));

			rules.insert(make_pair(make_pair(start_state_name, child_node->data), final_state_name));

			std::set<std::string> states;
			states.insert(start_state_name);
			states.insert(final_state_name);

			std::set<std::string> final_states;
			final_states.insert(final_state_name);
			return Automata(vocab,states, start_state_name, final_states, rules);
		}
		case EMPTY:
		{
			//name = std::string("[EMPTY SET]");
			snprintf(buf, sizeof(buf), "State_%d_Empty_start", uniq_index);
			std::string e_start_state_name = std::string(buf);
			snprintf(buf, sizeof(buf), "State_%d_Empty_start", uniq_index);
			std::string e_final_state_name = std::string(buf);
			uniq_index++;

			std::set<std::string> states;
			states.insert(e_start_state_name);
			states.insert(e_final_state_name);

			std::multimap<std::pair<std::string, char>, std::string> rules;

			std::set<std::string> final_states;
			final_states.insert(e_final_state_name);
			return Automata(vocab,states, e_start_state_name, final_states, rules);
		}
		default:
			assert(0);
		}

		assert(0);
		return Automata(vocab, std::set<std::string>(), std::string(), std::set<std::string>(), Automata::Rule());
	}
};


#endif /* SRC_AUTOMATA_HPP_ */
