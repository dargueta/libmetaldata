// SPDX-License-Identifier: MPL-2.0+
// Copyright (C) 2020-2025  Diego Argueta
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

/**
 * Cross-platform error codes used by MetalData.
 *
 * @file errors.h
 */

#ifndef INCLUDE_METALDATA_ERRORS_H_
#define INCLUDE_METALDATA_ERRORS_H_

#include <stdint.h>

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

/**
 * A sentinel value of type `size_t` used as an invalid array index.
 */
#define MDL_INVALID_INDEX SIZE_MAX

#endif /* INCLUDE_METALDATA_ERRORS_H_ */
