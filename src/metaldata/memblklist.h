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
 * A doubly-linked circular linked list.
 *
 * - Pushes and pops from both ends are O(1).
 * - Accessing the first and last elements are also O(1).
 * - All other accesses are O(n).
 * - Forward and backward iteration is supported.
 *
 * @warning All structures should be treated as opaque; they are defined here only so that
 *          they can be statically allocated when desired.
 *
 * @file memblklist.h
 */
#ifndef INCLUDE_METALDATA_LLIST_H_
#define INCLUDE_METALDATA_LLIST_H_

#include "configuration.h"
#include "internal/annotations.h"
#include "metaldata.h"
#include <stdbool.h>
#include <stddef.h>

struct MDLMemBlkListNode_;
typedef struct MDLMemBlkListNode_ MDLMemBlkListNode;

struct MDLMemBlkList_;
typedef struct MDLMemBlkList_ MDLMemBlkList;

struct MDLMemBlkListIterator_;
typedef struct MDLMemBlkListIterator_ MDLMemBlkListIterator;

/**
 * A single node in the linked list of memory blocks.
 */
struct MDLMemBlkListNode_
{
    /**
     * A pointer to the previous node in this linked list.
     *
     * This is never null; the first node in the list will have this pointing to the last
     * node in the list.
     */
    MDLMemBlkListNode *prev;

    /**
     * A pointer to the next node in this linked list.
     *
     * This is never null; the last node in the list will have this pointing to the first
     * node in the list.
     */
    MDLMemBlkListNode *next;

    /** The user data to store. */
    char data[] MDL_ANNOTN__NONSTRING;
};

/**
 * A list of blocks of memory.
 *
 * @warning The struct is declared in the header only to allow users to allocate it on the
 *          stack. Do not modify it directly.
 */
struct MDLMemBlkList_
{
    /** The MetalData state. */
    MDLState *ds;

    /**
     * A pointer to the first node in the list.
     *
     * This is always allocated, even if the list is empty. It simplifies the code by
     * not making us check for a null pointer all the time.
     */
    MDLMemBlkListNode *head;

    /** The size of a list element's memory block, in bytes. */
    size_t elem_size;

    /**
     * The number of elements in the list.
     *
     * Note, this is not necessarily the number of nodes allocated. It's only the length
     * of the list as far as the user is concerned. As mentioned above, @ref head is
     * always allocated, even if the list is empty.
     */
    size_t length;

    /**
     * True if this struct was allocated with @ref mdl_malloc and needs to be freed upon
     * destruction. Having this explicitly specified allows users call
     * @ref mdl_memblklist_destroy on a list, regardless of whether it was statically
     * allocated or not.
     */
    bool was_allocated;
};

struct MDLMemBlkListIterator_
{
    MDLMemBlkListNode *current;
    MDLMemBlkListNode *end;
    MDLMemBlkList *list;
    bool reverse;

    /**
     * True if this struct was allocated with @ref mdl_malloc and needs to be freed upon
     * destruction. Having this explicitly specified allows users call
     * @ref mdl_memblklistiter_destroy on a list, regardless of whether it was statically
     * allocated or not.
     */
    bool was_allocated;
};

/**
 * Allocate and initialize a new empty list.
 *
 * This is not quite the same as running @ref mdl_memblklist_init on memory you've
 * allocated with @ref mdl_malloc.
 *
 * @param ds
 * @param elem_size
 * @param elem_comparator
 * @return
 *
 * @see mdl_memblklist_init
 */
MDL_API
MDL_ANNOTN__NONNULL
MDL_ANNOTN__NODISCARD
MDLMemBlkList *mdl_memblklist_new(MDLState *ds, size_t elem_size);

/**
 * Initialize an allocated list.
 *
 * Users that wish to statically allocate a list (e.g. to avoid an additional allocation)
 * will use this to initialize it. They will still need to destroy the list using
 * @ref mdl_memblklist_destroy.
 *
 * @param ds The MetalData state.
 * @param list The list to initialize.
 * @param elem_size The size of a single element in the list.
 * @return 0 on success, an error code otherwise.
 *
 * @see mdl_memblklist_new
 * @see mdl_memblklist_destroy
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklist_init(MDLState *ds, MDLMemBlkList *list, size_t elem_size);

/**
 * Destroy a list.
 *
 * @param list
 * @return 0 on success, an error code otherwise.
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklist_destroy(MDLMemBlkList *list);

/**
 * Return the length of the queue.
 *
 * @param list The queue to examine.
 */
