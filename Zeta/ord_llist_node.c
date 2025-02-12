#include "ord_llist_node.h"

#pragma push_macro("LListNode")
#pragma push_macro("LColor")
#pragma push_macro("RColor")

#define LListNode OrdLListNode
#define LColor 0
#define RColor 0

#include "llist_node_temp.c"
#include "llist_temp.c"

#pragma pop_macro("LListNode")
#pragma pop_macro("LColor")
#pragma pop_macro("RColor")
