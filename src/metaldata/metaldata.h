// SPDX-License-Identifier: MPL-2.0+
// Copyright (C) 2020-2025  Diego Argueta
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

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
#include <stdint.h>

/**
 * A pointer to a function handling all memory allocation for MetalData.
 *
 * The following is a summary of expected behavior, and guarantees that MetalData makes
 * when calling the allocator. If the function is called directly, outside of MetalData,
 * none of the guarantees listed here apply.
 *
 * - Allocation: @a ptr is null, @a size is guaranteed to be non-zero. @a type_or_old_size
 *   is guaranteed to be one of the `MDL_T*` values such as @ref MDL_TSTRING,
 *   @ref MDL_TINTEGER, etc.
 * - Reallocation: @a ptr is non-null, both @a size and @a type_or_old_size are guaranteed
 *   to be non-zero.
 * - Freeing: @a ptr is non-null, @a size is 0, @a type_or_old_size is non-zero.
 *
 * Example usages:
 *
 * ```c
 * // Allocate 1024 bytes.
 * void *ptr = allocator(NULL, 1024, MDL_TBINARY, mdl->udata);
 *
 * // Increase the size to 2048.
 * ptr = allocator(ptr, 2048, 1024, mdl->udata);
 *
 * // Free the pointer.
 * allocator(ptr, 2048, 0, mdl->udata);
 * ```
 *
 * @param ptr
 *      If reallocating memory, a pointer to the existing memory to reallocate. When
 *      allocating new memory, this will be null.
 * @param size
 *      The new size of the allocation. When freeing memory, this is 0. If 0, @a ptr is
 *      guaranteed to not be null.
 * @param type_or_old_size
 *      When allocating new memory, this will be the type of the memory block being
 *      allocated (one of the `MDL_T*` values such as @ref MDL_TSTRING, @ref MDL_TINTEGER,
 *      etc.).
 *      When reallocating or freeing memory, this will be the previous size of the memory
 *      block. In these cases, it's guaranteed to be non-zero.
 * @param udata
 *      The custom userdata given when the MetalData state was created.
 *
 * @return
 *      NULL if an error occurred or when freeing memory. When a memory allocation
 *      succeeded, this is a pointer to a valid readable and writable memory block of at
 *      least @a size bytes.
 */
typedef void *(*mdl_alloc_fptr)(void *ptr, size_t size, size_t type_or_old_size,
                                void *udata)MDL_REENTRANT_MARKER;

typedef struct MDLState_
{
    void *udata;
    mdl_alloc_fptr allocator;
} MDLState;

/**
 * Initialize a new MetalData state.
 *
 * @param mds   The MetalData state.
 * @param alloc The function to use for memory allocation. Hosted implementations can use
 *              @ref mdl_default_hosted_alloc by including @file hosted_allocator.c in
 *              their sources.
 * @param udata User-defined data to store on the state, made available to callbacks.
 *                 MetalData ignores this.
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1, 2)
void mdl_initstate(MDLState *mds, mdl_alloc_fptr alloc, void *udata);

/**
 * Compare the values of two pointers; usable as a @ref mdl_comparator_fptr callback.
 *
 * @param mds Ignored, for compatibility with @ref mdl_comparator_fptr.
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
int mdl_default_ptr_value_comparator(MDLState *mds, const void *left, const void *right,
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
 * @param mds Ignored, for compatibility with @ref mdl_comparator_fptr.
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
int mdl_default_memory_comparator(MDLState *mds, const void *left, const void *right,
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
 * @param mds Ignored, for compatibility with @ref mdl_comparator_fptr.
 * @param left A pointer to a null-terminated C string to compare. May be null.
 * @param right A pointer to a null-terminated C string to compare. May be null.
 * @param size Ignored, for compatibility with @ref mdl_comparator_fptr.
 * @return See @ref mdl_comparator_fptr.
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
int mdl_default_string_comparator(MDLState *mds, const void *left, const void *right,
                                  size_t size);

typedef void (*mdl_destructor_fptr)(MDLState *mds, void *item) MDL_REENTRANT_MARKER;

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
void mdl_no_op_destructor(MDLState *mds, void *item) MDL_REENTRANT_MARKER;

/**
 * A function comparing @a left against @a right, whatever that means in the context it's
 * used in.
 *
 * MetalData only uses comparator functions for sorting and searching within a single
 * container. For example, if a comparator is passed to @ref mdl_array_sort, calling it
 * with any two elements in the array must yield a valid result.
 *
 * @param mds The MetalData state.
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
typedef int (*mdl_comparator_fptr)(MDLState *mds, const void *left, const void *right,
                                   size_t size) MDL_REENTRANT_MARKER;

/**
 * Free raw memory allocated by @ref mdl_malloc and its related functions.
 *
 * @param mds        The MetalData state.
 * @param pointer   The pointer to the block of memory to free.
 * @param old_size  The size of the memory block.
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
void mdl_free(MDLState *mds, void *pointer, size_t old_size);

/**
 * Allocate memory using the allocation function provided to @a mds.
 *
 * @param mds    The MetalData state.
 * @param size  The size of the memory block to allocate, in bytes.
 * @return A pointer to the allocated memory, or NULL if allocation failed.
 */
MDL_API
MDL_ANNOTN__NONNULL
MDL_ANNOTN__NODISCARD
// MDL_ANNOTN__MALLOC(mdl_free, 2)
void *mdl_malloc(MDLState *mds, size_t size);

/**
 * Change the size of a block of memory allocated by @ref mdl_malloc.
 *
 * This behaves the same as the C standard library realloc.
 *
 * @param mds The MetalData state.
 * @param pointer A pointer to the memory block to reallocate.
 * @param new_size The desired size of the memory block.
 * @param old_size The current size of the memory block.
 * @return A pointer to the reallocated memory, or NULL if reallocation failed.
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
MDL_ANNOTN__NODISCARD
void *mdl_realloc(MDLState *mds, void *pointer, size_t new_size, size_t old_size);

/**
 * The largest unsigned scalar value available.
 *
 * For systems defining `uintptr_t` this is guaranteed to be convertible to and from a
 * pointer. For systems that don't have `uintptr_t`, extreme care must be taken.
 */
typedef uintmax_t mdl_scalar_type;
#define MDL_SCALAR_MAX UINTMAX_MAX

#if defined(UINTPTR_MAX)
// If uintptr_t is defined, then uintmax_t must be at least that large.
#define MDL_SCALAR_CAN_HOLD_POINTER 1
#else
#define MDL_SCALAR_CAN_HOLD_POINTER 0
#endif // UINTPTR_MAX
#endif
