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

#if !MDL_COMPILED_AS_UNHOSTED
#    include <errno.h>
#    ifdef ENOMEM
#        define MDL_ERROR_NOMEM ENOMEM
#    else
#        define MDL_ERROR_NOMEM 6
#    endif /* ENOMEM */
#    define MDL_ERROR_OUT_OF_RANGE EDOM
#else
#    define MDL_ERROR_NOMEM 6
#    define MDL_ERROR_OUT_OF_RANGE 7
#endif /* !MDL_COMPILED_AS_UNHOSTED */

#endif /* INCLUDE_METALDATA_ERRORS_H_ */
