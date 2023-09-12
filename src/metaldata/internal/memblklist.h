#ifndef INCLUDE_METALDATA_INTERNAL_LLIST_H_
#define INCLUDE_METALDATA_INTERNAL_LLIST_H_

#include "../annotations.h"
#include "../memblklist.h"

MDL_ANNOTN__NONNULL_ARGS(1)
MDLMemBlkListNode *mdl_memblklist_findnode(const MDLMemBlkList *list, const void *value);

MDL_ANNOTN__NONNULL
void mdl_memblklist_movenodeafter(MDLMemBlkListNode *new_node,
                                  MDLMemBlkListNode *prev_node);

MDL_ANNOTN__NONNULL
MDL_ANNOTN__NODISCARD
MDLMemBlkListNode *mdl_memblklist_appendnewnode(MDLMemBlkList *list);

#endif
