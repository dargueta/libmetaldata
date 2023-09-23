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
