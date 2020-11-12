#include <string.h>
#include <stdlib.h>
#include "pool.h"
#include "set.h"

struct ListNode;
typedef struct ListNode ListNode;

enum Color {RED, BLACK};

struct ListNode{
	ListNode* left;
	ListNode* right;
	ListNode* parent;
	void* key;
	char color;
};

set_context set_create()
{
	set_context out;
	out.root = NULL;
	out.size = 0;
	out.pool = pool_create(sizeof(ListNode), 4096);
	return out;	
}

static inline ListNode* insert(ListNode** rootPtr, void* key, void* pool) {
	ListNode** subPtr = rootPtr;
	void* parent = NULL;
	while (*subPtr) {
		if (key == (*subPtr)->key)
			return (*subPtr)+1; //If key already existed, tag pointer with a one.
		parent = *subPtr;
		if (key > (*subPtr)->key)
			subPtr = &(*subPtr)->right;
		else
			subPtr = &(*subPtr)->left;
	}
	ListNode* newNode = pool_alloc(pool);
	if (!newNode)
		return NULL;
	*subPtr = newNode;
	newNode->parent = parent;
	newNode->color = RED;
	newNode->left = NULL;
	newNode->right = NULL;
	newNode->key = key;
	return newNode;
}

static inline ListNode* getSibling(ListNode* node) {
	ListNode* parent = node->parent;
	if (!parent) 
		return NULL;
	ListNode* left = parent->left;
	if (node == left)
		return parent->right;
	return left;
}

static inline ListNode* getUncle(ListNode* node) {
	ListNode* parent = node->parent;
	if (!parent)
		return NULL;
	return getSibling(parent);
}

static inline void rotateLeft(ListNode* subRoot, ListNode** subRootPtr) {
	ListNode* newSubRoot = subRoot->right;
	ListNode* parent = subRoot->parent;
	
	subRoot->right = newSubRoot->left;
	newSubRoot->left = subRoot;
	subRoot->parent = newSubRoot;
	if (subRoot->right)
		subRoot->right->parent = subRoot;
	
	*subRootPtr = newSubRoot;
	newSubRoot->parent = parent;
}

static inline void rotateRight(ListNode* subRoot, ListNode** subRootPtr) {
	ListNode* newSubRoot = subRoot->left;
	ListNode* parent = subRoot->parent;
	
	subRoot->left = newSubRoot->right;
	newSubRoot->right = subRoot;
	subRoot->parent = newSubRoot;
	if (subRoot->left)
		subRoot->left->parent = subRoot;
	
	*subRootPtr = newSubRoot;
	newSubRoot->parent = parent;
}

void repair(ListNode* node, ListNode** rootPtr) {
	ListNode* parent = node->parent;
	if (!parent) {
		*rootPtr = node;
		node->color = BLACK;
		return;
	}
	if (parent->color == BLACK) {
		return;
	}
	ListNode* uncle = getUncle(node);
	if (uncle && uncle->color == RED) {
		node->parent->color = BLACK;
		uncle->color = BLACK;
		ListNode* grandParent = parent->parent;
		grandParent->color = RED;
		repair(grandParent, rootPtr);
		return;
	}
	ListNode* grandParent = parent->parent;
	if (node == parent->right && parent == grandParent->left) {
		rotateLeft(parent, &grandParent->left);
		node = node->left;
	} else if (node == parent->left && parent == grandParent->right) {
		rotateRight(parent, &grandParent->right);
		node = node->right;
	}
	
	parent = node->parent;
	grandParent = parent->parent;
	
	if (node == parent->left)
		rotateRight(node, &parent->left);
	else
		rotateLeft(node, &parent->right);
	
	parent->color = BLACK;
	grandParent->color = RED;
	return;
}

int set_add(set_context* ct, void* key) {
	ListNode* node = insert((ListNode**) &ct->root, key, &ct->pool);
	if (!node)
		return -1;
	if (1 & (size_t)node) //If pointer has the one bit set, the key was already in the set.
		return 1;
	repair(node, (ListNode**)&ct->root);
	return 0;
}

void delete(ListNode** rootPtr, void* key, pool_context* pool) {
	
}

void set_remove(set_context* ct, void* key) {
	
}

int set_has(set_context* ct, void* key) {
	ListNode* node = ct->root;
	while (node) {
		if (key == node->key)
			return 1;
		if (key < node->key)
			node = node->left;
		else
			node = node->right;
	}
	return 0;
}

void set_clear(set_context* ct) {
	//TODO add logic to clear set.
}

void set_destroy(set_context* ct) {
	pool_destroy(&ct->pool);
	ct->root = NULL;
	ct->size = 0;
}

size_t set_size(set_context* ct) {
	return ct->size;
}

set_iterator set_begin(set_context* ct) {
	ListNode* node = ct->root;
	if (node)
		while (node->left)
			node = node->left;

	set_iterator out = {ct, node};
	return out;
}

set_iterator set_end(set_context* ct) {
	set_iterator out = {ct, NULL};
	return out;
}


void set_iterator_next(set_iterator* it) {
	ListNode* node = it->node;
	if (!node) { //If node is null loop to beginning
		node = it->ct->root;
		if (node)
			while(node->left)
				node = node->left;
		it->node = node;
		return;
	}
	if (node->right) {//If node has right child next is the bottom left of the right subtree.
		node = node->right;
		while (node->left)
			node = node->left;
		it->node = node;
		return;
	}
	//If node has no right children, next is somewhere above it.
	ListNode* parent = node->parent;
	while (parent) {
		if (node == parent->left) { //If the node returned from is a left child this parent is next.
			it->node = parent;
			return;
		}
		//Otherwise the node was the right child and the next node is still above it, keep looping.
		node = parent;
		parent = parent->parent;
	}
	//If loop exits, root has been reached from the right subtree and iteration is complete.
	it->node = NULL;
}

void set_iterator_prev(set_iterator* it) {
	ListNode* node = it->node;
	if (!node) { //If node is null loop to end
		node = it->ct->root;
		if (node)
			while(node->right)
				node = node->right;
		it->node = node;
		return;
	}
	if (node->left) {//If node has right child prev is the bottom right of the left subtree.
		node = node->left;
		while (node->right)
			node = node->right;
		it->node = node;
		return;
	}
	//If node has no left children, prev is somewhere above it.
	ListNode* parent = node->parent;
	while (parent) {
		if (node == parent->right) { //If the node returned from is a right child this parent is the previous in order.
			it->node = parent;
			return;
		}
		//Otherwise the node was the right child and the next node is still above it, keep looping upwards
		node = parent;
		parent = parent->parent;
	}
	//If loop exits, root has been reached from the left subtree and iteration is complete.
	it->node = NULL;
}

int set_iterator_get(set_iterator* it, void** out) {
	ListNode* node = it->node;
	if (!node)
		return -1;
	*out = node->key;
	return 1;
}

int set_iterator_cmp(set_iterator* a, set_iterator* b) {
	return (a->node == b->node);
}
