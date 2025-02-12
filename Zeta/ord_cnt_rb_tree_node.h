#pragma once

#pragma push_macro("TreeNode")
#pragma push_macro("PColor")
#pragma push_macro("LColor")
#pragma push_macro("RColor")
#pragma push_macro("AccSize")

#define TreeNode OrdCntRBTreeNode
#define PColor 1
#define LColor 0
#define RColor 0
#define AccSize 1

#include "bin_tree_node_temp.h"

//

#include "bin_tree_temp.h"

//

#include "rbtree_temp.h"

#pragma pop_macro("TreeNode")
#pragma pop_macro("PColor")
#pragma pop_macro("LColor")
#pragma pop_macro("RColor")
#pragma pop_macro("AccSize")
