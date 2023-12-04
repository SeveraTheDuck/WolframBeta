#pragma once

#include <math.h>
#include "BinTree_struct.h"
#include "wolfram_config.h"
#include "inline_func.h"

double
Evaluate (BinTree* const tree);

void
FillVariables (BinTree* const tree);

BinTree*
DifferentiateExpression (      BinTree* const   tree,
                               BinTree* const d_tree,
                         const char*    const diff_var_name);
