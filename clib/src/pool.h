#pragma once
#include <stddef.h>

typedef struct {
	void* currentBlock;
	void* freeList;
	void* currentIdx;
    size_t nextOffset;
    size_t blocksize;
	size_t objSize;
} pool_context;

pool_context pool_create(size_t objSize, size_t blocksize);

void* pool_alloc(pool_context* ct);

void pool_free(pool_context* ct, void* obj);

void pool_destroy(pool_context* ct);
