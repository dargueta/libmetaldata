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

#include "metaldata/internal/misc.h"
#include <stddef.h>

static const mdl_hash_type djb2_hash_init = 5381;

// DJB2 algorithm
mdl_hash_type mdl_hash_string(const char *restrict string)
{
    if (string == NULL)
        return 0;

    mdl_hash_type hash = djb2_hash_init;

    for (size_t i = 0; string[i] != '\0'; i++)
        hash = ((hash << 5) + hash) + string[i]; // hash = (hash * 33) + s[i]
    return hash;
}

mdl_hash_type mdl_hash_memory(const void *restrict block, size_t size)
{
    if (block == NULL)
        return 0;

    mdl_hash_type hash = djb2_hash_init;
    for (size_t i = 0; i < size; i++)
        hash = ((hash << 5) + hash) + ((const char *)block)[i];
    return hash;
}
