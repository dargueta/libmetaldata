// SPDX-License-Identifier: MPL-2.0+
// Copyright (C) 2020-2025  Diego Argueta
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

/**
 * Cross-compiler function and type annotations for better type checking and
 * (hopefully) better performance.
 *
 * More information on most of the function annotations described here is available in
 * GCC's documentation for
 * [function
 * attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html).
 *
 * @file annotations.h
 *
 *
 * @def MDL_API
 * A function attribute for marking MetalData's public API functions.
 *
 *
 * @def MDL_INTERNAL
 * A function attribute for marking MetalData's internal-only functions.
 *
 *
 * @def MDL_ANNOTN__ACCESS
 * A function attribute declaring how a pointer argument is accessed.
 *
 * This is only available for compilers compatible with GCC 10.5+.
 *
 * @param mode The access mode for the pointer.
 * @param argn The 1-based index of the pointer argument this annotation applies to.
 *
 *
 * @def MDL_ANNOTN__ACCESS_SIZED
 * Like @ref MDL_ANNOTN__ACCESS but used specifically for pointers to memory whose size is
 * given by another argument to the function.
 *
 * @param mode The access mode for the pointer.
 * @param argn The 1-based index of the pointer argument this annotation applies to.
 * @param sizen The 1-based index of the argument giving the size of the buffer, in bytes.
 *
 * @def MDL_ANNOTN__DEPRECATED
 * Marks a function as deprecated and should not be used in new code.
 *
 *
 * @def MDL_ANNOTN__MALLOC
 * Marks a function as working like `malloc()`. Allows the compiler to detect memory leaks
 * and make other optimizations like assuming the function will only rarely return a null
 * pointer.
 *
 *
 * @def MDL_ANNOTN__NODISCARD
 * Signals to the compiler that the return value of the function should not be ignored.
 *
 * This is commonly used on functions that allocate resources (like ``malloc()`` and
 * ``fopen()``), where failing to catch the return value will lead to a resource leak.
 *
 *
 * @def MDL_ANNOTN__NONNULL
 * A function attribute indicating all pointer arguments must not be null.
 *
 * This is equivalent to using @ref MDL_ANNOTN__NONNULL_ARGS and specifying each pointer
 * argument manually.
 *
 *
 * @def MDL_ANNOTN__NONNULL_ARGS
 * A function attribute indicating that only specific pointer arguments must not be null.
 *
 * This is like @ref MDL_ANNOTN__NONNULL, except that it allows being specific about which
 * arguments must not be null. This is useful when some pointers are allowed to be null.
 *
 * The macro takes any number of arguments. Each argument is an index of a pointer
 * argument that cannot be null. Indexes start from 1, going left to right.
 *
 *
 * @def MDL_ANNOTN__NORETURN
 * Indicates the function never returns, like ``abort()`` or ``exit()``.
 *
 *
 * @def MDL_ANNOTN__REPRODUCIBLE
 * Indicates that calling the function with the same arguments will return the same value.
 *
 *
 * @def MDL_ANNOTN__RETURNS_NONNULL
 * A function attribute that indicates the function never returns a null pointer.
 *
 *
 * @def MDL_FALLTHROUGH_MARKER
 * A marker used to indicate to the compiler that control from one `case` in a `switch`
 * statement is deliberately falling through to the next case.
 *
 *
 * @def MDL_REENTRANT_MARKER
 * A function attribute required by the SDCC compiler in certain instances for some
 * compilation targets.
 *
 *
 * @def MDL_ANNOTN__NONSTRING
 * Tell the compiler that a `char *` variable might not point to a null-terminated string.
 *
 *
 * @def GNU_ATTRIBUTE
 * A convenience macro that expands GCC attributes only if the compiler is compatible with
 * the version of GCC that implements that attribute.
 *
 *
 * @def MINIMUM_GNU_VERSION
 * Determine if the compiler is compatible with a version of GCC.
 *
 * Use this to determine if 1) the compiler is GCC-like, 2) the version of GCC it emulates
 * is at least the version given by the arguments.
 */

#ifndef INCLUDE_METALDATA_INTERNAL_ANNOTATIONS_H_
#define INCLUDE_METALDATA_INTERNAL_ANNOTATIONS_H_

/* This assumes that if __GNUC__ is defined, __GNUC_MINOR__ and __GNUC_PATCHLEVEL__ are
 * also defined. It's not exactly a safe assumption, but it's safe enough... Right? */
#ifdef __GNUC__
#    define MINIMUM_GNU_VERSION(major, minor, patch)                                     \
        ((__GNUC__ > (major)) || (__GNUC__ == (major) && (__GNUC_MINOR__ > (minor))) ||  \
         (__GNUC__ == (major) && (__GNUC_MINOR__ == (minor)) &&                          \
          (__GNUC_PATCHLEVEL__ >= (patch))))
#else
#    define MINIMUM_GNU_VERSION(major, minor, patch) (0)
#endif

