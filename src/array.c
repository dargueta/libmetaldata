#include "metaldata/array.h"
#include "metaldata/metaldata.h"

MDLArray *mdl_array_new(MDLState *ds, mdl_comparator_fptr elem_comparator,
                        mdl_destructor_fptr elem_destructor)
{
    int result;
    MDLArray *array;

    array = mdl_malloc(ds, sizeof(*array));
    if (array == NULL)
        return NULL;

    result = mdl_array_init(ds, array, elem_comparator, elem_destructor);
    if (result != MDL_OK)
    {
        mdl_free(ds, array, sizeof(*array));
        return NULL;
    }

    array->owned = 1;
    return array;
}

int mdl_array_init(MDLState *ds, MDLArray *array, mdl_comparator_fptr elem_comparator,
                   mdl_destructor_fptr elem_destructor)
{
    int result;

    result = mdl_memblklist_init(ds, &array->block_list, sizeof(MDLArrayBlock));
    if (result != MDL_OK)
        return result;

    array->length = 0;
    array->owned = 0;
    array->elem_comparator = elem_comparator;
    array->elem_destructor = elem_destructor;
    return MDL_OK;
}

int mdl_array_destroy(MDLArray *array)
{
    MDLArrayBlock block;
    int error;

    while (array->length > 0)
    {
        size_t elements_in_block, i;

        if (array->length > MDL_DEFAULT_ARRAY_BLOCK_SIZE)
            elements_in_block = MDL_DEFAULT_ARRAY_BLOCK_SIZE;
        else
            elements_in_block = array->length;

        mdl_memblklist_popfrontcopy(&array->block_list, block);

        for (i = 0; i < elements_in_block; i++)
            array->elem_destructor(array->block_list.ds, block[i]);
        array->length -= elements_in_block;
    }

    error = mdl_memblklist_destroy(&array->block_list);

    if (array->owned)
        mdl_free(array->block_list.ds, array, sizeof(*array));
    return error;
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
    if (element_offset == 0)
    {
        block = mdl_memblklist_appendblock(&array->block_list);
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

    /* If element_index is 0 then that means we just popped the last data from the
     * block. Free the empty block *if* it's not the head. We need the head. */
    if ((array->length == 0) || (element_index > 0))
        return MDL_OK;

    return mdl_memblklist_pop(&array->block_list);
}
