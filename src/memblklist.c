// SPDX-License-Identifier: MPL-2.0+
// Copyright (C) 2020-2025  Diego Argueta
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "metaldata/internal/memblklist.h"
#include "metaldata/errors.h"
#include "metaldata/internal/cstdlib.h"
#include "metaldata/memblklist.h"
#include "metaldata/metaldata.h"
#include <stdbool.h>
#include <stddef.h>

MDL_ANNOTN__NONNULL
static void unlink_node(MDLMemBlkListNode *node);

MDL_ANNOTN__NONNULL
static void unlink_and_free_node(MDLMemBlkList *list, MDLMemBlkListNode *node);

MDL_ANNOTN__NONNULL
static void unlink_and_maybe_free_node(MDLMemBlkList *list, MDLMemBlkListNode *node);

MDL_ANNOTN__NONNULL
static MDLMemBlkListNode *get_node_at_abs_index(const MDLMemBlkList *list, size_t index);

MDL_ANNOTN__NONNULL
MDL_ANNOTN__REPRODUCIBLE
static size_t get_node_size(const MDLMemBlkList *list);

void mdl_memblklist_init(MDLState *mds, MDLMemBlkList *list, size_t elem_size)
{
    list->was_allocated = false;
    list->mds = mds;
    list->length = 0;
    list->elem_size = elem_size;
    list->head = NULL;
}

MDLMemBlkList *mdl_memblklist_new(MDLState *mds, size_t elem_size)
{
    MDLMemBlkList *list = mdl_malloc(mds, sizeof(*list));
    if (list == NULL)
        return NULL;

    mdl_memblklist_init(mds, list, elem_size);
    list->was_allocated = true;
    return list;
}

int mdl_memblklist_destroy(MDLMemBlkList *list)
{
    mdl_memblklist_clear(list);
    if (list->was_allocated)
        mdl_free(list->mds, list, sizeof(*list));
    return MDL_OK;
}

size_t mdl_memblklist_length(const MDLMemBlkList *list)
{
    return list->length;
}

bool mdl_memblklist_isrelindexvalid(const MDLMemBlkList *list, long relative_index)
{
    if (relative_index >= 0)
        return (size_t)relative_index < list->length;
    return (size_t)(-relative_index) <= list->length;
}

size_t mdl_memblklist_absindex(const MDLMemBlkList *list, long relative_index)
{
    if (relative_index >= 0)
    {
        if ((size_t)relative_index < list->length)
            return (size_t)relative_index;
        return MDL_INVALID_INDEX;
    }

    if ((size_t)(-relative_index) <= list->length)
        return list->length - (size_t)(-relative_index);
    return MDL_INVALID_INDEX;
}

size_t mdl_memblklist_getelementsize(const MDLMemBlkList *list)
{
    return list->elem_size;
}

void *mdl_memblklist_push(MDLMemBlkList *list)
{
    MDLMemBlkListNode *new_tail = mdl_memblklist_appendnewnode(list);
    if (new_tail == NULL)
        return NULL;

    list->length++;
    return new_tail->data;
}

int mdl_memblklist_pop(MDLMemBlkList *list)
{
    if (list->length == 0)
        return MDL_ERROR_EMPTY;

    unlink_and_free_node(list, list->head->prev);
    list->length--;
    return MDL_OK;
}

int mdl_memblklist_popcopy(MDLMemBlkList *list, void *buf)
{
    if (list->length == 0)
        return MDL_ERROR_EMPTY;

    MDLMemBlkListNode *tail = mdl_memblklist_tail(list);
    mdl_memcpy(buf, tail->data, list->elem_size);
    unlink_and_maybe_free_node(list, tail);
    return MDL_OK;
}

void *mdl_memblklist_head(const MDLMemBlkList *list)
{
    if (list->length == 0)
        return NULL;
    return list->head->data;
}

void *mdl_memblklist_tail(const MDLMemBlkList *list)
{
    if (list->length == 0)
        return NULL;
    return list->head->prev->data;
}

void *mdl_memblklist_pushfront(MDLMemBlkList *list)
{
    /* Because this is a circularly linked list, we can push an item onto the front of the
     * list by appending it to the back and moving the head back to point to it. */
    void *new_buffer = mdl_memblklist_push(list);
    if (new_buffer != NULL)
        list->head = list->head->prev;
    return new_buffer;
}

