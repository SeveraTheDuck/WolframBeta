#include "BinTree_struct.h"

BinTree_error_type
BinTree_Ctor       (BinTree* const tree)
{
    if (!tree)
    {
        fprintf (stderr, "Invalid pointer to tree struct: [%p]", tree);
        return BINTREE_STRUCT_NULLPTR;
    }

    // errors ctor
    //

    tree->n_elem = 0;
    return tree->errors;
}

BinTree_node*
BinTree_CtorNode   (BinTree_data_type* const data,
                    BinTree_node*      const left,
                    BinTree_node*      const right,
                    BinTree_node*      const parent)
{
    BinTree_node* new_node = (BinTree_node*)
           calloc (1, sizeof (BinTree_node));
    if (!new_node)
    {
        perror ("new_node calloc error");
        return nullptr;
    }

    new_node->data   = data;
    new_node->left   = left;
    new_node->right  = right;
    new_node->parent = parent;

    return new_node;
}

BinTree_error_type
BinTree_DestroySubtree (BinTree_node* const node)
{
    if (!node)
    {
        return BINTREE_NODE_NULLPTR;
    }

    BinTree_DestroySubtree (node->left);
    BinTree_DestroySubtree (node->right);

    node->data->data_type = NO_TYPE;
    node->data->data_value.numerical_value = BinTree_POISON;
    node->left  = nullptr;
    node->right = nullptr;

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
        }

        node->parent = nullptr;
    }

    free (node);

    return BINTREE_NO_ERRORS;
}
