#pragma once
#include <stddef.h>

typedef struct {
	int(*cmp)(void*, void*);
	size_t(*hash)(void*);
	void* arr;
	size_t arrSize;
	size_t size;
} StringMap;

StringMap stringmap_create();

void* stringmap_insert(StringMap* ct, void* hash, void* item);

void* stringmap_remove(StringMap* ct, void* hash);

/*
 * Returns 1 if key exists, 0 if not
 */
int stringmap_has(StringMap* ct, void* hash);

void* stringmap_get(StringMap* ct, void* hash);

void stringmap_destroy(StringMap* ct);

/*
 * Returns 1 if the StringMaps contain the same elements, 0 if not
 */
int stringmap_equals(StringMap* a, StringMap* b);
