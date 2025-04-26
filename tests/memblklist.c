// SPDX-License-Identifier: MPL-2.0+
// Copyright (C) 2020-2025  Diego Argueta
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "metaldata/memblklist.h"
#include "metaldata/errors.h"
#include "metaldata/internal/annotations.h"
#include "munit/munit.h"
#include <limits.h>

MDL_ANNOTN__NONNULL
static void create_and_test_list_using_push(MDLMemBlkList *list, MDLState *mds,
                                            size_t n_elements, size_t element_size);

/**
 * Fill a buffer with random bytes.
 *
 * @param target A pointer to the memory to randomize.
 * @param size The size of the memory block, in bytes.
 */
MDL_ANNOTN__NONNULL
MDL_ANNOTN__ACCESS_SIZED(read_write, 1, 2)
static void randomize_buffer(void *target, size_t size);

MunitResult test_memblklist__length_zero(const MunitParameter params[], void *udata)
{
    (void)params;

    MDLState *mds = (MDLState *)udata;
    MDLMemBlkList list;

    mdl_memblklist_init(mds, &list, 32);
    munit_assert_size(list.elem_size, ==, 32);

    size_t length = mdl_memblklist_length(&list);
    munit_assert_size(length, ==, 0);
    munit_assert_int(mdl_memblklist_destroy(&list), ==, MDL_OK);
    return MUNIT_OK;
}

MunitResult test_memblklist__add_one(const MunitParameter params[], void *udata)
{
    (void)params;

    MDLState *mds = (MDLState *)udata;
    MDLMemBlkList list;

    create_and_test_list_using_push(&list, mds, 1, 31);
    munit_assert_int(mdl_memblklist_destroy(&list), ==, MDL_OK);
    return MUNIT_OK;
}

MunitResult test_memblklist__add_many_odd(const MunitParameter params[], void *udata)
{
    (void)params;

    MDLState *mds = (MDLState *)udata;
    MDLMemBlkList list;

    create_and_test_list_using_push(&list, mds, 83, 20);
    munit_assert_int(mdl_memblklist_destroy(&list), ==, MDL_OK);
    return MUNIT_OK;
}

MunitResult test_memblklist__add_many_even(const MunitParameter params[], void *udata)
{
    (void)params;

    MDLState *mds = (MDLState *)udata;
    MDLMemBlkList list;

    create_and_test_list_using_push(&list, mds, 40, 16);
    munit_assert_int(mdl_memblklist_destroy(&list), ==, MDL_OK);
    return MUNIT_OK;
}

MunitResult test_memblklist__relindex__empty(const MunitParameter params[], void *udata)
{
    (void)params;

    MDLState *mds = (MDLState *)udata;
    MDLMemBlkList list;

    mdl_memblklist_init(mds, &list, 64);
    munit_assert_false(mdl_memblklist_isrelindexvalid(&list, 0));
    munit_assert_size(mdl_memblklist_absindex(&list, 0), ==, MDL_INVALID_INDEX);

    int error = mdl_memblklist_destroy(&list);
    munit_assert_int(error, ==, MDL_OK);
    return MUNIT_OK;
}

MunitResult test_memblklist__relindex__basic(const MunitParameter params[], void *udata)
{
    (void)params;

    MDLState *mds = (MDLState *)udata;
    MDLMemBlkList list;

    mdl_memblklist_init(mds, &list, 32);
    for (long i = 0; i < 44; i++)
    {
        (void)mdl_memblklist_push(&list);

        munit_assert_size(mdl_memblklist_length(&list), ==, (size_t)i + 1);
        munit_assert_false(mdl_memblklist_isrelindexvalid(&list, i + 1));
        munit_assert_false(mdl_memblklist_isrelindexvalid(&list, -i - 2));
        munit_assert_size(mdl_memblklist_absindex(&list, i + 1), ==, MDL_INVALID_INDEX);
        munit_assert_size(mdl_memblklist_absindex(&list, -i - 2), ==, MDL_INVALID_INDEX);

        for (long j = 0; j <= i; j++)
        {
            munit_assert_true(mdl_memblklist_isrelindexvalid(&list, j));
            munit_assert_true(mdl_memblklist_isrelindexvalid(&list, -j - 1));
            munit_assert_size((size_t)j, ==, mdl_memblklist_absindex(&list, j));
            munit_assert_size((size_t)(i - j), ==,
                              mdl_memblklist_absindex(&list, -j - 1));
        }
    }

    int error = mdl_memblklist_destroy(&list);
    munit_assert_int(error, ==, MDL_OK);
    return MUNIT_OK;
}

MunitResult test_memblklist__pop__empty(const MunitParameter params[], void *udata)
{
    (void)params;

    MDLState *mds = (MDLState *)udata;
    MDLMemBlkList list;

    mdl_memblklist_init(mds, &list, 32);
    int error = mdl_memblklist_pop(&list);
    munit_assert_int(error, ==, MDL_ERROR_EMPTY);

    mdl_memblklist_destroy(&list);
    return MUNIT_OK;
}

MunitResult test_memblklist__popcopy__empty(const MunitParameter params[], void *udata)
{
    (void)params;

    MDLState *mds = (MDLState *)udata;
    MDLMemBlkList list;
    char expected_contents[23];
    char block_buf[sizeof(expected_contents)];

    mdl_memblklist_init(mds, &list, sizeof(expected_contents));

    // Fill the buffer with random bytes. This will let us detect if popcopy stomped on
    // the buffer.
    randomize_buffer(expected_contents, sizeof(expected_contents));
    memcpy(block_buf, expected_contents, sizeof(expected_contents));

    int error = mdl_memblklist_popcopy(&list, block_buf);
    munit_assert_int(error, ==, MDL_ERROR_EMPTY);
    munit_assert_memory_equal(sizeof(expected_contents), block_buf, expected_contents);

    mdl_memblklist_destroy(&list);
    return MUNIT_OK;
}

MunitResult test_memblklist__popfront__empty(const MunitParameter params[], void *udata)
{
    (void)params;

    MDLState *mds = (MDLState *)udata;
    MDLMemBlkList list;

    mdl_memblklist_init(mds, &list, 32);
    int error = mdl_memblklist_popfront(&list);
    munit_assert_int(error, ==, MDL_ERROR_EMPTY);

    mdl_memblklist_destroy(&list);
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

    mdl_memblklist_init(mds, list, element_size);
    munit_assert_size(list->elem_size, ==, element_size);

    size_t length = mdl_memblklist_length(list);
    munit_assert_size(length, ==, 0);

    for (size_t i = 0; i < n_elements; i++)
    {
        void *this_block = mdl_memblklist_push(list);
        munit_assert_ptr_not_null(this_block);
        munit_assert_size(mdl_memblklist_length(list), ==, i + 1);

        allocated_pointers[i] = this_block;
        memcpy(this_block, test_data[i], element_size);
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

static void randomize_buffer(void *target, size_t size)
{
    for (size_t i = 0; i < size; i++)
        ((char *)target)[i] = rand() % CHAR_MAX;
}
