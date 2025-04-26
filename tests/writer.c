// SPDX-License-Identifier: MPL-2.0+
// Copyright (C) 2020-2025  Diego Argueta
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "metaldata/writer.h"
#include "metaldata/errors.h"
#include "munit/munit.h"

MunitResult test_writer__buffer_init_static(const MunitParameter params[], void *udata)
{
    (void)params;
    MDLState *mds = (MDLState *)udata;
    MDLWriter writer;
    char buffer[8];

    mdl_writer_initwithbuffer(mds, &writer, buffer, 0);
    // Because the writer was statically allocated, was_allocated must be false.
    munit_assert_false(writer.was_allocated);
    mdl_writer_close(&writer);
    return MUNIT_OK;
}

MunitResult test_writer__buffer_putc(const MunitParameter params[], void *udata)
{
    (void)params;
    MDLState *mds = (MDLState *)udata;
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
