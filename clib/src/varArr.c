#include "varArr.h"
#include <stdlib.h>
#include <string.h>

//TODO update vararr to store only void*

VarArr varArr_create() {
    VarArr out;
	static const size_t reserve = 64;
    
    out.arr = malloc(reserve);
    if (!out.arr) {
        out.arrSize = 0;
        out.sizeBytes = 0;
        out.arrMaxSize = 0;
        return out;
    }
    
    out.arrSize = 0;
    out.sizeBytes = reserve;
    out.arrMaxSize = reserve / sizeof(void*);
    return out;
}

void** varArr_index(VarArr* ct, size_t idx) {
    if (idx >= ct->arrSize)
        return NULL;
    return &ct->arr[idx];
}

void* varArr_get(VarArr* ct, size_t idx) {
    if (idx >= ct->arrSize)
        return NULL;
    return ct->arr[idx];
}

void* varArr_top(VarArr* ct) {
    if (ct->arrSize == 0)
        return NULL;
    return ct->arr[ct->arrSize - 1];
}

void varArr_push(VarArr* ct, void* obj) {
    if (ct->arrSize == ct->arrMaxSize) {
		size_t newSize = ct->sizeBytes << 1;
        void* newArr = realloc(ct->arr, newSize);
        if (!newArr)
            return;
        ct->arr = newArr;
        ct->sizeBytes = newSize;
        ct->arrMaxSize = newSize/sizeof(void*);
    }
    ct->arr[ct->arrSize] = obj;
    ct->arrSize++;
}

void* varArr_pop(VarArr* ct) {
    if (ct->arrSize == 0)
        return NULL;
    ct->arrSize--;
    return ct->arr[ct->arrSize];
}

void varArr_insert(VarArr* ct, size_t idx, void* obj) {
    if (idx > ct->arrSize)
        return;
    if (ct->arrSize == ct->arrMaxSize) {
		size_t newSize = ct->sizeBytes << 1;
        void* newArr = realloc(ct->arr, newSize);
        if (!newArr)
            return;
        ct->arr = newArr;
        ct->sizeBytes = newSize;
        ct->arrMaxSize = newSize/sizeof(void*);
    }
    size_t bytesToMove = (ct->arrSize - idx) * sizeof(void*);
    memmove(&ct->arr[idx+1], &ct->arr[idx], bytesToMove);
	ct->arr[idx] = obj;
    ct->arrSize++;
}

void varArr_remove(VarArr* ct, size_t idx) {
    if (idx >= ct->arrSize)
        return;
    size_t bytesToMove = (ct->arrSize-idx-1)*sizeof(void*);
    memcpy(&ct->arr[idx], &ct->arr[idx+1], bytesToMove);
	ct->arrSize--;
}

void varArr_reserve(VarArr* ct, size_t num) {
    if (num <= ct->arrMaxSize)
        return;
    
    size_t reserve = (num)?num:8;
    reserve = reserve*sizeof(void*); //Scale reserve by size of objects
    size_t v = reserve;
	
	//Snippet thanks to https://graphics.stanford.edu/~seander/bithacks.html
    //Gets the power of 2 above the supplied number
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v |= v >> 32;
    v++;
    
    reserve = v; //Reserve now contains the number of bytes needed for the array rounded up to the nearest power of 2
    void* newArr = realloc(ct->arr, reserve);
    if (!newArr)
        return;
    ct->arr = newArr;
    ct->sizeBytes = reserve;
    ct->arrMaxSize = reserve/sizeof(void*);
}

size_t varArr_size(VarArr* ct) {
    return ct->arrSize;
}

size_t varArr_maxSize(VarArr* ct) {
    return ct->arrMaxSize;
}


void varArr_destroy(VarArr* ct) {
    if (ct->arr)
        free(ct->arr);
    ct->arr = NULL;
    ct->arrMaxSize = 0;
    ct->arrSize = 0;
    ct->sizeBytes = 0;
}

void varArr_clear(VarArr* ct)
{
	ct->arrSize = 0;
}

