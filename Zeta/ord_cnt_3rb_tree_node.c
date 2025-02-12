#include "ord_cnt_3rb_tree_node.h"

#pragma push_macro("TreeNode")
#pragma push_macro("PColor")
#pragma push_macro("LColor")
#pragma push_macro("RColor")
#pragma push_macro("AccSize")

#define TreeNode OrdCnt3RBTreeNode
#define PColor 1
#define LColor 1
#define RColor 1
#define AccSize 1

#include "bin_tree_node_temp.c"
#include "bin_tree_temp.c"
#include "rbtree_temp.c"

#pragma pop_macro("TreeNode")
#pragma pop_macro("PColor")
#pragma pop_macro("LColor")
#pragma pop_macro("RColor")
#pragma pop_macro("AccSize")
