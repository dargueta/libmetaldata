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

#include "metaldata/memblklist.h"
#include "munit/munit.h"

MunitResult test_memblklist__length_zero(const MunitParameter params[], void *userdata)
{
    (void)params;

    MDLState *ds = (MDLState *)userdata;
    MDLMemBlkList list;

    int error = mdl_memblklist_init(ds, &list, 32);
    munit_assert_int(error, ==, MDL_OK);
    munit_assert_size(list.elem_size, ==, 32);

    size_t length = mdl_memblklist_length(&list);
    munit_assert_size(length, ==, 0);

    error = mdl_memblklist_destroy(&list);
    munit_assert_int(error, ==, MDL_OK);

    return MUNIT_OK;
}
