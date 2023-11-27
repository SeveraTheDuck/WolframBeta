#include "wolfram.h"

static double
DoOperation (const double       left_part,
             const double       right_part,
             const op_code_type operation_number,
             BinTree* const     tree);

static double
EvaluateChecked (const BinTree_node* const node,
                       BinTree*      const tree);

double
Evaluate (BinTree* const tree)
{
    BinTree_VerifyAndDump (tree);

    FillVariables (tree);

    return EvaluateChecked (tree->root, tree);
}

void
FillVariables (BinTree* const tree)
{
    if (!tree)
    {
        fprintf (stderr, "Invalid tree struct pointer\n");
        return;
    }

    puts ("Please, give values to variables:");

    for (var_index_type var_index = 0;
                        var_index < tree->var_number;
                        var_index++)
    {
        printf  ("%s = ", tree->var_table[var_index].var_name);
        scanf   ("%lg",  &tree->var_table[var_index].var_value);
    }

    putchar ('\n');
}

static double
EvaluateChecked (const BinTree_node* const node,
                       BinTree*      const tree)
{
    if (!node)
    {
        return NAN;
    }

    if (node->data.data_type == NUMBER)
    {
        return node->data.data_value.num_value;
    }

    if (node->data.data_type == VARIABLE)
    {
        return tree->var_table[node->data.data_value.var_index]
                    .var_value;
    }

    double left_part  = EvaluateChecked (node->left,  tree);
    double right_part = EvaluateChecked (node->right, tree);

    if (node->data.data_type == OPERATION)
    {
        return DoOperation (left_part, right_part,
                            node->data.data_value.op_code,
                            tree);
    }

    else
    {
        tree->errors |= WOLFRAM_WRONG_DATA_TYPE;
        return NAN;
    }
}

static double
DoOperation (const double       left_part,
             const double       right_part,
             const op_code_type operation_number,
             BinTree* const     tree)
{
    switch (operation_number)
    {
        case ADD: return left_part + right_part;

        case SUB: return left_part - right_part;

        case MUL: return left_part * right_part;

        case DIV: return left_part / right_part;

        case POW: return pow (left_part, right_part);

        case SIN: return sin (right_part);

        default:
        {
            fprintf (stderr, "I don't know such operation number: %d",
                     operation_number);
            tree->errors |= WOLFRAM_WRONG_OPERATION_CODE;

            return NAN;
        }
    }
}
