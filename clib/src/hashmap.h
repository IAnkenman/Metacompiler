#pragma once
#include <stddef.h>

typedef struct {
	int(*cmp)(void*, void*);
	size_t(*hash)(void*);
	void* arr;
	size_t arrSize;
	size_t size;
} HashMap;

typedef struct {
	HashMap* ct;
	void* current;
} HashMap_iterator;

HashMap hashmap_create(size_t startSize, size_t(*hash)(void*), int(*cmp)(void*, void*));

void* hashmap_insert(HashMap* ct, void* key, void* item);

void* hashmap_remove(HashMap* ct, void* key);

/*
 * Returns 1 if key exists, 0 if not
 */
int hashmap_has(HashMap* ct, void* key);

void* hashmap_get(HashMap* ct, void* key);

void hashmap_destroy(HashMap* ct);

/*
 * Returns 1 if the hashmaps contain the same elements 0 if not
 */
int hashmap_equals(HashMap* a, HashMap* b);

HashMap_iterator hashmap_begin(HashMap* a);

HashMap_iterator hashmap_end(HashMap* a);

void hashmap_iterator_next(HashMap_iterator* it);

void* hashmap_iterator_key(HashMap_iterator* it);

void* hashmap_iterator_val(HashMap_iterator* it);
