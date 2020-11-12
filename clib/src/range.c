#include "range.h"
#include <stdlib.h>
#include <stdalign.h>
#define OFFSET (((alignof(max_align_t) - (sizeof(void*) % alignof(max_align_t))) % alignof(max_align_t)) + sizeof(void*))

Range range_create(size_t blockSize)
{
	Range out;
	if (blockSize <= OFFSET) {
		out.blockSize = OFFSET;
		out.currentBlock = NULL;
		out.spaceLeft = 0;
		out.head = NULL;
		return out;
	}
	out.blockSize = blockSize;
	out.currentBlock = malloc(blockSize);
	*(void**)out.currentBlock = NULL;
	out.head = out.currentBlock + OFFSET;
	out.spaceLeft = blockSize - OFFSET;
	return out;
}

void* range_alloc(Range* ct, size_t size)
{
	size_t pad = (alignof(max_align_t) - (size % alignof(max_align_t))) % alignof(max_align_t);
	size += pad;
	if (size > ct->blockSize - OFFSET)
		return NULL;
	if (size > ct->spaceLeft) {
		void* newBlock = malloc(ct->blockSize);
		if (!newBlock)
			return NULL;
		*(void**)newBlock = ct->currentBlock;
		ct->currentBlock = newBlock;
		ct->spaceLeft = ct->blockSize - OFFSET;
		ct->head = newBlock + OFFSET;
	}
	void* out = ct->head;
	ct->head += size;
	ct->spaceLeft -= size;
	return out;
}

void range_destroy(Range* ct)
{
	void* next;
	void* current = ct->currentBlock;
	while (current) {
		next = *(void**)current;
		free(current);
		current = next;
	}
	return;
}
