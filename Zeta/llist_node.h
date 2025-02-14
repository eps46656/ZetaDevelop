#pragma once

#pragma push_macro("LListNode")
#pragma push_macro("RelLinkType")
#pragma push_macro("LColor")
#pragma push_macro("RColor")

// -----------------------------------------------------------------------------

#undef LListNode
#undef RelLinkType
#undef LColor
#undef RColor

#define LListNode OrdLListNode
#undef RelLinkType
#define LColor 0
#define RColor 0

#include "llist_node_temp.h"
//
#include "llist_temp.h"

// -----------------------------------------------------------------------------

#undef LListNode
#undef RelLinkType
#undef LColor
#undef RColor

#define LListNode OrdRBLListNode
#undef RelLinkType
#define LColor 1
#define RColor 0

#include "llist_node_temp.h"
//
#include "llist_temp.h"

// -----------------------------------------------------------------------------

#undef LListNode
#undef RelLinkType
#undef LColor
#undef RColor

#define LListNode Ord2RBLListNode
#undef RelLinkType
#define LColor 1
#define RColor 1

#include "llist_node_temp.h"
//
#include "llist_temp.h"

// -----------------------------------------------------------------------------

#undef LListNode
#undef RelLinkType
#undef LColor
#undef RColor

#define LListNode ShortRel2RBLListNode
#define RelLinkType int
#define LColor 1
#define RColor 1

#include "llist_node_temp.h"
//
#include "llist_temp.h"

// -----------------------------------------------------------------------------

#pragma pop_macro("LListNode")
#pragma pop_macro("RelLinkType")
#pragma pop_macro("LColor")
#pragma pop_macro("RColor")
