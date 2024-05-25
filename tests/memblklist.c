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

static char **generate_list(MDLMemBlkList *list, MDLState *mds, size_t n_elements,
                            size_t element_size);
static void free_test_data_array(char **data, size_t n_elements);

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

    char **test_data = generate_list(&list, mds, 1, 31);
    free_test_data_array(test_data, 1);

    int error = mdl_memblklist_destroy(&list);
    munit_assert_int(error, ==, MDL_OK);
    return MUNIT_OK;
}

MunitResult test_memblklist__add_many_odd(const MunitParameter params[], void *udata)
{
    (void)params;

    MDLState *mds = (MDLState *)udata;
    MDLMemBlkList list;

    char **test_data = generate_list(&list, mds, 83, 20);
    free_test_data_array(test_data, 83);

    int error = mdl_memblklist_destroy(&list);
    munit_assert_int(error, ==, MDL_OK);
    return MUNIT_OK;
}

MunitResult test_memblklist__add_many_even(const MunitParameter params[], void *udata)
{
    (void)params;

    MDLState *mds = (MDLState *)udata;
    MDLMemBlkList list;

    char **test_data = generate_list(&list, mds, 40, 16);
    free_test_data_array(test_data, 40);

    int error = mdl_memblklist_destroy(&list);
    munit_assert_int(error, ==, MDL_OK);
    return MUNIT_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Helpers

static char **generate_list(MDLMemBlkList *list, MDLState *mds, size_t n_elements,
                            size_t element_size)
{
    char **test_data = munit_malloc(n_elements * sizeof(*test_data));
    munit_assert_ptr_not_null(test_data);

    for (size_t i = 0; i < n_elements; i++)
    {
        char *block = munit_malloc(element_size);
        munit_assert_ptr_not_null(block);

        for (size_t j = 0; j < element_size; j++)
            block[j] = (char)((i + j) % CHAR_MAX);

        test_data[i] = block;
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

        memcpy(this_block, test_data[i], element_size);
    }

    for (size_t i = 0; i < n_elements; i++)
    {
        void *this_block = mdl_memblklist_getblockat(list, (int)i);
        munit_assert_ptr_not_null(this_block);
        munit_assert_memory_equal(element_size, this_block, test_data[i]);
    }
    return test_data;
}

static void free_test_data_array(char **data, size_t n_elements)
{
    for (size_t i = 0; i < n_elements; i++)
        free(data[i]);
    free(data);
}
