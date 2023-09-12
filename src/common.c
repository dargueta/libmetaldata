#include "metaldata/configuration.h"
#include "metaldata/internal/cstdlib.h"
#include "metaldata/metaldata.h"
#include <stddef.h>

#if !MDL_COMPILED_AS_UNHOSTED
#    include <stdio.h>
#    include <stdlib.h>
#endif

#if !MDL_COMPILED_AS_UNHOSTED
void mdl_initdefaultstate(MDLState *ds, void *userdata)
{
    mdl_state_init(ds, mdl_default_alloc, mdl_default_panic, userdata);
}
#endif

void mdl_state_initwithalloc(MDLState *ds, mdl_alloc_fptr alloc, void *userdata)
{
    mdl_state_init(ds, alloc, mdl_default_panic, userdata);
}

void mdl_state_init(MDLState *ds, mdl_alloc_fptr alloc, mdl_panic_fptr panic,
                    void *userdata)
{
    ds->allocator = alloc;
    ds->panic = panic;
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
#    include "metaldata/internal/default_allocator.h"
#endif

void mdl_default_panic(const char *message, int error, void *userdata)
{
    (void)userdata;
#if !MDL_COMPILED_AS_UNHOSTED
    /* Target platform is hosted */
    fprintf(stderr, "Panic in MetalData library (error %d): %s", error, message);
    abort();
#elif MDL_LIBC_HAVE_BUILTIN_ABORT
    /* Target platform is unhosted, but we can still abort the program */
    (void)userdata, (void)message, (void)error;
    __builtin_trap();
#else
    (void)userdata, (void)message, (void)error;
#endif
}

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
