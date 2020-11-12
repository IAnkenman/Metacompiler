#pragma once
#include <stddef.h>

typedef struct {
	void* arr;
	size_t arrSize;
	size_t size;
} PtrMap;

typedef struct {
	void* current;
	PtrMap* ct;
} PtrMap_iterator;

PtrMap ptrmap_create();

void* ptrmap_insert(PtrMap* ct, void* hash, void* item);

void* ptrmap_remove(PtrMap* ct, void* hash);

/*
 * Returns 1 if key exists, 0 if not
 */
int ptrmap_has(PtrMap* ct, void* hash);

void* ptrmap_get(PtrMap* ct, void* hash);

void ptrmap_destroy(PtrMap* ct);

/*
 * Returns 1 if the hashmaps contain the same elements, 0 if not
 */
int ptrmap_equals(PtrMap* a, PtrMap* b);

PtrMap_iterator ptrmap_begin(PtrMap* ct);

void ptrmap_iterator_next(PtrMap_iterator* it);

void*  ptrmap_iterator_key(PtrMap_iterator* it);

void*  ptrmap_iterator_item(PtrMap_iterator* it);
