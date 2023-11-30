#include "wolfram.h"

#undef d
#undef dL
#undef dR
#undef c
#undef cL
#undef cR
#undef _ADD
#undef _SUB
#undef _MUL
#undef _DIV
#undef _POW
#undef _SIN
#undef _COS

/**
 * This is DSL for differentiator operations and functions.
 * It is used to write more in more common mathematician way.
 */

/// @brief Laconic and canonical way of writing differential.
#define d(node) DiffNode (node, d_tree, diff_var_index)

/// @brief Differential of the left operand.
#define dL d(node->left)

/// @brief Differential of the right operand.
#define dR d(node->right)

/// @brief Short way to call a copy function.
#define c(node) MakeNodeByData((node)->left,            \
                             &((node)->data),           \
                               (node)->right, d_tree)

/// @brief Copy of left operand.
#define cL c(node->left)

/// @brief Copy of right operand.
#define cR c(node->right)

/**
 * Next you will find short calls to make a node with given operation.
 * Their names say for their action.
*/

#define _CONST(value)     BinTree_CtorNode (NUMBER, value, nullptr, nullptr, d_tree)
#define _ADD(left, right) BinTree_CtorNode (OPERATION, ADD, left, right, d_tree)
#define _SUB(left, right) BinTree_CtorNode (OPERATION, SUB, left, right, d_tree)
#define _MUL(left, right) BinTree_CtorNode (OPERATION, MUL, left, right, d_tree)
#define _DIV(left, right) BinTree_CtorNode (OPERATION, DIV, left, right, d_tree)
#define _POW(left, right) BinTree_CtorNode (OPERATION, POW, left, right, d_tree)
#define _SIN(right)       BinTree_CtorNode (OPERATION, SIN, nullptr, right, d_tree)
#define _COS(right)       BinTree_CtorNode (OPERATION, COS, nullptr, right, d_tree)

static double
DoOperation (const double       left_part,
             const double       right_part,
             const op_code_type operation_number,
             BinTree* const     tree);

static double
EvaluateChecked (const BinTree_node* const node,
                       BinTree*      const tree);

static var_index_type
GetDiffVarIndex (const BinTree* const tree,
                 const char*    const diff_var_name);

static BinTree_node*
DiffNode (const BinTree_node*  const node,
                BinTree*       const d_tree,
          const var_index_type       diff_var_index);

static BinTree_node*
DiffVariable (const BinTree_node*  const node,
                    BinTree*       const d_tree,
              const var_index_type       diff_var_index);

static void
MakeDiffVariablesArray (const BinTree* const tree,
                              BinTree* const d_tree);

static void
EnableDiffVariables (const BinTree_node* const d_node,
                           BinTree*      const d_tree);

static is_simplified
WrapConstants (BinTree_node* const node,
               BinTree*      const tree);

static double
SimplifyBinaryOperation (BinTree_node* const node,
                        BinTree*      const tree);

static double
SimplifyUnaryOperation (BinTree_node* const node,
                        BinTree*      const tree);

static inline bool
IsNumber (const data_type data_type);

static inline bool
IsOperation (const data_type data_type);

