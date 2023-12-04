#pragma once

#include <math.h>
#include "BinTree_struct.h"
#include "wolfram_config.h"
#include "inline_func.h"
#include "make_latex.h"

const double EULER_CONSTANT = 2.718281828;

double
Evaluate (BinTree* const tree);

void
FillVariables (BinTree* const tree);

BinTree*
DifferentiateExpression (      BinTree* const   tree,
                               BinTree* const d_tree,
                         const char*    const diff_var_name);
