// SPDX-License-Identifier: MPL-2.0+
// Copyright (C) 2020-2025  Diego Argueta
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

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
mdl_hash_type mdl_hash_string(const char *restrict string);

/**
 * Generate a hash value for the given block of memory.
 *
 * If @a p is NULL, this will return 0.
 *
 * @param[in] block  A pointer to the block of memory to hash.
 * @param size   The size in bytes of the memory block @a p points to.
 * @return The integer hash of the memory block.
 */
MDL_API
MDL_ANNOTN__ACCESS_SIZED(read_only, 1, 2)
mdl_hash_type mdl_hash_memory(const void *restrict block, size_t size);

#endif // INCLUDE_METALDATA_INTERNAL_MISC_H_
