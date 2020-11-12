#include "IntSetMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void IntSetMap_insert(IntSetMap* map, IntSet* set, int item)
{
	if (map->size == 0) {
		map->setStorage[0] = IntSet_new();
		for (size_t i = 0; i < set->size; i++) {
			IntSet_add(&map->setStorage[0], set->arr[i]);
		}
		map->mapStorage[0] = item;
		map->size++;
	} else {
		size_t l = 0;
		size_t r = map->size - 1;
		while (l != r) {
			size_t m = (l + r + 1) / 2;
			if (IntSet_compare(set, &map->setStorage[map->setOrder[m]]) < 0)
				r = m - 1;
			else
				l = m;
		}
		size_t idx = l;
		if (IntSet_equals(set, &map->setStorage[map->setOrder[idx]])) {
			map->mapStorage[map->setOrder[idx]] = item;
			return;
		} else {
			if (map->size == map->maxSize) {
				size_t newSize = map->maxSize * 2;
				IntSet* newStorageArr = realloc(map->setStorage, sizeof(IntSet) * newSize);
				int* newMapStorage = realloc(map->mapStorage, sizeof(int) * newSize);
				size_t* newSetOrder = realloc(map->setStorage, sizeof(size_t) * newSize);
				
				if (newStorageArr && newMapStorage && newSetOrder) {
					map->mapStorage = newMapStorage;
					map->setOrder = newSetOrder;
					map->setStorage = newStorageArr;
					map->size = newSize;
				} else {
					printf("Failed to reallocate IntSetMap arrays\n");
					return;
				}
			}
			if (IntSet_compare(set, &map->setStorage[map->setOrder[idx]]) < 0)
				idx++;
			size_t* src = map->setOrder+idx;
			size_t* dest = map->setOrder+idx+1;
			size_t num = map->size - idx;
			memmove(dest, src, num * sizeof(size_t));
			map->setStorage[map->size] = IntSet_new();
			for (size_t i = 0; i < set->size; i++) {
				IntSet_add(&map->setStorage[map->size], set->arr[i]);
			}
			map->mapStorage[map->size] = item;
			map->setOrder[idx] = map->size;
			map->size++;
		}
	}
}

IntSetMap IntSetMap_new()
{
	IntSetMap out;
	out.mapStorage = malloc(sizeof(int) * 8);
	out.setOrder = malloc(sizeof(size_t) * 8);
	out.setStorage = malloc(sizeof(IntSet) * 8);
	out.maxSize = 8;
	out.size = 0;
	return out;
}

int IntSetMap_has(IntSetMap* map, IntSet* set)
{
	 if (map->size == 0) {
		 return 0;
	} else {
		size_t l = 0;
		size_t r = map->size - 1;
		while (l != r) {
			size_t m = (l + r + 1) / 2;
			if (IntSet_compare(set, &map->setStorage[map->setOrder[m]]) < 0)
				r = m - 1;
			else
				l = m;
		}
		size_t idx = l;
		if (IntSet_equals(set, &map->setStorage[map->setOrder[idx]])) {
			return 1;
		} else {
			return 0;
		}
	}
}

void IntSetMap_destroy(IntSetMap* map)
{
	for (size_t i = 0; i < map->size; i++) {
		IntSet_destroy(&map->setStorage[i]);
	}
	free(map->setStorage);
	free(map->mapStorage);
	free(map->setOrder);
	map->setStorage = NULL;
	map->mapStorage = NULL;
	map->setOrder = NULL;
	map->size = 0;
	map->maxSize = 0;
	return;
}
