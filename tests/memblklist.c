#include "metaldata/memblklist.h"
#include "munit/munit.h"

MunitResult test_memblklist__length_zero(const MunitParameter params[], void *userdata)
{
    (void)params;

    MDLState *ds = (MDLState *)userdata;
    MDLMemBlkList list;
    size_t length;
    int error;

    error = mdl_memblklist_init(ds, &list, 32);
    munit_assert_int(error, ==, MDL_OK);

    length = mdl_memblklist_length(&list);
    munit_assert_size(length, ==, 0);
    mdl_memblklist_destroy(&list);

    return MUNIT_OK;
}
