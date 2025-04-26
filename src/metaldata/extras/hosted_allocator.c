// SPDX-License-Identifier: MPL-2.0+
// Copyright (C) 2020-2025  Diego Argueta
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

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

void *mdl_default_hosted_alloc(void *ptr, size_t size, size_t type_or_old_size,
                               void *udata)
{
    (void)type_or_old_size, (void)udata;
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
