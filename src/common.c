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

#include "metaldata/configuration.h"
#include "metaldata/internal/cstdlib.h"
#include "metaldata/metaldata.h"
#include <stddef.h>

void mdl_initstate(MDLState *ds, mdl_alloc_fptr alloc, void *userdata)
{
    ds->allocator = alloc;
    ds->userdata = userdata;
}

int mdl_default_memory_comparator(MDLState *ds, const void *left, const void *right,
                                  size_t size)
{
    (void)ds;
    return mdl_memcmp(left, right, size);
}

int mdl_default_ptr_value_comparator(MDLState *ds, const void *left, const void *right,
                                     size_t size)
{
    (void)ds, (void)size;

    // Because ptrdiff_t may be larger than an int, and demotion of signed types is
    // implementation-defined, we do this instead of casting the result of a pointer
    // subtraction.
    if (left < right)
        return -1;
    if (left > right)
        return 1;
    return 0;
}

int mdl_default_string_comparator(MDLState *ds, const void *left, const void *right,
                                  size_t size)
{
    (void)ds, (void)size;

    if ((left != NULL) && (right != NULL))
        return mdl_strcmp((const char *)left, (const char *)right);

    // Don't use pointer subtraction for this. See comments in
    // mdl_default_ptr_value_comparator for details.
    if (left == NULL)
        return right == NULL ? 0 : -1;
    // Else: right is NULL, left is not null
    return 1;
}

void mdl_default_destructor(MDLState *ds, void *item) MDL_REENTRANT_MARKER
{
    (void)ds, (void)item;
}

#if !MDL_COMPILED_AS_UNHOSTED
#    include "metaldata/extras/hosted_allocator.c"
#endif

void mdl_free(MDLState *ds, void *pointer, size_t old_size)
{
    (void)ds->allocator(pointer, 0, old_size, ds->userdata);
}

void *mdl_malloc(MDLState *ds, size_t size)
{
    return ds->allocator(NULL, size, 0, ds->userdata);
}

void *mdl_realloc(MDLState *ds, void *pointer, size_t new_size, size_t old_size)
{
    return ds->allocator(pointer, new_size, old_size, ds->userdata);
}
