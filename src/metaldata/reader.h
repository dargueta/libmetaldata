// SPDX-License-Identifier: MPL-2.0+
// Copyright (C) 2020-2025  Diego Argueta
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

/**
 * A buffered input stream.
 *
 * @file reader.h
 */

#ifndef INCLUDE_METALDATA_READER_H_
#define INCLUDE_METALDATA_READER_H_

#include "internal/annotations.h"
#include "metaldata.h"
#include <stdbool.h>

struct MDLReader_;
typedef struct MDLReader_ MDLReader;

typedef int (*mdl_reader_getc_fptr)(MDLReader *reader, void *udata) MDL_REENTRANT_MARKER;
typedef int (*mdl_reader_close_fptr)(MDLReader *reader, void *udata) MDL_REENTRANT_MARKER;

/**
 * A byte-oriented reader that abstracts away details of the source.
 *
 * @warning The struct is declared in the header only to allow users to allocate it on the
 *          stack. Do not modify it directly.
 */
struct MDLReader_
{
    MDLState *mds; ///< The MetalData state.

    /**
     * A pointer to a function that reads one character from the input.
     *
     * When the input has been exhausted, the function returns a negative value.
     */
    mdl_reader_getc_fptr getc_ptr;
    mdl_reader_close_fptr close_ptr;
    void *udata;
    const char *input_buffer;
    size_t input_size;
    size_t buffer_position;
    int unget_character;
    bool was_allocated;
};

/**
 * Allocate and initialize a @ref MDLReader with a given reading function.
 *
 * @param mds  The MetalData state.
 * @param getc_ptr
 *      A pointer to a function that reads one character from the input. When the input
 *      has been exhausted, the function returns a negative value, and will continue to do
 *      so on every subsequent call. The value doesn't matter so long as it's negative,
 *      and may change between calls. It is not an error to continue calling this after
 *      the end of input has been reached.
 * @param close_ptr
 *      Optional. A function that closes the input stream when called.
 * @param udata
 *      Optional. An additional argument to pass to @a getc_ptr and @a close_ptr when
 *      they're called. MetalData does not inspect the value.
 *
 * @return A new MDLReader ready for use. It must be deallocated using
 *         @ref mdl_reader_close.
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1, 2)
MDL_ANNOTN__NODISCARD
MDLReader *mdl_reader_new(MDLState *mds, mdl_reader_getc_fptr getc_ptr,
                          mdl_reader_close_fptr close_ptr, void *udata);

/**
 * Allocate and initialize a @ref MDLReader that reads from a fixed-size memory buffer.
 */
MDL_API
MDL_ANNOTN__NONNULL
MDL_ANNOTN__NODISCARD
MDLReader *mdl_reader_newfrombuffer(MDLState *mds, const void *buffer, size_t size);

/**
 * Initialize an allocated @ref MDLReader.
 *
 * Arguments have the same meanings as @ref mdl_reader_new.
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1, 2)
void mdl_reader_init(MDLState *mds, MDLReader *reader, mdl_reader_getc_fptr getc_ptr,
                     mdl_reader_close_fptr close_ptr, void *udata);

/**
 * Initialize an allocated @ref MDLReader to read from a fixed-size memory buffer.
 *
 * Arguments have the same meanings as @ref mdl_reader_newfrombuffer.
 */

MDL_API
MDL_ANNOTN__NONNULL
void mdl_reader_initfrombuffer(MDLState *mds, MDLReader *reader, const void *buffer,
                               size_t size);

/**
 * Get the value of @a udata passed when the reader was created.
 */
MDL_API
MDL_ANNOTN__NONNULL
void *mdl_reader_getudata(const MDLReader *reader);

/**
 * Close the reader and free its resources.
 *
 * Do not use @a reader after this function has been called.
 *
 * If @a reader was created with @ref mdl_reader_new or @ref mdl_reader_newfrombuffer,
 * this will also deallocate its memory.
 */
MDL_API
MDL_ANNOTN__NONNULL
void mdl_reader_close(MDLReader *reader);

/**
 * Read the next character from the stream.
 *
 * If there is no more input, the return value will be negative, and will remain negative
 * on every subsequent call. (There are no guarantees on the value, nor if consecutive
 * calls will return the same negative value.)
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_reader_getc(MDLReader *reader);

/**
 * Look at the next character in the input stream without consuming it.
 */
MDL_API
MDL_ANNOTN__NONNULL
int mdl_reader_peekc(MDLReader *reader);

MDL_API
MDL_ANNOTN__NONNULL
int mdl_reader_ungetc(MDLReader *reader, int chr);

/**
 * Read at most @a size bytes into the memory pointed to by @a buf.
 *
 * @param reader    The input stream.
 * @param[out] buf  A pointer to the memory to write the data into.
 * @param size      The maximum number of bytes to read.
 *
 * @return The number of bytes read.
 *
 * @note If @a reader reads from a buffer and @a buf overlaps with it, the behavior is
 * undefined.
 */
MDL_API
MDL_ANNOTN__NONNULL
MDL_ANNOTN__ACCESS_SIZED(write_only, 2, 3)
size_t mdl_reader_read(MDLReader *reader, void *buf, size_t size);

#endif /* INCLUDE_METALDATA_READER_H_ */
