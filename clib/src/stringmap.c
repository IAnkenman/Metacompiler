#include <stdlib.h>
#include <stdio.h>
#include "stringmap.h"

#define LOAD_FACTOR (0.6)
#define START_SIZE (16)
struct Entry {
	size_t hashResult;
	void* key;
	void* item;
	char isItemFlag;
};

StringMap stringmap_create() { StringMap out; out.arr = NULL; return out;}

void* stringmap_insert(StringMap* ct, void* hash, void* item) {return NULL;}

void* stringmap_remove(StringMap* ct, void* hash) {return NULL;}

/*
 * Returns 1 if key exists, 0 if not
 */
int stringmap_has(StringMap* ct, void* hash) {return 1;}

void* stringmap_get(StringMap* ct, void* hash) { return NULL;}

void stringmap_destroy(StringMap* ct) {}

/*
 * Returns 1 if the StringMaps contain the same elements, 0 if not
 */
int stringmap_equals(StringMap* a, StringMap* b) {return 1;}
