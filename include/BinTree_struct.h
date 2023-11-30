#ifndef BINTREE_STRUCT
#define BINTREE_STRUCT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "BinTree_config.h"

#define BINTREE_CTOR_RECIVE_INFO const char*  const init_name,  \
                                 const size_t       init_line,  \
                                 const char*  const init_file,  \
                                 const char*  const init_func

#define BINTREE_CTOR_GET_INFO __LINE__, __FILE__, __func__

/*
 * This is a short define to call BinTree_Ctor with additional
 * information provided by macro.
 */
#define BINTREE_CTOR(tree)                                      \
        BinTree_Ctor ((tree), #tree, BINTREE_CTOR_GET_INFO)

/*
 * This define is for style purposes, because BinTree_Ctor is
 * an uppercased macro. It also gives an opportunity to call
 * for Dtor without aditional tree->root parameter.
 */
#define BINTREE_DTOR(tree)                                      \
        BinTree_DestroyVarTable ((tree));                       \
        BinTree_DestroySubtree  ((tree)->root, (tree));

#define BinTree_VerifyAndDump(tree)                             \
    if (!tree)                                                  \
    {                                                           \
        fprintf (stderr, "Invalid pointer to tree, aborting");  \
        exit (BINTREE_STRUCT_NULLPTR);                          \
    }                                                           \
                                                                \
    if (BinTree_Verify (tree))                                  \
    {                                                           \
        /* BinTree_Dump (tree); */                              \
        fprintf (stderr, "ERROR %llu", (tree)->errors);         \
        exit ((int32_t) (tree)->errors);                        \
    }

typedef uint64_t BinTree_error_type;

enum BinTree_errors
{
    NO_ERRORS = 0,

    BINTREE_STRUCT_NULLPTR    = 1 << 1,
    BINTREE_ROOT_NULLPTR      = 1 << 2,
    BINTREE_NODE_NULLPTR      = 1 << 3,
    BINTREE_VAR_TABLE_NULLPTR = 1 << 4,
    BINTREE_CYCLE_FOUND       = 1 << 5,

    WOLFRAM_WRONG_OPERATION_CODE            = 1 <<  6,
    WOLFRAM_WRONG_DATA_TYPE                 = 1 <<  7,
    WOLFRAM_BINARY_OPERATION_WRONG_CHILDREN = 1 <<  8,
    WOLFRAM_UNARY_OPERATION_WRONG_CHILDREN  = 1 <<  9,
    WOLFRAM_NUMBER_WRONG_CHILDREN           = 1 << 10,
    WOLFRAM_VARIABLE_WRONG_CHILDREN         = 1 << 11,

    NUMBER_OF_POSSIBLE_ERRORS = 11
};

/// @brief Used for derivatives, if several variables disapear during differentiation.
enum variable_status
{
    DISABLED = 0,
    ENABLED
};

enum is_simplified
{
    NOT_SIMPLIFIED = 0,
    SIMPLIFIED
};

struct BinTree_node
{
    BinTree_data_type data;
    BinTree_node*     left;
    BinTree_node*     right;
};

struct variable
{
    variable_status status;

    char*  var_name;
    double var_value;
};

struct BinTree
{
    BinTree_node* root;
    size_t n_elem;

    size_t      init_line;
    const char* init_name;
    const char* init_file;
    const char* init_func;

    variable*      var_table;
    var_index_type var_number;
    var_index_type var_table_capacity;

    is_simplified simplify_status;

    BinTree_error_type errors;
};

BinTree_error_type
BinTree_Ctor       (BinTree* const tree,
                    BINTREE_CTOR_RECIVE_INFO);

BinTree_node*
BinTree_CtorNode   (const data_type data_type,
                    const double    data_value,
                    BinTree_node* const left,
                    BinTree_node* const right,
                    BinTree*      const tree);

/*
 * This struct creates a node by data.
 * It doesn't require data_type from user, as BinTree_CtorNode does.
 */
BinTree_node*
MakeNodeByData (BinTree_node*      const node_left,
                BinTree_data_type* const node_data,
                BinTree_node*      const node_right,
                BinTree*           const tree);

BinTree_error_type
BinTree_DestroySubtree (BinTree_node* const node,
                        BinTree*      const tree);

BinTree_error_type
BinTree_DestroyVarTable (BinTree* const tree);

BinTree_error_type
BinTree_Verify (BinTree* const tree);

variable*
ReallocVarTable (BinTree* const tree);

#endif /* BINTREE_STRUCT */
