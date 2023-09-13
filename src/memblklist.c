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
static void unlink_and_free_node(MDLState *ds, MDLMemBlkListNode *node);

MDL_ANNOTN__NONNULL
static size_t to_absolute_index(const MDLMemBlkList *list, int index);

MDL_ANNOTN__NONNULL
static MDLMemBlkListNode *get_node_at_abs_index(const MDLMemBlkList *list, size_t index);

MDL_ANNOTN__NONNULL
static MDLMemBlkListNode *get_node_at_relative_index(const MDLMemBlkList *list,
                                                     int index);

int mdl_memblklist_init(MDLState *ds, MDLMemBlkList *list, size_t elem_size)
{
    list->head = mdl_malloc(ds, sizeof(*list->head));
    if (list->head == NULL)
        return MDL_ERROR_NOMEM;

    list->was_allocated = false;
    list->ds = ds;
    list->length = 0;
    list->elem_size = elem_size;
    list->head->next = list->head;
    list->head->prev = list->head;
    return 0;
}

MDLMemBlkList *mdl_memblklist_new(MDLState *ds, size_t elem_size)
{
    int init_result;
    MDLMemBlkList *list = mdl_malloc(ds, sizeof(*list));

    if (list == NULL)
        return NULL;

    init_result = mdl_memblklist_init(ds, list, elem_size);
    if (init_result == MDL_OK)
    {
        list->was_allocated = true;
        return list;
    }

    mdl_free(ds, list, sizeof(*list));
    return NULL;
}

int mdl_memblklist_destroy(MDLMemBlkList *list)
{
    mdl_memblklist_clear(list);
    if (list->was_allocated)
        mdl_free(list->ds, list, sizeof(*list));
    return MDL_OK;
}

size_t mdl_memblklist_length(const MDLMemBlkList *list)
{
    return list->length;
}

void *mdl_memblklist_appendblock(MDLMemBlkList *list)
{
    MDLMemBlkListNode *new_tail;

    new_tail = mdl_memblklist_appendnewnode(list);
    if (new_tail == NULL)
        return NULL;

    list->length++;
    return new_tail->data;
}

void *mdl_memblklist_pushcopy(MDLMemBlkList *list, const void *data)
{
    void *target;
    target = mdl_memblklist_appendblock(list);
    if (target == NULL)
        return NULL;

    mdl_memcpy(target, data, list->elem_size);
    return target;
}

int mdl_memblklist_pop(MDLMemBlkList *list)
{
    MDLMemBlkListNode *tail;
    size_t item_index;

    if (list->length == 0)
        return MDL_ERROR_EMPTY;

    tail = list->head->prev;

    /* Because we always allocate a new node every time the tail is full */
    item_index = list->length % MDL_LLIST_ELEMENTS;
    list->length--;

    /* If we just removed the last item from the tail node (and that tail is not the head
     * node) then free the node. */
    if ((item_index == 1) && (tail != list->head))
        unlink_and_free_node(list->ds, tail);

    return MDL_OK;
}

