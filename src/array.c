#include "metaldata/array.h"
#include "metaldata/metaldata.h"

static int get_node_location_by_index(const MDLArray *array, int index, void **block,
                                      size_t *offset);

MDLArray *mdl_array_new(MDLState *ds, mdl_destructor_fptr elem_destructor)
{
    int result;
    MDLArray *array;

    array = mdl_malloc(ds, sizeof(*array));
    if (array == NULL)
        return NULL;

    result = mdl_array_init(ds, array, elem_destructor);
    if (result != MDL_OK)
    {
        mdl_free(ds, array, sizeof(*array));
        return NULL;
    }

    array->was_allocated = true;
    return array;
}

int mdl_array_init(MDLState *ds, MDLArray *array, mdl_destructor_fptr elem_destructor)
{
    array->blocks = mdl_malloc(ds, sizeof(MDLArrayBlock *));
    if (array->blocks == NULL)
        return MDL_ERROR_NOMEM;

    array->length = 0;
    array->was_allocated = false;
    array->elem_destructor = elem_destructor;
    return MDL_OK;
}

int mdl_array_destroy(MDLArray *array)
{
    size_t num_blocks = array->length / MDL_DEFAULT_ARRAY_BLOCK_SIZE;
    if (array->length % MDL_DEFAULT_ARRAY_BLOCK_SIZE > 0)
        ++num_blocks;

    if (array->elem_destructor != NULL)
    {
        size_t elements_remaining = array->length;

        for (size_t block_i = 0; block_i < num_blocks; block_i++)
        {
            size_t loop_limit = (MDL_DEFAULT_ARRAY_BLOCK_SIZE < elements_remaining)
                                    ? MDL_DEFAULT_ARRAY_BLOCK_SIZE
                                    : elements_remaining;

            for (size_t elem_i = 0; elem_i < loop_limit; elem_i++)
                array->elem_destructor(array->ds, array->blocks[block_i][elem_i]);
        }
    }

    mdl_free(array->ds, array->blocks, sizeof(*array->blocks) * num_blocks);

    if (array->was_allocated)
        mdl_free(array->ds, array, sizeof(*array));

    return 0;
}

size_t mdl_array_length(const MDLArray *array)
{
    return array->length;
}

int mdl_array_head(const MDLArray *array, void **item)
{
    MDLArrayBlock *block;

    if (array->length == 0)
        return MDL_ERROR_EMPTY;

    block = mdl_memblklist_head(&array->block_list);
    if (block != NULL)
        *item = (*block)[0];
    return MDL_OK;
}

int mdl_array_tail(const MDLArray *array, void **item)
{
    MDLArrayBlock *block;
    size_t num_elements_in_last_block;

    if (array->length == 0)
        return MDL_ERROR_EMPTY;

    block = mdl_memblklist_tail(&array->block_list);

    num_elements_in_last_block = array->length % MDL_DEFAULT_ARRAY_BLOCK_SIZE;
    if (block != NULL)
        *item = (*block)[num_elements_in_last_block - 1];
    return MDL_OK;
}

int mdl_array_push(MDLArray *array, void *item)
{
    size_t element_offset;
    MDLArrayBlock *block;

    element_offset = array->length % MDL_DEFAULT_ARRAY_BLOCK_SIZE;

    /* If the number of elements in the list is an exact multiple of the block size, then
     * the final block is full, and we need to add a new one. In either case, after this
     * `block` will contain a pointer to the block we need to append the data to. */
    if ((element_offset == 0) && (array->length > 0))
    {
        block = mdl_memblklist_push(&array->block_list);
        if (block == NULL)
            return MDL_ERROR_NOMEM;
    }
    else
    {
        /* If our math is correct, this will never fail if the list is empty, and we
         * should never hit the error condition. */
        block = mdl_memblklist_tail(&array->block_list);
        if (block == NULL)
            return MDL_ERROR_ASSERT_FAILED;
    }

    (*block)[element_offset] = item;
    array->length++;
    return MDL_OK;
}

int mdl_array_pop(MDLArray *array, void **item)
{
    size_t element_index;
    MDLArrayBlock *last_block;

    last_block = mdl_memblklist_tail(&array->block_list);
    if (last_block == NULL)
        return MDL_ERROR_EMPTY;

    element_index = (array->length % MDL_DEFAULT_ARRAY_BLOCK_SIZE) - 1;
    *item = (*last_block)[element_index];
    array->length--;

    /* If element_index is 0 then that means we just popped the last data from the block.
     * Free the empty block *if* it's not the head. We need the head. */
    if ((array->length == 0) || (element_index > 0))
        return MDL_OK;

    return mdl_memblklist_pop(&array->block_list);
}

int mdl_array_pushfront(MDLArray *array, void *item);

int mdl_array_popfront(MDLArray *array, void **item);

int mdl_array_getat(const MDLArray *array, int index, void **value)
{
    void *block;
    size_t block_offset;
    int result;

    result = get_node_location_by_index(array, index, &block, &block_offset);
    if (result != MDL_OK)
        return result;

    *value = ((void **)block)[block_offset];
    return MDL_OK;
}

int mdl_array_setat(MDLArray *array, int index, void *new_value)
{
    void *block;
    size_t block_offset;
    int result;

    result = get_node_location_by_index(array, index, &block, &block_offset);
    if (result != MDL_OK)
        return result;

    ((void **)block)[block_offset] = new_value;
    return MDL_OK;
}

int mdl_array_insertafter(MDLArray *array, int index, void *new_value);

int mdl_array_removeat(MDLArray *array, int index, void **value);

void mdl_array_clear(MDLArray *array);

int mdl_array_find(const MDLArray *array, const void *value, mdl_comparator_fptr cmp);

int mdl_array_rfind(const MDLArray *array, const void *value, mdl_comparator_fptr cmp);

int mdl_array_removevalue(MDLArray *array, const void *value, mdl_comparator_fptr cmp);

MDLArrayIterator *mdl_array_getiterator(const MDLArray *array, bool reverse)
{
    MDLArrayIterator *iter = mdl_malloc(array->block_list.ds, sizeof(*iter));
    if (iter == NULL)
        return NULL;

    mdl_arrayiter_init(array, iter, reverse);
    iter->was_allocated = true;
    return iter;
}

void mdl_arrayiter_init(const MDLArray *array, MDLArrayIterator *iter, bool reverse)
{
    ;
    iter->was_allocated = false;
    iter->block_element_index = 0;
    mdl_memblklistiter_init(&array->block_list, &iter->block_iterator, reverse);
}

void *mdl_arrayiter_get(const MDLArrayIterator *iter);

int mdl_arrayiter_next(MDLArrayIterator *iter);

int mdl_arrayiter_hasnext(const MDLArrayIterator *iter);

void mdl_arrayiter_destroy(MDLArrayIterator *iter);

/******** Helper functions ********/

static int get_node_location_by_index(const MDLArray *array, int index, void **block,
                                      size_t *offset)
{
    size_t absolute_index, block_number, block_offset;

    if (index >= 0)
        absolute_index = (size_t)index;
    else
        absolute_index = array->length - ((size_t)-index);

    if (absolute_index >= array->length)
        return MDL_ERROR_OUT_OF_RANGE;

    block_number = absolute_index / MDL_DEFAULT_ARRAY_BLOCK_SIZE;
    block_offset = absolute_index % MDL_DEFAULT_ARRAY_BLOCK_SIZE;

    *block = mdl_memblklist_getblockat(&array->block_list, (int)block_number);
    *offset = block_offset;
    return 0;
}
