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

#include "metaldata/array.h"
#include "metaldata/memblklist.h"
#include "metaldata/metaldata.h"
#include "metaldata/reader.h"
#include "metaldata/writer.h"
#include "munit/munit.h"
#include <stddef.h>

#define import_test(suite, name)                                                         \
    extern MunitResult test_##suite##__##name(const MunitParameter params[], void *udata)
#define define_plain_test_case(suite, name)                                              \
    {"/" #name,      test_##suite##__##name, test_setup,                                 \
     test_tear_down, MUNIT_TEST_OPTION_NONE, NULL}

#define define_test_suite(name)                                                          \
    {"/" #name, name##_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE}

#define SUITE_END_SENTINEL {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}

typedef struct
{
    struct
    {
        size_t num_allocations;
        size_t num_allocations_of_0;
        size_t num_frees;
        size_t num_reallocations;
        size_t num_frees_of_null;
        long long current_memory_used;
    } memory_info;
} StateTracking;

void *malloc_for_tests(void *ptr, size_t size, size_t type_or_old_size, void *udata)
{
    StateTracking *state = (StateTracking *)udata;
    munit_assert_not_null(state);

    munit_logf(MUNIT_LOG_DEBUG,
               "Calling allocator: ptr=%p size=%zu old_size=%zu udata=%p", ptr, size,
               type_or_old_size, (void *)state);

    // If the size is non-zero then the caller wants to either allocate new memory (`ptr`
    // is NULL) or resize existing memory.
    if (size != 0)
    {
        long long delta = (long long)size - (long long)type_or_old_size;
        state->memory_info.current_memory_used += delta;

        if (state->memory_info.current_memory_used < 0)
        {
            munit_logf(MUNIT_LOG_ERROR,
                       "Allocation with current size %zu and new size %zu (delta=%lld) "
                       "gives memory usage of %lld",
                       type_or_old_size, size, delta,
                       state->memory_info.current_memory_used);
        }

        if (ptr != NULL)
        {
            munit_logf(
                MUNIT_LOG_DEBUG,
                "Reallocating pointer=%p from %zu to %zu (delta %lld; now using %lld)",
                ptr, type_or_old_size, size, delta,
                state->memory_info.current_memory_used);
            state->memory_info.num_reallocations++;
            return realloc(ptr, size);
        }
        else
        {
            munit_logf(MUNIT_LOG_DEBUG,
                       "Allocating new pointer of type %zu and size %zu (delta %lld; now "
                       "using %lld)",
                       type_or_old_size, size, delta,
                       state->memory_info.current_memory_used);
            state->memory_info.num_allocations++;
            return munit_malloc(size);
        }
    }

    // If the size is 0, the caller wants to free memory. It doesn't make sense to pass a
    // null pointer and a size of 0, but we'll follow `free()`'s behavior and do nothing.
    if (ptr == NULL)
    {
        if (type_or_old_size == 0)
        {
            // Old size is 0, this is actually an attempt to allocate but the size was 0.
            munit_log(MUNIT_LOG_WARNING,
                      "Attempting to allocate 0 bytes, returning NULL.");
            state->memory_info.num_allocations++;
            state->memory_info.num_allocations_of_0++;
        }
        else
        {
            // Old size is non-zero, meaning this is an intended free but `ptr` was null.
            // This is probably a bug.
            munit_log(MUNIT_LOG_WARNING, "Attempting to free a null pointer.");
            state->memory_info.num_frees_of_null++;
        }
        return NULL;
    }
    else
    {
        state->memory_info.current_memory_used -= (long long)type_or_old_size;
        munit_logf(MUNIT_LOG_DEBUG, "Freeing pointer=%p of size %zu (now using %lld)",
                   ptr, type_or_old_size, state->memory_info.current_memory_used);
        state->memory_info.num_frees++;
    }

    free(ptr);
    return NULL;
}

static void *test_setup(const MunitParameter params[], void *test_state)
{
    (void)params;

    memset(test_state, 0, sizeof(StateTracking));

    MDLState *mdl = munit_malloc(sizeof(*mdl));
    mdl_initstate(mdl, malloc_for_tests, test_state);
    munit_assert_not_null(mdl->udata);
    return mdl;
}

static void test_tear_down(void *fixture)
{
    MDLState *mdl = (MDLState *)fixture;
    StateTracking *test_state = (StateTracking *)mdl->udata;

    free(fixture);

    munit_logf(MUNIT_LOG_INFO, "num_allocations=%zu",
               test_state->memory_info.num_allocations);
    munit_logf(MUNIT_LOG_INFO, "num_allocations_of_0=%zu",
               test_state->memory_info.num_allocations_of_0);
    munit_logf(MUNIT_LOG_INFO, "num_frees=%zu", test_state->memory_info.num_frees);
    munit_logf(MUNIT_LOG_INFO, "num_reallocations=%zu",
               test_state->memory_info.num_reallocations);
    munit_logf(MUNIT_LOG_INFO, "num_frees_of_null=%zu",
               test_state->memory_info.num_frees_of_null);
    munit_assert_llong(test_state->memory_info.current_memory_used, ==, 0);
}

import_test(array, length_zero);
import_test(array, head_empty_fails);
import_test(array, tail_empty_fails);
import_test(array, allocate_empty_ok);
import_test(array, add_less_than_one_block);
import_test(array, add_exactly_one_block);
import_test(array, add_one_more_than_one_block);
import_test(array, add_more_than_one_block);
import_test(memblklist, length_zero);
import_test(memblklist, add_one);
import_test(memblklist, add_many_odd);
import_test(memblklist, add_many_even);
import_test(memblklist, relindex__empty);
import_test(memblklist, relindex__basic);
import_test(memblklist, pop__empty);
import_test(memblklist, popcopy__empty);
import_test(memblklist, popfront__empty);
import_test(reader, buffer_init_static);
import_test(reader, buffer_init_malloc);
import_test(reader, buffer_getc);
import_test(reader, buffer_getc_initially_empty);
import_test(reader, buffer_unget_at_eof);
import_test(reader, buffer_unget_at_sof);
import_test(reader, buffer_unget_empty_buffer);
import_test(writer, buffer_init_static);
import_test(writer, buffer_putc);

static MunitTest array_tests[] = {
    define_plain_test_case(array, length_zero),
    define_plain_test_case(array, head_empty_fails),
    define_plain_test_case(array, tail_empty_fails),
    define_plain_test_case(array, allocate_empty_ok),
    define_plain_test_case(array, add_less_than_one_block),
    define_plain_test_case(array, add_exactly_one_block),
    define_plain_test_case(array, add_one_more_than_one_block),
    define_plain_test_case(array, add_more_than_one_block),
    SUITE_END_SENTINEL};

static MunitTest memblklist_tests[] = {
    define_plain_test_case(memblklist, length_zero),
    define_plain_test_case(memblklist, add_one),
    define_plain_test_case(memblklist, add_many_odd),
    define_plain_test_case(memblklist, add_many_even),
    define_plain_test_case(memblklist, relindex__empty),
    define_plain_test_case(memblklist, relindex__basic),
    define_plain_test_case(memblklist, pop__empty),
    define_plain_test_case(memblklist, popcopy__empty),
    define_plain_test_case(memblklist, popfront__empty),
    SUITE_END_SENTINEL};

static MunitTest reader_tests[] = {
    define_plain_test_case(reader, buffer_init_static),
    define_plain_test_case(reader, buffer_init_malloc),
    define_plain_test_case(reader, buffer_getc),
    define_plain_test_case(reader, buffer_getc_initially_empty),
    define_plain_test_case(reader, buffer_unget_at_eof),
    define_plain_test_case(reader, buffer_unget_at_sof),
    define_plain_test_case(reader, buffer_unget_empty_buffer),
    SUITE_END_SENTINEL};

static MunitTest writer_tests[] = {define_plain_test_case(writer, buffer_init_static),
                                   define_plain_test_case(writer, buffer_putc),
                                   SUITE_END_SENTINEL};

static MunitSuite all_subsuites[] = {define_test_suite(array),
                                     define_test_suite(memblklist),
                                     define_test_suite(reader),
                                     define_test_suite(writer),
                                     {.prefix = NULL}};

static const MunitSuite suite = {"", NULL, all_subsuites, 1, MUNIT_SUITE_OPTION_NONE};

#define show_sizeof(t) munit_logf(MUNIT_LOG_INFO, "sizeof(" #t "): %zuB", sizeof(t));

int main(int argc, char **argv)
{
    StateTracking state_tracking;
    show_sizeof(MDLState);
    show_sizeof(MDLArray);
    show_sizeof(MDLArrayBlock);
    show_sizeof(MDLArrayIterator);
    show_sizeof(MDLMemBlkList);
    show_sizeof(MDLMemBlkListIterator);
    show_sizeof(MDLReader);
    show_sizeof(MDLWriter);
    return munit_suite_main(&suite, &state_tracking, argc, argv);
}
