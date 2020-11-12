#pragma once
#include <stddef.h>

typedef struct {
    void** arr;
    size_t sizeBytes;
    size_t arrSize;
    size_t arrMaxSize;
} VarArr;

VarArr vararr_create();

void** vararr_index(VarArr* ct, size_t idx);

void* vararr_get(VarArr* ct, size_t idx);

void* vararr_top(VarArr* ct);

void vararr_push(VarArr* ct, void* obj);

void* vararr_pop(VarArr* ct);

void vararr_insert(VarArr* ct, size_t idx, void* obj);

void vararr_remove(VarArr* ct, size_t idx);

void vararr_reserve(VarArr* ct, size_t num);

void vararr_clear(VarArr* ct);

size_t vararr_size(VarArr* ct);

size_t vararr_maxSize(VarArr* ct);

void vararr_destroy(VarArr* ct);
