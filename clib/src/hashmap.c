#include <stdlib.h>
#include "hashmap.h"

#define LOAD_FACTOR (0.6)
struct Entry {
	size_t hashResult;
	void* key;
	void* item;
	char hadItemFlag;
	char isItemFlag;
};

HashMap hashmap_create(size_t startSize, size_t(*hash)(void*), int(*cmp)(void*, void*)) {
	HashMap out;
	
	if (startSize < 10)
		startSize = 10;
	out.arr = calloc(sizeof(struct Entry), startSize);
	out.arrSize = startSize;
	out.size = 0;
	out.cmp = cmp;
	out.hash = hash;
	return out;
}

void hashmap_destroy(HashMap* ct)
{
	free(ct->arr);
	ct->arr = NULL;
	ct->arrSize = 0;
	ct->cmp = NULL;
	ct->hash = NULL;
}

static inline void* put(struct Entry* arr, int(*cmp)(void*, void*), size_t arrSize, size_t hash, void* key, void* item, size_t* size) {
	size_t index = hash % arrSize;
	struct Entry* e = &arr[index];
	
	if (e->isItemFlag == 0) {
		e->isItemFlag = 1;
		e->hadItemFlag = 1;
		e->hashResult = hash;
		e->item = item;
		e->key = key;
		(*size)++;
		return NULL;
	} else if (cmp(e->key, key) == 0) {
		void* out = e->item;
		e->item = item;
		return out;
	} else {
		for (size_t acc = 1; acc > 0; acc = acc << 1) {//Quadratic addressing with an accumulator
			size_t i = (index + acc) % arrSize;
			e = &arr[i];
			if (e->isItemFlag == 0) {
				e->isItemFlag = 1;
				e->hadItemFlag = 1;
				e->hashResult = hash;
				e->item = item;
				e->key = key;
				(*size)++;
				return NULL;
			} else if (cmp(e->key, key) == 0) {
				void* out = e->item;
				e->item = item;
				return out;
			}
		}
		return NULL;
	}
}

static inline int rehash(HashMap* ct) {
	size_t oldSize = ct->arrSize;
	size_t newSize = oldSize * 2;
	size_t empty;
	struct Entry* oldArr = ct->arr;
	struct Entry* newArr = malloc(newSize*sizeof(struct Entry));
	if (!newArr)
		return -1;
	for (size_t i = 0; i < ct->arrSize; i++) {
		struct Entry* e = &oldArr[i];
		if (e->isItemFlag)
			put(newArr, ct->cmp, newSize, e->hashResult, e->key, e->item, &empty);
	}
	free(oldArr);
	ct->arr = newArr;
	ct->arrSize = newSize;
	return 0;
}

void* hashmap_insert(HashMap* ct, void* key, void* item)
{
	if (ct->size / ct->arrSize >= LOAD_FACTOR)
		if (rehash(ct) < 0)
			return NULL;
			
	size_t result = ct->hash(key);
	void* out = put(ct->arr, ct->cmp, ct->arrSize, result, key, item, &ct->size);
	return out;
}


static inline struct Entry* find(size_t hash, void* key, size_t arrSize, struct Entry* arr, int(*cmp)(void*, void*))
{
	size_t index = hash % arrSize;
	struct Entry* e = &arr[index];
	
	if (e->isItemFlag == 0 && e->hadItemFlag == 0) {
		return NULL;
	} else if (e->isItemFlag && cmp(e->key, key) == 0) {
		return e;
	} else {
		for (size_t acc = 1; acc > 0; acc = acc << 1) {//Quadratic addressing with an accumulator
			size_t i = (index + acc) % arrSize;
			e = &arr[i];
			if (e->isItemFlag == 0 && e->hadItemFlag == 0) {
				return NULL;
			} else if (e->isItemFlag && cmp(e->key, key) == 0) {
				return e;
			}
		}
		return NULL;
	}
}

int hashmap_has(HashMap* ct, void* key)
{
	size_t result = ct->hash(key);
	struct Entry* e = find(result, key, ct->arrSize, ct->arr, ct->cmp);
	if (e)
		return 1;
	else
		return 0;
}

void* hashmap_get(HashMap* ct, void* key)
{
	size_t result = ct->hash(key);
	struct Entry* e = find(result, key, ct->arrSize, ct->arr, ct->cmp);
	if (e)
		return e->item;
	else
		return NULL;
}

void* hashmap_remove(HashMap* ct, void* key)
{
	size_t result = ct->hash(key);
	struct Entry* e = find(result, key, ct->arrSize, ct->arr, ct->cmp);
	if (e) {
		e->isItemFlag = 0;
		ct->size--;
		return e->item;
	} else
		return NULL;
}


int hashmap_equals(HashMap* a, HashMap* b)
{
	if (
		a->size == b->size &&
		a->cmp == b->cmp &&
		a->hash == b->hash
	)
	{
		if (a->arrSize > b->arrSize) {
			HashMap* tmp = a;
			a = b;
			b = tmp;
		}
		struct Entry* aArr = a->arr;
		struct Entry* bArr = b->arr;
		for (size_t i = 0; i < a->arrSize; i++) {
			struct Entry* aVal = &aArr[i];
			if (aVal->isItemFlag) {
				struct Entry* bVal = find(aVal->hashResult, aVal->key, b->arrSize, bArr, a->cmp);
				if (bVal)
					continue;
				else
					return 0;
			}
		}
		return 1;
	}
	else return 0;
}

HashMap_iterator hashmap_begin(HashMap* a)
{
	struct Entry* entries = a->arr;
	HashMap_iterator out;
	out.ct = a;
	for (size_t i = 0; i < a->arrSize; i++) {
		if (entries[0].isItemFlag) {
			out.current = entries;
			return out;
		}
		entries++;
	}
	out.current = NULL;
	return out;
}

HashMap_iterator hashmap_end(HashMap* a)
{
	HashMap_iterator out;
	out.ct = a;
	out.current = NULL;
	return out;
}

void hashmap_iterator_next(HashMap_iterator* it)
{
	if (it->current == NULL)
		return;
	size_t offset = (it->current - it->ct->arr);
	size_t location = offset / sizeof(struct Entry);
	for (size_t i = location; i < it->ct->arrSize; i++) {
		it->current += sizeof(struct Entry);
		struct Entry* e = it->current;
		if (e->isItemFlag) {
			return;
		}
	}
	it->current = NULL;
	return;
}

void * hashmap_iterator_key(HashMap_iterator* it)
{
	if (it->current == NULL)
		return NULL;
	struct Entry* e = it->current;
	return e->key;
}

void * hashmap_iterator_val(HashMap_iterator* it)
{
	if (it->current == NULL)
		return NULL;
	struct Entry* e = it->current;
	return e->item;
}