#if defined(__GNUC__) || defined(__SUNPRO_C)
#    define GNU_ATTRIBUTE(x) __attribute__((x))
#else
#    define GNU_ATTRIBUTE(x)
#endif

/* Clang, GCC 10.0+, and some compatible compilers (Intel is one exception).
 * We exclude SDCC here because it recognizes __has_attribute but 4.3+ chokes on it.
 * For details, see the bug ticket: <https://sourceforge.net/p/sdcc/bugs/3715/> */
#if defined(__has_attribute) && !defined(__SDCC)
#    if __has_attribute(visibility)
#        define MDL_API GNU_ATTRIBUTE(visibility("default"))
#        define MDL_INTERNAL GNU_ATTRIBUTE(visibility("hidden"))
#    endif

#    if __has_attribute(access)
#        ifndef MDL_ANNOTN__ACCESS
#            define MDL_ANNOTN__ACCESS(m, i) GNU_ATTRIBUTE(access(m, i))
#        endif
#        ifndef MDL_ANNOTN__ACCESS_SIZED
#            define MDL_ANNOTN__ACCESS_SIZED(m, i, s) GNU_ATTRIBUTE(access(m, i, s))
#        endif
#    endif

#    if !defined(MDL_ANNOTN__DEPRECATED) && __has_attribute(deprecated)
#        define MDL_ANNOTN__DEPRECATED GNU_ATTRIBUTE(deprecated)
#    endif

#    if !defined(MDL_ANNOTN__NODISCARD) && __has_attribute(warn_unused_result)
#        define MDL_ANNOTN__NODISCARD GNU_ATTRIBUTE(warn_unused_result)
#    endif

#    if !defined(MDL_ANNOTN__NORETURN) && __has_attribute(noreturn)
#        define MDL_ANNOTN__NORETURN GNU_ATTRIBUTE(noreturn)
#    endif

#    if !defined(MDL_ANNOTN__REPRODUCIBLE) && __has_attribute(const)
#        define MDL_ANNOTN__REPRODUCIBLE GNU_ATTRIBUTE(const)
#    endif

#    if !defined(MDL_FALLTHROUGH_MARKER) && __has_attribute(fallthrough)
#        define MDL_FALLTHROUGH_MARKER GNU_ATTRIBUTE(fallthrough)
#    endif

#    if __has_attribute(returns_nonnull)
#        define MDL_ANNOTN__RETURNS_NONNULL GNU_ATTRIBUTE(returns_nonnull)
#    endif

#    if __has_attribute(nonnull)
#        define MDL_ANNOTN__NONNULL GNU_ATTRIBUTE(nonnull)
#        define MDL_ANNOTN__NONNULL_ARGS(...) GNU_ATTRIBUTE(nonnull(__VA_ARGS__))
#    endif

#    if __has_attribute(nonstring)
#        define MDL_ANNOTN__NONSTRING GNU_ATTRIBUTE(nonstring)
#    endif

#    if __has_attribute(malloc) && !defined(__clang__)
#        define MDL_ANNOTN__MALLOC(dealloc, arg_i) GNU_ATTRIBUTE(malloc(dealloc, arg_i))
#    elif __has_attribute(malloc) && defined(__clang__)
#        define MDL_ANNOTN__MALLOC(dealloc, arg_i) GNU_ATTRIBUTE(malloc)
#    endif
#elif defined(__GNUC__)
/* GCC versions that don't have __has_attribute(), non-Windows ICC, TCC... */
#    define MDL_API GNU_ATTRIBUTE(visibility("default"))
#    define MDL_INTERNAL GNU_ATTRIBUTE(visibility("hidden"))
#    define MDL_ANNOTN__RETURNS_NONNULL GNU_ATTRIBUTE(returns_nonnull)
#    define MDL_ANNOTN__NONNULL GNU_ATTRIBUTE(nonnull)
#    define MDL_ANNOTN__NONNULL_ARGS(...) GNU_ATTRIBUTE(nonnull(__VA_ARGS__))

#    if !defined(MDL_ANNOTN__ACCESS) && MINIMUM_GNU_VERSION(10, 5, 0)
#        define MDL_ANNOTN__ACCESS(m, i) GNU_ATTRIBUTE(access(m, i))
#    endif

#    if !defined(MDL_ANNOTN__ACCESS_SIZED) && MINIMUM_GNU_VERSION(10, 5, 0)
#        define MDL_ANNOTN__ACCESS_SIZED(m, i, s) GNU_ATTRIBUTE(access(m, i, s))
#    endif

#    if !defined(MDL_ANNOTN__DEPRECATED)
#        define MDL_ANNOTN__DEPRECATED GNU_ATTRIBUTE(deprecated)
#    endif

#    if !defined(MDL_ANNOTN__NODISCARD)
#        define MDL_ANNOTN__NODISCARD GNU_ATTRIBUTE(warn_unused_result)
#    endif

#    if !defined(MDL_ANNOTN__NORETURN)
#        define MDL_ANNOTN__NORETURN GNU_ATTRIBUTE(noreturn)
#    endif

