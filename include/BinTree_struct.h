#ifndef BIN_TREE_STRUCT
#define BIN_TREE_STRUCT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BinTree_config.h"

typedef size_t BinTree_error_type;

enum BinTree_errors
{
    BINTREE_NO_ERRORS      = 0,
    BINTREE_STRUCT_NULLPTR = 1 << 1,
    BINTREE_NODE_NULLPTR   = 1 << 2,
    BINTREE_CYCLE_FOUND    = 1 << 3
    /* something else? */
};

struct BinTree_node
{
    BinTree_data_type data;
    BinTree_node*     left;
    BinTree_node*     right;
    BinTree_node*     parent;
};

struct BinTree
{
    BinTree_node* root;
    size_t n_elem;

    BinTree_error_type errors;
};

BinTree_error_type
BinTree_Ctor       (BinTree* const tree);

BinTree_node*
BinTree_CtorNode   (const BinTree_data_type   data,
                          BinTree_node* const left,
                          BinTree_node* const right,
                          BinTree_node* const parent);

BinTree_error_type
BinTree_DestroySubtree (BinTree_node* const node);

BinTree_node*
BinTree_InsertSorted (      BinTree* const    tree,
                      const BinTree_data_type data);

#endif /* BIN_TREE_STRUCT */
