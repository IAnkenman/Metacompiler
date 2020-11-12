#include <string.h>
#include "bitset.h"

void bitset_set(Bitset* ct, size_t num)
{
	size_t arrOff = num / 8;
	char byteOff = num % 8;
	
	if (arrOff >= BITSET_SIZE)
		return;
	
	
	char mask = 1 << byteOff;
	ct->bits[arrOff] |= mask;
	return;
}

void bitset_erase(Bitset* ct, size_t num)
{
	size_t arrOff = num / 8;
	char byteOff = num & 8;
	
	if (arrOff >= BITSET_SIZE)
		return;
	
	char mask = 1 << byteOff;
	mask = ~mask;
	ct->bits[arrOff] &= mask;
	return;
}

int bitset_isSet(Bitset* ct, size_t num)
{
	size_t arrOff = num / 8;
	char byteOff = num % 8;
	
	if (arrOff >= BITSET_SIZE)
		return 0;
	
	char mask = 1 << byteOff;
	
	return (ct->bits[arrOff] & mask);
}

void bitset_clear(Bitset* ct)
{
	memset(ct->bits, 0, BITSET_SIZE);
	return;
}

void bitset_union(Bitset* a, Bitset* b)
{
	static const size_t numIter = BITSET_SIZE / sizeof(size_t);
	static const  size_t reside = BITSET_SIZE % sizeof(size_t);
	
	size_t arrOff = 0;
	for (size_t i = 0; i < numIter; i++) {
		size_t* A = (size_t*)(a->bits + arrOff);
		size_t* B = (size_t*)(b->bits + arrOff);
		
		*A |= *B;
		arrOff += sizeof(size_t);
	}
	
	for (size_t i = 0; i < reside; i++) {
		a->bits[arrOff] |= b->bits[arrOff];
		arrOff++;
	}
	return;
}

void bitset_diff(Bitset* a, Bitset* b)
{
	static const size_t numIter = BITSET_SIZE / sizeof(size_t);
	static const size_t reside = BITSET_SIZE % sizeof(size_t);
	
	size_t arrOff = 0;
	for (size_t i = 0; i < numIter; i++) {
		size_t* A = (size_t*)(a->bits + arrOff);
		size_t* B = (size_t*)(b->bits + arrOff);
		
		*A &= ~(*B);
		arrOff += sizeof(size_t);
	}
	
	for (size_t i = 0; i < reside; i++) {
		a->bits[arrOff] &= ~(b->bits[arrOff]);
		arrOff++;
	}
	return;
}

void bitset_intersect(Bitset* a, Bitset* b)
{
	static const size_t numIter = BITSET_SIZE / sizeof(size_t);
	static const size_t reside = BITSET_SIZE % sizeof(size_t);
	
	size_t arrOff = 0;
	for (size_t i = 0; i < numIter; i++) {
		size_t* A = (size_t*)(a->bits + arrOff);
		size_t* B = (size_t*)(b->bits + arrOff);
		
		*A &= *B;
		arrOff += sizeof(size_t);
	}
	
	for (size_t i = 0; i < reside; i++) {
		a->bits[arrOff] &= b->bits[arrOff];
		arrOff++;
	}
	return;
}

Bitset bitset_copy(Bitset* ct) {
	return *ct;
}

void bitset_not(Bitset* ct) {
	
	static const size_t numIter = BITSET_SIZE / sizeof(size_t);
	static const size_t reside = BITSET_SIZE % sizeof(size_t);
	
	size_t arrOff = 0;
	for (size_t i = 0; i < numIter; i++) {
		size_t* A = (size_t*)(ct->bits + arrOff);
		
		*A = ~(*A);
		arrOff += sizeof(size_t);
	}
	
	for (size_t i = 0; i < reside; i++) {
		ct->bits[arrOff] = ~ct->bits[arrOff];
		arrOff++;
	}
	return;
}

int bitset_isEmpty(Bitset* ct) {
	static const size_t numIter = BITSET_SIZE / sizeof(size_t);
	static const size_t reside = BITSET_SIZE % sizeof(size_t);
	
	size_t arrOff = 0;
	size_t acc = 0;
	
	for (size_t i = 0; i < numIter; i++) {
		acc |= *(size_t*)(ct->bits + arrOff);
		arrOff += sizeof(size_t);
	}
	for (size_t i = 0; i < reside; i++) {
		acc |= ct->bits[arrOff];
		arrOff++;
	}
	return !acc;
}
