#include "BinTree_struct.h"

static BinTree_error_type
BinTree_CheckCycle (const BinTree_node* const node,
                          BinTree*      const tree,
                          uint32_t*     const counted_n_elements);

static BinTree_error_type
BinTree_CheckThreads (const BinTree_node* const node,
                            BinTree*      const tree);

BinTree_error_type
BinTree_Ctor       (BinTree* const tree,
                    BINTREE_CTOR_RECIVE_INFO)
{
    if (!tree)
    {
        fprintf (stderr, "Invalid tree struct pointer");
        return BINTREE_STRUCT_NULLPTR;
    }

    tree->init_name = init_name;
    tree->init_line = init_line;
    tree->init_file = init_file;
    tree->init_func = init_func;

    tree->var_table =
        (variable*) calloc (INIT_VAR_NUMBER, sizeof (variable));
    if (!tree->var_table)
    {
        perror ("var_table allocation error");
        tree->errors |= BINTREE_VAR_TABLE_NULLPTR;
    }

    tree->n_elem     = 0;
    tree->var_number = 0;
    tree->errors     = 0;

    return tree->errors;
}

BinTree_node*
BinTree_CtorNode   (BinTree_data_type* const data,
                    BinTree_node*      const left,
                    BinTree_node*      const right,
                    BinTree*           const tree)
{
    if (!tree)
    {
        fprintf (stderr, "Invalid tree struct pointer");
        return nullptr;
    }

    BinTree_node* new_node = (BinTree_node*)
           calloc (1, sizeof (BinTree_node));

    if (!new_node)
    {
        perror ("new_node calloc error");
        tree->errors |= BINTREE_NODE_NULLPTR;

        return nullptr;
    }

    memcpy (&new_node->data, data, sizeof (BinTree_data_type));

    new_node->left  = left;
    new_node->right = right;

    tree->n_elem++;

    return new_node;
}

BinTree_error_type
BinTree_DestroySubtree (BinTree_node* const node,
                        BinTree*      const tree)
{
    if (!tree)
    {
        fprintf (stderr, "Invalid tree struct pointer");
        return BINTREE_STRUCT_NULLPTR;
    }

    if (!node)
    {
        return BINTREE_NODE_NULLPTR;
    }

    BinTree_DestroySubtree (node->left,  tree);
    BinTree_DestroySubtree (node->right, tree);

    node->data.data_type = NO_TYPE;
    node->data.data_value.num_value = BinTree_POISON;
    node->left  = nullptr;
    node->right = nullptr;

    tree->n_elem--;

    free (node);

    return NO_ERRORS;
}

BinTree_error_type
BinTree_DestroyVarTable (BinTree* const tree)
{
    if (!tree)
    {
        fprintf (stderr, "Invalid tree struct pointer");
        return BINTREE_STRUCT_NULLPTR;
    }

    for (var_index_type var_index = 0;
                        var_index < tree->var_number;
                        var_index++)
    {
        tree->var_table[var_index].var_value = BinTree_POISON;
        free (tree->var_table[var_index].var_name);
    }

    return NO_ERRORS;
}

BinTree_error_type
BinTree_Verify (BinTree* const tree)
{
    if (!tree)
    {
        return BINTREE_STRUCT_NULLPTR;
    }

    uint32_t counted_n_elements = 0;
    BinTree_CheckCycle (tree->root, tree, &counted_n_elements);

    if (counted_n_elements != tree->n_elem && tree->n_elem > 0)
    {
        tree->errors |= BINTREE_CYCLE_FOUND;
    }

    BinTree_CheckThreads (tree->root, tree);

    return tree->errors;
}

static BinTree_error_type
BinTree_CheckThreads (const BinTree_node* const node,
                            BinTree*      const tree)
{
    assert (tree);

    if (!tree->root)
    {
        return BINTREE_ROOT_NULLPTR;
    }

    if (node->data.data_type == NUMBER &&
       (node->left != nullptr || node->right != nullptr))
    {
        tree->errors |= WOLFRAM_NUMBER_WRONG_CHILDREN;
    }

    if (node->data.data_type == OPERATION &&
        node->left == nullptr)
    {
        switch (node->data.data_value.op_code)
        {
            /*
             * No breaks used in ADD, SUB and MUL because
             * the behavior is the same for all of them,
             * so switch can move through them.
             * There are few of them, so it is not slow.
             */
            case ADD:
                [[fallthrough]];
            case SUB:
                [[fallthrough]];
            case MUL:
                [[fallthrough]];
            case DIV:
                [[fallthrough]];
            case POW:
                tree->errors |= WOLFRAM_BINARY_OPERATION_WRONG_CHILDREN;
                break;
        }
    }

    if (node->data.data_type == OPERATION &&
        node->right == nullptr)
    {
        switch (node->data.data_value.op_code)
        {
            /*
             * Same as above
             */
            case ADD:
                [[fallthrough]];
            case SUB:
                [[fallthrough]];
            case MUL:
                [[fallthrough]];
            case DIV:
                [[fallthrough]];
            case POW:
                tree->errors |= WOLFRAM_BINARY_OPERATION_WRONG_CHILDREN;
                break;

            default:
                tree->errors |= WOLFRAM_UNARY_OPERATION_WRONG_CHILDREN;
                break;
        }
    }

    if (node->left)
        BinTree_CheckThreads (node->left,  tree);
    if (node->right)
        BinTree_CheckThreads (node->right, tree);

    return tree->errors;
}

static BinTree_error_type
BinTree_CheckCycle (const BinTree_node* const node,
                          BinTree*      const tree,
                          uint32_t*     const counted_n_elements)
{
    assert (tree);

    if (!node)
    {
        tree->errors |= BINTREE_NODE_NULLPTR;
        return tree->errors;
    }

    (*counted_n_elements)++;

    if (node->left)
        BinTree_CheckCycle (node->left,  tree, counted_n_elements);
    if (node->right)
        BinTree_CheckCycle (node->right, tree, counted_n_elements);

    if (*counted_n_elements != tree->n_elem)
    {
        return BINTREE_CYCLE_FOUND;
    }

    return NO_ERRORS;
}
