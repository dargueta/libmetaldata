#ifndef INCLUDE_METALDATA_METALDATA_H_
#define INCLUDE_METALDATA_METALDATA_H_

#include "annotations.h"
#include "configuration.h"
#include "errors.h"
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

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
void mdl_state_defaultinit(MDLState *ds, void *userdata);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1, 2)
void mdl_initstate(MDLState *ds, mdl_alloc_fptr alloc, void *userdata);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
MDL_ANNOTN__ACCESS_SIZED(read_only, 3, 4)
int mdl_default_ptr_comparator(MDLState *ds, const void *left, const void *right,
                               size_t size);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
int mdl_default_str_comparator(MDLState *ds, const void *left, const void *right);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
void mdl_default_noop_destructor(MDLState *ds, void *item);

MDL_API
#if MDL_COMPILED_AS_UNHOSTED && !defined(MDL_CURRENTLY_COMPILING_TESTS)
MDL_ANNOTN__UNAVAILABLE("This function is unavailable because MetalData was compiled"
                        " for unhosted code. You must recompile it.")
#endif /* MDL_COMPILED_AS_UNHOSTED */
void *mdl_default_alloc(void *ptr, size_t size, size_t type_or_old_size, void *ud);

typedef void (*mdl_copy_fptr)(MDLState *ds, const void *MDL_ANNOTN__RESTRICT source,
                              void *MDL_ANNOTN__RESTRICT dest, size_t dest_size);

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
MDL_ANNOTN__GCC_MALLOC(mdl_free, 2)
MDL_ANNOTN__NONNULL
MDL_ANNOTN__NODISCARD
void *mdl_malloc(MDLState *ds, size_t size);

MDL_API void *mdl_typedmalloc(MDLState *ds, size_t size, size_t type);

MDL_API
void *mdl_realloc(MDLState *ds, void *pointer, size_t new_size, size_t old_size);

#endif
