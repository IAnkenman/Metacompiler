#include <stdlib.h>
#include <alloca.h>
#include "BTree.h"
#define NODE_SIZE 64
#define MAX 5
#define MIN 3 //Min = ceil(MAX / 2)

enum nodeType: uint8_t {
	internal, leaf
};

struct nodeHeader {
	enum nodeType type: 8;
	char pad : 8;
	uint16_t numElements : 16;
};

struct node {
	struct nodeHeader header;
	uint32_t keys[5];
	struct node * children[5];
};

struct BTree_context BTree_new() {
	struct BTree_context out;
	out.mem = pool_new();
	struct node * n = pool_alloc(&out.mem);
	n->header.numElements = 0;
	n->header.type = leaf;
	out.root = n;
	return out;
}

/*
   To fit as many tree entries into a cache line as possible parent pointers are not stored in tree nodes, but are instead traced by a stack built up during the search algorithm. To ensure that this trace stack remains cached as well
   it is allocated on the program stack using alloca(). In order to prevent buffer overflows the stack must be sufficiently big to trace the descent to any depth. The maximum depth is calulated as follows:

   The tree has a minimum branching factor of 2, therefore the minimum number of key value pairs it can index at some height is given by the exprssion 2^h. Assuming the tree is filled with every 32 bit key the maximum worst case
   height is given by the equation 2^h = 2^32. Solving this yeilds log(2^32) = h, where h = 32. Therefore the maximum stack size needed to trace paths through the tree without overflow is 256 bytes large. (32 * sizeof(void *))
   */
static int search(uint32_t key, struct node *** ptr_to_stack_ptr) {
	struct node ** stackPtr = *ptr_to_stack_ptr;
	struct node * n = *stackPtr;
	while (1) {
		uint32_t * keys = n->keys;
		struct node ** children = n->children;
		uint32_t left;
		uint32_t right;
		uint32_t mid;
		if(n->header.type == leaf) {
			left = 0;
			right = n->header.numElements - 1;
			while (left != right) {
				mid = (left + right + 1) / 2;
				if (keys[mid] > key) 
					right = mid - 1;
				else
					left = mid;
				int out;
				if (keys[left] == key)
					out = left;
				else
					out = -1;
				*ptr_to_stack_ptr = stackPtr;
				return out;
			}
			left = 0;
			right = n->header.numElements - 2;
			while (left != right) {
				mid = (left + right + 1) / 2;
				if (keys[mid] > key)
					right = mid - 1;
				else
					left = mid;
			}
			if (keys[left] >= key) 
				n = children[left +1];
			else
				n = children[left];
			stackPtr++;
			*stackPtr = n;
		}
	}
}

static inline struct node * split(struct BTree_context * ct, struct node * n, uint32_t key, struct node * value) {
	size_t old_i = MAX;
	size_t new_i = MIN;
	struct node * newNode = pool_alloc(&ct->mem);
	char wasInserted = 0;
	newNode->header.numElements = MIN;
	newNode->header.type = leaf;
	do { //Move keys to new leaf node.
		if (!wasInserted && key > n->keys[old_i]) {
			newNode->children[new_i] = value;
			newNode->keys[new_i] = key;
			wasInserted = 1;
		} else {
			newNode->children[new_i] = n->children[old_i];
			newNode->keys[new_i] = n->keys[old_i];
			old_i--;
		}
		new_i--;
	} while (new_i > 0);
	if (!wasInserted) {
		size_t i = 0;
		while (n->keys[i] < key && i == MIN)
			i++;
		for (; i < MIN; i++) {
			uint32_t tmp_k = n->keys[i];
			struct node * tmp_v = n->children[i];
			n->keys[i] = tmp_k;
			n->children[i] = tmp_v;
			key = tmp_k;
			value = tmp_v;
		}
	}
	n->header.numElements = MIN;
	return newNode;
}

void * BTree_insert(struct BTree_context * ct, uint32_t key, void * value) {
	struct node ** stack = (struct node**)alloca(256);
	*stack = ct->root;
	int result = search(key, &stack);
	struct node * n = *stack;
	if (result != -1) { //Key already exists, repalce and return previous value.
		void * out = n->children[result];
		n->children[result] = value;
		n->header.numElements++;
		return out;
	}
	if (n->header.numElements < MAX) { //Space exists for insertion, add element.
		size_t i = 0;
		while (n->keys[i] < key && i < n->header.numElements)
			i++;
		for (;i < n->header.numElements + 1; i++) {
			uint32_t tmp_k = n->keys[i];
			struct node * tmp_v = n->children[i];
			n->keys[i] = key;
			n->children[i] = value;
			key = tmp_k;
			value = tmp_v;
		}
		return NULL;
	}

	struct node * newNode = split(ct, n, key, value);
	//TODO insert into parent.
	struct node * parent = *stack;
	while (1) {
		if (n == ct->root) {
			struct node * newRoot = pool_alloc(&ct->mem);
			newRoot->header.numElements = 2;
			newRoot->header.type = internal;
			newRoot->children[0] = n;
			newRoot->children[1] = newNode;
			newRoot->keys[0] = newNode->keys[0];
			return NULL;
		}
		//TODO
	}
	return NULL;
}

/*
	if (n == ct->root) {
		struct node * newRoot = pool_alloc(&ct->mem);
		newRoot->header.type = internal;
		newRoot->header.numElements = 2;
		newRoot->children[0] = n;
		newRoot->children[1] = newNode;
		newRoot->keys[0] = newNode->keys[0];
		ct->root = newRoot;
		return NULL;
	}
*/
