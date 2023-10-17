#include "metaldata/writer.h"
#include "munit/munit.h"
#include <stdbool.h>

MunitResult test_writer__buffer_init_static(const MunitParameter params[], void *userdata)
{
    (void)params;
    MDLState *ds = (MDLState *)userdata;
    MDLWriter writer;
    char buffer[8];

    mdl_writer_initwithbuffer(ds, &writer, buffer, 0);
    // Because the writer was statically allocated, was_allocated must be false.
    munit_assert_false(writer.was_allocated);
    mdl_writer_close(&writer);
    return MUNIT_OK;
}

MunitResult test_writer__buffer_putc(const MunitParameter params[], void *userdata)
{
    (void)params;
    MDLState *ds = (MDLState *)userdata;
    MDLWriter writer;
    char buffer[] = {0, 0, 0, 0, 0, 0, 0, 0};

    mdl_writer_initwithbuffer(ds, &writer, buffer, 8);
    munit_assert_size(writer.output_size, ==, 8);
    munit_assert_size(writer.buffer_position, ==, 0);
    munit_assert_string_equal(buffer, "");

    int result = mdl_writer_putc(&writer, 'A');
    munit_assert_int(result, ==, MDL_OK);
    munit_assert_size(writer.output_size, ==, 8);
    munit_assert_size(writer.buffer_position, ==, 1);
    munit_assert_string_equal(buffer, "A");

    result = mdl_writer_putc(&writer, 'B');
    munit_assert_int(result, ==, MDL_OK);
    munit_assert_size(writer.output_size, ==, 8);
    munit_assert_size(writer.buffer_position, ==, 2);
    munit_assert_string_equal(buffer, "AB");

    mdl_writer_close(&writer);
    return MUNIT_OK;
}
