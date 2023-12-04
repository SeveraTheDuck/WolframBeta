#include "../include/simplifier.h"

static double
SimplifyBinaryOperation (BinTree_node* const node,
                         BinTree*      const tree);

static double
SimplifyUnaryOperation (BinTree_node* const node,
                        BinTree*      const tree);

static void
UtilizeNeutralAdd (BinTree_node* const node,
                   BinTree*      const tree);

static void
UtilizeNeutralSub (BinTree_node* const node,
                   BinTree*      const tree);

static void
UtilizeNeutralMul (BinTree_node* const node,
                   BinTree*      const tree);

static void
UtilizeNeutralDiv (BinTree_node* const node,
                   BinTree*      const tree);

static void
UtilizeNeutralPow (BinTree_node* const node,
                   BinTree*      const tree);

static void
SetParentAfterUtilize (const BinTree_node* const node,
                             BinTree_node* const parent,
                             BinTree_node* const copy_node,
                             BinTree*      const tree);

BinTree*
SimplifyExpression (BinTree* const tree)
{
    BinTree_VerifyAndDump (tree);

    do
    {
        tree->simplify_status = NOT_SIMPLIFIED;
        CollapseConstants        (tree->root, tree);
        UtilizeNeutralOperations (tree->root, tree);
    }
    while (tree->simplify_status == SIMPLIFIED);

    return tree;
}

is_simplified
CollapseConstants (BinTree_node* const node,
                   BinTree*      const tree)
{
    assert (tree);
    if (!node)
    {
        fprintf (stderr, "Invalid pointer to node.\n");
        return NOT_SIMPLIFIED;
    }

    double total_value = BinTree_POISON;

    if (node->left)  CollapseConstants (node->left,  tree);
    if (node->right) CollapseConstants (node->right, tree);

    if (node->left && node->right)
    {
        total_value = SimplifyBinaryOperation (node, tree);
    }

    else if (node->right)
    {
        total_value = SimplifyUnaryOperation (node, tree);
    }

    if (fabs (total_value - BinTree_POISON) > __DBL_EPSILON__)
    {
        tree->simplify_status = SIMPLIFIED;

        node->data.data_type = NUMBER;
        node->data.data_value.num_value = total_value;

        BinTree_DestroySubtree (node->left,  tree);
        BinTree_DestroySubtree (node->right, tree);

        node->left  = nullptr;
        node->right = nullptr;

        return SIMPLIFIED;
    }

    return NOT_SIMPLIFIED;
}

static double
SimplifyBinaryOperation (BinTree_node* const node,
                         BinTree*      const tree)
{
    assert (node);
    assert (tree);

    double total_value = BinTree_POISON;

    if (IsNumber (node->left)  && IsNumber (node->right) &&
        IsOperation (node))
    {
        double left_value  = node->left ->data.data_value.num_value;
        double right_value = node->right->data.data_value.num_value;

        switch (node->data.data_value.op_code)
        {
            case ADD:
                total_value = left_value + right_value;
                break;

            case SUB:
                total_value = left_value - right_value;
                break;

            case MUL:
                total_value = left_value * right_value;
                break;

            case DIV:
                total_value = left_value / right_value;
                break;

            case POW:
                total_value = pow (left_value, right_value);
                break;

            default:
                fprintf (stderr, "Unknown operation\n");
                return BinTree_POISON;
        }
    }

    return total_value;
}

static double
SimplifyUnaryOperation (BinTree_node* const node,
                        BinTree*      const tree)
{
    assert (node);
    assert (tree);

    double total_value = BinTree_POISON;

    if (IsNumber (node->right) && IsOperation (node))
    {
        double right_value = node->right->data.data_value.num_value;

        switch (node->data.data_value.op_code)
        {
            case SIN:
                total_value = sin (right_value);
                break;

            case COS:
                total_value = cos (right_value);
                break;

            case LN:
                total_value = log (right_value);
                break;

            default:
                fprintf (stderr, "Unknown operation\n");
                return BinTree_POISON;
        }
    }

    return total_value;
}

is_simplified
UtilizeNeutralOperations (BinTree_node* const node,
                          BinTree*      const tree)
{
    assert (tree);

    if (!node)
    {
        fprintf (stderr, "Invalid pointer to node.\n");
        return NOT_SIMPLIFIED;
    }

    if (node->left)  UtilizeNeutralOperations (node->left,  tree);
    if (node->right) UtilizeNeutralOperations (node->right, tree);

    if (IsOperation (node))
    {
        switch (node->data.data_value.op_code)
        {
            case ADD:
                UtilizeNeutralAdd (node, tree);
                break;

            case SUB:
                UtilizeNeutralSub (node, tree);
                break;

            case MUL:
                UtilizeNeutralMul (node, tree);
                break;

            case DIV:
                UtilizeNeutralDiv (node, tree);
                break;

            case POW:
                UtilizeNeutralPow (node, tree);
                break;

            default:
                break;
        }
    }

    return NOT_SIMPLIFIED;
}

