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

#ifndef INCLUDE_METALDATA_EXTRAS_HOSTED_ALLOCATOR_H_
#define INCLUDE_METALDATA_EXTRAS_HOSTED_ALLOCATOR_H_

#include <stddef.h>
void *mdl_default_hosted_alloc(void *ptr, size_t size, size_t type_or_old_size, void *ud);

#endif /* INCLUDE_METALDATA_EXTRAS_HOSTED_ALLOCATOR_H_ */
