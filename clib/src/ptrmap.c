#include <stdlib.h>
#include "ptrmap.h"

#define LOAD_FACTOR (0.6)
#define START_SIZE (16)

struct Entry {
	void* ptr;
	void* item;
};

PtrMap ptrmap_create() {
	PtrMap out;
	out.arr = calloc(START_SIZE, sizeof(struct Entry));
	out.arrSize = START_SIZE;
	out.size = 0;
	return out;
}

void ptrmap_destroy(PtrMap* ct)
{
	free(ct->arr);
	ct->arr = NULL;
	ct->arrSize = 0;
	ct->size = 0;
}

static size_t idealIndex(void* ptr, size_t arrSize)
{
	return (size_t)ptr % arrSize;
}

static size_t realIndex(void* ptr, size_t idealIdx, struct Entry* items, size_t arrSize)
{
	for (size_t i = 0; i < arrSize; i++) {
		size_t idx =  (idealIdx + i) % arrSize;
		if (!items[idx].ptr || items[idx].ptr == ptr) {
			return idx;
		}
	}
	return 0;//This should never be reached
}

/*
 * Returns the index of the location where an entry should be or is if it exists.
 */
static size_t indexOf(void* ptr, struct Entry* items, size_t arrSize)
{
	return realIndex(ptr, idealIndex(ptr, arrSize), items, arrSize);
}

static int rehash(PtrMap* ct) {
	size_t newArrSize = ct->arrSize * 2;
	struct Entry* newArr = calloc(newArrSize, sizeof(struct Entry));
	if (!newArr)
		return 0;
	struct Entry* items = ct->arr;
	for (size_t i = 0; i < ct->arrSize; i++) {
		if (items[i].ptr) {
			size_t newIdx = indexOf(items[i].ptr, newArr, newArrSize);
			newArr[newIdx] = items[i];
		}
	}
	free(ct->arr);
	ct->arr = newArr;
	ct->arrSize = newArrSize;
	return 1;
}

void* ptrmap_insert(PtrMap* ct, void* ptr, void* item)
{
	if (!ptr)
		return NULL;
	if ((float)ct->size / (float)ct->arrSize >= LOAD_FACTOR) {
		int rehashSuccsess = rehash(ct);
		if (!rehashSuccsess)
			return NULL;
	}
	
	struct Entry* items = ct->arr;
	
	size_t itemIdx = indexOf(ptr, items, ct->arrSize);
	if (items[itemIdx].ptr) {
		void* out;
		out = items[itemIdx].item;
		items[itemIdx].item = item;
		return out;
	} else {
		items[itemIdx].item = item;
		items[itemIdx].ptr = ptr;
		ct->size++;
		return NULL;
	}
}

int ptrmap_has(PtrMap* ct, void* ptr)
{
	if (!ptr)
		return 0;
	struct Entry* items = ct->arr;
	size_t idx = indexOf(ptr, items, ct->arrSize);
	if (items[idx].ptr == ptr)
		return 1;
	else
		return 0;
}

void* ptrmap_get(PtrMap* ct, void* ptr)
{
	if (!ptr)
		return NULL;
	struct Entry* items = ct->arr;
	size_t idx = indexOf(ptr, items, ct->arrSize);
	
	if (items[idx].ptr == ptr)
		return items[idx].item;
	else
		return NULL;
}

void* ptrmap_remove(PtrMap* ct, void* ptr)
{
	if (!ptr)
		return NULL;
	void* out = NULL;
	struct Entry* items = ct->arr;
	size_t idx = indexOf(ptr, items, ct->arrSize);
	if (items[idx].ptr == ptr) { //If the key to delete was actually found
		ct->size--;
		out = items[idx].item;//Save the value of the item to return
		for (size_t i = 1; i < ct->arrSize; i++) { //For every spot in the table look for a replacement
			size_t searchIdx = (i + idx) % ct->arrSize;
			if (!items[searchIdx].ptr) { //If an empty space was encountered, simply delete the entry and do nothing else
				items[idx].ptr = NULL;
				return out;
			} else {//If an item was found see if it can be moved up
				size_t idealLoc = idealIndex(items[searchIdx].ptr, ct->arrSize);
				size_t itemOffset;
				
				if (idealLoc <= searchIdx)//Compute the offset of the ideal location from the actual location
					itemOffset = searchIdx - idealLoc;
				else
					itemOffset = ct->arrSize - (idealLoc - searchIdx);
				
				if (itemOffset >= i) {//if the item's ideal location is at or beyond the location of the hole, it can be moved up
					items[idx] = items[searchIdx];
					items[searchIdx].ptr = NULL;
					return out;
				}
			}
		}
		return NULL; //This should never be reached
	} else {
		return NULL;
	}
}


int ptrmap_equals(PtrMap* a, PtrMap* b)
{
	if (a->arrSize > b->arrSize) {
		PtrMap* tmp = a;
		a = b;
		b = tmp;
	}
	struct Entry* aItems = a->arr;
	struct Entry* bItems = b->arr;
	for (size_t i = 0; i < a->arrSize; i++) {
		if (aItems[i].ptr) {
			size_t bIdx = indexOf(aItems[i].ptr, bItems, b->arrSize);
			if (bItems[bIdx].ptr == aItems[i].ptr)
				continue;
			else
				return 0;
		}
	}
	return -1;
}

PtrMap_iterator ptrmap_begin(PtrMap* ct)
{
	PtrMap_iterator out;
	out.ct = ct;
	out.current = NULL;
	
	struct Entry* arr = ct->arr;
	
	if (ct)
		for (size_t i = 0; i < ct->arrSize; i++) {
			if (arr[i].ptr) {
				out.current = arr[i].ptr;
				return out;
			}
		}
	return out;
}

void ptrmap_iterator_next(PtrMap_iterator* it)
{
	struct Entry* arr = it->ct->arr;
	if (it && it->current)
		for (size_t i = 0; i < it->ct->arrSize; i++) {
			if (arr[i].ptr) {
				it->current = arr[i].ptr;
			}
		}
}

void* ptrmap_iterator_item(PtrMap_iterator* it)
{
	if (it && it->current)
		return ((struct Entry*)it->current)->item;
	else
		return NULL;
}

void* ptrmap_iterator_key(PtrMap_iterator* it)
{
	if (it && it->current)
		return ((struct Entry*)it->current)->ptr;
	else
		return NULL;
}