// KOPYPASTA AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA_______________________________________________
static void
UtilizeNeutralAdd (BinTree_node* const node,
                   BinTree*      const tree)
{
    assert (node);
    assert (tree);

    if (IsNullValue (node->left) || IsNullValue (node->right))
    {
        BinTree_node* copy_node = nullptr;
        BinTree_node* parent    = node->parent;

        if (IsNullValue (node->left))
        {
            copy_node = CopyNode (node->right, node->parent, tree);
        }

        else
        {
            copy_node = CopyNode (node->left, node->parent, tree);
        }

        BinTree_DestroySubtree (node, tree);

        SetParentAfterUtilize (node, parent, copy_node, tree);

        tree->simplify_status = SIMPLIFIED;
    }
}

static void
UtilizeNeutralSub (BinTree_node* const node,
                   BinTree*      const tree)
{
    assert (node);
    assert (tree);

    if (IsNullValue (node->right))
    {
        BinTree_node* copy_node =
            CopyNode (node->left, node->parent, tree);
        BinTree_node* parent = node->parent;

        BinTree_DestroySubtree (node, tree);

        SetParentAfterUtilize (node, parent, copy_node, tree);

        tree->simplify_status = SIMPLIFIED;
    }
}

static void
UtilizeNeutralMul (BinTree_node* const node,
                   BinTree*      const tree)
{
    assert (node);
    assert (tree);

    if (IsNullValue (node->left) || IsNullValue (node->right))
    {
        BinTree_DestroySubtree (node->left,  tree);
        BinTree_DestroySubtree (node->right, tree);

        node->left  = nullptr;
        node->right = nullptr;

        node->data.data_type = NUMBER;
        node->data.data_value.num_value = 0;

        tree->simplify_status = SIMPLIFIED;
    }

    if (IsOneValue (node->left) || IsOneValue (node->right))
    {
        BinTree_node* copy_node = nullptr;
        BinTree_node* parent = node->parent;

        if (IsOneValue (node->left))
        {
            copy_node = CopyNode (node->right, node->parent, tree);
        }

        else
        {
            copy_node = CopyNode (node->left,  node->parent, tree);
        }

        BinTree_DestroySubtree (node, tree);

        SetParentAfterUtilize (node, parent, copy_node, tree);

        tree->simplify_status = SIMPLIFIED;
    }
}

static void
UtilizeNeutralDiv (BinTree_node* const node,
                   BinTree*      const tree)
{
    assert (node);
    assert (tree);

    if (IsNullValue (node->left))
    {
        BinTree_DestroySubtree (node->left,  tree);
        BinTree_DestroySubtree (node->right, tree);

        node->left  = nullptr;
        node->right = nullptr;

        node->data.data_type = NUMBER;
        node->data.data_value.num_value = 0;

        tree->simplify_status = SIMPLIFIED;
    }

    if (IsOneValue (node->right))
    {
        BinTree_node* copy_node =
            CopyNode (node->left, node->parent, tree);
        BinTree_node* parent = node->parent;

        BinTree_DestroySubtree (node, tree);

        SetParentAfterUtilize (node, parent, copy_node, tree);

        tree->simplify_status = SIMPLIFIED;
    }
}

static void
UtilizeNeutralPow (BinTree_node* const node,
                   BinTree*      const tree)
{
    assert (node);
    assert (tree);

    if (IsNullValue (node->left) || IsNullValue (node->right) ||
        IsOneValue  (node->left))
    {
        node->data.data_type = NUMBER;

        if (IsNullValue (node->left))
        {
            node->data.data_value.num_value = 0;
        }

        else
        {
            node->data.data_value.num_value = 1;
        }

        BinTree_DestroySubtree (node->left,  tree);
        BinTree_DestroySubtree (node->right, tree);

        node->left  = nullptr;
        node->right = nullptr;

        tree->simplify_status = SIMPLIFIED;
    }

    if (IsOneValue (node->right))
    {
        BinTree_node* copy_node =
            CopyNode (node->left, node->parent, tree);
        BinTree_node* parent = node->parent;

        BinTree_DestroySubtree (node, tree);

        SetParentAfterUtilize (node, parent, copy_node, tree);

        tree->simplify_status = SIMPLIFIED;
    }
}

static void
SetParentAfterUtilize (const BinTree_node* const node,
                             BinTree_node* const parent,
                             BinTree_node* const copy_node,
                             BinTree*      const tree)
{
    assert (node);
    assert (copy_node);
    assert (tree);

    if (parent)
    {
        if (node == parent->left)
        {
            parent->left = copy_node;
        }

        else
        {
            parent->right = copy_node;
        }
    }

    else
    {
        tree->root = copy_node;
    }
}
