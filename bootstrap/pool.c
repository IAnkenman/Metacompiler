#include "pool.h"
#include <stdlib.h>

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

#ifndef ALLOC_SIZE
#define ALLOC_SIZE 64
#endif

struct pool_context pool_new() {
	struct pool_context out;
	out.metaPage = (void **)aligned_alloc(64, PAGE_SIZE);
	*out.metaPage = aligned_alloc(64, PAGE_SIZE);
	out.freeList = NULL;
	out.metaIndex = 1;
	out.pageBgn = *out.metaPage;
	out.freePtr = *out.metaPage;
	out.metaPage[(PAGE_SIZE / sizeof(void *)) - 1] = NULL;
	return out;
}

static inline void newMetaPage(struct pool_context * ct) {
	void ** newPage = (void**)aligned_alloc(64, PAGE_SIZE);
	newPage[(PAGE_SIZE / sizeof(void*)) - 1] = ct->metaPage;
	ct->metaPage = newPage;
	ct->metaIndex = 0;
}

static inline void newPage(struct pool_context * ct) {
	if (ct->metaIndex == (PAGE_SIZE / sizeof(void*)) - 1)
		newMetaPage(ct);
	ct->metaPage[ct->metaIndex] = aligned_alloc(64, PAGE_SIZE);
	ct->freePtr = ct->metaPage[ct->metaIndex];
	ct->pageBgn = ct->freePtr;
	ct->metaIndex++;
}

void * pool_alloc(struct pool_context * ct) {
	void * out;
	if (ct->freeList) {
		out = ct->freeList;
		ct->freeList = *(void **)ct->freeList;
		return out;
	}
	size_t memLeft = PAGE_SIZE - (ct->freePtr - ct->pageBgn);
	if (memLeft < ALLOC_SIZE)
		newPage(ct);
	out = ct->freePtr;
	ct->freePtr += ALLOC_SIZE;
	return out;
}

void pool_free(struct pool_context * ct, void * obj) {
	*(void **)obj = ct->freeList;
	ct->freeList = obj;
}

void pool_destroy(struct pool_context * ct) {
	if (ct->metaIndex < (PAGE_SIZE / sizeof(void*)) - 1)
		ct->metaPage[ct->metaIndex] = NULL;
	void ** pageToClear = ct->metaPage;
	while (pageToClear != NULL) {
		for (size_t i = 0; i < (PAGE_SIZE / sizeof(void*)) - 1; i++ ) {
			if (pageToClear[i] == NULL)
				break;
			free(pageToClear[i]);
		}
		pageToClear = (void**)pageToClear[(PAGE_SIZE / sizeof(void*)) - 1];
	}
}
