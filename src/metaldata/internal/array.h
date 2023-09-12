#ifndef INCLUDE_METALDATA_INTERNAL_ARRAY_H_
#define INCLUDE_METALDATA_INTERNAL_ARRAY_H_

#include "../annotations.h"
#include "../array.h"

MDL_ANNOTN__NODISCARD
MDL_ANNOTN__NONNULL
MDLArrayBlock *mdl_array_appendnewblock(MDLArray *array);

MDL_ANNOTN__NONNULL
MDLArrayBlock *mdl_array_getpenultimateblock(const MDLArray *array);

MDL_ANNOTN__NONNULL
int mdl_array_removelastblock(MDLArray *array);

#endif /* INCLUDE_METALDATA_INTERNAL_ARRAY_H_ */
