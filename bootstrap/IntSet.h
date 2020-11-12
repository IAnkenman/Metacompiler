#pragma once
#include <stddef.h>

typedef struct {
	int* arr;
	size_t size;
	size_t maxSize;
} IntSet;

/*
 * Adds an item to the set
 */
void IntSet_add(IntSet* set, int item);

/*
 * Checks if a set contains the given item.
 */
int IntSet_has(IntSet* set, int item);
/*
 * Removes an item from the set.
 */
void IntSet_remove(IntSet* set, int item);

/*
 * Compares two sets for equality
 */
int IntSet_equals(IntSet* a, IntSet* b);

/*
 * Comparision that provides a total ordering between sets.
 */
int IntSet_compare(IntSet* a, IntSet* b);

/*
 * Creates a new set
 */
IntSet IntSet_new();

/*
 * Frees all memeory related to a set
 */
void IntSet_destroy(IntSet* set);
