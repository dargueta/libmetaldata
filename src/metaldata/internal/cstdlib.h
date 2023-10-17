/**
 * Implementations of C standard library functions.
 *
 * @file cstdlib.h
 */

#ifndef INCLUDE_METALDATA_INTERNAL_CSTDLIB_H_
#define INCLUDE_METALDATA_INTERNAL_CSTDLIB_H_

#include "../configuration.h"
#include "../metaldata.h"
#include "annotations.h"
#include <stddef.h>

#if MDL_COMPILED_AS_UNHOSTED
#    if defined(__has_builtin)
#        define MDL_LIBC_HAVE_BUILTIN_MEMCPY __has_builtin(__builtin_memcpy)
#        define MDL_LIBC_HAVE_BUILTIN_MEMSET __has_builtin(__builtin_memset)
#        define MDL_LIBC_HAVE_BUILTIN_STRCMP __has_builtin(__builtin_strcmp)
#        define MDL_LIBC_HAVE_BUILTIN_ABORT __has_builtin(__builtin_trap)
#    elif defined(__GNUC__)
#        define MDL_LIBC_HAVE_BUILTIN_MEMCPY MINIMUM_GNU_VERSION(4, 0, 0)
#        define MDL_LIBC_HAVE_BUILTIN_MEMSET MINIMUM_GNU_VERSION(4, 0, 0)
#        define MDL_LIBC_HAVE_BUILTIN_STRCMP MINIMUM_GNU_VERSION(4, 0, 0)
#        define MDL_LIBC_HAVE_BUILTIN_ABORT MINIMUM_GNU_VERSION(4, 2, 0)
#    else
#        define MDL_LIBC_HAVE_BUILTIN_MEMCPY 0
#        define MDL_LIBC_HAVE_BUILTIN_MEMSET 0
#        define MDL_LIBC_HAVE_BUILTIN_STRCMP 0
#        define MDL_LIBC_HAVE_BUILTIN_ABORT 0
#    endif

#    define MDL_LIBC_NEED_CUSTOM_ASSERT 1
#    define MDL_LIBC_NEED_CUSTOM_MEMCPY (!MDL_LIBC_HAVE_BUILTIN_MEMCPY)
#    define MDL_LIBC_NEED_CUSTOM_MEMSET (!MDL_LIBC_HAVE_BUILTIN_MEMSET)
#    define MDL_LIBC_NEED_CUSTOM_STRCMP (!MDL_LIBC_HAVE_BUILTIN_STRCMP)
#else
#    include <assert.h>
#    include <string.h>

#    define mdl_assert(ds, expr) assert(expr)
#    define mdl_memcpy memcpy
#    define mdl_memset memset
#    define mdl_strcmp strcmp
#    define MDL_LIBC_NEED_CUSTOM_MEMCPY 0
#    define MDL_LIBC_NEED_CUSTOM_MEMSET 0
#    define MDL_LIBC_NEED_CUSTOM_STRCMP 0
#    define MDL_LIBC_NEED_CUSTOM_ASSERT 0
#    define MDL_LIBC_HAVE_BUILTIN_MEMCPY 0
#    define MDL_LIBC_HAVE_BUILTIN_MEMSET 0
#    define MDL_LIBC_HAVE_BUILTIN_STRCMP 0
#    define MDL_LIBC_HAVE_BUILTIN_ABORT 0
#endif

#if MDL_LIBC_NEED_CUSTOM_MEMCPY
/**
 * A naive implementation of the C standard library's `memcpy()`.
 *
 * @warning @a dest and @a src must not overlap.
 *
 * @param dest The target buffer to copy data into.
 * @param src The source buffer to copy data from.
 * @param size The number of bytes to copy.
 *
 * @return Always returns @a dest.
 */
MDL_INTERNAL
MDL_ANNOTN__NONNULL
MDL_ANNOTN__RETURNS_NONNULL
void *mdl_memcpy(void *restrict dest, const void *restrict src, size_t size);
#elif MDL_LIBC_HAVE_BUILTIN_MEMCPY
#    define mdl_memcpy __builtin_memcpy
#endif

#if MDL_LIBC_NEED_CUSTOM_MEMSET
/**
 * A naive implementation of the C standard library's `memset()`.
 *
 * @param ptr A pointer to the memory block to initialize.
 * @param value The value to set all the bytes in the memory block to.
 * @param size The number of bytes to set.
 *
 * @return Always returns @a ptr.
 */
MDL_INTERNAL
MDL_ANNOTN__NONNULL
MDL_ANNOTN__RETURNS_NONNULL
void *mdl_memset(void *ptr, int value, size_t size);
#elif MDL_LIBC_HAVE_BUILTIN_MEMSET
#    define mdl_memset __builtin_memset
#endif

#if MDL_LIBC_NEED_CUSTOM_STRCMP
/**
 * A naive implementation of the C standard library's `strcmp()`.
 *
 * Because this is only used if the standard library is unavailable, this doesn't take
 * locale into account and is a simple byte-by-byte comparison.
 *
 * @param left A null-terminated C string.
 * @param right Another null-terminated C string to compare against @a left.
 *
 * @return A negative number if @a left sorts before @a right, 0 if they're equal, or a
 *         positive number if @a right sorts before @a left.
 */
MDL_INTERNAL
MDL_ANNOTN__NONNULL
int mdl_strcmp(const char *left, const char *right);
#elif MDL_LIBC_HAVE_BUILTIN_STRCMP
#    define mdl_strcmp __builtin_strcmp
#endif

/* On GCC, fail compilation if there's any direct usage of the functions we've implemented
 * or aliased here.
 *
 * This helps us ensure that, for example, we only use mdl_memset() regardless of whether
 * we have access to the C standard library or not. That way, we don't need to change any
 * code when compiling without C standard library support.
 *
 * Granted, this won't work on compilers that don't support this directive, but if it
 * compiles on GCC then that means we didn't accidentally use a standard library function
 * and can be pretty sure it'll work for any compiler. Probably. In an ideal world. Then
 * again, 2020 wouldn't've happened in an ideal world... */
#if MDL_CURRENTLY_COMPILING_LIBRARY && defined(__GNUC__)
/* You can't poison a macro, and which identifiers are macros depends on the OS. */
#    ifdef __APPLE__
#        if !MDL_COMPILED_AS_UNHOSTED
/* assert() is a macro in hosted mode, so we can't poison it. */
#            pragma GCC poison strcmp
#            undef assert
#        else
#            pragma GCC poison assert strcmp
#        endif /* MDL_COMPILED_AS_UNHOSTED */
#        undef memcpy
#        undef memset
#    else
#        pragma GCC poison memcpy memset strcmp
#        undef assert
#    endif
#endif

#endif /* INCLUDE_METALDATA_INTERNAL_CSTDLIB_H_ */
