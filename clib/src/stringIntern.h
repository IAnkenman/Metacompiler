#pragma once
#include "range.h"

#define MAX_INTERN_LENGTH (4095)

typedef struct {
	void* arr;
	size_t arrSize;
	size_t size;
	Range mem;
} StringStore;

StringStore intern_create();

/*
 * Interns a string and returns a pointer to an interned copy of it.
 * This function will automatically prevent duplicate entries.
 * Passing NULL will always map to NULL
 * strSize is the length of the string not including the null terminator
 */
const char* intern_intern(StringStore* ct, const char* str, size_t strLen);

void intern_destroy(StringStore* ct);
