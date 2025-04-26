// SPDX-License-Identifier: MPL-2.0+
// Copyright (C) 2020-2025  Diego Argueta
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "metaldata/internal/cstdlib.h"
#include <stddef.h>

#if MDL_LIBC_NEED_CUSTOM_MEMCMP && !MDL_LIBC_HAVE_BUILTIN_MEMCMP
int mdl_memcmp(const void *restrict left, const void *restrict right, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        int cmp = (int)(((const char *)left)[i]) - (int)(((const char *)right)[i]);
        if (cmp != 0)
            return cmp;
    }
    return 0;
}
#endif

#if MDL_LIBC_NEED_CUSTOM_MEMCPY && !MDL_LIBC_HAVE_BUILTIN_MEMCPY
void *mdl_memcpy(void *restrict dest, const void *restrict src, size_t size)
{
    for (size_t i = 0; i < size; i++)
        ((char *)dest)[i] = ((const char *)src)[i];
    return dest;
}
#endif

#if MDL_LIBC_NEED_CUSTOM_MEMSET && !MDL_LIBC_HAVE_BUILTIN_MEMSET
void *mdl_memset(void *restrict ptr, int value, size_t size)
{
    for (size_t i = 0; i < size; i++)
        ((char *)ptr)[i] = (char)value;
    return ptr;
}
#endif

#if MDL_LIBC_NEED_CUSTOM_STRCMP && !MDL_LIBC_HAVE_BUILTIN_STRCMP
int mdl_strcmp(const char *restrict left, const char *restrict right)
{
    for (size_t i = 0; left[i] != '\0' && right[i] != '\0'; i++)
    {
        if (left[i] != right[i])
            return left[i] - right[i];
    }
    return 0;
}
#endif
