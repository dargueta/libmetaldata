# Because Make interprets # as a comment, even inside a multiline string, we use
# backticks here and replace them later.
define CONFIGURATION_HEADER_TEXT
/* AUTOGENERATED -- DO NOT MODIFY */

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

`ifndef MDL_CONFIGURATION_H_
`define MDL_CONFIGURATION_H_
`define MDL_VERSION_MAJOR $(VERSION_MAJOR)
`define MDL_VERSION_MINOR $(VERSION_MINOR)
`define MDL_VERSION_PATCH $(VERSION_PATCH)
`define MDL_MAKE_VERSION(x, y, z) (((x)*10000) + ((y)*100) + (z))
`define MDL_VERSION_FULL                                                       \
    MDL_MAKE_VERSION(MDL_VERSION_MAJOR, MDL_VERSION_MINOR, MDL_VERSION_PATCH)
`define MDL_VERSION_FULL_STRING "$(VERSION_FULL)"
`define MDL_COMPILED_AS_UNHOSTED $(UNHOSTED_IMPLEMENTATION)
`if !defined(MDL_CURRENTLY_COMPILING_LIBRARY) &&                               \
    !defined(MDL_CURRENTLY_COMPILING_TESTS) && defined(__STDC_HOSTED__)
`    if !MDL_COMPILED_AS_UNHOSTED && !__STDC_HOSTED__
`        error "Program is unhosted. Recompile MetalData for unhosted code."
`    endif
`endif /* __STDC_HOSTED__ */
`endif /* MDL_CONFIGURATION_H_ */
endef
