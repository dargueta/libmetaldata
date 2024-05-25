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

static void create_and_test_list_using_push(MDLMemBlkList *list, MDLState *mds,
                                            size_t n_elements, size_t element_size);

MunitResult test_memblklist__length_zero(const MunitParameter params[], void *udata)
{
    (void)params;

    MDLState *mds = (MDLState *)udata;
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

MunitResult test_memblklist__add_one(const MunitParameter params[], void *udata)
{
    (void)params;

    MDLState *mds = (MDLState *)udata;
    MDLMemBlkList list;

    create_and_test_list_using_push(&list, mds, 1, 31);

    int error = mdl_memblklist_destroy(&list);
    munit_assert_int(error, ==, MDL_OK);
    return MUNIT_OK;
}

MunitResult test_memblklist__add_many_odd(const MunitParameter params[], void *udata)
{
    (void)params;

    MDLState *mds = (MDLState *)udata;
    MDLMemBlkList list;

    create_and_test_list_using_push(&list, mds, 83, 20);

    int error = mdl_memblklist_destroy(&list);
    munit_assert_int(error, ==, MDL_OK);
    return MUNIT_OK;
}

MunitResult test_memblklist__add_many_even(const MunitParameter params[], void *udata)
{
    (void)params;

    MDLState *mds = (MDLState *)udata;
    MDLMemBlkList list;

    create_and_test_list_using_push(&list, mds, 40, 16);

    int error = mdl_memblklist_destroy(&list);
    munit_assert_int(error, ==, MDL_OK);
    return MUNIT_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Helpers

static void create_and_test_list_using_push(MDLMemBlkList *list, MDLState *mds,
                                            size_t n_elements, size_t element_size)
{
    char test_data[n_elements][element_size];
    void *allocated_pointers[n_elements];

    for (size_t i = 0; i < n_elements; i++)
    {
        for (size_t j = 0; j < element_size; j++)
            test_data[i][j] = (char)munit_rand_int_range(0, CHAR_MAX);
    }

    int error = mdl_memblklist_init(mds, list, element_size);
    munit_assert_int(error, ==, MDL_OK);
    munit_assert_size(list->elem_size, ==, element_size);

    size_t length = mdl_memblklist_length(list);
    munit_assert_size(length, ==, 0);

    for (size_t i = 0; i < n_elements; i++)
    {
        void *this_block = mdl_memblklist_push(list);
        munit_assert_ptr_not_null(this_block);
        munit_assert_size(mdl_memblklist_length(list), ==, i + 1);

        allocated_pointers[i] = this_block;
        memcpy(this_block, &test_data[i], element_size);
        munit_assert_memory_equal(element_size, this_block, test_data[i]);
    }

    for (size_t i = 0; i < n_elements; i++)
    {
        void *this_block = mdl_memblklist_getblockat(list, i);
        munit_assert_ptr_not_null(this_block);
        munit_assert_ptr(allocated_pointers[i], ==, this_block);
        munit_assert_memory_equal(element_size, this_block, test_data[i]);
    }
}
