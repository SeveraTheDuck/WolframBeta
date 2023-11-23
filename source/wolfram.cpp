#include "wolfram.h"

static double
DoOperation (const double left_part, const double right_part,
             const operation operation_number,
             BinTree* const tree);

static double
EvaluateChecked (const BinTree_node* const node,
                       BinTree*      const tree,
                 const double              x_value);

double
Evaluate (BinTree* const tree, const double x_value)
{
    BinTree_VerifyAndDump (tree);

    return EvaluateChecked (tree->root, tree, x_value);
}

static double
EvaluateChecked (const BinTree_node* const node,
                       BinTree*      const tree,
                 const double              x_value)
{
    if (!node)
    {
        return NAN;
    }

    if (node->data->data_type == NUMBER)
    {
        return node->data->data_value.numerical_value;
    }

    if (node->data->data_type == VARIABLE)
    {
        return x_value;
    }

    double left_part  = EvaluateChecked (node->left,  tree, x_value);
    double right_part = EvaluateChecked (node->right, tree, x_value);

    if (node->data->data_type == OPERATION)
    {
        return DoOperation (left_part, right_part,
                            node->data->data_value.operation_number,
                            tree);
    }

    else
    {
        tree->errors |= WOLFRAM_WRONG_DATA_TYPE;
        return NAN;
    }
}

static double
DoOperation (const double left_part, const double right_part,
             const operation operation_number,
             BinTree* const tree)
{
    switch (operation_number)
    {
        case ADD: return left_part + right_part;

        case SUB: return left_part - right_part;

        case MUL: return left_part * right_part;

        case DIV: return left_part / right_part;

        case SIN: return sin (left_part);

        default:
        {
            fprintf (stderr, "I don't know such operation number: %d",
                     operation_number);
            tree->errors |= WOLFRAM_WRONG_OPERATION_CODE;

            return NAN;
        }
    }
}
