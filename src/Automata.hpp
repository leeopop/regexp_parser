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
#include <set>
#include <map>
#include <map>

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
	const std::set<char> default_vocabulary = {
			'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
			'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F',
			'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
			'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
			' ', '|', '+', '.', '*', '(', ')', '[', ']', '\"', '\'', '?', '^', '\\'
	};

	typedef std::multimap<std::pair<std::string, char>, std::string> Rule;
	Automata(std::set<char> vocab,
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
private:
	bool evaluate(std::string current_state, std::string input)
	{
		if(input.length() == 0)
			if(final_states.find(current_state) != final_states.end())
				return true;

		//e-transitions
		std::pair<std::string, char> e_pair(current_state, 0);
		auto iter = rules.find(e_pair);
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
		std::pair<std::string, char> current_pair(current_state, first_char);
		iter = rules.find(e_pair);
		while(iter != rules.end())
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
			snprintf(buf, sizeof(buf), "%d_Verb[%c]_start", uniq_index, currentNode->data);
			std::string start_state_name = std::string(buf);
			snprintf(buf, sizeof(buf), "%d_Verb[%c]_final", uniq_index, currentNode->data);
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
			Automata leftAutomata = generateAutomata(vocab, currentNode->child, uniq_index);
			Automata rightAutomata = generateAutomata(vocab, currentNode->child->sibling, uniq_index);

			snprintf(buf, sizeof(buf), "%d_Concat_start", uniq_index);
			std::string start_state_name = std::string(buf);
			snprintf(buf, sizeof(buf), "%d_Concat_mid", uniq_index);
			std::string mid_state_name = std::string(buf);
			snprintf(buf, sizeof(buf), "%d_Concat_final", uniq_index);
			std::string final_state_name = std::string(buf);
			uniq_index++;

			std::string left_start_state = leftAutomata.getStartState();
			std::string right_start_state = rightAutomata.getStartState();

			std::set<std::string> states;
			states.insert(start_state_name);
			states.insert(mid_state_name);
			states.insert(final_state_name);
			for(auto left_state : leftAutomata.getStates())
				states.insert(left_state);
			for(auto right_state : rightAutomata.getStates())
				states.insert(right_state);

			std::multimap<std::pair<std::string, char>, std::string> rules;
			{//left -> mid
				rules.insert(make_pair(make_pair(start_state_name, 0), left_start_state));
				for(auto states : leftAutomata.getFinalStates())
					rules.insert(make_pair(make_pair(states, 0), mid_state_name));
			}
			{//mid->right
				rules.insert(make_pair(make_pair(mid_state_name, 0), right_start_state));
				for(auto states : rightAutomata.getFinalStates())
					rules.insert(make_pair(make_pair(states, 0), final_state_name));
			}

			for(auto sub_rules : leftAutomata.getRules())
				rules.insert(sub_rules);
			for(auto sub_rules : rightAutomata.getRules())
				rules.insert(sub_rules);

			std::set<std::string> final_states;
			final_states.insert(final_state_name);
			return Automata(vocab,states, start_state_name, final_states, rules);
			break;
		}
		case UNION:
		{
			//name = std::string("<UNION[|]>");
			Automata leftAutomata = generateAutomata(vocab, currentNode->child, uniq_index);
			Automata rightAutomata = generateAutomata(vocab, currentNode->child->sibling, uniq_index);

			snprintf(buf, sizeof(buf), "%d_Union_start", uniq_index);
			std::string start_state_name = std::string(buf);
			snprintf(buf, sizeof(buf), "%d_Union_final", uniq_index);
			std::string final_state_name = std::string(buf);
			uniq_index++;

			std::string left_start_state = leftAutomata.getStartState();
			std::string right_start_state = rightAutomata.getStartState();

			std::set<std::string> states;
			states.insert(start_state_name);
			states.insert(final_state_name);
			for(auto left_state : leftAutomata.getStates())
				states.insert(left_state);
			for(auto right_state : rightAutomata.getStates())
				states.insert(right_state);

			std::multimap<std::pair<std::string, char>, std::string> rules;
			{//left -> mid
				rules.insert(make_pair(make_pair(start_state_name, 0), left_start_state));
				for(auto states : leftAutomata.getFinalStates())
					rules.insert(make_pair(make_pair(states, 0), final_state_name));
			}
			{//mid->right
				rules.insert(make_pair(make_pair(start_state_name, 0), right_start_state));
				for(auto states : rightAutomata.getFinalStates())
					rules.insert(make_pair(make_pair(states, 0), final_state_name));
			}

			for(auto sub_rules : leftAutomata.getRules())
				rules.insert(sub_rules);
			for(auto sub_rules : rightAutomata.getRules())
				rules.insert(sub_rules);

			std::set<std::string> final_states;
			final_states.insert(final_state_name);
			return Automata(vocab,states, start_state_name, final_states, rules);
		}
		case CLOSURE:
		{
			//name = std::string("<CLOSURE[*]>");
			Automata childAutomata = generateAutomata(vocab, currentNode->child, uniq_index);

			snprintf(buf, sizeof(buf), "%d_Closure_start", uniq_index);
			std::string start_state_name = std::string(buf);
			snprintf(buf, sizeof(buf), "%d_Closure_final", uniq_index);
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

			std::set<std::string> final_states;
			final_states.insert(final_state_name);
			return Automata(vocab,states, start_state_name, final_states, rules);
			break;
		}
		case CLOSURE_LEAST_ONCE:
		{
			//name = std::string("<CLOSURE[+]>");
			Automata childAutomata = generateAutomata(vocab, currentNode->child, uniq_index);

			snprintf(buf, sizeof(buf), "%d_Closure_start", uniq_index);
			std::string start_state_name = std::string(buf);
			snprintf(buf, sizeof(buf), "%d_Closure_final", uniq_index);
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

			std::set<std::string> final_states;
			final_states.insert(final_state_name);
			return Automata(vocab,states, start_state_name, final_states, rules);
		}
		case CLOSURE_ONCE:
		{
			//name = std::string("<CLOSURE[?]>");
			Automata childAutomata = generateAutomata(vocab, currentNode->child, uniq_index);

			snprintf(buf, sizeof(buf), "%d_Closure_start", uniq_index);
			std::string start_state_name = std::string(buf);
			snprintf(buf, sizeof(buf), "%d_Closure_final", uniq_index);
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

			std::set<std::string> final_states;
			final_states.insert(final_state_name);
			return Automata(vocab,states, start_state_name, final_states, rules);
		}
		case EPSILON:
		{
			//name = std::string("[EPSILON]");

			snprintf(buf, sizeof(buf), "%d_Epsilon", uniq_index);
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
			snprintf(buf, sizeof(buf), "%d_Character_class[%c]_start", uniq_index, currentNode->data);
			std::string start_state_name = std::string(buf);
			snprintf(buf, sizeof(buf), "%d_Character_class[%c]_final", uniq_index, currentNode->data);
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
			snprintf(buf, sizeof(buf), "%d_CHARSET_start", uniq_index);
			std::string start_state_name = std::string(buf);
			snprintf(buf, sizeof(buf), "%d_CHARSET_final", uniq_index);
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
			snprintf(buf, sizeof(buf), "%d_NEGATIVE_CHARSET_start", uniq_index);
			std::string start_state_name = std::string(buf);
			snprintf(buf, sizeof(buf), "%d_NEGATIVE_CHARSET_final", uniq_index);
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
			snprintf(buf, sizeof(buf), "%d_Empty_start", uniq_index);
			std::string e_start_state_name = std::string(buf);
			snprintf(buf, sizeof(buf), "%d_Empty_start", uniq_index);
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