int mdl_memblklist_popfront(MDLMemBlkList *list)
{
    if (list->length == 0)
        return MDL_ERROR_EMPTY;

    MDLMemBlkListNode *next_head = list->head->next;
    unlink_and_free_node(list, list->head);
    list->head = next_head;
    list->length--;
    return 0;
}

int mdl_memblklist_popfrontcopy(MDLMemBlkList *list, void *buf)
{
    if (list->length == 0)
        return MDL_ERROR_EMPTY;

    mdl_memcpy(buf, list->head->data, list->elem_size);
    return mdl_memblklist_popfront(list);
}

void *mdl_memblklist_getblockat(const MDLMemBlkList *list, size_t index)
{
    const MDLMemBlkListNode *node = get_node_at_abs_index(list, index);
    if (node == NULL)
        return NULL;
    return (void *)node->data;
}

int mdl_memblklist_set(MDLMemBlkList *list, size_t index, const void *src)
{
    MDLMemBlkListNode *node = get_node_at_abs_index(list, index);
    if (node == NULL)
        return MDL_ERROR_OUT_OF_RANGE;

    mdl_memcpy(node->data, src, list->elem_size);
    return 0;
}

int mdl_memblklist_removeat(MDLMemBlkList *list, size_t index)
{
    MDLMemBlkListNode *node = get_node_at_abs_index(list, index);
    if (node == NULL)
        return MDL_ERROR_OUT_OF_RANGE;

    unlink_and_free_node(list, node);
    return 0;
}

int mdl_memblklist_removeatcopy(MDLMemBlkList *list, size_t index, void *buf)
{
    MDLMemBlkListNode *node = get_node_at_abs_index(list, index);
    if (node == NULL)
        return MDL_ERROR_OUT_OF_RANGE;

    mdl_memcpy(buf, node->data, list->elem_size);
    unlink_and_free_node(list, node);
    return 0;
}

void mdl_memblklist_clear(MDLMemBlkList *list)
{
    MDLMemBlkListNode *current_node = list->head;
    size_t node_size = get_node_size(list);

    for (size_t i = 0; i < list->length; i++)
    {
        MDLMemBlkListNode *next_node = current_node->next;
        mdl_free(list->mds, current_node, node_size);
        current_node = next_node;
    }

    list->head = NULL;
    list->length = 0;
}

MDLMemBlkListNode *mdl_memblklist_findnode(const MDLMemBlkList *list, const void *value,
                                           mdl_comparator_fptr cmp)
{
    MDLMemBlkListNode *node = list->head;

    for (size_t index = 0; index < list->length; ++index)
    {
        if (cmp(list->mds, node->data, value, list->elem_size) == 0)
            return node;

        node = node->next;
    }

    return NULL;
}

int mdl_memblklist_find(const MDLMemBlkList *list, const void *value,
                        mdl_comparator_fptr cmp, const void **ptr)
{
    const MDLMemBlkListNode *node = mdl_memblklist_findnode(list, value, cmp);
    if (node == NULL)
        return MDL_ERROR_NOT_FOUND;

    *ptr = node->data;
    return MDL_OK;
}

size_t mdl_memblklist_findindex(const MDLMemBlkList *list, const void *value,
                                mdl_comparator_fptr cmp)
{
    const MDLMemBlkListNode *current_node = list->head;
    for (size_t index = 0; index < list->length; ++index)
    {
        if (cmp(list->mds, current_node->data, value, list->elem_size) == 0)
            return index;

        current_node = current_node->next;
    }

    return MDL_INVALID_INDEX;
}

size_t mdl_memblklist_rfindindex(const MDLMemBlkList *list, const void *value,
                                 mdl_comparator_fptr cmp)
{
    MDLMemBlkListNode *current_node = list->head->prev;
    for (size_t index = list->length; index > 0; index--)
    {
        if (cmp(list->mds, current_node->data, value, list->elem_size) == 0)
            return index - 1;

        current_node = current_node->prev;
    }

    return MDL_INVALID_INDEX;
}

