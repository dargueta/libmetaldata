#include "metaldata/writer.h"
#include "metaldata/metaldata.h"

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

MDLWriter *mdl_writer_newfrombuffer(MDLState *restrict ds, void *restrict buffer,
                                    size_t size)
{
    MDLWriter *writer = mdl_writer_new(ds, memory_putc, NULL, NULL);
    if (writer == NULL)
        return NULL;

    mdl_writer_initfrombuffer(ds, writer, buffer, size);
    return writer;
}

void mdl_writer_init(MDLState *restrict ds, MDLWriter *restrict writer,
                     mdl_writer_putc_fptr putc_ptr, mdl_writer_close_fptr close_ptr,
                     void *udata)
{
    writer->ds = ds;
    writer->putc_ptr = putc_ptr;
    writer->close_ptr = close_ptr;
    writer->udata = udata;
    writer->output_buffer = NULL;
    writer->output_size = 0;
    writer->buffer_position = 0;
    writer->was_allocated = false;
}

void mdl_writer_initfrombuffer(MDLState *restrict ds, MDLWriter *restrict writer,
                               void *restrict buffer, size_t size)
{
    mdl_writer_init(ds, writer, memory_putc, NULL, NULL);
    writer->output_buffer = buffer;
    writer->output_size = size;
}

void *mdl_writer_getudata(const MDLWriter *writer)
{
    return writer->udata;
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

size_t mdl_writer_write(MDLWriter *restrict writer, const void *data, size_t size)
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