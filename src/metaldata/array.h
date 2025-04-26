// SPDX-License-Identifier: MPL-2.0+
// Copyright (C) 2020-2025  Diego Argueta
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

/**
 * A resizable array of pointers.
 *
 * - Reads and writes are O(1).
 * - Pushes and pops from the end are O(1).
 * - Pushes and pops from the front are O(n).
 * - Forward and backward iteration is supported.
 *
 * @file array.h
 */
#ifndef INCLUDE_METALDATA_ARRAY_H_
#define INCLUDE_METALDATA_ARRAY_H_

#include "configuration.h"
#include "internal/annotations.h"
#include "metaldata.h"
#include <stdbool.h>
#include <stddef.h>

#define MDL_DEFAULT_ARRAY_BLOCK_SIZE 16

typedef struct MDLArrayBlock_
{
    void *values[MDL_DEFAULT_ARRAY_BLOCK_SIZE];
} MDLArrayBlock;

typedef struct MDLArray_
{
    MDLState *mds;

    /**
     * A function to call when deleting an element.
     */
    mdl_destructor_fptr elem_destructor;

    /**
     * The current length of the array.
     */
    size_t length;
    size_t n_allocated_blocks;

    MDLArrayBlock **blocks;

    /**
     * True if this struct was allocated with @ref mdl_malloc and needs to be freed upon
     * destruction.
     *
     * Having this explicitly specified allows users call @ref mdl_array_destroy on an
     * array, regardless of whether it was statically allocated or not.
     */
    bool was_allocated;
} MDLArray;

typedef struct MDLArrayIterator_
{
    const MDLArray *array;
    size_t block_index;
    size_t block_element_index;
    size_t absolute_index;
    bool reverse;

    /**
     * True if this struct was allocated with @ref mdl_malloc and needs to be freed upon
     * destruction.
     *
     * Having this explicitly specified allows users call @ref mdl_arrayiter_destroy on an
     * iterator, regardless of whether it was statically allocated or not.
     */
    bool was_allocated;
} MDLArrayIterator;

/**
 * Allocate and initialize a new empty array.
 *
 * This is similar to but not quite the same as @ref mdl_malloc followed by @ref
 * mdl_array_init. @ref mdl_array_destroy will deallocate an array created with this
 * function, but will not deallocate an array created an array allocated independently.
 *
 * @param mds The MetalData state.
 * @param elem_destructor
 *      A destructor function to call on elements when deleted. If no destructor is
 *      needed, pass NULL (or use @ref mdl_array_basicnew instead).
 * @return A pointer to the newly-allocated array, or NULL if an error occurred.
 *
 * @see mdl_array_init
 * @see mdl_destructor_fptr
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
MDL_ANNOTN__NODISCARD
MDLArray *mdl_array_new(MDLState *mds, mdl_destructor_fptr elem_destructor);

/**
 * Like @ref mdl_array_new but the destructor is a no-op.
 */
#define mdl_array_basicnew(mds) mdl_array_new((mds), mdl_no_op_destructor)

/**
 * Initialize an allocated array.
 *
 * Users that want to statically allocate an array (e.g. to avoid an additional
 * allocation) will use this to initialize the array. They will still need to destroy the
 * array using @ref mdl_array_destroy.
 *
 * @param mds The MetalData state.
 * @param array The array to initialize.
 * @param elem_destructor
 *      A destructor function to call on elements when deleted. If no destructor is
 *      needed, pass NULL (or use @ref mdl_array_basicnew instead).
 * @return 0 on success, an error code otherwise.
 *
 * @see mdl_array_new
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1, 2)
int mdl_array_init(MDLState *mds, MDLArray *array, mdl_destructor_fptr elem_destructor);

/**
 * Destroy an array.
 *
 * @param array The array to operate on.
 * @return 0 on success, an error code otherwise. (This is highly unlikely.)
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_array_destroy(MDLArray *array);

/**
 * Return the length of the array.
 *
 * @param array The array to examine.
 * @return The number of elements in the array.
 */
MDL_API
MDL_ANNOTN__NONNULL
size_t mdl_array_length(const MDLArray *array);

/**
 * Get the first element in the array.
 *
 * @param array The array to operate on.
 * @param item  A pointer to where the array data's value will be stored.
 * @return 0 on success, @ref MDL_ERROR_OUT_OF_RANGE if the array is empty. @a item will
 * not be modified in this case.
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_array_head(const MDLArray *array, void **item);

/**
 * Get the last element in the array.
 *
 * @param array The array to operate on.
 * @param item A pointer to where the array data's value will be stored.
 * @return 0 on success, @ref MDL_ERROR_OUT_OF_RANGE if the array is empty. @a item will
 * not be modified in this case.
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_array_tail(const MDLArray *array, void **item);

/**
 * Append the given item to the end of the array.
 *
 * @param array The array to operate on.
 * @param item The value to append to the array.
 * @return 0 on success, an error code otherwise.
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
int mdl_array_push(MDLArray *array, void *item);

/**
 * Append multiple items to the end of the array.
 *
 * @param array The array to operate on.
 * @param[in] items An array of values to append to the array.
 * @param count The number of values to append. @a items must have at least this
 *              many elements.
 * @return 0 on success, an error code otherwise.
 */