int mdl_memblklist_rotate(MDLMemBlkList *list, long places)
{
    if (list->length == 0)
        return MDL_ERROR_EMPTY;

    if (places > 0)
    {
        /* Rotate to the right; the head moves to higher indexes. */
        for (; places > 0; places--)
            list->head = list->head->next;
    }
    else
    {
        /* Rotate to the left; the head moves to the end, then to lower indexes from
         * there. */
        for (; places < 0; places++)
            list->head = list->head->prev;
    }
    return MDL_OK;
}

int mdl_memblklist_removevalue(MDLMemBlkList *list, const void *value,
                               mdl_comparator_fptr cmp)
{
    MDLMemBlkListNode *node = mdl_memblklist_findnode(list, value, cmp);
    if (node == NULL)
        return 0;

    unlink_and_free_node(list, node);
    return 1;
}

MDLMemBlkListIterator *mdl_memblklist_getiterator(const MDLMemBlkList *list, bool reverse)
{
    MDLMemBlkListIterator *iter = mdl_malloc(list->mds, sizeof(*iter));
    if (iter == NULL)
        return NULL;

    mdl_memblklistiter_init(list, iter, reverse);
    iter->was_allocated = true;
    return iter;
}

void mdl_memblklistiter_init(const MDLMemBlkList *list, MDLMemBlkListIterator *iterator,
                             bool reverse)
{
    iterator->list = list;
    iterator->current = list->head;
    iterator->n_seen = 0;
    iterator->reverse = reverse;
    iterator->was_allocated = false;
}

void *mdl_memblklistiter_get(MDLMemBlkListIterator *iter)
{
    return iter->current->data;
}

int mdl_memblklistiter_next(MDLMemBlkListIterator *iter)
{
    if (!mdl_memblklistiter_hasnext(iter))
        return MDL_EOF;

    if (iter->reverse)
        iter->current = iter->current->prev;
    else
        iter->current = iter->current->next;

    iter->n_seen++;
    return 0;
}

int mdl_memblklistiter_hasnext(const MDLMemBlkListIterator *iter)
{
    return iter->n_seen < iter->list->length;
}

void mdl_memblklistiter_destroy(MDLMemBlkListIterator *iter)
{
    if (iter->was_allocated)
        mdl_free(iter->list->mds, iter, sizeof(*iter));
}

/******** Helper functions ********/

static void unlink_node(MDLMemBlkListNode *node)
{
    /* Don't bother with anything if the node points to itself.*/
    if (node->next == node)
        return;

    node->prev->next = node->next;
    node->next->prev = node->prev;
}

static void unlink_and_free_node(MDLMemBlkList *list, MDLMemBlkListNode *node)
{
    unlink_node(node);
    mdl_free(list->mds, node, get_node_size(list));
}

static void unlink_and_maybe_free_node(MDLMemBlkList *list, MDLMemBlkListNode *node)
{
    unlink_node(node);
    mdl_free(list->mds, node, get_node_size(list));
}

void mdl_memblklist_movenodeafter(MDLMemBlkListNode *new_node,
                                  MDLMemBlkListNode *prev_node)
{
    new_node->prev = prev_node;
    new_node->next = prev_node->next;
    new_node->next->prev = new_node;
    prev_node->next = new_node;
}

/* TODO (dargueta): Iterate backwards if the node is past the halfway point.
 *
 * The algorithm is still O(n) but for iterations forward or backward it can
 * save a lot of time.
 */
static MDLMemBlkListNode *get_node_at_abs_index(const MDLMemBlkList *list, size_t index)
{
    if (index >= list->length)
        return NULL;

    const MDLMemBlkListNode *node = list->head;
    for (size_t i = 0; i < index; i++)
        node = node->next;
    return (MDLMemBlkListNode *)node;
}

MDLMemBlkListNode *mdl_memblklist_appendnewnode(MDLMemBlkList *list)
{
    MDLMemBlkListNode *node = mdl_malloc(list->mds, get_node_size(list));
    if (node == NULL)
        return NULL;

    if (list->head != NULL)
        mdl_memblklist_movenodeafter(node, list->head->prev);
    else
    {
        node->prev = node;
        node->next = node;
        list->head = node;
    }
    return node;
}

size_t get_node_size(const MDLMemBlkList *list)
{
    return sizeof(*list->head) + list->elem_size;
}
