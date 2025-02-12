#pragma once

#pragma push_macro("LListNode")
#pragma push_macro("LColor")
#pragma push_macro("RColor")

#define LListNode OrdLListNode
#define LColor 1
#define RColor 1

#include "llist_node_temp.h"

//

#include "llist_temp.h"

#pragma pop_macro("LListNode")
#pragma pop_macro("LColor")
#pragma pop_macro("RColor")