MDL_API
MDL_ANNOTN__NONNULL
MDL_ANNOTN__ACCESS_SIZED(read_only, 2, 3)
int mdl_array_bulkpush(MDLArray *array, void *const *items, size_t count);

/**
 * Remove an item from the end of the array.
 *
 * @param array The array to operate on.
 * @param[out] item
 *      A pointer to a pointer receiving the value just popped. Callers may pass NULL if
 *      the value doesn't need to be saved.
 * @return 0 on success, an error code otherwise. If the array is empty, this will be
 * @ref MDL_ERROR_EMPTY.
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
int mdl_array_pop(MDLArray *array, void **item);

/**
 * Remove @a count items from the end of the array.
 *
 * If @a count is greater than the number of elements left in the list,
 *
 * @param array The array to operate on.
 * @param count
 *      The number of values to pop off the end.
 * @param[out] items
 *      A pointer to an array of pointers receiving the values just popped. Callers may
 *      pass NULL if the removed values don't need to be saved. If it's non-NULL, @a items
 *      must have space for at least @a count elements.
 * @return 0 on success, an error code otherwise.
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
MDL_ANNOTN__ACCESS_SIZED(write_only, 2, 3)
int mdl_array_bulkpop(MDLArray *array, void **items, size_t count);

/**
 * Insert a value at the beginning of the array.
 *
 * @param array The array to operate on.
 * @param item The value to insert.
 * @return 0 on success, an error code otherwise.
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
int mdl_array_pushfront(MDLArray *array, void *item);

/**
 * Remove a value from the beginning of the array.
 *
 * @param array The array to operate on.
 * @param[out] item
 *      A pointer to a pointer receiving the value just popped. Callers may pass NULL if
 *      the value doesn't need to be saved.
 * @return 0 on success, an error code otherwise. If the array is empty, this will be
 * @ref MDL_ERROR_EMPTY.
 */
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
 * @return 0 on success, an error code otherwise.
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_array_clear(MDLArray *array);

/**
 * Search the array for the first element matching @a value.
 *
 * @param array The array to operate on.
 * @param[in] value The value to search for.
 * @param cmp The comparator function to use to compare two values.
 * @return The absolute index of the first matching element found, or a negative number if
 * no match was found. Negative values have no significance aside from indicating that no
 * match was found.
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1, 3)
int mdl_array_find(const MDLArray *array, const void *value, mdl_comparator_fptr cmp);

/**
 * Like @ref mdl_array_find except this searches the array back to front.
 *
 * @param array The array to operate on.
 * @param[in] value The value to search for.
 * @param cmp The comparator function to use to compare two values.
 * @return The absolute index of the first matching element found, or a negative number if
 * no match was found. Negative values have no significance aside from indicating that no
 * match was found.
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
int mdl_array_rfind(const MDLArray *array, const void *value, mdl_comparator_fptr cmp);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
int mdl_array_removevalue(MDLArray *array, const void *value, mdl_comparator_fptr cmp);

MDL_API
MDL_ANNOTN__NONNULL
int mdl_array_ensurecapacity(MDLArray *array, size_t capacity);

MDL_API
MDL_ANNOTN__NODISCARD
MDL_ANNOTN__NONNULL
MDLArrayIterator *mdl_array_getiterator(const MDLArray *array, bool reverse);

MDL_API
MDL_ANNOTN__NONNULL
void mdl_arrayiter_init(const MDLArray *array, MDLArrayIterator *iter, bool reverse);

/**
 * Get the value of the element the iterator is pointing to.
 *
 * @warning If the array is empty, the return value is @e undefined.
 *
 * @param iter
 * @return The value of the element the iterator is pointing to.
 *
 * @see mdl_arrayiter_hasnext
 */
MDL_API
MDL_ANNOTN__NONNULL
void *mdl_arrayiter_get(const MDLArrayIterator *iter);

/**
 * Advance the iterator to the next element in the input.
 *
 * @param iter The iterator to operate on.
 * @return 0 on success, @ref MDL_EOF if the input has been exhausted.
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_arrayiter_next(MDLArrayIterator *iter);

MDL_API
MDL_ANNOTN__NONNULL
bool mdl_arrayiter_hasnext(const MDLArrayIterator *iter);

MDL_API
MDL_ANNOTN__NONNULL
void mdl_arrayiter_destroy(MDLArrayIterator *iter);

#endif /* INCLUDE_METALDATA_ARRAY_H_ */
