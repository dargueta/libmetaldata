#ifndef INCLUDE_METALDATA_ERRORS_H_
#define INCLUDE_METALDATA_ERRORS_H_

#include "configuration.h"

#define MDL_EOF (-1)
#define MDL_OK 0
#define MDL_ERROR_EMPTY 1
#define MDL_ERROR_MODIFIED_ITERABLE 2
#define MDL_ERROR_NOT_FOUND 3
#define MDL_ERROR_ALREADY_EXISTS 4
#define MDL_ERROR_ASSERT_FAILED 5
#define MDL_ERROR_FULL 6
#define MDL_ERROR_NOMEM 7
#define MDL_ERROR_OUT_OF_RANGE 8

/**
 * A general-purpose error code when a more specific one like @ref MDL_ERROR_OUT_OF_RANGE
 * isn't available.
 */
#define MDL_ERROR_INVALID_ARGUMENT 9

/**
 * An error code indicating the attempted operation is not supported by the target.
 */
#define MDL_ERROR_NOT_SUPPORTED 10

#endif /* INCLUDE_METALDATA_ERRORS_H_ */
