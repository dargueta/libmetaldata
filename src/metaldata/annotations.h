/**
 * Cross-compiler function and type annotations for better type checking and
 * (hopefully) better performance.
 *
 * @file annotations.h
 */

#ifndef INCLUDE_METALDATA_ANNOTATIONS_H_
#define INCLUDE_METALDATA_ANNOTATIONS_H_

#ifdef __GNUC__
#    define MINIMUM_GNU_VERSION(major, minor, patch)                                     \
        ((__GNUC__ > (major)) || (__GNUC__ == (major) && (__GNUC_MINOR__ > (minor))) ||  \
         (__GNUC__ == (major) && (__GNUC_MINOR__ == (minor)) &&                          \
          (__GNUC_PATCHLEVEL__ >= (patch))))
#else
#    define MINIMUM_GNU_VERSION(major, minor, patch) (0)
#endif

/* First, try using built-in C2x standard attributes. We need to put this behind
 * a version guard because GCC's preprocessor expands these even when compiling for an
 * earlier standard. The result is a syntax error. */
#ifdef __STDC_VERSION__
#    if __STDC_VERSION__ >= 201904L
#        ifdef __has_c_attribute
#            if __has_c_attribute(nodiscard)
#                define MDL_ANNOTN__NODISCARD [[nodiscard]]
#            endif

#            if __has_c_attribute(noreturn)
#                define MDL_ANNOTN__NORETURN [[noreturn]]
#            endif

#            if __has_c_attribute(reproducible)
#                define MDL_ANNOTN__REPRODUCIBLE [[reproducible]]
#            endif

#            if __has_c_attribute(fallthrough)
#                define MDL_FALLTHROUGH_MARKER [[fallthrough]]
#            endif

#            if __has_c_attribute(deprecated)
#                define MDL_ANNOTN__DEPRECATED [[deprecated]]
#            endif
#        endif /* __has_c_attribute */
#    endif     /* C20 */

#    if __STDC_VERSION__ >= 199901L
#        define MDL_ANNOTN__RESTRICT restrict
#    endif /* C99 */
#endif     /* __STDC_VERSION__ */

#if defined(__GNUC__) || defined(__SUNPRO_C)
#    define GNU_ATTRIBUTE(x) __attribute__((x))
#else
#    define GNU_ATTRIBUTE(x)
#endif

/* Clang, GCC 10.0+, and some compatible compilers (Intel is one exception). */
#if defined(__has_attribute)
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

#    if __has_attribute(unavailable)
#        define MDL_ANNOTN__UNAVAILABLE(x) GNU_ATTRIBUTE(unavailable(x))
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

#    if !defined(MDL_ANNOTN__UNAVAILABLE) && !defined(__PCC__)
#        define MDL_ANNOTN__UNAVAILABLE(x) GNU_ATTRIBUTE(unavailable(x))
#    endif
#endif

/* Visual Studio */
#ifdef _MSC_VER
#    define MDL_API __dllspec(export)
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
#    define MDL_ANNOTN__ACCESS(mode, value)
#endif

#ifndef MDL_ANNOTN__ACCESS_SIZED
#    define MDL_ANNOTN__ACCESS_SIZED(mode, value, size)
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

#ifndef MDL_ANNOTN__RESTRICT
#    define MDL_ANNOTN__RESTRICT
#endif

#ifdef __SDCC
#    define MDL_REENTRANT_MARKER __reentrant
#else
#    define MDL_REENTRANT_MARKER
#endif

#ifndef MDL_ANNOTN__UNAVAILABLE
#    define MDL_ANNOTN__UNAVAILABLE(m)
#endif

/**
 * @def MDL_API
 *
 * A function attribute for marking MetalData's public API functions.
 */
#endif /* INCLUDE_METALDATA_ANNOTATIONS_H_ */
