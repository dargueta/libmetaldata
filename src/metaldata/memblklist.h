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

#include "annotations.h"
#include "configuration.h"
#include "metaldata.h"
#include <stdbool.h>
#include <stddef.h>

#ifndef MDL_LLIST_ELEMENTS
#    define MDL_LLIST_ELEMENTS 8
#endif

#if (MDL_LLIST_ELEMENTS <= 0) || (MDL_LLIST_ELEMENTS % 2 != 0)
#    error MDL_LLIST_ELEMENTS must be an even number greater than 0.
#endif

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
    char data[];
};

/**
 * A list of blocks of memory.
 *
 * @warning The struct is declared in the header only to allow users to allocate it on the
 *          stack. Do not modify it directly.
 */
struct MDLMemBlkList_
{
    MDLState *ds;
    size_t elem_size;
    MDLMemBlkListNode *head;
    size_t length;
    bool owned;
};

struct MDLMemBlkListIterator_
{
    MDLMemBlkListNode *current;
    MDLMemBlkListNode *end;
    MDLMemBlkList *list;
    int reverse;
    bool owned;
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
MDL_ANNOTN__NONNULL_ARGS(1)
MDL_ANNOTN__NODISCARD
MDLMemBlkList *mdl_memblklist_new(MDLState *ds, size_t elem_size);

/**
 * Initialize an allocated list.
 *
 * Users that wish to statically allocate a list (e.g. to avoid an additional allocation)
 * will use this to initialize it. They will still need to destroy the list using
 * @ref mdl_memblklist_destroy.
 *
 * @param ds
 * @param list The queue to initialize.
 * @param elem_size
 * @param elem_comparator
 *
 * @return 0 on success, an error code otherwise.
 *
 * @see mdl_memblklist_new
 * @see mdl_memblklist_destroy
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1, 2)
int mdl_memblklist_init(MDLState *ds, MDLMemBlkList *list, size_t elem_size);

/**
 * Destroy a double-ended queue.
 *
 * @param list
 * @return
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklist_destroy(MDLMemBlkList *list);

/**
 * Return the length of the queue.
 *
 * @param list The queue to examine.
 *
 * @return The length of the queue.
 */
MDL_API
MDL_ANNOTN__NONNULL
size_t mdl_memblklist_length(const MDLMemBlkList *list);

/**
 * Get the first data in the queue.
 *
 * This is marginally more efficient than @ref mdl_memblklist_get.
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
 * Get the last data in the queue.
 *
 * @param list The queue to operate on.
 *
 * @return NULL if the list is empty, otherwise a pointer to the value of the first
 *         element in the list.
 */
MDL_API
MDL_ANNOTN__NONNULL
void *mdl_memblklist_tail(const MDLMemBlkList *list);

/**
 * Append a new memory block to the end of the list.
 *
 * @param list The list to operate on.
 *
 * @return A pointer to the block of memory in the new element, or NULL if allocation
 *         failed.
 */
MDL_API
MDL_ANNOTN__NONNULL
void *mdl_memblklist_appendblock(MDLMemBlkList *list);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
void *mdl_memblklist_pushcopy(MDLMemBlkList *list, const void *data);

/**
 * Remove a memory block from the end of the list.
 *
 * @param list The list to operate on.
 *
 * @return @ref MDL_OK on success, @ref MDL_ERROR_EMPTY if the list is empty.
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
int mdl_memblklist_pop(MDLMemBlkList *list);

MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklist_popcopy(MDLMemBlkList *list, void *buf);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
void *mdl_memblklist_prependblock(MDLMemBlkList *list);

MDL_API
MDL_ANNOTN__NONNULL
void *mdl_memblklist_pushfrontcopy(MDLMemBlkList *list, const void *data);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
int mdl_memblklist_popfront(MDLMemBlkList *list);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
int mdl_memblklist_popfrontcopy(MDLMemBlkList *list, void *buf);

/**
 *
 * @param list The list to operate on.
 * @param index The index of the data to access. Negative values index from the end of
 *              the list, i.e. -1 is the last data, -2 is second-to-last, etc.
 * @param ptr
 * @return @ref MKL_OK if the item was found, an error code otherwise.
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklist_getblockat(const MDLMemBlkList *list, int index, void **ptr);

MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklist_copyblockat(const MDLMemBlkList *list, int index, void *buf);

MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklist_set(MDLMemBlkList *list, int index, const void *src);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
int mdl_memblklist_insertafter(MDLMemBlkList *list, int index, void **ptr);

MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklist_insertaftercopy(MDLMemBlkList *list, int index, const void *buf);

MDL_API
MDL_ANNOTN__NONNULL
int mdl_memblklist_removeat(MDLMemBlkList *list, int index);

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
 * Search the queue for the first data matching @a value.
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
 * is to say, rotating by +1 replaces `list[0]` with `list[1]`, `list[1]` with `list[2]`,
 * etc.
 *
 * A negative number rotates the list backwards. Rotating a list of size @e N by -1
 * replaces `list[0]` with `list[N]`, `list[1]` with `list[0]`, etc.
 *
 * @param list The list to operate on.
 * @param places The number of elements to rotate.
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

MDL_ANNOTN__NODISCARD
MDL_ANNOTN__NONNULL
MDLMemBlkListIterator *mdl_memblklist_getiterator(const MDLMemBlkList *list,
                                                  bool reverse);

MDL_ANNOTN__NONNULL
int mdl_memblklist_inititerator(const MDLMemBlkList *list,
                                MDLMemBlkListIterator *iterator, bool reverse);

MDL_ANNOTN__NONNULL
void *mdl_memblklistiter_get(MDLMemBlkListIterator *iter);

MDL_ANNOTN__NONNULL
int mdl_memblklistiter_next(MDLMemBlkListIterator *iter);

MDL_ANNOTN__NONNULL
int mdl_memblklistiter_hasnext(const MDLMemBlkListIterator *iter);

MDL_ANNOTN__NONNULL
void mdl_memblklistiter_destroy(MDLMemBlkListIterator *iter);

#endif /* INCLUDE_METALDATA_LLIST_H_ */
