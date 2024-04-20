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

#include "metaldata/memblklist.h"
#include "metaldata/errors.h"
#include "munit/munit.h"
#include <limits.h>
#include <stdlib.h>

MunitResult test_memblklist__length_zero(const MunitParameter params[], void *userdata)
{
    (void)params;

    MDLState *mds = (MDLState *)userdata;
    MDLMemBlkList list;

    int error = mdl_memblklist_init(mds, &list, 32);
    munit_assert_int(error, ==, MDL_OK);
    munit_assert_size(list.elem_size, ==, 32);

    size_t length = mdl_memblklist_length(&list);
    munit_assert_size(length, ==, 0);

    error = mdl_memblklist_destroy(&list);
    munit_assert_int(error, ==, MDL_OK);
    return MUNIT_OK;
}

MunitResult test_memblklist__add_one(const MunitParameter params[], void *userdata)
{
    (void)params;

    MDLState *mds = (MDLState *)userdata;
    MDLMemBlkList list;
    char test_data[31];

    for (int i = 0; i < 31; i++)
        test_data[i] = (char)i;

    int error = mdl_memblklist_init(mds, &list, 31);
    munit_assert_int(error, ==, MDL_OK);
    munit_assert_size(list.elem_size, ==, 31);

    size_t length = mdl_memblklist_length(&list);
    munit_assert_size(length, ==, 0);

    void *block_pointer = mdl_memblklist_push(&list);
    munit_assert_not_null(block_pointer);

    length = mdl_memblklist_length(&list);
    munit_assert_size(length, ==, 1);

    memcpy(block_pointer, test_data, 31);

    void *retrieved_pointer = mdl_memblklist_getblockat(&list, 0);
    munit_assert_ptr_equal(block_pointer, retrieved_pointer);

    retrieved_pointer = mdl_memblklist_head(&list);
    munit_assert_ptr_equal(block_pointer, retrieved_pointer);

    retrieved_pointer = mdl_memblklist_tail(&list);
    munit_assert_ptr_equal(block_pointer, retrieved_pointer);

    int cmp_result = memcmp(retrieved_pointer, test_data, 31);
    munit_assert_int(0, ==, cmp_result);

    error = mdl_memblklist_destroy(&list);
    munit_assert_int(error, ==, MDL_OK);
    return MUNIT_OK;
}
