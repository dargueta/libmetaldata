// SPDX-License-Identifier: MPL-2.0+
// Copyright (C) 2020-2025  Diego Argueta
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef INCLUDE_METALDATA_EXTRAS_HOSTED_ALLOCATOR_H_
#define INCLUDE_METALDATA_EXTRAS_HOSTED_ALLOCATOR_H_

#include <stddef.h>
void *mdl_default_hosted_alloc(void *ptr, size_t size, size_t type_or_old_size,
                               void *udata);

#endif /* INCLUDE_METALDATA_EXTRAS_HOSTED_ALLOCATOR_H_ */
