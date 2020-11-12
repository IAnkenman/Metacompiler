#pragma once
#include <stddef.h>

typedef struct {
    void** arr;
    size_t sizeBytes;
    size_t arrSize;
    size_t arrMaxSize;
} VarArr;

VarArr varArr_create();

void** varArr_index(VarArr* ct, size_t idx);

void* varArr_get(VarArr* ct, size_t idx);

void* varArr_top(VarArr* ct);

void varArr_push(VarArr* ct, void* obj);

void* varArr_pop(VarArr* ct);

void varArr_insert(VarArr* ct, size_t idx, void* obj);

void varArr_remove(VarArr* ct, size_t idx);

void varArr_reserve(VarArr* ct, size_t num);

void varArr_clear(VarArr* ct);

size_t varArr_size(VarArr* ct);

size_t varArr_maxSize(VarArr* ct);

void varArr_destroy(VarArr* ct);
