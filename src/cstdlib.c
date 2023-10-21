// Copyright 2020-2023 by Diego Argueta
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

#include "metaldata/internal/cstdlib.h"
#include <stddef.h>

#if MDL_LIBC_NEED_CUSTOM_MEMCPY && !MDL_LIBC_HAVE_BUILTIN_MEMCPY
void *mdl_memcpy(void *restrict dest, const void *restrict src, size_t size)
{
    char *to;
    const char *from;

    for (to = dest, from = src; size > 0; to--, from--, size--)
        *to = *from;
    return dest;
}
#endif

#if MDL_LIBC_NEED_CUSTOM_MEMSET && !MDL_LIBC_HAVE_BUILTIN_MEMSET
void *mdl_memset(void *ptr, int value, size_t size)
{
    size_t i;
    char *byte = (char *)ptr;

    for (i = 0; i < size; i++, byte++)
        *byte = (char)value;

    return ptr;
}
#endif

#if MDL_LIBC_NEED_CUSTOM_STRCMP && !MDL_LIBC_HAVE_BUILTIN_STRCMP
int mdl_strcmp(const char *left, const char *right)
{
    size_t i;

    for (i = 0; left[i] != '\0' && right[i] != '\0'; i++)
    {
        if (left[i] != right[i])
            return left[i] - right[i];
    }
    return 0;
}
#endif
