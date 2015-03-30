/*
 * node.h
 *
 *  Created on: 2015. 3. 31.
 *      Author: 근홍
 */

#ifndef INCLUDE_NODE_H_
#define INCLUDE_NODE_H_

#ifdef __cplusplus
extern "C" {
#endif

enum NodeType
{
	VOCABULARY, //char
	CONCAT, //concat
	UNION, //|
	CLOSURE, //*
	CLOSURE_LEAST_ONCE, //+
	CLOSURE_ONCE, //?
	EPSILON, // .
	CHARSET, //[]
	NEGATIVE_CHARSET, //[^asdf]
	EMPTY //PHI
};

typedef struct node_t
{
	enum NodeType type;
	char data;
	struct node_t* child;
	struct node_t* sibling;
	struct node_t* __next_alloc;
}Node;

Node* makeNode(void);
void clearNode(void);
extern Node* last_accept_node;

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_NODE_H_ */
