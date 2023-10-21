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
