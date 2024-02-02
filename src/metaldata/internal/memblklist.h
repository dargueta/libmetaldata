// Copyright 2020-2024 by Diego Argueta
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
