/**
 * A buffered input stream.
 *
 * @file reader.h
 */

#ifndef INCLUDE_METALDATA_READER_H_
#define INCLUDE_METALDATA_READER_H_

#include "annotations.h"
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
    MDLState *ds;
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
MDLReader *mdl_reader_new(MDLState *ds, mdl_reader_getc_fptr getc_ptr,
                          mdl_reader_close_fptr close_ptr, void *udata);

MDL_API
MDL_ANNOTN__NONNULL
MDL_ANNOTN__NODISCARD
MDLReader *mdl_reader_newfrombuffer(MDLState *MDL_ANNOTN__RESTRICT ds,
                                    const void *MDL_ANNOTN__RESTRICT buffer, size_t size);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1, 2)
void mdl_reader_init(MDLState *MDL_ANNOTN__RESTRICT ds,
                     MDLReader *MDL_ANNOTN__RESTRICT reader,
                     mdl_reader_getc_fptr getc_ptr, mdl_reader_close_fptr close_ptr,
                     void *udata);

MDL_API
MDL_ANNOTN__NONNULL
void mdl_reader_initfrombuffer(MDLState *MDL_ANNOTN__RESTRICT ds,
                               MDLReader *MDL_ANNOTN__RESTRICT reader,
                               const void *MDL_ANNOTN__RESTRICT buffer, size_t size);

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
int mdl_reader_ungetc(MDLReader *reader, int c);

MDL_API
MDL_ANNOTN__NONNULL
MDL_ANNOTN__ACCESS_SIZED(write_only, 2, 3)
size_t mdl_reader_read(MDLReader *MDL_ANNOTN__RESTRICT reader, void *buf, size_t size);

#endif /* INCLUDE_METALDATA_READER_H_ */
