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
#ifndef INCLUDE_METALDATA_INTERNAL_MISC_H_
#define INCLUDE_METALDATA_INTERNAL_MISC_H_

#include "annotations.h"
#include <stddef.h>

typedef unsigned long mdl_hash_type;

MDL_API
mdl_hash_type mdl_hash_string(const char *restrict s);

MDL_API
MDL_ANNOTN__ACCESS_SIZED(read_only, 1, 2)
mdl_hash_type mdl_hash_memory(const void *restrict p, size_t size);

#endif // INCLUDE_METALDATA_INTERNAL_MISC_H_
