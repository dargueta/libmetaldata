// Copyright 2020-2023 by Diego Argueta
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "metaldata/writer.h"
#include "metaldata/metaldata.h"

/**
 * A default putc function that writes a single character to memory.
 *
 * @return 0 on success, an error code otherwise, most likely @ref MDL_ERROR_FULL.
 */
static int memory_putc(MDLWriter *writer, int chr);

MDLWriter *mdl_writer_new(MDLState *ds, mdl_writer_putc_fptr putc_ptr,
                          mdl_writer_close_fptr close_ptr, void *udata)
{
    MDLWriter *writer;
    writer = mdl_malloc(ds, sizeof(*writer));
    if (writer == NULL)
        return NULL;

    mdl_writer_init(ds, writer, putc_ptr, close_ptr, udata);
    return writer;
}

MDLWriter *mdl_writer_newwithbuffer(MDLState *ds, void *buffer, size_t size)
{
    MDLWriter *writer = mdl_writer_new(ds, memory_putc, NULL, NULL);
    if (writer == NULL)
        return NULL;

    mdl_writer_initwithbuffer(ds, writer, buffer, size);
    return writer;
}

void mdl_writer_init(MDLState *ds, MDLWriter *writer, mdl_writer_putc_fptr putc_ptr,
                     mdl_writer_close_fptr close_ptr, void *udata)
{
    if (close_ptr == NULL)
        close_ptr = mdl_writer_noopclose;

    writer->ds = ds;
    writer->putc_ptr = putc_ptr;
    writer->close_ptr = close_ptr;
    writer->udata = udata;
    writer->output_buffer = NULL;
    writer->output_size = 0;
    writer->buffer_position = 0;
    writer->was_allocated = false;
}

void mdl_writer_initwithbuffer(MDLState *ds, MDLWriter *writer, void *buffer, size_t size)
{
    mdl_writer_init(ds, writer, memory_putc, NULL, NULL);
    writer->output_buffer = buffer;
    writer->output_size = size;
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
        *p_length = writer->output_size;
    return writer->output_buffer;
}

int mdl_writer_close(MDLWriter *writer)
{
    if (writer->close_ptr)
        writer->close_ptr(writer);

    if (writer->was_allocated)
        mdl_free(writer->ds, writer, sizeof(*writer));
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
    if (writer->buffer_position == writer->output_size)
        return MDL_ERROR_FULL;

    writer->output_buffer[writer->buffer_position++] = (char)chr;
    return 0;
}

void mdl_writer_noopclose(MDLWriter *writer)
{
    (void)writer;
}
