/**
 * An array.
 *
 * - Pushes and pops from both ends are O(1).
 * - Accessing the first and last elements are also O(1).
 * - All other accesses are O(n).
 * - Forward and backward iteration is supported.
 *
 * @file array.h
 */
#ifndef INCLUDE_METALDATA_ARRAY_H_
#define INCLUDE_METALDATA_ARRAY_H_

#include "annotations.h"
#include "configuration.h"
#include "memblklist.h"
#include "metaldata.h"
#include <stdbool.h>
#include <stddef.h>

#define MDL_DEFAULT_ARRAY_BLOCK_SIZE 8

typedef void *MDLArrayBlock[MDL_DEFAULT_ARRAY_BLOCK_SIZE];

typedef struct MDLArray_
{
    MDLMemBlkList block_list;
    size_t length;
    mdl_destructor_fptr elem_destructor;

    /**
     * True if this struct was allocated with @ref mdl_malloc and needs to be freed upon
     * destruction. Having this explicitly specified allows users call
     * @ref mdl_array_destroy on an array, regardless of whether it was statically
     * allocated or not.
     */
    bool was_allocated;
} MDLArray;

typedef struct MDLArrayIterator_
{
    MDLArray *array;
    MDLMemBlkListIterator block_iterator;
    size_t block_element_index;

    /**
     * True if this struct was allocated with @ref mdl_malloc and needs to be freed upon
     * destruction. Having this explicitly specified allows users call
     * @ref mdl_arrayiter_destroy on an iterator, regardless of whether it was statically
     * allocated or not.
     */
    bool was_allocated;
} MDLArrayIterator;

/**
 * Allocate and initialize a new empty array.
 *
 * This is similar to but not the same as @ref mdl_malloc followed by @ref
 * mdl_array_init.
 *
 * @param ds
 * @param elem_size
 * @param elem_destructor
 * @return
 *
 * @see mdl_array_init
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
MDL_ANNOTN__NODISCARD
MDLArray *mdl_array_new(MDLState *ds, mdl_destructor_fptr elem_destructor);

#define mdl_array_basicnew(ds) mdl_array_new((ds), mdl_default_destructor)

/**
 * Initialize an allocated array.
 *
 * Users that wish to statically allocate a queue (e.g. to avoid an additional allocation)
 * will use this to initialize the queue. They will still need to destroy the queue using
 * @ref mdl_array_destroy.
 *
 * @param ds
 * @param array The array to initialize.
 * @param elem_destructor
 *
 * @return 0 on success, an error code otherwise.
 *
 * @see mdl_array_new
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1, 2)
int mdl_array_init(MDLState *ds, MDLArray *array, mdl_destructor_fptr elem_destructor);

/**
 * Destroy a array.
 *
 * @param array
 * @return
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_array_destroy(MDLArray *array);

/**
 * Return the length of the array.
 *
 * @param array The array to examine.
 *
 * @return The number of elements in the array.
 */
MDL_API
MDL_ANNOTN__NONNULL
size_t mdl_array_length(const MDLArray *array);

/**
 * Get the first data in the array.
 *
 * This is marginally more efficient than @ref mdl_array_get.
 *
 * @param array The array to operate on.
 * @param item  A pointer to where the array data's value will be stored.
 *
 * @return 0 on success, @ref MDL_ERROR_EMPTY if the array is empty. @a item will not be
 * modified in this case.
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_array_head(const MDLArray *array, void **item);

/**
 * Get the last element in the array.
 *
 * @param array The array to operate on.
 * @param item A pointer to where the array data's value will be stored.
 *
 * @return 0 on success, @ref MDL_ERROR_EMPTY if the array is empty. @a item will not be
 * modified in this case.
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_array_tail(const MDLArray *array, void **item);

/**
 * Append the given item to the end of the array.
 *
 * @param array
 * @param item
 *
 * @return 0 on success, an error code otherwise.
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
int mdl_array_push(MDLArray *array, void *item);

/**
 * Remove an item from the end of the array.
 *
 * @param array
 * @param item
 * @return
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
int mdl_array_pop(MDLArray *array, void **item);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
int mdl_array_pushfront(MDLArray *array, void *item);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
int mdl_array_popfront(MDLArray *array, void **item);

MDL_API
MDL_ANNOTN__NONNULL
int mdl_array_getat(const MDLArray *array, int index, void **value);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
int mdl_array_setat(MDLArray *array, int index, void *new_value);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
int mdl_array_insertafter(MDLArray *array, int index, void *new_value);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
int mdl_array_removeat(MDLArray *array, int index, void **value);

/**
 * Remove all items in the array.
 *
 * @param array The array to operate on.
 * @return
 */
MDL_API
MDL_ANNOTN__NONNULL
void mdl_array_clear(MDLArray *array);

/**
 * Search the array for the first data matching @a value.
 *
 * @param array
 * @param value
 *
 * @return The absolute index of the first matching data found, or a negative
 * number if no match was found.
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1, 3)
int mdl_array_find(const MDLArray *array, const void *value, mdl_comparator_fptr cmp);

/**
 * Like @ref mdl_array_find except this searches the array back to front.
 *
 * @param array
 * @param value
 * @return
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
int mdl_array_rfind(const MDLArray *array, const void *value, mdl_comparator_fptr cmp);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
int mdl_array_removevalue(MDLArray *array, const void *value, mdl_comparator_fptr cmp);

MDL_API
MDL_ANNOTN__NODISCARD
MDL_ANNOTN__NONNULL
MDLArrayIterator *mdl_array_getiterator(const MDLArray *array, int reverse);

MDL_API
MDL_ANNOTN__NONNULL
void *mdl_arrayiter_get(const MDLArrayIterator *iter);

MDL_API
MDL_ANNOTN__NONNULL
int mdl_arrayiter_next(MDLArrayIterator *iter);

MDL_API
MDL_ANNOTN__NONNULL
int mdl_arrayiter_hasnext(const MDLArrayIterator *iter);

MDL_API
MDL_ANNOTN__NONNULL
void mdl_arrayiter_destroy(MDLArrayIterator *iter);

#endif /* INCLUDE_METALDATA_ARRAY_H_ */
