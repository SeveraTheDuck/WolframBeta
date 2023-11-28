#include "wolfram.h"

static double
DoOperation (const double       left_part,
             const double       right_part,
             const op_code_type operation_number,
             BinTree* const     tree);

static double
EvaluateChecked (const BinTree_node* const node,
                       BinTree*      const tree);

static BinTree_node*
DiffNode (const BinTree_node* const node,
          const BinTree*      const tree,
                BinTree*      const d_tree,
          const char*         const var_name);

static BinTree_node*
DiffVariable (const BinTree_node* const node,
              const BinTree*      const tree,
                    BinTree*      const d_tree,
              const char*         const var_name);

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

    if (tree->var_number <= 0)
    {
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

BinTree*
DifferentiateTree (const BinTree* const   tree,
                         BinTree* const d_tree,
                   const char*    const var_name = nullptr)
{
    if (!tree)
    {
        fprintf (stderr, "Invalid tree struct pointer\n");
        return nullptr;
    }

    if (!d_tree)
    {
        fprintf (stderr, "Invalid d_tree struct pointer\n");
        return nullptr;
    }

    d_tree->root = DiffNode (tree->root, tree, d_tree, var_name);

    d_tree->var_number = tree->var_number;

    if (d_tree->var_table_capacity < d_tree->var_number)
    {
        d_tree->var_table_capacity = d_tree->var_number;
        ReallocVarTable (d_tree);
    }

    return d_tree;
}

static BinTree_node*
DiffNode (const BinTree_node* const node,
          const BinTree*      const tree,
                BinTree*      const d_tree,
          const char*         const var_name)
{
    if (!node)
    {
        fprintf (stderr, "Invalid node pointer\n");
        return nullptr;
    }

    assert (d_tree);

    if (node->data.data_type == NUMBER)
    {
        return BinTree_CtorNode (NUMBER, 0, nullptr, nullptr, d_tree);
    }

    else if (node->data.data_type == VARIABLE)
    {
        return DiffVariable (node, tree, d_tree, var_name);

    }

    else if (node->data.data_type == OPERATION)
    {
        /*
         * Macro DSL used. Check the definitions in wolfram.h file.
         */
        switch (node->data.data_value.op_code)
        {
            case ADD:
                return _ADD (dL, dR);
            case SUB:
                return _SUB (dL, dR);
            case MUL:
                return _ADD (_MUL (dL, cR),
                             _MUL (cL, dR));
            case DIV:
                return _DIV (_SUB (_MUL (dL, cR),
                                   _MUL (cL, dR)),
                             _MUL (cR, cR));
            // case POW:
            //     return _MUL ()
            default:
                return nullptr;
        }
    }

    else
    {
        fprintf (stderr, "Unknown type\n");
        return nullptr;
    }
}

static BinTree_node*
DiffVariable (const BinTree_node* const node,
              const BinTree*      const tree,
                    BinTree*      const d_tree,
              const char*         const var_name)
{
    assert (node);
    assert (tree);
    assert (d_tree);

    if (!var_name)
    {
        fprintf (stderr, "Warning! Differentiating by no variable. "
                            "All variable are considered as constants.\n");
        return BinTree_CtorNode (NUMBER, 0, nullptr, nullptr, d_tree);
    }

    if (strcmp ( tree->var_table
                [node->data.data_value.var_index].var_name,
                var_name) == 0)
    {
        return BinTree_CtorNode (NUMBER, 1, nullptr, nullptr, d_tree);
    }

    else
    {
        return BinTree_CtorNode (NUMBER, 0, nullptr, nullptr, d_tree);
    }
}