static inline bool
IsVariable (const data_type data_type);

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
    assert (tree);

    if (!node)
    {
        return NAN;
    }

    if (IsNumber (node->data.data_type))
    {
        return node->data.data_value.num_value;
    }

    if (IsVariable (node->data.data_type))
    {                                   // which is prettier?
        return tree->var_table[node->data.data_value.var_index]
                    .var_value;
    }

    double left_part  = EvaluateChecked (node->left,  tree);
    double right_part = EvaluateChecked (node->right, tree);

    if (IsOperation (node->data.data_type))
    {
        return DoOperation (left_part, right_part,
                            node->data.data_value.op_code, tree);
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
    assert (tree);

    switch (operation_number)
    {
        case ADD: return left_part + right_part;

        case SUB: return left_part - right_part;

        case MUL: return left_part * right_part;

        case DIV: return left_part / right_part;

        case POW: return pow (left_part, right_part);

        case SIN: return sin (right_part);

        case COS: return cos (right_part);

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
DifferentiateExpression (      BinTree* const   tree,
                               BinTree* const d_tree,
                         const char*    const diff_var_name)
{
    BinTree_VerifyAndDump (tree);

    /*
     * Nothing to check in d_tree except its existence,
     * so just if, not BinTree_VerifyAndDump ();
     */
    if (!d_tree)
    {
        fprintf (stderr, "Invalid d_tree struct pointer\n");
        return nullptr;
    }

    const var_index_type diff_var_index =
        GetDiffVarIndex (tree, diff_var_name);

    /* no variable found, everything is const */
    if (diff_var_index == VAR_INDEX_POISON)
    {
        d_tree->root = BinTree_CtorNode (NUMBER, 0,
                                         nullptr, nullptr, d_tree);

        return d_tree;
    }

    d_tree->root = DiffNode (tree->root, d_tree, diff_var_index);

    MakeDiffVariablesArray (tree, d_tree);

    return d_tree;
}

static var_index_type
GetDiffVarIndex (const BinTree* const tree,
                 const char*    const diff_var_name)
{
    assert (tree);

    if (!diff_var_name)
    {
        fprintf (stderr, "Warning! Differentiating by no variable.\n"
                         "All expressions will be considered as const.\n");
        return VAR_INDEX_POISON;
    }

    for (var_index_type i = 0; i < tree->var_number; ++i)
    {
        if (strcmp (diff_var_name, tree->var_table [i].var_name) == 0)
        {
            return i;
        }
    }

    fprintf (stderr, "Warning! Differentiating by unknown variable.\n"
                     "All expressions will be considered as const.\n");
    return VAR_INDEX_POISON;
}

static BinTree_node*
DiffNode (const BinTree_node*  const node,
                BinTree*       const d_tree,
          const var_index_type       diff_var_index)
{
    assert (d_tree);

    if (!node)
    {
        fprintf (stderr, "Invalid node pointer\n");
        return nullptr;
    }

    if (IsNumber (node->data.data_type))
    {
        return BinTree_CtorNode (NUMBER, 0, nullptr, nullptr, d_tree);
    }

    else if (IsVariable (node->data.data_type))
    {
        return DiffVariable (node, d_tree, diff_var_index);
    }

    else if (IsOperation (node->data.data_type))
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

            // different cases!!!
            case POW:
                return _MUL (_MUL (cR, _POW (cL,
                             _CONST (node->right->data
                                     .data_value.num_value - 1))), dL);

            case SIN:
                return _MUL (dR, _COS (cR));

            case COS:
                return _MUL (dR, _SIN (cR));

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
DiffVariable (const BinTree_node*  const node,
                    BinTree*       const d_tree,
              const var_index_type       diff_var_index)
{
    assert (node);
    assert (d_tree);

    if (node->data.data_value.var_index == diff_var_index)
    {
        return BinTree_CtorNode (NUMBER, 1, nullptr, nullptr, d_tree);
    }

    else
    {
        return BinTree_CtorNode (NUMBER, 0, nullptr, nullptr, d_tree);
    }
}

static void
MakeDiffVariablesArray (const BinTree* const tree,
                              BinTree* const d_tree)
{
    assert (tree);
    assert (d_tree);

    d_tree->var_number = tree->var_number;
    if (d_tree->var_number < d_tree->var_table_capacity)
    {
        d_tree->var_table_capacity = tree->var_table_capacity;
        ReallocVarTable (d_tree);
    }

    for (var_index_type i = 0; i < d_tree->var_number; ++i)
    {
        d_tree->var_table[i].status = DISABLED;

        d_tree->var_table[i].var_name =
            (char*) calloc (VAR_NAME_MAX_LEN, sizeof (char));
        if (!d_tree->var_table->var_name)
        {
            perror ("var_name allocation error");
            return;
        }

        strcpy (d_tree->var_table[i].var_name, tree->var_table[i].var_name);
    }

    EnableDiffVariables (d_tree->root, d_tree);
}

static void
EnableDiffVariables (const BinTree_node* const d_node,
                           BinTree*      const d_tree)
{
    assert (d_tree);
    if (!d_node) return;

    if (d_node->data.data_type == VARIABLE)
    {
        d_tree->var_table [d_node->data.data_value.var_index]
            .status = ENABLED;
    }

    EnableDiffVariables (d_node->left,  d_tree);
    EnableDiffVariables (d_node->right, d_tree);
}

BinTree*
SimplifyExpression (BinTree* const tree)
{
    if (!tree)
    {
        fprintf (stderr, "Wrong tree struct pointer.\n");
        return nullptr;
    }

    BinTree_VerifyAndDump (tree);

    tree->simplify_status = NOT_SIMPLIFIED;
    WrapConstants (tree->root, tree);

    return tree;
}

static is_simplified
WrapConstants (BinTree_node* const node,
               BinTree*      const tree)
{
    assert (tree);
    if (!node)
    {
        fprintf (stderr, "Invalid pointer to node.\n");
        return NOT_SIMPLIFIED;
    }

    double total_value = BinTree_POISON;

    if (node->left)  WrapConstants (node->left,  tree);
    if (node->right) WrapConstants (node->right, tree);

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

    if (node->left ->data.data_type == NUMBER &&
        node->right->data.data_type == NUMBER &&
        node->data.data_type        == OPERATION)
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
                return NOT_SIMPLIFIED;
        }
    }

    if (fabs (total_value - BinTree_POISON) > __DBL_EPSILON__)
    {
        tree->simplify_status = SIMPLIFIED;
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

    if (node->right->data.data_type == NUMBER &&
        node->data.data_type == OPERATION)
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

            default:
                fprintf (stderr, "Unknown operation\n");
                return NOT_SIMPLIFIED;
        }
    }

    if (fabs (total_value - BinTree_POISON) > __DBL_EPSILON__)
    {
        tree->simplify_status = SIMPLIFIED;
    }

    return total_value;
}

static inline bool
IsNumber (const data_type data_type)
{
    if (data_type == NUMBER) return true;
    return false;
}

static inline bool
IsOperation (const data_type data_type)
{
    if (data_type == OPERATION) return true;
    return false;
}

static inline bool
IsVariable (const data_type data_type)
{
    if (data_type == VARIABLE) return true;
    return false;
}

#undef d
#undef dL
#undef dR
#undef c
#undef cL
#undef cR
#undef _ADD
#undef _SUB
#undef _MUL
#undef _DIV
#undef _POW
