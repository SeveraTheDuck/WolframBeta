#include "wolfram.h"

static double
DoOperation (const double left_part, const double right_part,
             const operation operation_number);

double
Evaluate (const BinTree_node* const node)
{
    if (!node)
    {
        return NAN;
    }

    if (node->data->data_type == NUMBER)
    {
        return node->data->data_value.numerical_value;
    }

    double left_part  = Evaluate (node->left);
    double right_part = Evaluate (node->right);

    if (node->data->data_type == OPERATION)
    {
        return DoOperation (left_part, right_part,
                            node->data->data_value.operation_number);
    }

    else
    {
        /* errors */
        return NAN;
    }
}

static double
DoOperation (const double left_part, const double right_part,
             const operation operation_number)
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
            fprintf (stderr, "I don't know such operation number: %d, "
                             "aborting", operation_number);

            exit (WOLFRAM_WRONG_OPERATION_CODE);
        }
    }
}
