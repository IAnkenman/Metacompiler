#pragma once
#include <stdint.h>
#include "pool.h"

struct BTree_context {
	void * root;
	struct pool_context mem;
};

// 64 elements

struct BTree_context BTree_new();

void * BTree_insert(struct BTree_context * ct, uint32_t key, void * value);

void * BTree_delete(struct BTree_context * ct, uint32_t key);

void * BTree_find(struct BTree_context * ct, uint32_t key);

