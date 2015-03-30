/*
 * RegularParseTree.hpp
 *
 *  Created on: 2015. 3. 31.
 *      Author: 근홍
 */

#ifndef SRC_REGULARPARSETREE_HPP_
#define SRC_REGULARPARSETREE_HPP_


#include "node.h"
#include <cstring>
#include <string>
#include <list>
#include <cassert>
#include <iostream>

class TreeNode
{
private:
	std::list<TreeNode*> children;
	std::string nodeName;
	int index;
public:
	TreeNode()
	{
		nodeName = std::string("UnnamedNode");
		index = -1;
	}
	TreeNode(const std::string& name, int index)
	{
		nodeName = std::string(name);
		this->index = index;
	}
	virtual ~TreeNode()
	{
		for(auto iter : children)
			delete iter;
	}
	int getIndex()
	{
		return index;
	}
public:
	virtual std::string getNodeName()
	{
		return nodeName;
	}

public:
	void addChild(TreeNode* node)
	{
		children.push_back(node);
	}
	void removeChild(TreeNode* node)
	{
		children.remove(node);
	}
	size_t numChildren()
	{
		return children.size();
	}
	const std::list<TreeNode*>& childrenList()
	{
		return children;
	}
};

class RegularParseTree
{
private:
	TreeNode* root;
	std::list<TreeNode*> allNode;
public:
	RegularParseTree(const Node* node)
	{
		root = generatedNode(node);
		if(node->child)
			traverseGraph(root, node->child);
	}
	~RegularParseTree()
	{
		delete root;
	}

	void printParseTree(std::ostream& out)
	{
		out<<"digraph RegularParseTree {"<<std::endl;
		for(auto node : allNode)
		{
			out<<"Node"<<node->getIndex()<<"\t[label=\""<<node->getNodeName()<<"\"];"<<std::endl;
		}
		for(auto node : allNode)
		{
			for(auto child : node->childrenList())
			{
				out<<"Node"<<node->getIndex()<<"\t->\t"<<"Node"<<child->getIndex()<<";"<<std::endl;
			}
		}
		out<<"}"<<std::endl;
	}

private:
	TreeNode* generatedNode(const Node* currentNode)
	{
		std::string name;
		switch(currentNode->type)
		{
		case VOCABULARY:
		{
			char buf[128];
			snprintf(buf, sizeof(buf), "V: %c", currentNode->data);
			name = std::string(buf);
			break;
		}
		case CONCAT:
		{
			name = std::string("OPERATION: CONCAT");
			break;
		}
		case UNION:
		{
			name = std::string("OPERATION: UNION");
			break;
		}
		case CLOSURE:
		{
			name = std::string("OPERATION: CLOSURE*");
			break;
		}
		case CLOSURE_LEAST_ONCE:
		{
			name = std::string("OPERATION: CLOSURE+");
			break;
		}
		case CLOSURE_ONCE:
		{
			name = std::string("OPERATION: CLOSURE?");
			break;
		}
		case EPSILON:
		{
			name = std::string("[EPSILON]");
			break;
		}
		case CHARSET:
		{
			name = std::string("[CHARSET]");
			break;
		}
		case NEGATIVE_CHARSET:
		{
			name = std::string("[^CHARSET]");
			break;
		}
		case EMPTY:
		{
			name = std::string("[EMPTY SET]");
			break;
		}
		default:
			assert(0);
		}

		TreeNode* node = new TreeNode(name, currentNode->index);
		allNode.push_back(node);
		return node;
	}
	void traverseGraph(TreeNode* parent, const Node* currentNode)
	{
		const Node* iter = currentNode;
		while(iter)
		{
			TreeNode* current = generatedNode(iter);
			parent->addChild(current);
			if(iter->child)
			{
				traverseGraph(current, iter->child);
			}
			iter = iter->sibling;
		}
	}
};

#endif /* SRC_REGULARPARSETREE_HPP_ */
