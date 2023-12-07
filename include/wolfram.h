#pragma once

#include <math.h>
#include "BinTree_struct.h"
#include "wolfram_config.h"
#include "inline_func.h"
#include "make_latex.h"

const double EULER_CONSTANT = 2.718281828;
const double GRAPH_STEP     = 1e-3;
double
Evaluate (BinTree* const tree);

void
FillVariables (BinTree* const tree);

BinTree*
DifferentiateExpression (      BinTree* const   tree,
                               BinTree* const d_tree,
                         const char*    const diff_var_name);

void
MakeFunctionGraph (      BinTree* const tree,
                   const char*    const dep_var_name,
                   const double         left_border,
                   const double         right_border);
