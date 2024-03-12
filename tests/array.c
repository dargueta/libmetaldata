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

#include "metaldata/array.h"
#include "munit/munit.h"

static void helper_test_adding_blocks(MDLArray *array, ptrdiff_t n_to_add);

MunitResult test_array__length_zero(const MunitParameter params[], void *userdata)
{
    (void)params;

    MDLState *ds = (MDLState *)userdata;
    MDLArray array;
    size_t length;
    int error;

    error = mdl_array_init(ds, &array, NULL);
    munit_assert_int(error, ==, MDL_OK);

    length = mdl_array_length(&array);
    munit_assert_size(length, ==, 0);
    mdl_array_destroy(&array);

    return MUNIT_OK;
}

// head() on an empty list should return an error code and leave the pointer argument
// unchanged.
MunitResult test_array__head_empty_fails(const MunitParameter params[], void *userdata)
{
    (void)params;

    MDLState *ds = (MDLState *)userdata;
    MDLArray array;
    int error;

    error = mdl_array_init(ds, &array, NULL);
    munit_assert_int(error, ==, MDL_OK);

    void *value = (void *)12345;

    error = mdl_array_head(&array, &value);
    munit_assert_int(error, ==, MDL_ERROR_OUT_OF_RANGE);
    munit_assert_ptr_equal(value, (void *)12345);

    mdl_array_destroy(&array);
    return MUNIT_OK;
}

// tail() on an empty list should return an error code and leave the pointer argument
// unchanged.
MunitResult test_array__tail_empty_fails(const MunitParameter params[], void *userdata)
{
    (void)params;

    MDLState *ds = (MDLState *)userdata;
    MDLArray array;
    int error;

    error = mdl_array_init(ds, &array, NULL);
    munit_assert_int(error, ==, MDL_OK);

    void *value = (void *)12345;

    error = mdl_array_tail(&array, &value);
    munit_assert_int(error, ==, MDL_ERROR_OUT_OF_RANGE);
    munit_assert_ptr_equal(value, (void *)12345);

    mdl_array_destroy(&array);
    return MUNIT_OK;
}

// Allocating an array on the heap should be fine.
MunitResult test_array__allocate_empty_ok(const MunitParameter params[], void *userdata)
{
    (void)params;

    MDLState *ds = (MDLState *)userdata;
    MDLArray *array = mdl_array_basicnew(ds);
    munit_assert_not_null(array);

    munit_assert_size(mdl_array_length(array), ==, 0);
    munit_assert_true(array->was_allocated);

    // The memory usage checking done at the end of every test will ensure that all memory
    // used by the allocated array gets freed.
    mdl_array_destroy(array);
    return MUNIT_OK;
}

// Add a few elements to the list, but shorter than one block.
MunitResult test_array__add_less_than_one_block(const MunitParameter params[],
                                                void *userdata)
{
    (void)params;

    MDLState *ds = (MDLState *)userdata;
    MDLArray *array = mdl_array_basicnew(ds);
    munit_assert_not_null(array);

    helper_test_adding_blocks(array, MDL_DEFAULT_ARRAY_BLOCK_SIZE - 1);

    // The memory usage checking done at the end of every test will ensure that all memory
    // used by the allocated array gets freed.
    mdl_array_destroy(array);
    return MUNIT_OK;
}

// Add a few elements to the list, but shorter than one block.
MunitResult test_array__add_exactly_one_block(const MunitParameter params[],
                                              void *userdata)
{
    (void)params;

    MDLState *ds = (MDLState *)userdata;
    MDLArray *array = mdl_array_basicnew(ds);
    munit_assert_not_null(array);

    helper_test_adding_blocks(array, MDL_DEFAULT_ARRAY_BLOCK_SIZE);

    mdl_array_destroy(array);
    return MUNIT_OK;
}

MunitResult test_array__add_one_more_than_one_block(const MunitParameter params[],
                                                    void *userdata)
{
    (void)params;

    MDLState *ds = (MDLState *)userdata;
    MDLArray *array = mdl_array_basicnew(ds);
    munit_assert_not_null(array);

    helper_test_adding_blocks(array, MDL_DEFAULT_ARRAY_BLOCK_SIZE + 1);

    mdl_array_destroy(array);
    return MUNIT_OK;
}

MunitResult test_array__add_more_than_one_block(const MunitParameter params[],
                                                void *userdata)
{
    (void)params;

    MDLState *ds = (MDLState *)userdata;
    MDLArray *array = mdl_array_basicnew(ds);
    munit_assert_not_null(array);

    helper_test_adding_blocks(array, (MDL_DEFAULT_ARRAY_BLOCK_SIZE * 2) + 1);

    mdl_array_destroy(array);
    return MUNIT_OK;
}

void helper_test_adding_blocks(MDLArray *array, ptrdiff_t n_to_add)
{
    munit_assert_not_null(array);

    for (ptrdiff_t i = 0; i < n_to_add; i++)
        mdl_array_push(array, (void *)i);

    for (ptrdiff_t i = 0; i < n_to_add; i++)
    {
        void *value;
        int result = mdl_array_getat(array, (int)i, &value);

        munit_assert_int(result, ==, MDL_OK);
        munit_assert_ptr_equal((void *)(ptrdiff_t)i, value);
    }
}
