// SPDX-License-Identifier: MPL-2.0+
// Copyright (C) 2020-2025  Diego Argueta
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "metaldata/array.h"
#include "metaldata/errors.h"
#include "metaldata/internal/annotations.h"
#include "metaldata/metaldata.h"

#include <stdbool.h>
#include <stddef.h>

MDL_ANNOTN__NONNULL
static int resize_block_list(MDLArray *array, size_t new_total);

MDL_ANNOTN__NONNULL
static int get_node_location_by_index(const MDLArray *array, int index,
                                      MDLArrayBlock **block, size_t *offset);

MDLArray *mdl_array_new(MDLState *mds, mdl_destructor_fptr elem_destructor)
{
    MDLArray *array = mdl_malloc(mds, sizeof(*array));
    if (array == NULL)
        return NULL;

    int result = mdl_array_init(mds, array, elem_destructor);
    if (result != MDL_OK)
    {
        mdl_free(mds, array, sizeof(*array));
        return NULL;
    }

    array->was_allocated = true;
    return array;
}

int mdl_array_init(MDLState *mds, MDLArray *array, mdl_destructor_fptr elem_destructor)
{
    array->blocks = (MDLArrayBlock **)mdl_malloc(mds, sizeof(MDLArrayBlock *));
    if (array->blocks == NULL)
        return MDL_ERROR_NOMEM;

    array->blocks[0] = mdl_malloc(mds, sizeof(MDLArrayBlock));
    if (array->blocks[0] == NULL)
    {
        mdl_free(mds, (void *)array->blocks, sizeof(MDLArrayBlock *));
        return MDL_ERROR_NOMEM;
    }

    array->mds = mds;
    array->length = 0;
    array->was_allocated = false;
    array->elem_destructor = elem_destructor;
    array->n_allocated_blocks = 1;
    return MDL_OK;
}

int mdl_array_destroy(MDLArray *array)
{
    int result = mdl_array_clear(array);
    if (result != MDL_OK)
        return result;

    // After the array has been cleared there will be exactly one allocated block left.
    mdl_free(array->mds, array->blocks[0], sizeof(MDLArrayBlock));
    mdl_free(array->mds, (void *)array->blocks, sizeof(MDLArrayBlock *));

    if (array->was_allocated)
        mdl_free(array->mds, array, sizeof(*array));

    return 0;
}

size_t mdl_array_length(const MDLArray *array)
{
    return array->length;
}

int mdl_array_head(const MDLArray *array, void **item)
{
    if (array->length == 0)
        return MDL_ERROR_OUT_OF_RANGE;

    *item = array->blocks[0]->values[0];
    return MDL_OK;
}

int mdl_array_tail(const MDLArray *array, void **item)
{
    if (array->length == 0)
        return MDL_ERROR_OUT_OF_RANGE;

    *item = array->blocks[array->length / MDL_DEFAULT_ARRAY_BLOCK_SIZE]
                ->values[array->length % MDL_DEFAULT_ARRAY_BLOCK_SIZE];
    return MDL_OK;
}

int mdl_array_push(MDLArray *array, void *item)
{
    int error = mdl_array_ensurecapacity(array, array->length + 1);
    if (error != MDL_OK)
        return error;

    array->blocks[array->length / MDL_DEFAULT_ARRAY_BLOCK_SIZE]
        ->values[array->length % MDL_DEFAULT_ARRAY_BLOCK_SIZE] = item;
    array->length++;
    return MDL_OK;
}

int mdl_array_bulkpush(MDLArray *array, void *const *items, size_t count)
{
    int error = mdl_array_ensurecapacity(array, array->length + count);
    if (error != MDL_OK)
        return error;

    for (size_t i = 0; i < count; i++, array->length++)
    {
        array->blocks[array->length / MDL_DEFAULT_ARRAY_BLOCK_SIZE]
            ->values[array->length % MDL_DEFAULT_ARRAY_BLOCK_SIZE] = items[i];
    }
    return MDL_OK;
}

