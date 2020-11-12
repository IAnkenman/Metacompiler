#include "pool.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdalign.h>

#define NEXT(node,off) (*(void**)((node)+(off)))

pool_context pool_create(size_t objSize, size_t blocksize) {
	pool_context out;
    out.blocksize = blocksize;
	out.currentBlock = malloc(blocksize);
    out.nextOffset = blocksize - sizeof(void*);
	NEXT(out.currentBlock, out.nextOffset) = NULL;
	out.freeList = NULL;
	out.currentIdx = out.currentBlock;
    const size_t align = alignof(max_align_t);
	out.objSize = objSize + ((align - (objSize % align)) % align);

	return out;
}

void* pool_alloc(pool_context* ct) {
	if (!ct->currentBlock)
		return NULL;
	if (ct->freeList) {
		void* out = ct->freeList;
		ct->freeList = *(void**)ct->freeList;
		return out;
	}
	
	if (ct->objSize > ct->blocksize - sizeof(void*))
			return NULL;
	if (ct->objSize > (ct->blocksize - sizeof(void*))-(ct->currentIdx - ct->currentBlock)) {
		void* newBlock = malloc(ct->blocksize);
        if (!newBlock)
            return NULL;
		NEXT(newBlock, ct->nextOffset) = ct->currentBlock;
		ct->currentBlock = newBlock;
		ct->currentIdx = newBlock;
	}
	
	void* out = ct->currentIdx;
	ct->currentIdx += ct->objSize;
	return out;
}

void pool_free(pool_context* ct, void* obj) {
	*(void**)obj = ct->freeList;
	ct->freeList = obj;
}

void pool_destroy(pool_context* ct) {
	void* toFree = ct->currentBlock;
	ct->currentBlock = NULL;
    ct->freeList = NULL;
    ct->currentIdx = NULL;
	while (toFree) {
		void* next = NEXT(toFree, ct->nextOffset);
		free(toFree);
		toFree = next;
	}
	return;
}
