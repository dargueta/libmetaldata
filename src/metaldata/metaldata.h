// Copyright 2020-2024 by Diego Argueta
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * The primary include file with core definitions for users of MetalData.
 *
 * @file metaldata.h
 */

#ifndef INCLUDE_METALDATA_METALDATA_H_
#define INCLUDE_METALDATA_METALDATA_H_

#include "configuration.h"
#include "internal/annotations.h"
#include <stddef.h>

/**
 * A pointer to a function handling all memory allocation for MetalData.
 *
 * @param ptr
 *      If reallocating memory, the old pointer. When allocating new memory,
 *      this will be null.
 * @param size
 *      The new size of the allocation. To free memory, set this to 0.
 * @param type_or_old_size
 *      When allocating new memory, this will be the type of the memory block
 *      being allocated (one of the `MDL_T*` values such as @ref MDL_TSTRING,
 *      @ref MDL_TINTEGER, etc.).
 *      When reallocating or freeing memory, this will be the previous size of
 *      the memory block.
 * @param userdata
 *      The custom userdata given when the MetalData state was created.
 */
typedef void *(*mdl_alloc_fptr)(void *ptr, size_t size, size_t type_or_old_size,
                                void *ud)MDL_REENTRANT_MARKER;

typedef struct MDLState_
{
    void *userdata;
    mdl_alloc_fptr allocator;
} MDLState;

/**
 * Initialize a new MetalData state.
 *
 * @param ds       The MetalData state.
 * @param alloc    The function to use for memory allocation. Hosted implementations can
 *                 use @ref mdl_default_hosted_alloc by including @file hosted_allocator.c
 *                 in their sources.
 * @param userdata User-defined data to store on the state, made available to callbacks.
 *                 MetalData ignores this.
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1, 2)
void mdl_initstate(MDLState *ds, mdl_alloc_fptr alloc, void *userdata);

/**
 * Compare the values of two pointers; usable as a @ref mdl_comparator_fptr callback.
 *
 * @param ds Ignored, for compatibility with @ref mdl_comparator_fptr.
 * @param left A pointer value to compare against @a right. The memory is not accessed.
 * @param right A pointer value to compare against @a left. The memory is not accessed.
 * @param size Ignored, for compatibility with @ref mdl_comparator_fptr.
 *
 * @return See @ref mdl_comparator_fptr.
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
MDL_ANNOTN__ACCESS_SIZED(read_only, 2, 4)
MDL_ANNOTN__ACCESS_SIZED(read_only, 3, 4)
int mdl_default_ptr_value_comparator(MDLState *ds, const void *left, const void *right,
                                     size_t size);

/**
 * Compare bytes in two blocks of memory; usable as a @ref mdl_comparator_fptr callback.
 *
 * The function behaves just like `memcmp()` except for two cases:
 *
 * - If @a left and @a right are both null, they compare equal.
 * - If only one pointer is null, the null pointer compares as strictly less than the
 *   non-null pointer. In other words, if only @a left is null, the return value is
 *   negative. If only @a right is null, the return value is positive.
 *
 * @param ds Ignored, for compatibility with @ref mdl_comparator_fptr.
 * @param left A pointer to a block of memory to compare against @a right. May be null. If
 *             not null, the memory block must be at least @a size bytes long.
 * @param right A pointer to a block of memory to compare against @a left. May be null. If
 *             not null, the memory block must be at least @a size bytes long.
 * @param size The maximum number of bytes to compare.
 *
 * @return See @ref mdl_comparator_fptr.
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
MDL_ANNOTN__ACCESS_SIZED(read_only, 2, 4)
MDL_ANNOTN__ACCESS_SIZED(read_only, 3, 4)
int mdl_default_memory_comparator(MDLState *ds, const void *left, const void *right,
                                  size_t size);

/**
 * Compare two C strings, where one or both pointers may be null.
 *
 * If one or both pointers is null, this behaves like integer arithmetic, i.e.:
 *
 * - @a left is NULL, @a right is not: returns a negative value
 * - @a left is NULL, @a right is also NULL: returns 0
 * - @a left is not NULL, @a right is NULL: returns a positive value
 *
 * @param ds Ignored, for compatibility with @ref mdl_comparator_fptr.
 * @param left A pointer to a null-terminated C string to compare. May be null.
 * @param right A pointer to a null-terminated C string to compare. May be null.
 * @param size Ignored, for compatibility with @ref mdl_comparator_fptr.
 * @return See @ref mdl_comparator_fptr.
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
int mdl_default_string_comparator(MDLState *ds, const void *left, const void *right,
                                  size_t size);

typedef void (*mdl_destructor_fptr)(MDLState *ds, void *item) MDL_REENTRANT_MARKER;

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
void mdl_no_op_destructor(MDLState *ds, void *item) MDL_REENTRANT_MARKER;

/**
 * A function comparing @a left against @a right, whatever that means in the context it's
 * used in.
 *
 * MetalData only uses comparator functions for sorting and searching within a single
 * container. For example, if a comparator is passed to @ref mdl_array_sort, calling it
 * with any two elements in the array must yield a valid result.
 *
 * @param ds The MetalData state.
 * @param left A pointer or pointer-like value to compare against @a right.
 * @param right A pointer or pointer-like value to compare against @a left.
 * @param size For containers whose elements have a fixed size, this is that size.
 *
 * The return value must be:
 *
 * - Positive if @a left compares strictly greater than @a right;
 * - 0 if @a left and @a right compare equal;
 * - Negative if @a left compares strictly less than @a right.
 *
 * Aside from 0, MetalData only examines the sign of the return value, and doesn't care
 * about the magnitude.
 *
 * @see mdl_default_memory_comparator
 * @see mdl_default_ptr_value_comparator
 * @see mdl_default_string_comparator
 */
MDL_ANNOTN__NONNULL_ARGS(1)
typedef int (*mdl_comparator_fptr)(MDLState *ds, const void *left, const void *right,
                                   size_t size) MDL_REENTRANT_MARKER;

/**
 * Free raw memory allocated by @ref mdl_malloc and its related functions.
 *
 * @param ds        The MetalData state.
 * @param pointer   The pointer to the block of memory to free.
 * @param old_size  The size of the memory block.
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
void mdl_free(MDLState *ds, void *pointer, size_t old_size);

/**
 * Allocate memory using the allocation function provided to @a ds.
 *
 * @param ds    The MetalData state.
 * @param size  The size of the memory block to allocate, in bytes.
 * @return A pointer to the allocated memory, or NULL if allocation failed.
 */
MDL_API
MDL_ANNOTN__NONNULL
MDL_ANNOTN__NODISCARD
void *mdl_malloc(MDLState *ds, size_t size);

/**
 * Change the size of a block of memory allocated by @ref mdl_malloc.
 *
 * This behaves the same as the C standard library realloc.
 *
 * @param ds The MetalData state.
 * @param pointer A pointer to the memory block to reallocate.
 * @param new_size The desired size of the memory block.
 * @param old_size The current size of the memory block.
 * @return A pointer to the reallocated memory, or NULL if reallocation failed.
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
MDL_ANNOTN__NODISCARD
void *mdl_realloc(MDLState *ds, void *pointer, size_t new_size, size_t old_size);

#endif
