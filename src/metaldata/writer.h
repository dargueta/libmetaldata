/**
 * An unbuffered output stream abstraction.
 *
 * @file writer.h
 */

#ifndef INCLUDE_METALDATA_WRITER_H_
#define INCLUDE_METALDATA_WRITER_H_

#include "annotations.h"
#include "metaldata.h"
#include <stdbool.h>

struct MDLWriter_;
typedef struct MDLWriter_ MDLWriter;

typedef int (*mdl_writer_putc_fptr)(MDLWriter *writer, int chr) MDL_REENTRANT_MARKER;
typedef void (*mdl_writer_close_fptr)(MDLWriter *writer) MDL_REENTRANT_MARKER;

/**
 * A byte-oriented writer that abstracts away details of the source.
 *
 * @warning The struct is declared in the header only to allow users to allocate it on the
 *          stack. Do not modify it directly.
 */
struct MDLWriter_
{
    MDLState *ds;
    mdl_writer_putc_fptr putc_ptr;
    mdl_writer_close_fptr close_ptr;
    void *udata;
    char *output_buffer;
    size_t output_size;
    size_t buffer_position;
    bool was_allocated;
};

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1, 2)
MDL_ANNOTN__NODISCARD
MDLWriter *mdl_writer_new(MDLState *ds, mdl_writer_putc_fptr putc_ptr,
                          mdl_writer_close_fptr close_ptr, void *udata);

MDL_API
MDL_ANNOTN__NONNULL
MDL_ANNOTN__NODISCARD
MDL_ANNOTN__ACCESS_SIZED(read_only, 2, 3)
MDLWriter *mdl_writer_newfrombuffer(MDLState *restrict ds, void *restrict buffer,
                                    size_t size);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1, 2)
void mdl_writer_init(MDLState *restrict ds, MDLWriter *restrict writer,
                     mdl_writer_putc_fptr putc_ptr, mdl_writer_close_fptr close_ptr,
                     void *udata);

MDL_API
MDL_ANNOTN__NONNULL
MDL_ANNOTN__ACCESS_SIZED(write_only, 3, 4)
void mdl_writer_initfrombuffer(MDLState *restrict ds, MDLWriter *restrict writer,
                               void *restrict buffer, size_t size);

MDL_API
MDL_ANNOTN__NONNULL
void *mdl_writer_getudata(const MDLWriter *writer);

MDL_API
MDL_ANNOTN__NONNULL
int mdl_writer_close(MDLWriter *writer);

MDL_API
MDL_ANNOTN__NONNULL
int mdl_writer_putc(MDLWriter *writer, int chr);

MDL_API
MDL_ANNOTN__NONNULL
MDL_ANNOTN__ACCESS_SIZED(read_only, 2, 3)
size_t mdl_writer_write(MDLWriter *restrict writer, const void *data, size_t size);

#endif /* INCLUDE_METALDATA_WRITER_H_ */