int mdl_array_pop(MDLArray *array, void **item)
{
    if (array->length == 0)
        return MDL_ERROR_EMPTY;

    MDLArrayBlock *block;
    size_t element_index;

    int error = get_node_location_by_index(array, -1, &block, &element_index);
    if (error != MDL_OK)
        return error;

    if (item != NULL)
        *item = block->values[element_index];

    array->length--;

    // If the index of the element we just popped is 0, that means the last block in the
    // array is now empty. We always want to ensure that there's at least one block in the
    // array, so if the list is empty we won't deallocate the last block either.
    if ((element_index != 0) || (array->length == 0))
        return MDL_OK;

    return resize_block_list(array, array->n_allocated_blocks - 1);
}

// int mdl_array_bulkpop(MDLArray *array, size_t count, void **items);

// int mdl_array_pushfront(MDLArray *array, void *item);

// int mdl_array_popfront(MDLArray *array, void **item);

int mdl_array_getat(const MDLArray *array, int index, void **value)
{
    MDLArrayBlock *block;
    size_t block_offset;

    int result = get_node_location_by_index(array, index, &block, &block_offset);
    if (result != MDL_OK)
        return result;

    *value = block->values[block_offset];
    return MDL_OK;
}

int mdl_array_setat(MDLArray *array, int index, void *new_value)
{
    MDLArrayBlock *block;
    size_t block_offset;

    int result = get_node_location_by_index(array, index, &block, &block_offset);
    if (result != MDL_OK)
        return result;

    block->values[block_offset] = new_value;
    return MDL_OK;
}

// int mdl_array_insertafter(MDLArray *array, int index, void *new_value);

// int mdl_array_removeat(MDLArray *array, int index, void **value);

int mdl_array_clear(MDLArray *array)
{
    size_t num_blocks = array->n_allocated_blocks;

    if (array->elem_destructor != NULL)
    {
        size_t elements_remaining = array->length;

        for (size_t block_i = 0; block_i < num_blocks; block_i++)
        {
            size_t loop_limit = (MDL_DEFAULT_ARRAY_BLOCK_SIZE < elements_remaining)
                                    ? MDL_DEFAULT_ARRAY_BLOCK_SIZE
                                    : elements_remaining;

            for (size_t elem_i = 0; elem_i < loop_limit; elem_i++)
            {
                array->elem_destructor(array->mds,
                                       array->blocks[block_i]->values[elem_i]);
                elements_remaining--;
            }
        }
    }

    array->length = 0;

    // Always keep at least one block allocated so that we don't have to do null checks
    // everywhere.
    return resize_block_list(array, 1);
}

// int mdl_array_find(const MDLArray *array, const void *value, mdl_comparator_fptr cmp);

// int mdl_array_rfind(const MDLArray *array, const void *value, mdl_comparator_fptr cmp);

// int mdl_array_removevalue(MDLArray *array, const void *value, mdl_comparator_fptr cmp);

int mdl_array_ensurecapacity(MDLArray *array, size_t capacity)
{
    size_t min_required_blocks = capacity / MDL_DEFAULT_ARRAY_BLOCK_SIZE;

    if (min_required_blocks % MDL_DEFAULT_ARRAY_BLOCK_SIZE > 0)
        min_required_blocks++;

    size_t n_current_blocks = array->n_allocated_blocks;
    if (n_current_blocks >= min_required_blocks)
        return MDL_OK;

    return resize_block_list(array, min_required_blocks);
}

MDLArrayIterator *mdl_array_getiterator(const MDLArray *array, bool reverse)
{
    MDLArrayIterator *iter = mdl_malloc(array->mds, sizeof(*iter));
    if (iter == NULL)
        return NULL;

    mdl_arrayiter_init(array, iter, reverse);
    iter->was_allocated = true;
    return iter;
}

void mdl_arrayiter_init(const MDLArray *array, MDLArrayIterator *iter, bool reverse)
{
    iter->array = array;
    iter->reverse = reverse;
    iter->was_allocated = false;

    if (!reverse)
    {
        iter->absolute_index = 0;
        iter->block_element_index = 0;
        iter->block_index = 0;
    }
    else
    {
        iter->absolute_index = array->length - 1;
        iter->block_element_index = array->length / MDL_DEFAULT_ARRAY_BLOCK_SIZE;
        iter->block_index = array->length % MDL_DEFAULT_ARRAY_BLOCK_SIZE;
    }
}

