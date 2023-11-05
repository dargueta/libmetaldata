#include "metaldata/internal/misc.h"
#include <stddef.h>

// DJB2 algorithm
mdl_hash_type mdl_hash_string(const char *restrict s)
{
    if (s == NULL)
        return 0;

    mdl_hash_type hash = 5381;

    for (size_t i = 0; s[i] != '\0'; i++)
        hash = ((hash << 5) + hash) + s[i]; // hash = (hash * 33) + s[i]
    return hash;
}

mdl_hash_type mdl_hash_memory(const void *restrict p, size_t size)
{
    if (p == NULL)
        return 0;

    mdl_hash_type hash = 5381;
    for (size_t i = 0; i < size; i++)
        hash = ((hash << 5) + hash) + ((const char *)p)[i];
    return hash;
}
