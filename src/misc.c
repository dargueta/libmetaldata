#include "metaldata/internal/misc.h"
#include <stddef.h>

static const mdl_hash_type djb2_hash_init = 5381;

// DJB2 algorithm
mdl_hash_type mdl_hash_string(const char *restrict string)
{
    if (string == NULL)
        return 0;

    mdl_hash_type hash = djb2_hash_init;

    for (size_t i = 0; string[i] != '\0'; i++)
        hash = ((hash << 5) + hash) + string[i]; // hash = (hash * 33) + s[i]
    return hash;
}

mdl_hash_type mdl_hash_memory(const void *restrict block, size_t size)
{
    if (block == NULL)
        return 0;

    mdl_hash_type hash = djb2_hash_init;
    for (size_t i = 0; i < size; i++)
        hash = ((hash << 5) + hash) + ((const char *)block)[i];
    return hash;
}
