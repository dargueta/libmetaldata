// Copyright 2020-2024 by Diego Argueta
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

#include "metaldata/reader.h"
#include "metaldata/errors.h"
#include "metaldata/metaldata.h"

static int buffer_getc(MDLReader *reader, void *udata);

MDLReader *mdl_reader_new(MDLState *mds, mdl_reader_getc_fptr getc_ptr,
                          mdl_reader_close_fptr close_ptr, void *udata)
{
    MDLReader *reader = mdl_malloc(mds, sizeof(*reader));
    if (!reader)
        return NULL;

    mdl_reader_init(mds, reader, getc_ptr, close_ptr, udata);
    reader->was_allocated = true;
    return reader;
}

MDLReader *mdl_reader_newfrombuffer(MDLState *mds, const void *buffer, size_t size)
{
    MDLReader *reader = mdl_malloc(mds, sizeof(*reader));
    if (!reader)
        return NULL;

    mdl_reader_initfrombuffer(mds, reader, buffer, size);
    reader->was_allocated = true;
    return reader;
}

void mdl_reader_init(MDLState *mds, MDLReader *reader, mdl_reader_getc_fptr getc_ptr,
                     mdl_reader_close_fptr close_ptr, void *udata)
{
    reader->mds = mds;
    reader->getc_ptr = getc_ptr;
    reader->close_ptr = close_ptr;
    reader->udata = udata;
    reader->input_buffer = NULL;
    reader->input_size = 0;
    reader->buffer_position = 0;
    reader->unget_character = MDL_EOF;
    reader->was_allocated = false;
}

void mdl_reader_initfrombuffer(MDLState *mds, MDLReader *reader, const void *buffer,
                               size_t size)
{
    mdl_reader_init(mds, reader, buffer_getc, NULL, NULL);
    reader->input_buffer = buffer;
    reader->input_size = size;
}

void *mdl_reader_getudata(const MDLReader *reader)
{
    return reader->udata;
}

void mdl_reader_close(MDLReader *reader)
{
    if (reader->close_ptr)
        reader->close_ptr(reader, reader->udata);
    if (reader->was_allocated)
        mdl_free(reader->mds, reader, sizeof(*reader));
}

int mdl_reader_getc(MDLReader *reader)
{
    return reader->getc_ptr(reader, reader->udata);
}

int mdl_reader_peekc(MDLReader *reader)
{
    int value = mdl_reader_getc(reader);
    mdl_reader_ungetc(reader, value);
    return value;
}

int mdl_reader_ungetc(MDLReader *reader, int c)
{
    if (reader->unget_character != MDL_EOF)
        return MDL_ERROR_ALREADY_EXISTS;

    reader->unget_character = c;
    return MDL_OK;
}

size_t mdl_reader_read(MDLReader *reader, void *buf, size_t size)
{
    char *current_byte = (char *)buf;

    for (size_t i = 0; i < size; i++, current_byte++)
    {
        int value = mdl_reader_getc(reader);
        if (value == MDL_EOF)
            return i;

        *current_byte = (char)value;
    }
    return size;
}

static int buffer_getc(MDLReader *reader, void *udata)
{
    (void)udata;

    if (reader->unget_character != MDL_EOF)
    {
        int return_value = reader->unget_character;
        reader->unget_character = MDL_EOF;
        return return_value;
    }
    else if (reader->buffer_position >= reader->input_size)
        return MDL_EOF;
    return (int)reader->input_buffer[reader->buffer_position++];
}
