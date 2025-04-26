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
    MDLState *mds;
    mdl_reader_getc_fptr getc_ptr;
    mdl_reader_close_fptr close_ptr;
    void *udata;
    const char *input_buffer;
    size_t input_size;
    size_t buffer_position;
    int unget_character;
    bool was_allocated;
};

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1, 2)
MDL_ANNOTN__NODISCARD
MDLReader *mdl_reader_new(MDLState *mds, mdl_reader_getc_fptr getc_ptr,
                          mdl_reader_close_fptr close_ptr, void *udata);

MDL_API
MDL_ANNOTN__NONNULL
MDL_ANNOTN__NODISCARD
MDLReader *mdl_reader_newfrombuffer(MDLState *mds, const void *buffer, size_t size);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1, 2)
void mdl_reader_init(MDLState *mds, MDLReader *reader, mdl_reader_getc_fptr getc_ptr,
                     mdl_reader_close_fptr close_ptr, void *udata);

MDL_API
MDL_ANNOTN__NONNULL
void mdl_reader_initfrombuffer(MDLState *mds, MDLReader *reader, const void *buffer,
                               size_t size);

MDL_API
MDL_ANNOTN__NONNULL
void *mdl_reader_getudata(const MDLReader *reader);

MDL_API
MDL_ANNOTN__NONNULL
void mdl_reader_close(MDLReader *reader);

MDL_API
MDL_ANNOTN__NONNULL
int mdl_reader_getc(MDLReader *reader);

MDL_API
MDL_ANNOTN__NONNULL
int mdl_reader_peekc(MDLReader *reader);

MDL_API
MDL_ANNOTN__NONNULL
int mdl_reader_ungetc(MDLReader *reader, int chr);

MDL_API
MDL_ANNOTN__NONNULL
MDL_ANNOTN__ACCESS_SIZED(write_only, 2, 3)
size_t mdl_reader_read(MDLReader *reader, void *buf, size_t size);

#endif /* INCLUDE_METALDATA_READER_H_ */