int mdl_memblklist_popcopy(MDLMemBlkList *list, void *buf)
{
    MDLMemBlkListNode *tail;

    if (list->length == 0)
        return MDL_ERROR_EMPTY;

    tail = list->head->prev;
    mdl_memcpy(buf, tail->data, list->elem_size);
    list->length--;

    unlink_and_free_node(list->ds, tail);
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

void *mdl_memblklist_prependblock(MDLMemBlkList *list)
{
    /* Because this is a circularly linked list, we can push an item onto the
     * front of the list by appending it to the back and moving the head back to
     * point to it. */
    void *new_buffer = mdl_memblklist_appendblock(list);
    if (new_buffer != NULL)
        list->head = list->head->prev;
    return new_buffer;
}

void *mdl_memblklist_pushfrontcopy(MDLMemBlkList *list, const void *data)
{
    void *new_buffer = mdl_memblklist_prependblock(list);
    if (new_buffer != NULL)
        mdl_memcpy(new_buffer, data, list->elem_size);
    return new_buffer;
}

int mdl_memblklist_popfront(MDLMemBlkList *list)
{
    MDLMemBlkListNode *next_head;

    if (list->length == 0)
        return MDL_ERROR_EMPTY;

    next_head = list->head->next;
    unlink_and_free_node(list->ds, list->head);
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

int mdl_memblklist_getblockat(const MDLMemBlkList *list, int index, void **ptr)
{
    const MDLMemBlkListNode *node = get_node_at_relative_index(list, index);
    if (node == NULL)
        return MDL_ERROR_OUT_OF_RANGE;

    *ptr = (void *)node->data;
    return 0;
}

int mdl_memblklist_copyblockat(const MDLMemBlkList *list, int index, void *buf)
{
    void *source;
    int result;

    result = mdl_memblklist_getblockat(list, index, &source);
    if (result == MDL_OK)
        mdl_memcpy(buf, source, list->elem_size);
    return result;
}

int mdl_memblklist_set(MDLMemBlkList *list, int index, const void *src)
{
    MDLMemBlkListNode *node = get_node_at_relative_index(list, index);
    if (node == NULL)
        return MDL_ERROR_OUT_OF_RANGE;

    mdl_memcpy(node->data, src, list->elem_size);
    return 0;
}

int mdl_memblklist_removeat(MDLMemBlkList *list, int index)
{
    MDLMemBlkListNode *node = get_node_at_relative_index(list, index);
    if (node == NULL)
        return MDL_ERROR_OUT_OF_RANGE;

    unlink_and_free_node(list->ds, node);
    return 0;
}

int mdl_memblklist_removeatcopy(MDLMemBlkList *list, int index, void *buf)
{
    MDLMemBlkListNode *node = get_node_at_relative_index(list, index);
    if (node == NULL)
        return MDL_ERROR_OUT_OF_RANGE;

    mdl_memcpy(buf, node->data, list->elem_size);
    unlink_and_free_node(list->ds, node);
    return 0;
}

void mdl_memblklist_clear(MDLMemBlkList *list)
{
    MDLMemBlkListNode *next_node, *current_node;

    /* Free every node except the head, which we always keep. */
    for (current_node = list->head; list->length > 0; list->length--)
    {
        next_node = current_node->next;
        unlink_and_free_node(list->ds, current_node);
        current_node = next_node;
    }
}

MDLMemBlkListNode *mdl_memblklist_findnode(const MDLMemBlkList *list, const void *value,
                                           mdl_comparator_fptr cmp)
{
    size_t index;
    MDLMemBlkListNode *node = list->head;

    for (index = 0; index < list->length; ++index)
    {
        if (cmp(list->ds, node->data, value, list->elem_size) == 0)
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

int mdl_memblklist_findindex(const MDLMemBlkList *list, const void *value,
                             mdl_comparator_fptr cmp)
{
    int index;
    const MDLMemBlkListNode *current_node = list->head;

    for (index = 0; (size_t)index < list->length; ++index)
    {
        if (cmp(list->ds, current_node->data, value, list->elem_size) == 0)
            return index;

        current_node = current_node->next;
    }

    return -1;
}

int mdl_memblklist_rfindindex(const MDLMemBlkList *list, const void *value,
                              mdl_comparator_fptr cmp)
{
    int index;
    MDLMemBlkListNode *current_node = list->head->prev;

    /* Unlike with find, we need to check to see if the list is empty so that we
     * don't overflow when subtracting 1 from unsigned 0. */
    if (list->length == 0)
        return -1;

    for (index = (int)list->length - 1; index >= 0; --index)
    {
        if (cmp(list->ds, current_node->data, value, list->elem_size) == 0)
            return index;

        current_node = current_node->prev;
    }

    return -1;
}

int mdl_memblklist_rotate(MDLMemBlkList *list, int places)
{
    if (list->length == 0)
        return MDL_ERROR_EMPTY;

    if (places > 0)
    {
        for (; places > 0; places--)
            list->head = list->head->next;
    }
    else
    {
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

    unlink_and_free_node(list->ds, node);
    return 1;
}

MDLMemBlkListIterator *mdl_memblklist_getiterator(const MDLMemBlkList *list, bool reverse)
{
    MDLMemBlkListIterator *iter;

    iter = mdl_malloc(list->ds, sizeof(*iter));
    if (iter == NULL)
        return NULL;

    mdl_memblklist_inititerator(list, iter, reverse);
    iter->was_allocated = true;
    return iter;
}

int mdl_memblklist_inititerator(const MDLMemBlkList *list,
                                MDLMemBlkListIterator *iterator, bool reverse)
{

    iterator->current = list->head;
    iterator->reverse = reverse;
    iterator->end = iterator->current->prev;
    iterator->was_allocated = false;
    return 0;
}

void *mdl_memblklistiter_get(MDLMemBlkListIterator *iter)
{
    return iter->current->data;
}

int mdl_memblklistiter_next(MDLMemBlkListIterator *iter)
{
    if (!mdl_memblklistiter_hasnext(iter))
        return MDL_ERROR_EMPTY;

    if (iter->reverse)
        iter->current = iter->current->prev;
    else
        iter->current = iter->current->next;
    return 0;
}

int mdl_memblklistiter_hasnext(const MDLMemBlkListIterator *iter)
{
    return iter->current != iter->end;
}

void mdl_memblklistiter_destroy(MDLMemBlkListIterator *iter)
{
    if (iter->was_allocated)
        mdl_free(iter->list->ds, iter, sizeof(*iter));
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

static void unlink_and_free_node(MDLState *ds, MDLMemBlkListNode *node)
{
    unlink_node(node);
    mdl_free(ds, node, sizeof(*node));
}

void mdl_memblklist_movenodeafter(MDLMemBlkListNode *new_node,
                                  MDLMemBlkListNode *prev_node)
{
    new_node->prev = prev_node;
    new_node->next = prev_node->next;
    prev_node->next = new_node;
}

static size_t to_absolute_index(const MDLMemBlkList *list, int index)
{
    if (index >= 0)
        return (size_t)index;

    return list->length - (size_t)(-index);
}

/* TODO (dargueta): Iterate backwards if the node is past the halfway point.
 *
 * The algorithm is still O(n) but for iterations forward or backward it can
 * save a lot of time.
 */
static MDLMemBlkListNode *get_node_at_abs_index(const MDLMemBlkList *list, size_t index)
{
    size_t i;
    const MDLMemBlkListNode *node;

    if (index >= list->length)
        return NULL;

    node = list->head;
    for (i = 0; i < index; i++)
        node = node->next;
    return (MDLMemBlkListNode *)node;
}

static MDLMemBlkListNode *get_node_at_relative_index(const MDLMemBlkList *list, int index)
{
    size_t absolute_index = to_absolute_index(list, index);
    return get_node_at_abs_index(list, absolute_index);
}

MDLMemBlkListNode *mdl_memblklist_appendnewnode(MDLMemBlkList *list)
{
    MDLMemBlkListNode *node = mdl_malloc(list->ds, sizeof(*node));
    if (node == NULL)
        return NULL;

    mdl_memblklist_movenodeafter(node, list->head->prev);
    return node;
}
