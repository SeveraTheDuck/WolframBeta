#include "BinTree_struct.h"

BinTree_error_type
BinTree_Ctor       (BinTree* const tree,
                    BINTREE_CTOR_RECIVE_INFO)
{
    if (!tree)
    {
        fprintf (stderr, "Invalid pointer to tree struct: [%p]", tree);
        return BINTREE_STRUCT_NULLPTR;
    }

    tree->init_name = init_name;
    tree->init_line = init_line;
    tree->init_file = init_file;
    tree->init_func = init_func;

    tree->n_elem = 0;
    tree->errors = 0;

    return tree->errors;
}

BinTree_node*
BinTree_CtorNode   (BinTree_data_type* const data,
                    BinTree_node*      const left,
                    BinTree_node*      const right,
                    BinTree_node*      const parent,
                    BinTree*           const tree)
{
    if (!tree)
    {
        fprintf (stderr, "Invalid pointer to tree struct: [%p]", tree);
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

    new_node->data   = data;
    new_node->left   = left;
    new_node->right  = right;
    new_node->parent = parent;

    tree->n_elem++;

    return new_node;
}

BinTree_error_type
BinTree_DestroySubtree (BinTree_node* const node,
                        BinTree*      const tree)
{
    if (!tree)
    {
        fprintf (stderr, "Invalid pointer to tree struct: [%p]", tree);
        return BINTREE_STRUCT_NULLPTR;
    }

    if (!node)
    {
        return BINTREE_NODE_NULLPTR;
    }

    BinTree_DestroySubtree (node->left,  tree);
    BinTree_DestroySubtree (node->right, tree);

    node->data->data_type = NO_TYPE;
    node->data->data_value.numerical_value = BinTree_POISON;
    node->left  = nullptr;
    node->right = nullptr;

    tree->n_elem--;

    if (node->parent)
    {
        if (node == node->parent->left)
        {
            node->parent->left  = nullptr;
        }

        else if (node == node->parent->right)
        {
            node->parent->right = nullptr;
        }

        else
        {
            fprintf (stderr, "Wrong parent [%p] of node: [%p]",
                              node->parent, node);
            tree->errors |= BINTREE_WRONG_PARENT;
        }

        node->parent = nullptr;
    }

    free (node);

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
    CheckTreeCycle (tree->root, tree, &counted_n_elements);

    if (counted_n_elements != tree->n_elem && tree->n_elem > 0)
    {
        tree->errors |= BINTREE_CYCLE_FOUND;
    }

    return tree->errors;
}

BinTree_error_type
CheckTreeCycle (const BinTree_node* const node,
                      BinTree*      const tree,
                      uint32_t*     const counted_n_elements)
{
    if (!tree)
    {
        return BINTREE_STRUCT_NULLPTR;
    }

    if (!tree->root)
    {
        return BINTREE_ROOT_NULLPTR;
    }

    if (!node)
    {
        tree->errors |= BINTREE_NODE_NULLPTR;
        return tree->errors;
    }

    (*counted_n_elements)++;

    if (node->left)
    {
        CheckTreeCycle (node->left,  tree, counted_n_elements);
    }
    if (node->right)
    {
        CheckTreeCycle (node->right, tree, counted_n_elements);
    }

    if (node->data->data_type == NUMBER &&
       (node->left != nullptr || node->right != nullptr))
    {
        tree->errors |= WOLFRAM_NUMBER_WRONG_CHILDREN;
    }

    if (node->data->data_type == OPERATION &&
        node->left == nullptr)
    {
        // unary operations hardcode
        if (node->data->data_value.operation_number == SIN)
        {
            tree->errors |= WOLFRAM_UNARY_OPERATION_WRONG_CHILDREN;
        }
        else
        {
            tree->errors |= WOLFRAM_BINARY_OPERATION_WRONG_CHILDREN;
        }
    }

    if (node->data->data_type == OPERATION &&
        node->right == nullptr)
    {
        // unary operations hardcode
        if (node->data->data_value.operation_number != SIN)
        {
            tree->errors |= WOLFRAM_BINARY_OPERATION_WRONG_CHILDREN;
        }
    }

    if (*counted_n_elements != tree->n_elem)
    {
        return BINTREE_CYCLE_FOUND;
    }

    return NO_ERRORS;
}