MDL_API
MDL_ANNOTN__NONNULL
size_t mdl_memblklist_length(const MDLMemBlkList *list);

/**
 * Get the size of an element data block, in bytes.
 *
 * @param list The list to operate on.
 *
 * @return The size of a data block, in bytes.
 */
MDL_API
MDL_ANNOTN__NONNULL
size_t mdl_memblklist_getelementsize(const MDLMemBlkList *list);

/**
 * Get the first data block in the list.
 *
 * This is marginally more efficient than @ref mdl_memblklist_getblockat.
 *
 * @param list The list to operate on.
 *
 * @return NULL if the list is empty, otherwise a pointer to the value of the first
 *         element in the list.
 */
MDL_API
MDL_ANNOTN__NONNULL
void *mdl_memblklist_head(const MDLMemBlkList *list);

/**
 * Get the last data block in the list.
 *
 * @param list The list to operate on.
 *
 * @return NULL if the list is empty, otherwise a pointer to the value of the last
 *         element in the list.
 */
MDL_API
MDL_ANNOTN__NONNULL
void *mdl_memblklist_tail(const MDLMemBlkList *list);

/**
 * Append a new data block to the end of the list.
 *
 * @param list The list to operate on.
 *
 * @return A pointer to the block of memory in the new element, or NULL if allocation
 *         failed. If the operation fails, the list is unmodified.
 */
MDL_API
MDL_ANNOTN__NONNULL
void *mdl_memblklist_push(MDLMemBlkList *list);

/**
 * Remove a memory block from the end of the list.
 *
 * @param list The list to operate on.
 *
 * @return @ref MDL_OK on success, @ref MDL_ERROR_EMPTY if the list is empty.
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklist_pop(MDLMemBlkList *list);

/**
 * Like @ref mdl_memblklist_pop, but copies the data block to @a buf before removing the
 * element.
 *
 * @param list The list to operate on.
 *
 * @return @ref MDL_OK on success, @ref MDL_ERROR_EMPTY if the list is empty.
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklist_popcopy(MDLMemBlkList *list, void *buf);

MDL_API
MDL_ANNOTN__NONNULL
void *mdl_memblklist_pushfront(MDLMemBlkList *list);

/**
 * Remove the element at the front of the list.
 *
 * @param list The list to operate on.
 *
 * @return @ref MDL_OK on success, @ref MDL_ERROR_EMPTY if the list is empty.
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklist_popfront(MDLMemBlkList *list);

/**
 * Like @ref mdl_memblklist_popfront, but copies the data block to @a buf before removing
 * the element.
 *
 * @param list The list to operate on.
 *
 * @return @ref MDL_OK on success, @ref MDL_ERROR_EMPTY if the list is empty. If an error
 *         occurs, @a buf is left unmodified.
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklist_popfrontcopy(MDLMemBlkList *list, void *buf);

/**
 * Get a pointer to the data block at the given index of the list.
 *
 * @param list The list to operate on.
 * @param index The index of the data to access. Negative values index from the end of
 *              the list, i.e. -1 is the last data, -2 is second-to-last, etc.
 * @return A pointer to the data block, or NULL if the index is invalid.
 */
MDL_API
MDL_ANNOTN__NONNULL
void *mdl_memblklist_getblockat(const MDLMemBlkList *list, int index);

