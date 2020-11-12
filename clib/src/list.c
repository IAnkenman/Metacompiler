#include <stdalign.h>
#include <stddef.h>
#include <string.h>
#include "list.h"

#define NEXT(node) (*(void**)node)
#define PREV(node) (*(void**)(node + sizeof(void*)))

list_context list_create(size_t objSize, size_t blocksize) {
	list_context out;
	out.current = NULL;
	out.size = 0;
	out.objSize = objSize;
    const static size_t align = alignof(max_align_t);
	const static size_t stubSize = sizeof(void*) * 2;
    
	size_t pad = (align - (stubSize % align)) % align;
	size_t allocSize = stubSize + pad + objSize;
	
	out.offset = stubSize + pad;
	out.pool = pool_create(allocSize, blocksize);
	return out;	
}

int list_push(list_context* ct, void* obj) {
	void* newNode = pool_alloc(&ct->pool);
	if (!newNode)
		return -1;

	memcpy(newNode+ct->offset, obj, ct->objSize);
	ct->size++;
	if (!ct->current) {
		ct->current = newNode;
		PREV(newNode) = NEXT(newNode) = newNode;
		return 0;
	}

	NEXT(newNode) = ct->current;
	PREV(newNode) = PREV(ct->current);
	NEXT(PREV(ct->current)) = newNode;
    PREV(ct->current) = newNode;
	return 0;
}

int list_pop(list_context* ct, void* out) {
	if (ct->size == 0) {
		return -1;
	} else if (ct->size == 1) {
		memcpy(out, ct->current+ct->offset, ct->objSize);
		pool_free(&ct->pool, ct->current);
		ct->size = 0;
		ct->current = NULL;
		return 1;
	}
	
	void* node = PREV(ct->current);
	memcpy(out, node+ct->offset, ct->objSize);
	NEXT(PREV(node)) = NEXT(node);
	PREV(NEXT(node)) = PREV(node);
    pool_free(&ct->pool, node);
	ct->size--;
	return 1;
}

static void* seek(void* root, size_t idx, size_t size) {
	size_t half = size/2;
	void* node = root;
	if (idx > half) {
		size_t currentIdx = size;
		while (currentIdx != idx) {
			node = PREV(node);
			currentIdx--;
		}
	} else {
		size_t currentIdx = 0;
		while (currentIdx != idx) {
			node = NEXT(node);
			currentIdx++;
		}
	}
	return node;
}


int list_insert(list_context* ct, size_t idx, void* obj) {
	if (idx > ct->size)
		return -1;
	void* newNode = pool_alloc(&ct->pool);
	if (!newNode)
		return -1;

	memcpy(newNode+ct->offset, obj, ct->objSize);

	if (!ct->current) {
		ct->current = newNode;
		PREV(newNode) = NEXT(newNode) = newNode;
		ct->size = 1;
		return 0;
	}
	
	void* node = seek(ct->current, idx, ct->size);

	NEXT(newNode) = node;
	PREV(newNode) = PREV(node);
	NEXT(PREV(node)) = newNode;
	PREV(node) = newNode;

	ct->size++;
	if (idx == 0)
		ct->current = newNode;
	return 0;
}

int list_delete(list_context* ct, size_t idx) {
	if (idx >= ct->size)
		return -1;
		
	void* node = seek(ct->current, idx, ct->size);

	if (ct->size == 1) {
		ct->current = 0;
		ct->size = 0;
		pool_free(&ct->pool, node);
		return 0;
	}
	if (idx == 0)
		ct->current = NEXT(node);

	NEXT(PREV(node)) = NEXT(node);
	PREV(NEXT(node)) = PREV(node);

	pool_free(&ct->pool, node);
	ct->size--;
	return 0;
}


int list_remove(list_context* ct, size_t idx, void* out) {
	if (idx >= ct->size)
		return -1;
		
	void* node = seek(ct->current, idx, ct->size);

	if (ct->size == 1) {
		ct->current = 0;
		ct->size = 0;
		memcpy(out, node+ct->offset, ct->objSize);
		pool_free(&ct->pool, node);
		return 1;
	}
	if (idx == 0)
		ct->current = NEXT(node);

	NEXT(PREV(node)) = NEXT(node);
	PREV(NEXT(node)) = PREV(node);

	memcpy(out, node+ct->offset, ct->objSize);
	pool_free(&ct->pool, node);
	ct->size--;
	return 1;
}

int list_at(list_context* ct, size_t idx, void* out) {
	if (idx >= ct->size)
		return -1;
	void* node = seek(ct->current, idx, ct->size);
	memcpy(out, node+ct->offset, ct->objSize);
	return 1;
}

size_t list_size(list_context* ct) {
    return ct->size;
}

void list_clear(list_context* ct) {
    while (ct->size > 0) {
        list_delete(ct, 0);
    }
}

void list_destroy(list_context* ct) {
    ct->current = NULL;
    ct->size = 0;
    pool_destroy(&ct->pool);
}

list_iterator list_begin(list_context* ct) {
    list_iterator out;
    out.current = ct->current;
    out.ct = ct;
    return out;
}

void list_iterator_next(list_iterator* it) {
    it->current = NEXT(it->current);
}

void list_iterator_prev(list_iterator* it) {
    it->current = PREV(it->current);
}

void list_iterator_get(list_iterator* it, void* out) {
    memcpy(out, it->current+it->ct->offset, it->ct->objSize);
}

void list_iterator_destory(list_iterator* it) {
    it->ct = NULL;
    it->current = NULL;
}

