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
