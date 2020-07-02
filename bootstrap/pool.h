#include <stdlib.h>
/*
The pool is used to quickly allocate and deallocate objects of a fixed size less than PAGE_SIZE and at least 16 bytes.
*/
struct pool_context {
	void ** metaPage;
	void * freeList;
	void * pageBgn;
	void * freePtr;
	size_t metaIndex;
};

struct pool_context pool_new();

void * pool_alloc(struct pool_context *);

void pool_free(struct pool_context *, void * obj);

void pool_destroy(struct pool_context *);