#    if !defined(MDL_ANNOTN__REPRODUCIBLE)
#        define MDL_ANNOTN__REPRODUCIBLE GNU_ATTRIBUTE(const)
#    endif

#    if !defined(MDL_FALLTHROUGH_MARKER)
#        define MDL_FALLTHROUGH_MARKER GNU_ATTRIBUTE(fallthrough)
#    endif

/* GCC 8+ supports the `nonstring` attribute but Clang doesn't. */
#    if !defined(__clang__) && MINIMUM_GNU_VERSION(8, 0, 0)
#        define MDL_ANNOTN__NONSTRING GNU_ATTRIBUTE(nonstring)
#    endif

/* Clang's implementation of `malloc` doesn't support arguments. */
#    if !defined(__clang__) && MINIMUM_GNU_VERSION(11, 1, 0)
#        define MDL_ANNOTN__MALLOC(dealloc, arg_i) GNU_ATTRIBUTE(malloc(dealloc, arg_i))
#    elif defined(__clang__) || MINIMUM_GNU_VERSION(4, 0, 0)
#        define MDL_ANNOTN__MALLOC(dealloc, arg_i) GNU_ATTRIBUTE(malloc)
#    endif
#endif

/* Visual Studio */
#ifdef _MSC_VER
#    ifndef MDL_API
#        define MDL_API __dllspec(export)
#    endif /* MDL_API */

#    if (_MSC_VER >= 1700) && !defined(MDL_ANNOTN__NODISCARD)
#        define MDL_ANNOTN__NODISCARD _Must_inspect_result_
#    endif

#    ifndef MDL_ANNOTN__DEPRECATED
#        define MDL_ANNOTN__DEPRECATED __dllspec(deprecated)
#    endif

#    ifndef MDL_ANNOTN__NORETURN
#        define MDL_ANNOTN__NORETURN __declspec(noreturn)
#    endif
#endif

/* OpenWatcom */
#ifdef __WATCOM__
#    define MDL_API __declspec(dllexport)

#    ifndef MDL_ANNOTN__NORETURN
#        define MDL_ANNOTN__NORETURN __declspec(noreturn)
#    endif
#endif

#ifdef __TINYC__
#    if !defined(MDL_API) && (defined(_WIN16) || defined(_WIN32) || defined(_WIN64))
/* TCC on Windows */
#        define MDL_API __attribute__((dllexport))
#    endif
#endif

/* Fallbacks */

#ifndef MDL_API
#    define MDL_API
#endif

#ifndef MDL_INTERNAL
#    define MDL_INTERNAL
#endif

#ifndef MDL_ANNOTN__ACCESS
#    define MDL_ANNOTN__ACCESS(mode, argn)
#endif

#ifndef MDL_ANNOTN__ACCESS_SIZED
#    define MDL_ANNOTN__ACCESS_SIZED(mode, argn, size)
#endif

#ifndef MDL_ANNOTN__DEPRECATED
#    define MDL_ANNOTN__DEPRECATED
#endif

#ifndef MDL_ANNOTN__NODISCARD
#    define MDL_ANNOTN__NODISCARD
#endif

#ifndef MDL_ANNOTN__NORETURN
#    if __STDC_VERSION__ >= 201112L
#        define MDL_ANNOTN__NORETURN _Noreturn
#    else
#        define MDL_ANNOTN__NORETURN
#    endif
#endif

#ifndef MDL_ANNOTN__REPRODUCIBLE
#    define MDL_ANNOTN__REPRODUCIBLE
#endif

#ifndef MDL_FALLTHROUGH_MARKER
#    define MDL_FALLTHROUGH_MARKER
#endif

#ifndef MDL_ANNOTN__NONNULL
#    define MDL_ANNOTN__NONNULL
#endif

#ifndef MDL_ANNOTN__NONNULL_ARGS
#    define MDL_ANNOTN__NONNULL_ARGS(...)
#endif

#ifndef MDL_ANNOTN__RETURNS_NONNULL
#    define MDL_ANNOTN__RETURNS_NONNULL
#endif

#ifndef MDL_ANNOTN__NONSTRING
#    define MDL_ANNOTN__NONSTRING
#endif

#ifndef MDL_ANNOTN__MALLOC
#    define MDL_ANNOTN__MALLOC(a, b)
#endif

#if defined(__SDCC_ds390) || defined(__SDCC_ds400) || defined(__SDCC_hc08) ||            \
    defined(__SDCC_mcs51) || defined(__SDCC_mos6502) || defined(__SDCC_mos65c02) ||      \
    defined(__SDCC_pdk13) || defined(__SDCC_pdk14) || defined(__SDCC_pdk15) ||           \
    defined(__SDCC_s08)
#    define MDL_REENTRANT_MARKER __reentrant
#else
#    define MDL_REENTRANT_MARKER
#endif
#endif /* INCLUDE_METALDATA_INTERNAL_ANNOTATIONS_H_ */
