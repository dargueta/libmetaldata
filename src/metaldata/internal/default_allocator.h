#ifndef INCLUDE_METALDATA_DEFAULT_ALLOCATOR_H_
#define INCLUDE_METALDATA_DEFAULT_ALLOCATOR_H_
#include "../annotations.h"
#include "../configuration.h"
#include "../metaldata.h"

#if (MDL_COMPILED_AS_UNHOSTED && defined(MDL_CURRENTLY_COMPILING_LIBRARY)) ||            \
    (!MDL_COMPILED_AS_UNHOSTED && defined(MDL_CURRENTLY_COMPILING_TESTS))
/* Unhosted, compiling the library. Do not define the function. */
#    define MDL_DO_NOT_DEFINE_ALLOC
#endif /* MDL_COMPILED_AS_UNHOSTED */

void *mdl_default_alloc(void *ptr, size_t size, size_t type_or_old_size, void *ud);

#if !defined(MDL_HOSTED_DEFAULT_ALLOC_IMPLEMENTED) && !defined(MDL_DO_NOT_DEFINE_ALLOC)
#    define MDL_HOSTED_DEFAULT_ALLOC_IMPLEMENTED
#    include <stdlib.h>
void *mdl_default_alloc(void *ptr, size_t size, size_t type_or_old_size, void *ud)
{
    (void)type_or_old_size, (void)ud;
    /* If the size is non-zero then the caller wants to either allocate new memory (`ptr`
     * is NULL) or resize existing memory. */
    if (size != 0)
        return realloc(ptr, size);

    /* If the size is 0, the caller wants to free memory. It doesn't make sense to pass a
     * null pointer and a size of 0, but we'll follow `free()`'s behavior and do nothing.
     */
    free(ptr);
    return NULL;
}
#endif /* MDL_HOSTED_DEFAULT_ALLOC_IMPLEMENTED */
#endif /* INCLUDE_METALDATA_DEFAULT_ALLOCATOR_H_ */
