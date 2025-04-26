// SPDX-License-Identifier: MPL-2.0+
// Copyright (C) 2020-2025  Diego Argueta
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

/**
 * Internal functions used by @ref MDLArray that are not part of the public API.
 *
 * @file array.h
 */

#ifndef INCLUDE_METALDATA_INTERNAL_ARRAY_H_
#define INCLUDE_METALDATA_INTERNAL_ARRAY_H_

#include "../array.h"
#include "annotations.h"

MDL_ANNOTN__NODISCARD
MDL_ANNOTN__NONNULL
MDLArrayBlock *mdl_array_appendnewblock(MDLArray *array);

MDL_ANNOTN__NONNULL
MDLArrayBlock *mdl_array_getpenultimateblock(const MDLArray *array);

MDL_ANNOTN__NONNULL
int mdl_array_removelastblock(MDLArray *array);

#endif /* INCLUDE_METALDATA_INTERNAL_ARRAY_H_ */
