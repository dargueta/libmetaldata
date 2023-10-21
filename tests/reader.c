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

#include "metaldata/reader.h"
#include "munit/munit.h"
#include <string.h>

MunitResult test_reader__buffer_init_static(const MunitParameter params[], void *userdata)
{
    (void)params;
    MDLState *ds = (MDLState *)userdata;
    MDLReader reader;

    mdl_reader_initfrombuffer(ds, &reader, "", 0);
    // Because the reader was statically allocated, was_allocated must be false.
    munit_assert_false(reader.was_allocated);
    mdl_reader_close(&reader);
    return MUNIT_OK;
}

MunitResult test_reader__buffer_init_malloc(const MunitParameter params[], void *userdata)
{
    (void)params;
    MDLState *ds = (MDLState *)userdata;
    MDLReader *reader;

    reader = mdl_reader_newfrombuffer(ds, "", 0);
    munit_assert_not_null(reader);
    munit_assert_true(reader->was_allocated);
    mdl_reader_close(reader);
    return MUNIT_OK;
}

MunitResult test_reader__buffer_getc_initially_empty(const MunitParameter params[],
                                                     void *userdata)
{
    (void)params;
    MDLState *ds = (MDLState *)userdata;
    MDLReader reader;

    mdl_reader_initfrombuffer(ds, &reader, "", 0);

    for (size_t i = 0; i < 3; i++)
    {
        munit_assert_int(mdl_reader_getc(&reader), ==, MDL_EOF);
        munit_assert_size(0, ==, reader.buffer_position);
    }

    mdl_reader_close(&reader);
    return MUNIT_OK;
}

MunitResult test_reader__buffer_getc(const MunitParameter params[], void *userdata)
{
    (void)params;
    MDLState *ds = (MDLState *)userdata;
    MDLReader reader;
    const char *data = "qwertyuiop";
    size_t data_length = strlen(data);

    mdl_reader_initfrombuffer(ds, &reader, data, data_length);

    for (size_t i = 0; i < data_length; i++)
    {
        int read_value = mdl_reader_getc(&reader);
        munit_assert_char(data[i], ==, read_value);
    }

    // Read the entire buffer, so we should get -1 from now on no matter how many times we
    // call getc.
    for (size_t i = 0; i < 3; i++)
    {
        munit_assert_int(mdl_reader_getc(&reader), ==, MDL_EOF);
        munit_assert_size(data_length, ==, reader.buffer_position);
    }

    mdl_reader_close(&reader);
    return MUNIT_OK;
}

MunitResult test_reader__buffer_unget_at_eof(const MunitParameter params[],
                                             void *userdata)
{
    (void)params;
    MDLState *ds = (MDLState *)userdata;
    MDLReader reader;
    const char *data = "qwertyuiop";
    size_t data_length = strlen(data);

    mdl_reader_initfrombuffer(ds, &reader, data, data_length);

    for (size_t i = 0; i < data_length; i++)
    {
        int read_value = mdl_reader_getc(&reader);
        munit_assert_char(data[i], ==, read_value);
    }

    // Read the entire buffer, so we should get -1 from now on no matter how many times we
    // call getc.
    munit_assert_int(mdl_reader_getc(&reader), ==, MDL_EOF);
    munit_assert_size(data_length, ==, reader.buffer_position);

    // ungetc() should put something back in
    mdl_reader_ungetc(&reader, 123);
    munit_assert_int(mdl_reader_getc(&reader), ==, 123);
    munit_assert_size(data_length, ==, reader.buffer_position);

    // After we consume the ungetted character, we should get EOF again.
    munit_assert_int(mdl_reader_getc(&reader), ==, MDL_EOF);
    munit_assert_size(data_length, ==, reader.buffer_position);

    mdl_reader_close(&reader);
    return MUNIT_OK;
}

MunitResult test_reader__buffer_unget_at_sof(const MunitParameter params[],
                                             void *userdata)
{
    (void)params;
    MDLState *ds = (MDLState *)userdata;
    MDLReader reader;
    const char *data = "qwertyuiop";
    size_t data_length = strlen(data);

    mdl_reader_initfrombuffer(ds, &reader, data, data_length);

    // Even though we're at the beginning of the stream, getc() should still return the
    // character we inserted.
    mdl_reader_ungetc(&reader, 123);
    munit_assert_int(mdl_reader_getc(&reader), ==, 123);
    munit_assert_size(0, ==, reader.buffer_position);

    // Read the stream
    for (size_t i = 0; i < data_length; i++)
    {
        int read_value = mdl_reader_getc(&reader);
        munit_assert_char(data[i], ==, read_value);
    }

    munit_assert_int(mdl_reader_getc(&reader), ==, MDL_EOF);
    munit_assert_size(data_length, ==, reader.buffer_position);

    mdl_reader_close(&reader);
    return MUNIT_OK;
}

MunitResult test_reader__buffer_unget_empty_buffer(const MunitParameter params[],
                                                   void *userdata)
{
    (void)params;
    MDLState *ds = (MDLState *)userdata;
    MDLReader reader;

    mdl_reader_initfrombuffer(ds, &reader, "", 0);

    // The stream is empty but ungetc() should still make getc() return something.
    mdl_reader_ungetc(&reader, 123);
    munit_assert_int(mdl_reader_getc(&reader), ==, 123);
    munit_assert_size(0, ==, reader.buffer_position);

    munit_assert_int(mdl_reader_getc(&reader), ==, MDL_EOF);
    munit_assert_size(0, ==, reader.buffer_position);

    mdl_reader_close(&reader);
    return MUNIT_OK;
}
