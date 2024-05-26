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
    MDLState *mds;

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
    const MDLMemBlkList *list;
    MDLMemBlkListNode *current;
    size_t n_seen;
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
 * @param mds
 * @param elem_size
 * @param elem_comparator
 * @return
 *
 * @see mdl_memblklist_init
 */
MDL_API
MDL_ANNOTN__NONNULL
MDL_ANNOTN__NODISCARD
MDLMemBlkList *mdl_memblklist_new(MDLState *mds, size_t elem_size);

/**
 * Initialize an allocated list.
 *
 * Users that wish to statically allocate a list (e.g. to avoid an additional allocation)
 * will use this to initialize it. They will still need to destroy the list using
 * @ref mdl_memblklist_destroy.
 *
 * @param mds The MetalData state.
 * @param list The list to initialize.
 * @param elem_size The size of a single element in the list.
 *
 * @see mdl_memblklist_new
 * @see mdl_memblklist_destroy
 */
MDL_API
MDL_ANNOTN__NONNULL
void mdl_memblklist_init(MDLState *mds, MDLMemBlkList *list, size_t elem_size);

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
 * Return the number of elements in the list.
 *
 * @param list The list to examine.
 */
MDL_API
MDL_ANNOTN__NONNULL
size_t mdl_memblklist_length(const MDLMemBlkList *list);

/**
 * Determine if the given relative index is within the bounds of @a list.
 *
 * @param list The list to operate on.
 * @param relative_index A relative index to validate.
 * @return
 *      A boolean indicating if @a relative_index can be converted into a valid absolute
 *      index.
 *
 * @see mdl_memblklist_absindex
 */
MDL_API
MDL_ANNOTN__NONNULL
bool mdl_memblklist_isrelindexvalid(const MDLMemBlkList *list, long relative_index);

/**
 * Convert a relative index into an absolute index.
 *
 * The return value is guaranteed to be valid if @ref mdl_memblklist_isrelindexvalid
 * returns true. If @a relative_index evaluates to an invalid absolute index, the return
 * value is @ref MDL_INVALID_INDEX.
 *
 * @param list
 * @param relative_index
 *
 * @return
 *      The absolute index into the array corresponding to @a relative_index, or
 *      @ref MDL_INVALID_INDEX if @a relative_index is out of bounds.
 *
 * @see mdl_memblklist_isrelindexvalid
 */
MDL_API
MDL_ANNOTN__NONNULL
size_t mdl_memblklist_absindex(const MDLMemBlkList *list, long relative_index);

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
 * @param index The index of the data to access.
 * @return A pointer to the data block, or NULL if the index is invalid.
 */
MDL_API
MDL_ANNOTN__NONNULL
void *mdl_memblklist_getblockat(const MDLMemBlkList *list, size_t index);

/**
 * Copy @a src to the list element at @a index.
 *
 * @param list    The list to operate on.
 * @param index   The index of the element to modify.
 * @param[in] src A pointer to the data to copy.
 *
 * @return 0 on success, @ref MDL_ERROR_OUT_OF_RANGE if the index is invalid.
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklist_set(MDLMemBlkList *list, size_t index, const void *src);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
int mdl_memblklist_insertafter(MDLMemBlkList *list, size_t index, void **ptr);

MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklist_insertaftercopy(MDLMemBlkList *list, size_t index, const void *buf);

/**
 * Remove an item from the list.
 *
 * @param list  The list to operate on.
 * @param index The index of the element to remove.
 *
 * @return 0 on success, an error code otherwise (most likely @ref MDL_ERROR_OUT_OF_RANGE)
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklist_removeat(MDLMemBlkList *list, size_t index);

/**
 * Copy the memory block at the given index, then remove it.
 *
 * This is functionally equivalent to copying the block in @ref mdl_memblklist_getat
 * followed by @ref mdl_memblklist_removeat, however it's much more efficient because it
 * only makes one pass through the list.
 *
 * @param list          The list to operate on.
 * @param index         The index of the element to remove.
 * @param[out] buf      The buffer that the memory block will be copied to.
 *
 * @return @ref MDL_OK on success, @ref MDL_ERROR_EMPTY if the list is empty. If the list
 *         is empty, @a buf is unmodified.
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklist_removeatcopy(MDLMemBlkList *list, size_t index, void *buf);

/**
 * Remove all elements in the list.
 *
 * @param list The list to operate on.
 */
MDL_API
MDL_ANNOTN__NONNULL
void mdl_memblklist_clear(MDLMemBlkList *list);

/**
 * Search the list for the first data matching @a value.
 *
 * @param list The list to search through.
 * @param value
 * @param cmp
 * @param[out] ptr
 *
 * @return @ref MDL_OK if a match was found, an error code otherwise.
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklist_find(const MDLMemBlkList *list, const void *value,
                        mdl_comparator_fptr cmp, const void **ptr);

/**
 * Like @ref mdl_memblklist_find, but returns the index of the first matching element.
 *
 * @param list The list to search through.
 * @param value The value to compare the list elements against.
 * @param cmp A function to use to compare elements against @a value.
 * @return
 *      The index of the first element matching @a value, or @ref MDL_INVALID_INDEX if no
 *      match was found.
 */
MDL_API
MDL_ANNOTN__NONNULL
size_t mdl_memblklist_findindex(const MDLMemBlkList *list, const void *value,
                                mdl_comparator_fptr cmp);

/**
 * Like @ref mdl_memblklist_find except this searches the list back to front.
 *
 * @param list
 * @param value
 * @return @ref MDL_OK if a match was found, an error code otherwise.
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklist_rfind(const MDLMemBlkList *list, const void *value,
                         mdl_comparator_fptr cmp, void **ptr);

MDL_API
MDL_ANNOTN__NONNULL
size_t mdl_memblklist_rfindindex(const MDLMemBlkList *list, const void *value,
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
int mdl_memblklist_rotate(MDLMemBlkList *list, long places);

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
