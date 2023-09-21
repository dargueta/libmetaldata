#include "metaldata/configuration.h"
#include "metaldata/internal/cstdlib.h"
#include "metaldata/metaldata.h"
#include <stddef.h>

void mdl_initstate(MDLState *ds, mdl_alloc_fptr alloc, void *userdata)
{
    ds->allocator = alloc;
    ds->userdata = userdata;
}

int mdl_default_ptr_comparator(MDLState *ds, const void *left, const void *right,
                               size_t size)
{
    (void)ds, (void)size;
    if (left < right)
        return -1;
    else if (left > right)
        return 1;
    return 0;
}

int mdl_default_str_comparator(MDLState *ds, const void *left, const void *right)
{
    (void)ds;
    return mdl_strcmp((const char *)left, (const char *)right);
}

void mdl_default_noop_destructor(MDLState *ds, void *item)
{
    (void)ds, (void)item;
}

#if !MDL_COMPILED_AS_UNHOSTED
#    include "metaldata/extras/hosted_allocator.c"
#endif

void mdl_free(MDLState *ds, void *pointer, size_t old_size)
{
    (void)ds->allocator(pointer, 0, old_size, ds->userdata);
}

void *mdl_malloc(MDLState *ds, size_t size)
{
    return ds->allocator(NULL, size, 0, ds->userdata);
}

void *mdl_realloc(MDLState *ds, void *pointer, size_t new_size, size_t old_size)
{
    return ds->allocator(pointer, new_size, old_size, ds->userdata);
}
