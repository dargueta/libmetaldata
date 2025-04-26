// SPDX-License-Identifier: MPL-2.0+
// Copyright (C) 2020-2025  Diego Argueta
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

/**
 * Internal functions used by @ref MDLMemBlkList that are not part of the public API.
 *
 * @file memblklist.h
 */

#ifndef INCLUDE_METALDATA_INTERNAL_LLIST_H_
#define INCLUDE_METALDATA_INTERNAL_LLIST_H_

#include "../memblklist.h"
#include "annotations.h"

MDL_ANNOTN__NONNULL
MDLMemBlkListNode *mdl_memblklist_findnode(const MDLMemBlkList *list, const void *value,
                                           mdl_comparator_fptr cmp);

MDL_ANNOTN__NONNULL
void mdl_memblklist_movenodeafter(MDLMemBlkListNode *new_node,
                                  MDLMemBlkListNode *prev_node);

MDL_ANNOTN__NONNULL
MDL_ANNOTN__NODISCARD
MDLMemBlkListNode *mdl_memblklist_appendnewnode(MDLMemBlkList *list);

#endif
