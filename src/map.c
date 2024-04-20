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

#include "metaldata/map.h"
#include "metaldata/internal/misc.h"
#include "metaldata/metaldata.h"

mdl_hash_type mdl_strmap_hashstring(const void *p, size_t size)
{
    (void)size;
    return mdl_hash_string((const char *)p);
}

int mdl_strmap_init(MDLState *mds, MDLMap *map, mdl_destructor_fptr value_destructor)
{
    return mdl_map_init(mds, map, mdl_strmap_hashstring, mdl_default_string_comparator,
                        mdl_no_op_destructor, value_destructor);
}
