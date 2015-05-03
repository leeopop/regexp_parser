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
#include <algorithm>

class Automata
{
private:
	std::set<char> vocab;
	std::set<std::string> states;
	std::string start_state;
	std::set<std::string> final_states;
	std::multimap<std::pair<std::string, char>, std::string> rules;
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
		for(auto node : states)
		{
			std::string color = "white";
			std::string shape = "circle";
			if(node == start_state)
				color = "cyan";
			if(final_states.find(node) != final_states.end())
				shape = "doublecircle";
			out<<node<<"\t[label=\""<<node<<"\", style=\"filled\", fillcolor=\""<<color<<"\", shape=\""<<shape<<"\"];"<<std::endl;
		}
		//int uniq_edge = 0;
		std::map<std::pair<std::string, std::string>, std::set<char>> merge_edges;
		for(auto edge : rules)
		{
			const std::string& from = edge.first.first;
			const std::string& to = edge.second;

			auto iter = merge_edges.find(make_pair(from, to));
			if(iter == merge_edges.end())
			{
				merge_edges.insert(make_pair(make_pair(from, to), std::set<char>()));
				iter = merge_edges.find(make_pair(from, to));
				assert(iter != merge_edges.end());
			}

			iter->second.insert(edge.first.second);
		}
		for(auto edge : merge_edges)
		{
			std::string color = "black";
			auto symbols = edge.second;
			out<<edge.first.first<<"\t->\t"<<edge.first.second<<"\t[label=\"";

			std::vector<char> result(symbols.size() + vocab.size());
			auto result_iter = std::set_difference (vocab.begin(), vocab.end(),
					symbols.begin(), symbols.end(), result.begin());

			result.resize(result_iter-result.begin());

			if((symbols.find('\0') != symbols.end()) || (result.size() > (vocab.size()/2)))
			{
				for(auto voc : symbols)
				{
					if(voc == 0)
					{
						out<<"_Epsilon_, ";
						color = "orange";
					}
					else
						out<<(char)voc<<", ";
				}
			}
			else if(result.size() == 0)
			{
				out<<"_VOCABULARY_";
			}
			else
			{
				out<<"__EXCEPT__, ";
				for(auto voc : result)
				{
					out<<(char)voc<<", ";
				}
			}

			out<<"\", style=\"bold\", color=\""<<color<<"\"];"<<std::endl;
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

	static Automata minimalize(const Automata& orig)
	{
		typedef std::set<std::string> MergeState;
		typedef std::set<std::pair<char, MergeState>> MergeTrans;
		std::set<MergeState> all_state;
		std::set<MergeState> final_states;
		MergeState start_state;
		std::map<MergeState, MergeTrans> temp_rules;

		start_state.insert(orig.getStartState());
		for(auto state : orig.getStates())
		{
			MergeState item;
			item.insert(state);
			all_state.insert(item);
		}
		for(auto state : orig.getFinalStates())
		{
			MergeState item;
			item.insert(state);
			final_states.insert(item);
		}
		for(auto rule : orig.getRules())
		{
			MergeState src;
			src.insert(rule.first.first);
			MergeState dest;
			dest.insert(rule.second);
			char voc = rule.first.second;
			auto iter = temp_rules.find(src);
			if(iter == temp_rules.end())
			{
				temp_rules.insert(make_pair(src, MergeTrans()));
				iter = temp_rules.find(src);
			}

			iter->second.insert(make_pair(voc, dest));
		}

		//---
		while(true)
		{

			std::multimap<MergeTrans, MergeState> merge_rules;
			for(auto rule : temp_rules)
			{
				merge_rules.insert(make_pair(rule.second, rule.first));
			}
			std::map<MergeState, MergeState> merged_states;
			std::map<MergeState, MergeTrans> merged_states_to_rules;
			for(auto iter = merge_rules.begin(); iter != merge_rules.end(); iter = merge_rules.upper_bound(iter->first))
			{
				auto range = merge_rules.equal_range(iter->first);
				std::set<MergeState> sameStates;
				for(auto range_iter = range.first; range_iter != range.second; ++range_iter)
				{
					sameStates.insert(range_iter->second);
				}

				if(sameStates.size() > 1)
				{
					MergeState merge;
					for(auto mergeState : sameStates)
					{
						for(auto state : mergeState)
						{
							merge.insert(state);
						}
					}
					for(auto mergeState : sameStates)
					{
						merged_states.insert(make_pair(mergeState, merge));
						merged_states_to_rules.insert(make_pair(merge, iter->first));
					}
				}
			}

			if(merged_states.size() == 0)
				break;


			{
				auto iter = merged_states.find(start_state);
				if(iter != merged_states.end())
					start_state = iter->second;
			}
			std::set<MergeState> new_all_state;
			std::set<MergeState> new_final_states;
			std::map<MergeState, MergeTrans> new_temp_rules;
			for(auto mergeState : all_state)
			{
				auto iter = merged_states.find(mergeState);
				if(iter == merged_states.end())
					new_all_state.insert(mergeState);
				else if(new_all_state.find(iter->second) == new_all_state.end())
					new_all_state.insert(iter->second);
			}
			for(auto mergeState : final_states)
			{
				auto iter = merged_states.find(mergeState);
				if(iter == merged_states.end())
					new_final_states.insert(mergeState);
				else if(new_final_states.find(iter->second) == new_final_states.end())
					new_final_states.insert(iter->second);
			}
			for(auto rule : temp_rules)
			{
				auto current_state = rule.first;
				auto current_rule = rule.second;
				auto iter_state = merged_states.find(current_state);
				if(iter_state != merged_states.end())
				{
					current_state = iter_state->second;
					auto iter_merged_rule = merged_states_to_rules.find(current_state);
					assert(iter_merged_rule != merged_states_to_rules.end());
					current_rule = iter_merged_rule->second;
				}

				auto iter_rule = new_temp_rules.find(current_state);
				if(iter_rule == new_temp_rules.end())
				{
					MergeTrans new_trans;
					for(auto prev_rule : current_rule)
					{
						auto dest = prev_rule.second;
						auto dest_iter = merged_states.find(dest);
						if(dest_iter != merged_states.end())
							dest = dest_iter->second;
						new_trans.insert(make_pair(prev_rule.first, dest));
					}
					new_temp_rules.insert(make_pair(current_state, new_trans));
				}
			}

			all_state = new_all_state;
			final_states = new_final_states;
			temp_rules = new_temp_rules;

		}
		//--

		std::set<std::string> merged_all_state;
		std::set<std::string> merged_final_state;
		std::string merged_start_state;
		Rule merged_rules;

		char buf[4096];
		std::map<MergeState, std::string> name_mapping;
		int name_index = 0;
		for(auto state : all_state)
		{
			snprintf(buf, sizeof(buf), "Merged_State_%d___%d", name_index++, (int)state.size());
			std::string state_name(buf);
			name_mapping.insert(make_pair(state, state_name));
			merged_all_state.insert(state_name);
		}
		{
			auto iter = name_mapping.find(start_state);
			assert(iter != name_mapping.end());
			merged_start_state = iter->second;
		}
		for(auto state : final_states)
		{
			auto iter = name_mapping.find(state);
			assert(iter != name_mapping.end());
			merged_final_state.insert(iter->second);
		}
		for(auto rule : temp_rules)
		{
			auto state = rule.first;
			auto iter = name_mapping.find(state);
			assert(iter != name_mapping.end());
			auto src = iter->second;

			for(auto trans : rule.second)
			{
				char voc = trans.first;
				auto state2 = trans.second;
				auto iter2 = name_mapping.find(state2);
				assert(iter2 != name_mapping.end());
				auto dest = iter2->second;

				merged_rules.insert(make_pair(make_pair(src, voc), dest));
			}
		}

		return Automata(orig.vocab, merged_all_state, merged_start_state, merged_final_state, merged_rules);
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
				for(auto iter = range.first; iter != range.second; ++iter)
				{
					ambiguous.push_back(iter->second);
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
			for(auto iter = range.first; iter != range.second; ++iter)
			{
				rules.insert(*iter);
			}
		}

		return Automata(vocab, all_state, start_state, final_state, rules);
	}

	static std::set<std::string> getEpsilonClosure(const Automata& orig, const std::string& state)
	{
		std::set<std::string> visited;
		std::list<std::string> toVisit;
		toVisit.push_back(state);
		visited.insert(state);
		auto rules = orig.getRules();

		while(!toVisit.empty())
		{
			auto currentVisit = toVisit.front();
			toVisit.pop_front();

			auto range = rules.equal_range(make_pair(currentVisit, 0));

			for(auto iter = range.first; iter != range.second; ++iter)
			{
				auto nextVisit = iter->second;
				if(visited.find(nextVisit) != visited.end())
					continue;
				toVisit.push_back(nextVisit);
				visited.insert(nextVisit);
			}
		}
		return visited;
	}

	static Automata removeEpsilon(const Automata& orig, bool include_failure_state = false)
	{
		auto vocab = orig.getVocab();
		auto rules = orig.getRules();
		std::map<std::pair<int, char>, int> integer_rules;

		char buf[4096];
		int closure_index = 0;
		std::map<std::set<std::string>, int> unique_closure;

		std::list<std::set<std::string>> toVisit;
		auto start_closure = getEpsilonClosure(orig, orig.getStartState());
		toVisit.push_back(start_closure);
		unique_closure.insert(make_pair(start_closure, closure_index++));

		while(!toVisit.empty())
		{
			auto curVisit = toVisit.front();
			toVisit.pop_front();
			auto cur_iter = unique_closure.find(curVisit);
			assert(cur_iter != unique_closure.end());
			int cur_index = cur_iter->second;

			for(auto voc : vocab)
			{
				std::set<std::string> vocab_closure;
				for(auto each_state : curVisit)
				{
					auto range = rules.equal_range(make_pair(each_state, voc));

					for(auto iter = range.first; iter != range.second; ++iter)
					{
						auto each_closure = getEpsilonClosure(orig, iter->second);
						for(auto each_closure_item : each_closure)
							vocab_closure.insert(each_closure_item);
					}
				}
				if(vocab_closure.size() == 0 && !include_failure_state)
					continue;

				auto iter = unique_closure.find(vocab_closure);
				int next_index = -1;
				if(iter == unique_closure.end())
				{
					next_index = closure_index++;
					unique_closure.insert(make_pair(vocab_closure, next_index));
					toVisit.push_back(vocab_closure);
				}
				else
				{
					next_index = iter->second;
				}
				integer_rules.insert(std::pair<std::pair<int, char>, int>(std::pair<int, char>(cur_index, voc), next_index));
			}
		}

		assert(unique_closure.find(start_closure) != unique_closure.end());
		int new_start_index = unique_closure.find(start_closure)->second;
		std::string new_start_state;
		auto final_states = orig.getFinalStates();

		std::set<std::string> new_states;
		std::set<std::string> new_final_states;
		for(auto each_closure : unique_closure)
		{
			auto closure = each_closure.first;
			int each_index = each_closure.second;
			snprintf(buf, sizeof(buf), "Closure_%d", each_index);
			std::string closure_name(buf);

			new_states.insert(closure_name);
			if(each_index == new_start_index)
				new_start_state = closure_name;

			std::vector<std::string> result(closure.size() + final_states.size());
			auto result_iter = std::set_intersection (closure.begin(), closure.end(),
					final_states.begin(), final_states.end(), result.begin());

			if(result_iter != result.begin())
			{
				new_final_states.insert(closure_name);
			}
		}

		Rule new_rules;
		for(auto each_rule : integer_rules)
		{
			int each_index = each_rule.first.first;
			char voc = each_rule.first.second;
			snprintf(buf, sizeof(buf), "Closure_%d", each_index);
			std::string source_name(buf);
			snprintf(buf, sizeof(buf), "Closure_%d", each_rule.second);
			std::string destination_name(buf);
			new_rules.insert(make_pair(make_pair(source_name, voc), destination_name));
		}

		return Automata(vocab, new_states, new_start_state, new_final_states, new_rules);
	}
private:
	bool evaluate(std::string current_state, std::string input)
	{
		if(input.length() == 0)
			if(final_states.find(current_state) != final_states.end())
				return true;

		//e-transitions
		auto range = rules.equal_range(make_pair(current_state, 0));

		for(auto iter = range.first; iter != range.second; ++iter)
		{
			std::string next_state = iter->second;
			bool ret = evaluate(next_state, input);
			if(ret == true)
				return true;
		}

		char first_char = input[0];
		std::string remaining = input.substr(1);
		range = rules.equal_range(make_pair(current_state, first_char));
		for(auto iter = range.first; iter != range.second; ++iter)
		{
			std::string next_state = iter->second;
			bool ret = evaluate(next_state, remaining);
			if(ret == true)
				return true;
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
