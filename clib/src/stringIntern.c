#include <stdlib.h>
#include <string.h>
#include "stringIntern.h"

#define LOAD_FACTOR (0.6)
#define START_SIZE (16)

struct Entry {
	size_t hashResult;
	void* str;
};

static size_t getLength(const char* str, size_t maxLength) {
	for (size_t i = 0; i < maxLength; i++, str++) {
		if (*str == 0)
			return i;
	}
	return maxLength;
}

static size_t hashStr(const char* str, size_t strLen)
{
	const unsigned char* ustr = (const unsigned char*) str;//Type pun to unsigned char for the hash
	/* djb2 hash function modified to take a max size */
	size_t hash = 5381;
	int c;
	
	for (size_t i = 0; i < strLen && (c = *ustr++); i++)//Assignment in the condition is intentional
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
		
	return hash;
}

static char cmpStr(const char* a, const char* b, size_t strLen) {
	return !strncmp(a,b,strLen);
}

StringStore intern_create() {
	StringStore out;
	out.arr = calloc(START_SIZE, sizeof(struct Entry));
	out.arrSize = START_SIZE;
	out.size = 0;
	out.mem = range_create((16384 < MAX_INTERN_LENGTH) ? MAX_INTERN_LENGTH : 16384);
	return out;
}

void intern_destroy(StringStore* ct)
{
	free(ct->arr);
	ct->arr = NULL;
	ct->arrSize = 0;
	ct->size = 0;
	range_destroy(&ct->mem);
}

static size_t idealIndex(size_t hash, size_t arrSize)
{
	return hash % arrSize;
}

static size_t realIndex(const char* str, size_t strLen, size_t idealIdx, size_t hash, struct Entry* items, size_t arrSize)
{
	for (size_t i = 0; i < arrSize; i++) {
		size_t idx =  (idealIdx + i) % arrSize;
		if (!items[idx].str || (hash == items[idx].hashResult && cmpStr(items[idx].str, str, strLen))) { //If the space is empty or the strings match return the location
			return idx;
		}
	}
	return 0;//This should never be reached
}

/*
 * Returns the index of the location where an entry should be or is if it exists.
 */
static size_t indexOf(const char* str, size_t strLen, size_t hash, struct Entry* items, size_t arrSize)
{
	return realIndex(str, strLen, idealIndex(hash, arrSize), hash, items, arrSize);
}

static int rehash(StringStore* ct) {
	size_t newArrSize = ct->arrSize * 2;
	struct Entry* newArr = calloc(newArrSize, sizeof(struct Entry));
	if (!newArr)
		return 0;
	struct Entry* items = ct->arr;
	for (size_t i = 0; i < ct->arrSize; i++) {
		if (items[i].str) {
			size_t newIdx = indexOf(items[i].str, MAX_INTERN_LENGTH, items[i].hashResult, newArr, newArrSize);//This is comparing strings alreay interned, so passing in MAX_STRLEN as a limiter is not a problem as a null terminator will exist.
			newArr[newIdx] = items[i];
		}
	}
	free(ct->arr);
	ct->arr = newArr;
	ct->arrSize = newArrSize;
	return 1;
}

const char* intern_intern(StringStore* ct, const char* str, size_t strLen)
{
	if (!str)
		return NULL;
	strLen = (strLen > MAX_INTERN_LENGTH) ? MAX_INTERN_LENGTH : strLen;
	if ((float)ct->size / (float)ct->arrSize >= LOAD_FACTOR) {
		int rehashSuccsess = rehash(ct);
		if (!rehashSuccsess)
			return NULL;
	}
	
	struct Entry* items = ct->arr;
	size_t hash = hashStr(str, strLen);
	
	size_t itemIdx = indexOf(str, strLen, hash, ct->arr, ct->arrSize);
	if (items[itemIdx].str) {//If this is defined, the string has previously been interned, just return the location
		return items[itemIdx].str;
	} else {//Intern the string and return the new allocation.
		size_t realLength = getLength(str, strLen);
		char* internMemory = range_alloc(&ct->mem, realLength + 1);
		memcpy(internMemory, str, realLength);
		internMemory[realLength] = 0; //Add a null terminator.
		items[itemIdx].str = internMemory;
		items[itemIdx].hashResult = hash;
		ct->size++;
		return internMemory;
	}
}
