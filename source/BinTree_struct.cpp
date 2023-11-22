#include "../include/BinTree_struct.h"

BinTree_error_type
BinTree_Ctor       (BinTree* const tree)
{
    if (!tree)
    {
        fprintf (stderr, "Invalid pointer to tree struct: [%p]", tree);
        return BINTREE_STRUCT_NULLPTR;
    }

    tree->n_elem = 0;
    return tree->errors;
}

BinTree_node*
BinTree_CtorNode   (const BinTree_data_type   data,
                          BinTree_node* const left,
                          BinTree_node* const right,
                          BinTree_node* const parent)
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

    node->data  = BinTree_POISON;
    node->left  = nullptr;
    node->right = nullptr;

    if (node->parent)
    {
        if (node == node->parent->left)
        {
            node->parent->left  = nullptr;
        }

        else
        {
            node->parent->right = nullptr;
        }

        node->parent = nullptr;
    }

    free (node);

    return BINTREE_NO_ERRORS;
}

BinTree_node*
BinTree_InsertSorted (      BinTree* const    tree,
                      const BinTree_data_type data)
{
    if (!tree)
    {
        fprintf (stderr, "Invalid pointer to tree struct: [%p]", tree);
        return nullptr;
    }

    if (!tree->root)
    {
        fprintf (stderr, "Invalid pointer to tree root: [%p]", tree->root);
        return nullptr;
    }

    BinTree_node* node = tree->root;

    BinTree_node* new_node =
        BinTree_CtorNode (data, nullptr, nullptr, nullptr);

    while (true)
    {
        if (data <= node->data)
        {
            if (!node->left)
            {
                node->left = new_node;
                break;
            }
            node = node->left;
        }

        else
        {
            if (!node->right)
            {
                node->right = new_node;
                break;
            }
            node = node->right;
        }
    }

    new_node->parent = node;

    return new_node;
}
