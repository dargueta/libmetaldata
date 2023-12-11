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

#include "metaldata/array.h"
#include "munit/munit.h"

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

// head() on an empty list should return an error code and leave the pointer argument
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
