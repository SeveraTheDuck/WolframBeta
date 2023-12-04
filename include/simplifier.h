#pragma once

#include <math.h>
#include "BinTree_struct.h"
#include "inline_func.h"

BinTree*
SimplifyExpression (BinTree* const tree);

is_simplified
CollapseConstants (BinTree_node* const node,
                   BinTree*      const tree);

is_simplified
UtilizeNeutralOperations (BinTree_node* const node,
                          BinTree*      const tree);
