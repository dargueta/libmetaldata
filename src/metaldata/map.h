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
 * A hash map or dictionary.
 *
 * @file map.h
 */

#ifndef INCLUDE_METALDATA_MAP_H_
#define INCLUDE_METALDATA_MAP_H_

#include "array.h"
#include "internal/misc.h"
#include "metaldata.h"
#include <stdbool.h>
#include <stddef.h>

#ifndef MDL_DEFAULT_INITIAL_BUCKETS
#    define MDL_DEFAULT_INITIAL_BUCKETS 7
#endif

/**
 * A pointer to a key-hashing function.
 *
 * The function takes two arguments, a pointer to the memory to hash, and a `size_t`
 * giving the size of the block of memory, in bytes.
 */
typedef mdl_hash_type (*mdl_hash_fptr)(const void *what, size_t size);

MDL_API
mdl_hash_type mdl_strmap_hashstring(const void *p, size_t size);

typedef struct MDLMapNode_
{
    const void *key;   /**< A pointer to the key for this map entry. */
    const void *value; /**< A pointer to the value of this map entry. */
    /**
     * A pointer to the next entry in this bucket, or NULL if it's the last one.
     */
    struct MDLMapNode_ *next_node;
} MDLMapNode;

typedef struct MDLMapBucket_
{
    /** The number of allocated slots in the bucket.
     *
     * When an item is removed from the map, to avoid reallocation the node is removed
     * from the list and placed at the end. A certain number of nodes are
     */
    size_t n_elements;
    MDLMapNode head;
} MDLMapBucket;

typedef struct MDLMap_
{
    size_t n_items;
    size_t n_buckets;
    MDLMapBucket *buckets;
    mdl_hash_fptr hash_fn;
    mdl_comparator_fptr key_cmp;
} MDLMap;

typedef MDLMap MDLStrMap;
typedef MDLMap MDLScalarMap;

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1, 2)
int mdl_map_init(MDLState *mds, MDLMap *map, mdl_hash_fptr key_hasher,
                 mdl_comparator_fptr key_comparator, mdl_destructor_fptr key_destructor,
                 mdl_destructor_fptr value_destructor);

/**
 * Initialize an allocated map that uses strings as its keys.
 *
 * This is a special case of maps, optimized for better performance when strings are used
 * as the keys.
 *
 * @param mds
 * @param map
 * @param value_destructor
 * @return
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_strmap_init(MDLState *mds, MDLMap *map, mdl_destructor_fptr value_destructor);

/**
 * Initialize an allocated map that uses integer scalars as its keys.
 *
 * This is a special case of maps, optimized for better performance when pointers or
 * other int-like values are used as the keys.
 *
 * @param mds
 * @param map
 * @return
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_scalarmap_init(MDLState *mds, MDLMap *map);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
int mdl_strmap_insert(const MDLStrMap *map, const char *key, const void *value);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
int mdl_scalarmap_insert(const MDLStrMap *map, void *key, void *value);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
int mdl_strmap_remove(const MDLStrMap *map, const char *key, const void *value);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
int mdl_scalarmap_remove(const MDLStrMap *map, void *key, const void *value);

/**
 * Like @ref mdl_strmap_insert but doesn't overwrite the value if the key already exists.
 *
 * @param map
 * @param key
 * @param value
 *
 * @return 0 on success, an error code otherwise.
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
int mdl_strmap_setdefault(const MDLStrMap *map, const char *key, const void *value);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
int mdl_scalarmap_setdefault(const MDLStrMap *map, void *key, void *value);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
bool mdl_strmap_contains(const MDLStrMap *map, const char *key);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
bool mdl_scalarmap_contains(const MDLScalarMap *map, void *key);

#endif
