#pragma once
#include <stddef.h>

typedef struct {
	size_t spaceLeft;
	size_t blockSize;
	void* head;
	void* currentBlock;
} Range;

Range range_create(size_t blockSize);

void* range_alloc(Range* ct, size_t size);

void range_destroy(Range* ct);
