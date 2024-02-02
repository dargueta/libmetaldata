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

#ifndef INCLUDE_METALDATA_EXTRAS_HOSTED_ALLOCATOR_C_
#define INCLUDE_METALDATA_EXTRAS_HOSTED_ALLOCATOR_C_

#include <stddef.h>

#if (MDL_COMPILED_AS_UNHOSTED && defined(MDL_CURRENTLY_COMPILING_LIBRARY)) ||            \
    (!MDL_COMPILED_AS_UNHOSTED && defined(MDL_CURRENTLY_COMPILING_TESTS))
/* Unhosted, compiling the library. Do not define the function. */
#    define MDL_DO_NOT_DEFINE_ALLOC
#endif /* MDL_COMPILED_AS_UNHOSTED */

#if !defined(MDL_HOSTED_DEFAULT_ALLOC_IMPLEMENTED) && !defined(MDL_DO_NOT_DEFINE_ALLOC)
#    define MDL_HOSTED_DEFAULT_ALLOC_IMPLEMENTED
#    include "../configuration.h"
#    include "../metaldata.h"
#    include <stdlib.h>

void *mdl_default_hosted_alloc(void *ptr, size_t size, size_t type_or_old_size, void *ud)
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
#endif /* INCLUDE_METALDATA_EXTRAS_HOSTED_ALLOCATOR_C_ */