/**
 * Copy @a src to the list element at @a index.
 *
 * @param list    The list to operate on.
 * @param index   The index of the element to modify. Negative values index from the end
 *                of the array.
 * @param[in] src A pointer to the data to copy.
 *
 * @return 0 on success, @ref MDL_ERROR_OUT_OF_RANGE if the index is invalid.
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklist_set(MDLMemBlkList *list, int index, const void *src);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
int mdl_memblklist_insertafter(MDLMemBlkList *list, int index, void **ptr);

MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklist_insertaftercopy(MDLMemBlkList *list, int index, const void *buf);

/**
 * Remove an item from the list.
 *
 * @param list  The list to operate on.
 * @param index The index of the element to remove. Negative values index from the end of
 *              the array.
 *
 * @return 0 on success, an error code otherwise (most likely @ref MDL_ERROR_OUT_OF_RANGE)
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklist_removeat(MDLMemBlkList *list, int index);

/**
 * Copy the memory block at the given index, then remove it.
 *
 * This is functionally equivalent to copying the block in @ref mdl_memblklist_getat
 * followed by @ref mdl_memblklist_removeat, however it's much more efficient because it
 * only makes one pass through the list.
 *
 * @param list          The list to operate on.
 * @param index         The index of the element to remove. Negative numbers index from
 *                      the end of the list.
 * @param[out] buf      The buffer that the memory block will be copied to.
 *
 * @return @ref MDL_OK on success, @ref MDL_ERROR_EMPTY if the list is empty. If the list
 *         is empty, @a buf is unmodified.
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklist_removeatcopy(MDLMemBlkList *list, int index, void *buf);

/**
 * Remove all elements in the queue.
 *
 * @param list The queue to operate on.
 * @return
 */
MDL_API
MDL_ANNOTN__NONNULL
void mdl_memblklist_clear(MDLMemBlkList *list);

/**
 * Search the list for the first data matching @a value.
 *
 * @param list
 * @param value
 *
 * @return The absolute index of the first matching data found, or a negative
 * number if no match was found.
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklist_find(const MDLMemBlkList *list, const void *value,
                        mdl_comparator_fptr cmp, const void **ptr);

MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklist_findindex(const MDLMemBlkList *list, const void *value,
                             mdl_comparator_fptr cmp);

/**
 * Like @ref mdl_memblklist_find except this searches the queue back to front.
 *
 * @param list
 * @param value
 * @return
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklist_rfind(const MDLMemBlkList *list, const void *value,
                         mdl_comparator_fptr cmp, void **ptr);

MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklist_rfindindex(const MDLMemBlkList *list, const void *value,
                              mdl_comparator_fptr cmp);

/**
 * Rotate the list forward or backward without copying any data.
 *
 * A positive number rotates the head of of the list forward, toward higher indexes. That
 * is to say, rotating by +1 moves `list[x]` to `list[x+1]`. The final element of the list
 * moves to index 0.
 *
 * A negative number rotates the list in the opposite direction. Rotating a list by -1
 * moves `list[x]` to `list[x-1]`, and `list[0]` moves to the end.
 *
 * @param list The list to operate on.
 * @param places The number of places to rotate.
 *
 * @return 0 on success, @ref MDL_ERROR_EMPTY if the list is empty.
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklist_rotate(MDLMemBlkList *list, int places);

#define mdl_memblklist_rotateone(list) mdl_memblklist_rotate((list), 1)

MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklist_removevalue(MDLMemBlkList *list, const void *value,
                               mdl_comparator_fptr cmp);

MDL_API
MDL_ANNOTN__NODISCARD
MDL_ANNOTN__NONNULL
MDLMemBlkListIterator *mdl_memblklist_getiterator(const MDLMemBlkList *list,
                                                  bool reverse);

MDL_API
MDL_ANNOTN__NONNULL
void mdl_memblklistiter_init(const MDLMemBlkList *list, MDLMemBlkListIterator *iterator,
                             bool reverse);

MDL_API
MDL_ANNOTN__NONNULL
void *mdl_memblklistiter_get(MDLMemBlkListIterator *iter);

/**
 * Advance the iterator to the next element in the input.
 *
 * @param iter The iterator to operate on.
 * @return 0 on success, @ref MDL_EOF if the input has been exhausted.
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklistiter_next(MDLMemBlkListIterator *iter);

MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklistiter_hasnext(const MDLMemBlkListIterator *iter);

MDL_API
MDL_ANNOTN__NONNULL
void mdl_memblklistiter_destroy(MDLMemBlkListIterator *iter);

#endif /* INCLUDE_METALDATA_LLIST_H_ */
