#ifndef WOLFRAM_H
#define WOLFRAM_H

#include <math.h>
#include "BinTree_struct.h"
#include "wolfram_config.h"

double
Evaluate (BinTree* const tree);

void
FillVariables (BinTree* const tree);

BinTree*
DifferentiateExpression (      BinTree* const   tree,
                               BinTree* const d_tree,
                         const char*    const diff_var_name);

BinTree*
SimplifyExpression (BinTree* const tree);

#endif /* WOLFRAM_H */
