#include <string.h>
#include <stdlib.h>
#include "IntSet.h"

void IntSet_add(IntSet* set, int item) {
	if (set->size == 0) {
		set->arr[0] = item;
		set->size++;
	} else {
		size_t l = 0;
		size_t r = set->size - 1;
		while (l != r) {
			size_t m = (l + r + 1) / 2;
			if (item < set->arr[m])
				r = m - 1;
			else
				l = m;
		}
		size_t idx = l;
		if (set->arr[idx] == item) {
			return;
		} else {
			if (set->size == set->maxSize) {
				int* newArr = realloc(set->arr, sizeof(int) * set->maxSize * 2);
				if (newArr)
					set->arr = newArr;
				else
					return;
			}
			if (set->arr[idx] < item)
				idx++;
			int* src = set->arr+idx;
			int* dest = set->arr+idx+1;
			size_t num = set->size - idx;
			memmove(dest, src, num* sizeof(int));
			set->arr[idx] = item;
			set->size++;
		}
	}
}

int IntSet_has(IntSet* set, int item)
{
	if (set->size == 0) {
		return 0;
	} else {
	size_t l = 0;
		size_t r = set->size - 1;
		while (l != r) {
			size_t m = (l + r + 1) / 2;
			if (item < set->arr[m])
				r = m - 1;
			else
				l = m;
		}
		size_t idx = l;
		if (set->arr[idx] == item)
			return 1;
		else
			return 0;
	}
}


void IntSet_remove(IntSet* set, int item)
{
	if (set->size == 0)
		return;
	size_t l = 0;
	size_t r = set->size - 1;
	while (l != r) {
		size_t m = (l + r + 1) / 2;
		if (item < set->arr[m])
			r = m - 1;
		else
			l = m;
	}
	size_t idx = l;
	if (set->arr[idx] == item) {
		int* dest = set->arr+idx;
		int* src = dest + 1;
		size_t num = set->size - idx - 1;
		memmove(dest, src, sizeof(int) * num);
		set->size--;
	} else {
		return;
	}
}


int IntSet_equals(IntSet* a, IntSet* b) {
	if (a->size != b->size)
		return 0;
	for (size_t i = 0; i < a->size; i++) {
		if (a->arr[i] != b->arr[i])
			return 0;
	}
	return 1;
}

IntSet IntSet_new() {
	IntSet out;
	out.arr = malloc(sizeof(int) * 8); //Allocate starting size
	out.size = 0;
	out.maxSize = 8;
	return out;
}

void IntSet_destroy(IntSet* set)
{
	free(set->arr);
	set->maxSize = 0;
	set->size = 0;
	set->arr = NULL;
}

int IntSet_compare(IntSet* a, IntSet* b)
{
	if (a->size > b->size)
		return 1;
	else if (a->size < b->size)
		return -1;
	else {
		for (size_t i = 0; i < a->size; i++) {
			if (a->arr[i] > b->arr[i])
				return 1;
			else if (a->arr[i] < b->arr[i])
				return -1;
			else
				continue;
		}
		return 0;
	}
}
