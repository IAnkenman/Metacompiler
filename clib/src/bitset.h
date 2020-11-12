#pragma once
#include <stddef.h>

#define BITSET_SIZE 32
typedef struct {
	char bits[BITSET_SIZE];
} Bitset;

void bitset_set(Bitset* ct, size_t num);

void bitset_erase(Bitset* ct, size_t num);

int bitset_isSet(Bitset* ct, size_t num);

void bitset_clear(Bitset* ct);

/*
 * Performs the union of sets a and b. a will contain the resultant set.
 */
void bitset_union(Bitset* a, Bitset* b);

/*
 * Performs the difference of sets a and b. a will contain all elements in a that were not in b
 */
void bitset_diff(Bitset* a, Bitset* b);

/*
 * Performs the intersection of sets a and b. a will contain the resultant set.
 */
void bitset_intersect(Bitset* a, Bitset* b);

Bitset bitset_copy(Bitset* ct);

/*
 * Inverts a bitset
 */
void bitset_not(Bitset* ct);

int bitset_isEmpty(Bitset* ct);

