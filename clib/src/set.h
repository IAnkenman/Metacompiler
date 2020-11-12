#pragma once
#include "pool.h"

typedef struct {
    pool_context pool;
    void* root;
    size_t size;
} set_context;

typedef struct {
    set_context* ct;
    void* node;
} set_iterator;

/*
 * Creates a set given a keysize, compare fuunction, and block allocation size
 */
set_context set_create();

/*
 * Adds a member to a set. Returns 0 if succsessful, 1 if the node already existed and -1 if space allocation for the new node fails.
 */
int set_add(set_context* ct, void* key);

/*
 * Removes a member from the set.
 */
void set_remove(set_context* ct, void* key);

/*
 * Returns 1 if item exists, 0 if not.
 */
int set_has(set_context* ct, void* key);

/*
 * Clears a set of all members
 */
void set_clear(set_context* ct);

/*
 * Destroys a set and frees all memory associated with it.
 */
void set_destroy(set_context* ct);

/*
 * Returns the size of the set. set_context::size can also be checked directly.
 */
size_t set_size(set_context* ct);

/*
 * Returns an iterator to the beginning of the set.
 */
set_iterator set_begin(set_context* ct);

/*
 * Returns an iterator to the end of the set.
 */
set_iterator set_end(set_context* ct);

/*
 * Advances an iterator to the next element
 */
void set_iterator_next(set_iterator* it);

/*
 * Backtracks an iterator to the previous element.
 */
void set_iterator_prev(set_iterator* it);

/*
 * Gets the key currently pointed to by the iterator and copies it to location pointed to by out.
 * Returns 1 if data is available, -1 if the iterator points to the end of the set.
 */
int set_iterator_get(set_iterator* it, void** out);

/*
 * Compares iterators for equality. 1 if equal, 0 if not
 */
int set_iterator_cmp(set_iterator* a, set_iterator* b);