void *mdl_arrayiter_get(const MDLArrayIterator *iter)
{
    // Because there is always at least one block allocated, and we don't advance the
    // iterator past the end of the array, we don't need to do bounds checking here. Doing
    // so would only protect us against the case where this is called on an empty list, so
    // it isn't useful for most cases. The documentation explicitly states that calling
    // this on an empty list returns an undefined value, so we can get away with it.
    return iter->array->blocks[iter->block_index]->values[iter->block_element_index];
}

int mdl_arrayiter_next(MDLArrayIterator *iter)
{
    if ((!iter->reverse && (iter->absolute_index + 1 >= iter->array->length)) ||
        (iter->reverse && (iter->absolute_index == 0)))
        return MDL_EOF;

    if (!iter->reverse)
        iter->absolute_index++;
    else
        iter->absolute_index--;
    iter->block_index = iter->absolute_index / MDL_DEFAULT_ARRAY_BLOCK_SIZE;
    iter->block_element_index = iter->absolute_index % MDL_DEFAULT_ARRAY_BLOCK_SIZE;
    return MDL_OK;
}

bool mdl_arrayiter_hasnext(const MDLArrayIterator *iter)
{
    if (iter->array->length > 0)
        return iter->absolute_index < (iter->array->length - 1);
    return false;
}

void mdl_arrayiter_destroy(MDLArrayIterator *iter)
{
    if (iter->was_allocated)
        mdl_free(iter->array->mds, iter, sizeof(*iter));
}

/******** Helper functions ********/

static int resize_block_list(MDLArray *array, size_t new_total)
{
    size_t n_current_blocks = array->n_allocated_blocks;

    // Always keep at least one block allocated.
    if (new_total == 0)
        return MDL_ERROR_INVALID_ARGUMENT;

    if (new_total == n_current_blocks)
        return MDL_OK;

    MDLArrayBlock **new_block_list;
    if (new_total > n_current_blocks)
    {
        // Growing the array.
        new_block_list = mdl_realloc(array->mds, (void *)array->blocks,
                                     new_total * sizeof(MDLArrayBlock *),
                                     n_current_blocks * sizeof(MDLArrayBlock *));

        if (new_block_list == NULL)
            return MDL_ERROR_NOMEM;

        for (size_t i = n_current_blocks; i < new_total; i++)
        {
            MDLArrayBlock *new_block = mdl_malloc(array->mds, sizeof(MDLArrayBlock));
            if (new_block == NULL)
            {
                // Failed to allocate a new block. Free any new blocks we'd previously
                // allocated inside this loop, as well as the resized block list. We
                // decrement i in the initial condition because `new_block_list[i]` hasn't
                // been set yet.
                for (--i; i >= n_current_blocks; i--)
                    mdl_free(array->mds, new_block_list[i], sizeof(MDLArrayBlock));

                mdl_free(array->mds, new_block_list, new_total * sizeof(MDLArrayBlock *));
                return MDL_ERROR_NOMEM;
            }

            new_block_list[i] = new_block;
        }
    }
    else
    {
        // Shrinking the array. Free blocks from the end.
        for (size_t i = n_current_blocks - 1; i >= new_total; i--)
            mdl_free(array->mds, array->blocks[i], sizeof(MDLArrayBlock));

        // Shrink the top-level block array. This shouldn't fail.
        new_block_list =
            mdl_realloc(array->mds, array->blocks, new_total * sizeof(MDLArrayBlock *),
                        n_current_blocks * sizeof(MDLArrayBlock *));

        if (new_block_list == NULL)
            return MDL_ERROR_NOMEM;
    }

    array->blocks = new_block_list;
    array->n_allocated_blocks = new_total;
    return MDL_OK;
}

static int get_node_location_by_index(const MDLArray *array, int index,
                                      MDLArrayBlock **block, size_t *offset)
{
    size_t absolute_index;

    if (index >= 0)
        absolute_index = (size_t)index;
    else
        absolute_index = array->length - ((size_t)-index);

    if (absolute_index >= array->length)
        return MDL_ERROR_OUT_OF_RANGE;

    size_t block_number = absolute_index / MDL_DEFAULT_ARRAY_BLOCK_SIZE;

    *block = array->blocks[block_number];
    *offset = absolute_index % MDL_DEFAULT_ARRAY_BLOCK_SIZE;
    return 0;
}
