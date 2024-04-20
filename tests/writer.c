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

#include "metaldata/writer.h"
#include "metaldata/errors.h"
#include "munit/munit.h"
#include <stdbool.h>

MunitResult test_writer__buffer_init_static(const MunitParameter params[], void *userdata)
{
    (void)params;
    MDLState *mds = (MDLState *)userdata;
    MDLWriter writer;
    char buffer[8];

    mdl_writer_initwithbuffer(mds, &writer, buffer, 0);
    // Because the writer was statically allocated, was_allocated must be false.
    munit_assert_false(writer.was_allocated);
    mdl_writer_close(&writer);
    return MUNIT_OK;
}

MunitResult test_writer__buffer_putc(const MunitParameter params[], void *userdata)
{
    (void)params;
    MDLState *mds = (MDLState *)userdata;
    MDLWriter writer;
    char buffer[] = {0, 0, 0, 0, 0, 0, 0, 0};

    mdl_writer_initwithbuffer(mds, &writer, buffer, 8);
    munit_assert_size(writer.buffer_size, ==, 8);
    munit_assert_size(writer.buffer_position, ==, 0);
    munit_assert_string_equal(buffer, "");

    int result = mdl_writer_putc(&writer, 'A');
    munit_assert_int(result, ==, MDL_OK);
    munit_assert_size(writer.buffer_size, ==, 8);
    munit_assert_size(writer.buffer_position, ==, 1);
    munit_assert_string_equal(buffer, "A");

    result = mdl_writer_putc(&writer, 'B');
    munit_assert_int(result, ==, MDL_OK);
    munit_assert_size(writer.buffer_size, ==, 8);
    munit_assert_size(writer.buffer_position, ==, 2);
    munit_assert_string_equal(buffer, "AB");

    mdl_writer_close(&writer);
    return MUNIT_OK;
}
