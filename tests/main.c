#include "metaldata/metaldata.h"
#include "munit/munit.h"
#include <stddef.h>

#include "metaldata/extras/hosted_allocator.c"

#define import_test(suite, name)                                                         \
    extern MunitResult test_##suite##__##name(const MunitParameter params[],             \
                                              void *userdata)
#define define_plain_test_case(suite, name)                                              \
    {                                                                                    \
        "/" #name, test_##suite##__##name, test_setup, test_tear_down,                   \
            MUNIT_TEST_OPTION_NONE, NULL                                                 \
    }

#define define_test_suite(name)                                                          \
    {                                                                                    \
        "/" #name, name##_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE                        \
    }

static void *test_setup(const MunitParameter params[], void *user_data)
{
    (void)params, (void)user_data;
    MDLState *state = munit_malloc(sizeof(*state));

    mdl_initstate(state, mdl_default_hosted_alloc, NULL);
    return state;
}

static void test_tear_down(void *fixture)
{
    free(fixture);
}

import_test(memblklist, length_zero);
import_test(reader, buffer_init_static);
import_test(reader, buffer_init_malloc);
import_test(reader, buffer_getc);
import_test(reader, buffer_getc_initially_empty);

static MunitTest memblklist_tests[] = {
    define_plain_test_case(memblklist, length_zero),
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

static MunitTest reader_tests[] = {
    define_plain_test_case(reader, buffer_init_static),
    define_plain_test_case(reader, buffer_init_malloc),
    define_plain_test_case(reader, buffer_getc),
    define_plain_test_case(reader, buffer_getc_initially_empty),
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

static MunitSuite all_subsuites[] = {
    define_test_suite(memblklist), define_test_suite(reader), {.prefix = NULL}};

static const MunitSuite suite = {"", NULL, all_subsuites, 1, MUNIT_SUITE_OPTION_NONE};

int main(int argc, char **argv)
{
    return munit_suite_main(&suite, NULL, argc, argv);
}
