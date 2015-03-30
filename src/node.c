/*
 * node.c
 *
 *  Created on: 2015. 3. 31.
 *      Author: 근홍
 */


#include "node.h"
#include <stdlib.h>

static Node* head = 0;

Node* makeNode(void)
{
	Node* ret = malloc(sizeof(Node));
	ret->type = EMPTY;
	ret->__next_alloc = head;
	ret->child = 0;
	ret->data = 0;
	ret->sibling = 0;
	head = ret;
	return ret;
}

void clearNode(void)
{
	while(head)
	{
		Node* next = head->__next_alloc;
		free(head);
		head = next;
	}
}
