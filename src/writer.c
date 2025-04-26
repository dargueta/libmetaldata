// SPDX-License-Identifier: MPL-2.0+
// Copyright (C) 2020-2025  Diego Argueta
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "metaldata/writer.h"
#include "metaldata/errors.h"
#include "metaldata/metaldata.h"

/**
 * A default putc function that writes a single character to memory.
 *
 * @return 0 on success, an error code otherwise, most likely @ref MDL_ERROR_FULL.
 */
static int memory_putc(MDLWriter *writer, int chr);

MDLWriter *mdl_writer_new(MDLState *mds, mdl_writer_putc_fptr putc_ptr,
                          mdl_writer_close_fptr close_ptr, void *udata)
{
    MDLWriter *writer;
    writer = mdl_malloc(mds, sizeof(*writer));
    if (writer == NULL)
        return NULL;

    mdl_writer_init(mds, writer, putc_ptr, close_ptr, udata);
    return writer;
}

MDLWriter *mdl_writer_newwithbuffer(MDLState *mds, void *buffer, size_t size)
{
    MDLWriter *writer = mdl_writer_new(mds, memory_putc, NULL, NULL);
    if (writer == NULL)
        return NULL;

    mdl_writer_initwithbuffer(mds, writer, buffer, size);
    return writer;
}

void mdl_writer_init(MDLState *mds, MDLWriter *writer, mdl_writer_putc_fptr putc_ptr,
                     mdl_writer_close_fptr close_ptr, void *udata)
{
    if (close_ptr == NULL)
        close_ptr = mdl_writer_noopclose;

    writer->mds = mds;
    writer->putc_ptr = putc_ptr;
    writer->close_ptr = close_ptr;
    writer->udata = udata;
    writer->output_buffer = NULL;
    writer->buffer_size = 0;
    writer->buffer_position = 0;
    writer->was_allocated = false;
}

void mdl_writer_initwithbuffer(MDLState *mds, MDLWriter *writer, void *buffer,
                               size_t size)
{
    mdl_writer_init(mds, writer, memory_putc, NULL, NULL);
    writer->output_buffer = buffer;
    writer->buffer_size = size;
}

void *mdl_writer_getudata(const MDLWriter *writer)
{
    return writer->udata;
}

void *mdl_writer_getbuffer(const MDLWriter *writer, size_t *p_length)
{
    if (writer->output_buffer == NULL)
        return NULL;

    if (p_length != NULL)
        *p_length = writer->buffer_size;
    return writer->output_buffer;
}

int mdl_writer_close(MDLWriter *writer)
{
    if (writer->close_ptr)
        writer->close_ptr(writer);

    if (writer->was_allocated)
        mdl_free(writer->mds, writer, sizeof(*writer));
    return 0;
}

int mdl_writer_putc(MDLWriter *writer, int chr)
{
    return writer->putc_ptr(writer, chr);
}

size_t mdl_writer_write(MDLWriter *writer, const void *data, size_t size)
{
    size_t n_written;
    const char *current_byte = data;

    for (n_written = 0; n_written < size; n_written++)
    {
        int result = writer->putc_ptr(writer, *current_byte);
        if (result != MDL_OK)
            return n_written;
        ++current_byte;
    }
    return size;
}

static int memory_putc(MDLWriter *writer, int chr)
{
    if (writer->buffer_position == writer->buffer_size)
        return MDL_ERROR_FULL;

    writer->output_buffer[writer->buffer_position++] = (char)chr;
    return 0;
}

void mdl_writer_noopclose(MDLWriter *writer)
{
    (void)writer;
}
