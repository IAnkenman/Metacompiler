#pragma once
#include "IntSet.h"

typedef struct {
	//Array that stores all the sets given to the object.
	IntSet* setStorage;
	//Array that stores a set's mapped integer
	int* mapStorage;
	//Array of indexes into the setStorage sorted by their respective set's total ordering
	size_t* setOrder;
	//Number of sets currently stored
	size_t size;
	//Maximum number of sets that can be stored
	size_t maxSize;
} IntSetMap;

/*
 * Creates a map
 */
IntSetMap IntSetMap_new();

/*
 * Checks if the set given exists already
 */
int IntSetMap_has(IntSetMap* map, IntSet* set);

/*
 * Inserts the given set and maps it to an integer.
 */
void IntSetMap_insert(IntSetMap* map, IntSet* set, int item);

/*
 * Frees all memory held by the data structure
 */
void IntSetMap_destroy(IntSetMap* map);
