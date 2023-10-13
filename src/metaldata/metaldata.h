#ifndef INCLUDE_METALDATA_METALDATA_H_
#define INCLUDE_METALDATA_METALDATA_H_

#include "configuration.h"
#include "errors.h"
#include "internal/annotations.h"
#include "internal/cstdlib.h"
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
 * @param[in] ds
 * @param alloc    The function to use for memory allocation. Hosted implementations can
 *                 use @ref mdl_default_hosted_alloc by including @file hosted_allocator.c
 *                 in their sources.
 * @param userdata User-defined data to store on the state, made available to callbacks.
 *                 MetalData ignores this.
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1, 2)
void mdl_initstate(MDLState *ds, mdl_alloc_fptr alloc, void *userdata);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
MDL_ANNOTN__ACCESS_SIZED(read_only, 2, 4)
MDL_ANNOTN__ACCESS_SIZED(read_only, 3, 4)
int mdl_default_ptr_value_comparator(MDLState *ds, const void *left, const void *right,
                                     size_t size);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
MDL_ANNOTN__ACCESS_SIZED(read_only, 2, 4)
MDL_ANNOTN__ACCESS_SIZED(read_only, 3, 4)
int mdl_default_memory_comparator(MDLState *ds, const void *left, const void *right,
                                  size_t size);

#if !MDL_COMPILED_AS_UNHOSTED
MDL_API
void *mdl_default_hosted_alloc(void *ptr, size_t size, size_t type_or_old_size, void *ud);
#endif /* MDL_COMPILED_AS_UNHOSTED */

typedef void (*mdl_destructor_fptr)(MDLState *ds, void *item) MDL_REENTRANT_MARKER;

MDL_ANNOTN__NONNULL_ARGS(1)
typedef int (*mdl_comparator_fptr)(MDLState *ds, const void *left, const void *right,
                                   size_t size) MDL_REENTRANT_MARKER;

/**
 * Free raw memory allocated by @ref mdl_malloc and its related functions.
 *
 * @param ds       The
 * @param pointer   The pointer to the block of memory to free.
 * @param old_size  The size of the memory block.
 */
MDL_API void mdl_free(MDLState *ds, void *pointer, size_t old_size);

MDL_API
MDL_ANNOTN__NONNULL
MDL_ANNOTN__NODISCARD
void *mdl_malloc(MDLState *ds, size_t size);

MDL_API
void *mdl_realloc(MDLState *ds, void *pointer, size_t new_size, size_t old_size);

#endif
