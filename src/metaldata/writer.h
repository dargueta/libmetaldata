/**
 * An unbuffered output stream abstraction.
 *
 * The @ref MDLWriter only supports writing individual characters and blocks of memory.
 * Since it's an output-only stream, there is no notion of offset and thus no seek/tell
 * capability. This does, however, provide a way to build these abstractions on top of it.
 *
 * @file writer.h
 */

#ifndef INCLUDE_METALDATA_WRITER_H_
#define INCLUDE_METALDATA_WRITER_H_

#include "internal/annotations.h"
#include "metaldata.h"
#include <stdbool.h>

struct MDLWriter_;
typedef struct MDLWriter_ MDLWriter;

/**
 * A pointer to a function that writes a single character to the writer's output.
 *
 * @param writer The @ref MDLWriter this function is modifying.
 * @param chr The int promotion of the character to write.
 * @return 0 on success, an MDL error otherwise, e.g. @ref MDL_ERROR_INVALID_ARGUMENT.
 */
typedef int (*mdl_writer_putc_fptr)(MDLWriter *writer, int chr) MDL_REENTRANT_MARKER;

/**
 * A pointer to a function that closes an open writer.
 *
 * @see mdl_writer_getudata
 */
typedef void (*mdl_writer_close_fptr)(MDLWriter *writer) MDL_REENTRANT_MARKER;

/**
 * A byte-oriented writer that abstracts away details of the target.
 *
 * @warning The struct is declared in the header only to allow users to allocate it on the
 *          stack. Do not modify it directly.
 */
struct MDLWriter_
{
    MDLState *mds;
    mdl_writer_putc_fptr putc_ptr;
    mdl_writer_close_fptr close_ptr;
    void *udata;
    char *output_buffer;

    /**
     * The size of the output buffer.
     *
     * If @ref output_buffer is null, this field has no meaning and should not be used.
     */
    size_t buffer_size;

    /**
     * The offset into the output buffer where the next character is to be written.
     *
     * If @ref output_buffer is null, this field has no meaning and should not be used.
     */
    size_t buffer_position;
    bool was_allocated;
};

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1, 2)
MDL_ANNOTN__NODISCARD
MDLWriter *mdl_writer_new(MDLState *mds, mdl_writer_putc_fptr putc_ptr,
                          mdl_writer_close_fptr close_ptr, void *udata);

/**
 * Allocate a new @ref MDLWriter that writes into a fixed-size buffer.
 *
 * @param mds
 * @param buffer A pointer to the buffer to write to.
 * @param size The size of the buffer to write to, in bytes.
 *
 * @return A new initialized writer.
 */
MDL_API
MDL_ANNOTN__NONNULL
MDL_ANNOTN__NODISCARD
MDLWriter *mdl_writer_newwithbuffer(MDLState *mds, void *buffer, size_t size);

MDL_API
MDL_ANNOTN__NONNULL_ARGS(1, 2)
void mdl_writer_init(MDLState *mds, MDLWriter *writer, mdl_writer_putc_fptr putc_ptr,
                     mdl_writer_close_fptr close_ptr, void *udata);

MDL_API
MDL_ANNOTN__NONNULL
MDL_ANNOTN__ACCESS_SIZED(write_only, 3, 4)
void mdl_writer_initwithbuffer(MDLState *mds, MDLWriter *writer, void *buffer,
                               size_t size);

MDL_API
MDL_ANNOTN__NONNULL
void *mdl_writer_getudata(const MDLWriter *writer);

/**
 * Get a pointer to the output buffer @e if this was initialized with one.
 *
 * If the writer was initialized to write to memory (e.g. @ref mdl_writer_newwithbuffer
 * or @ref mdl_writer_initwithbuffer), return this buffer and its size, in bytes.
 *
 * @param writer The writer to operate on.
 * @param[out] p_length
 *      An optional pointer to a size_t where the size will be stored. If this writer
 *      doesn't write to a memory buffer, @a p_length is not modified.
 * @return NULL if the writer doesn't write to memory, or a pointer to memory otherwise.
 */
MDL_API
MDL_ANNOTN__NONNULL_ARGS(1)
void *mdl_writer_getbuffer(const MDLWriter *writer, size_t *p_length);

MDL_API
MDL_ANNOTN__NONNULL
int mdl_writer_close(MDLWriter *writer);

MDL_API
MDL_ANNOTN__NONNULL
int mdl_writer_putc(MDLWriter *writer, int chr);

MDL_API
MDL_ANNOTN__NONNULL
MDL_ANNOTN__ACCESS_SIZED(read_only, 2, 3)
size_t mdl_writer_write(MDLWriter *writer, const void *data, size_t size);

/**
 * A dummy close function that does nothing.
 */
MDL_API
MDL_ANNOTN__NONNULL
void mdl_writer_noopclose(MDLWriter *writer);

#endif /* INCLUDE_METALDATA_WRITER_H_ */
