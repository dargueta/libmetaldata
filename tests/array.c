// SPDX-License-Identifier: MPL-2.0+
// Copyright (C) 2020-2025  Diego Argueta
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "metaldata/array.h"
#include "metaldata/errors.h"
#include "munit/munit.h"

static void helper_test_adding_blocks(MDLArray *array, ptrdiff_t n_to_add);

MunitResult test_array__length_zero(const MunitParameter params[], void *udata)
{
    (void)params;

    MDLState *mds = (MDLState *)udata;
    MDLArray array;
    size_t length;
    int error;

    error = mdl_array_init(mds, &array, NULL);
    munit_assert_int(error, ==, MDL_OK);

    length = mdl_array_length(&array);
    munit_assert_size(length, ==, 0);

    int destroy_result = mdl_array_destroy(&array);
    munit_assert_int(destroy_result, ==, MDL_OK);
    return MUNIT_OK;
}

// head() on an empty list should return an error code and leave the pointer argument
// unchanged.
MunitResult test_array__head_empty_fails(const MunitParameter params[], void *udata)
{
    (void)params;

    MDLState *mds = (MDLState *)udata;
    MDLArray array;
    int error;

    error = mdl_array_init(mds, &array, NULL);
    munit_assert_int(error, ==, MDL_OK);

    void *value = (void *)12345;

    error = mdl_array_head(&array, &value);
    munit_assert_int(error, ==, MDL_ERROR_OUT_OF_RANGE);
    munit_assert_ptr_equal(value, (void *)12345);

    int destroy_result = mdl_array_destroy(&array);
    munit_assert_int(destroy_result, ==, MDL_OK);
    return MUNIT_OK;
}

// tail() on an empty list should return an error code and leave the pointer argument
// unchanged.
MunitResult test_array__tail_empty_fails(const MunitParameter params[], void *udata)
{
    (void)params;

    MDLState *mds = (MDLState *)udata;
    MDLArray array;
    int error;

    error = mdl_array_init(mds, &array, NULL);
    munit_assert_int(error, ==, MDL_OK);

    void *value = (void *)12345;

    error = mdl_array_tail(&array, &value);
    munit_assert_int(error, ==, MDL_ERROR_OUT_OF_RANGE);
    munit_assert_ptr_equal(value, (void *)12345);

    int destroy_result = mdl_array_destroy(&array);
    munit_assert_int(destroy_result, ==, MDL_OK);
    return MUNIT_OK;
}

// Allocating an array on the heap should be fine.
MunitResult test_array__allocate_empty_ok(const MunitParameter params[], void *udata)
{
    (void)params;

    MDLState *mds = (MDLState *)udata;
    MDLArray *array = mdl_array_basicnew(mds);
    munit_assert_not_null(array);

    munit_assert_size(mdl_array_length(array), ==, 0);
    munit_assert_true(array->was_allocated);

    // The memory usage checking done at the end of every test will ensure that all memory
    // used by the allocated array gets freed.
    int destroy_result = mdl_array_destroy(array);
    munit_assert_int(destroy_result, ==, MDL_OK);
    return MUNIT_OK;
}

// Add a few elements to the list, but shorter than one block.
MunitResult test_array__add_less_than_one_block(const MunitParameter params[],
                                                void *udata)
{
    (void)params;

    MDLState *mds = (MDLState *)udata;
    MDLArray *array = mdl_array_basicnew(mds);
    munit_assert_not_null(array);

    helper_test_adding_blocks(array, MDL_DEFAULT_ARRAY_BLOCK_SIZE - 1);

    // The memory usage checking done at the end of every test will ensure that all memory
    // used by the allocated array gets freed.
    int destroy_result = mdl_array_destroy(array);
    munit_assert_int(destroy_result, ==, MDL_OK);
    return MUNIT_OK;
}

// Add a few elements to the list, but shorter than one block.
MunitResult test_array__add_exactly_one_block(const MunitParameter params[], void *udata)
{
    (void)params;

    MDLState *mds = (MDLState *)udata;
    MDLArray *array = mdl_array_basicnew(mds);
    munit_assert_not_null(array);

    helper_test_adding_blocks(array, MDL_DEFAULT_ARRAY_BLOCK_SIZE);

    int destroy_result = mdl_array_destroy(array);
    munit_assert_int(destroy_result, ==, MDL_OK);
    return MUNIT_OK;
}

MunitResult test_array__add_one_more_than_one_block(const MunitParameter params[],
                                                    void *udata)
{
    (void)params;

    MDLState *mds = (MDLState *)udata;
    MDLArray *array = mdl_array_basicnew(mds);
    munit_assert_not_null(array);

    helper_test_adding_blocks(array, MDL_DEFAULT_ARRAY_BLOCK_SIZE + 1);

    int destroy_result = mdl_array_destroy(array);
    munit_assert_int(destroy_result, ==, MDL_OK);
    return MUNIT_OK;
}

MunitResult test_array__add_more_than_one_block(const MunitParameter params[],
                                                void *udata)
{
    (void)params;

    MDLState *mds = (MDLState *)udata;
    MDLArray *array = mdl_array_basicnew(mds);
    munit_assert_not_null(array);

    helper_test_adding_blocks(array, (MDL_DEFAULT_ARRAY_BLOCK_SIZE * 2) + 1);

    int destroy_result = mdl_array_destroy(array);
    munit_assert_int(destroy_result, ==, MDL_OK);
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
