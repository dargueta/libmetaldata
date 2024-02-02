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

/**
 * Miscellaneous functions mostly for internal use only.
 *
 * @file misc.h
 */

#ifndef INCLUDE_METALDATA_INTERNAL_MISC_H_
#define INCLUDE_METALDATA_INTERNAL_MISC_H_

#include "annotations.h"
#include <stddef.h>

typedef unsigned long mdl_hash_type;

/**
 * Generate a hash value for the given string.
 *
 * If @a s is NULL, this will return 0.
 *
 * @param[in] s The string to create a hash for.
 * @return The integer hash of the string.
 */
MDL_API
mdl_hash_type mdl_hash_string(const char *restrict s);

/**
 * Generate a hash value for the given block of memory.
 *
 * If @a p is NULL, this will return 0.
 *
 * @param[in] p  A pointer to the block of memory to hash.
 * @param size   The size in bytes of the memory block @a p points to.
 * @return The integer hash of the memory block.
 */
MDL_API
MDL_ANNOTN__ACCESS_SIZED(read_only, 1, 2)
mdl_hash_type mdl_hash_memory(const void *restrict p, size_t size);

#endif // INCLUDE_METALDATA_INTERNAL_MISC_H_
