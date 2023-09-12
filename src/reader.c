#include "metaldata/reader.h"
#include "metaldata/errors.h"
#include "metaldata/metaldata.h"

static int buffer_getc(MDLReader *reader, void *udata);

MDLReader *mdl_reader_new(MDLState *ds, mdl_reader_getc_fptr getc_ptr, void *udata)
{
    MDLReader *reader = mdl_malloc(ds, sizeof(*reader));
    if (!reader)
        return NULL;

    mdl_reader_init(ds, reader, getc_ptr, udata);
    reader->was_allocated = true;
    return reader;
}

MDLReader *mdl_reader_newfrombuffer(MDLState *ds, const void *buffer, size_t size)
{
    MDLReader *reader = mdl_malloc(ds, sizeof(*reader));
    if (!reader)
        return NULL;

    mdl_reader_initfrombuffer(ds, reader, buffer, size);
    reader->was_allocated = true;
    return reader;
}

void mdl_reader_init(MDLState *ds, MDLReader *reader, mdl_reader_getc_fptr getc_ptr,
                     void *udata)
{
    reader->ds = ds;
    reader->getc_ptr = getc_ptr;
    reader->udata = udata;
    reader->input_buffer = NULL;
    reader->input_size = 0;
    reader->buffer_position = 0;
    reader->unget_character = MDL_EOF;
    reader->was_allocated = false;
}

void mdl_reader_initfrombuffer(MDLState *ds, MDLReader *reader, const void *buffer,
                               size_t size)
{
    mdl_reader_init(ds, reader, buffer_getc, NULL);
    reader->input_buffer = buffer;
    reader->input_size = size;
}

void *mdl_reader_getudata(const MDLReader *reader)
{
    return reader->udata;
}

int mdl_reader_close(MDLReader *reader)
{
    if (reader->was_allocated)
        mdl_free(reader->ds, reader, sizeof(*reader));
    return 0;
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
    for (size_t i = 0; i < size; i++, buf++)
    {
        int value = mdl_reader_getc(reader);
        if (value == MDL_EOF)
            return i;

        *(char *)buf = (char)value;
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
