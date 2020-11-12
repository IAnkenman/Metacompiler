#pragma once
#include <stddef.h>
#include "pool.h"

typedef struct {
	void* current;
	size_t size;
	size_t offset;
	size_t objSize;
	pool_context pool;
} list_context;

typedef struct {
    void* current;
    list_context* ct;
} list_iterator;

list_context list_create(size_t objSize, size_t blocksize);

int list_push(list_context* ct, void* obj);

int list_pop(list_context* ct, void* out);

int list_insert(list_context* ct, size_t idx, void* obj);

int list_delete(list_context* ct, size_t idx);

int list_remove(list_context* ct, size_t idx, void* out);

int list_at(list_context* ct, size_t idx, void* out);

size_t list_size(list_context* ct);

void list_clear(list_context* ct);

void list_destroy(list_context* ct);

list_iterator list_begin(list_context* ct);

void list_iterator_next(list_iterator* it);

void list_iterator_prev(list_iterator* it);

void list_iterator_get(list_iterator* it, void* out);

void list_iterator_destory(list_iterator* it);
