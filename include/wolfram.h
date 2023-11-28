#ifndef WOLFRAM_H
#define WOLFRAM_H

#include <math.h>
#include "BinTree_struct.h"
#include "wolfram_config.h"

#define d(node) DiffNode (node, tree, d_tree, var_name)
#define dL d(node->left)
#define dR d(node->right)

#define cL MakeNodeByData (node->left->left,            \
                          &node->left->data,            \
                           node->left->right, d_tree)

#define cR MakeNodeByData (node->right->left,           \
                          &node->right->data,           \
                           node->right->right, d_tree)

#define _ADD(left, right) BinTree_CtorNode (OPERATION, ADD, left, right, d_tree)
#define _SUB(left, right) BinTree_CtorNode (OPERATION, SUB, left, right, d_tree)
#define _MUL(left, right) BinTree_CtorNode (OPERATION, MUL, left, right, d_tree)
#define _DIV(left, right) BinTree_CtorNode (OPERATION, DIV, left, right, d_tree)
#define _POW(left, right) BinTree_CtorNode (OPERATION, POW, left, right, d_tree)

double
Evaluate (BinTree* const tree);

void
FillVariables (BinTree* const tree);

BinTree*
DifferentiateTree (const BinTree* const   tree,
                         BinTree* const d_tree,
                   const char*    const var_name);

#endif /* WOLFRAM_H */
