#include "wolfram.h"
// #include "make_latex.h"

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
#define c(node) CopyNode(node, nullptr, d_tree)

/// @brief Copy of left operand.
#define cL c(node->left)

/// @brief Copy of right operand.
#define cR c(node->right)

/**
 * Next you will find short calls to make a node with given operation.
 * Their names say for their action.
*/

#define _CONST(value)     BinTree_CtorNode (NUMBER, value, nullptr, nullptr, nullptr, d_tree)
#define _ADD(left, right) BinTree_CtorNode (OPERATION, ADD, left, right, nullptr, d_tree)
#define _SUB(left, right) BinTree_CtorNode (OPERATION, SUB, left, right, nullptr, d_tree)
#define _MUL(left, right) BinTree_CtorNode (OPERATION, MUL, left, right, nullptr, d_tree)
#define _DIV(left, right) BinTree_CtorNode (OPERATION, DIV, left, right, nullptr, d_tree)
#define _POW(left, right) BinTree_CtorNode (OPERATION, POW, left, right, nullptr, d_tree)
#define _SIN(right)       BinTree_CtorNode (OPERATION, SIN, nullptr, right, nullptr, d_tree)
#define _COS(right)       BinTree_CtorNode (OPERATION, COS, nullptr, right, nullptr, d_tree)
#define _LN(right)        BinTree_CtorNode (OPERATION, LN,  nullptr, right, nullptr, d_tree)

static double
EvaluateChecked (const BinTree_node* const node,
                       BinTree*      const tree);

static double
DoOperation (const double       left_part,
             const double       right_part,
             const op_code_type operation_number,
             BinTree* const     tree);

static var_index_type
GetDiffVarIndex (const BinTree* const tree,
                 const char*    const diff_var_name);

static BinTree_node*
DiffNode (const BinTree_node*  const node,
                BinTree*       const d_tree,
          const var_index_type       diff_var_index);

static BinTree_node*
DiffPowNode (const BinTree_node*  const node,
                   BinTree*       const d_tree,
             const var_index_type       diff_var_index);

static bool
IsNodeVarDependant (const BinTree_node*  const node,
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

    if (IsNumber (node))
    {
        return node->data.data_value.num_value;
    }

    if (IsVariable (node))
    {                                   // which is prettier?
        return tree->var_table[node->data.data_value.var_index]
                    .var_value;
    }

    double left_part  = EvaluateChecked (node->left,  tree);
    double right_part = EvaluateChecked (node->right, tree);

    if (IsOperation (node))
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

        case LN:  return log (right_part);

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

    fprintf (tree->latex_out, "Дифференцируем выражение.\\\\");

    const var_index_type diff_var_index =
        GetDiffVarIndex (tree, diff_var_name);

    /* no variable found, everything is const */
    if (diff_var_index == VAR_INDEX_POISON)
    {
        d_tree->root = BinTree_CtorNode (NUMBER, 0, nullptr,
                                         nullptr, nullptr, d_tree);

        return d_tree;
    }

    d_tree->latex_out = tree->latex_out;
    MakeDiffVariablesArray (tree, d_tree);

    d_tree->root = DiffNode (tree->root, d_tree, diff_var_index);

    EnableDiffVariables (d_tree->root, d_tree);

    SetParents (nullptr, d_tree->root);

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
            return i;
    }

    // fprintf (stderr, "Warning! Differentiating by unknown variable.\n"
    //                  "All expressions will be considered as const.\n");

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

    if (IsNumber (node))
    {
        return _CONST (0);
    }

    else if (IsVariable (node))
    {
        return DiffVariable (node, d_tree, diff_var_index);
    }

    else if (IsOperation (node))
    {
        BinTree_node* diff_node = nullptr;

        /**
         * Macro DSL used. Check the definitions above file.
         */
        switch (node->data.data_value.op_code)
        {
            case ADD:
                diff_node = _ADD (dL, dR);
                break;

            case SUB:
                diff_node = _SUB (dL, dR);
                break;

            case MUL:
                diff_node = _ADD (_MUL (dL, cR),
                                  _MUL (cL, dR));
                break;

            case DIV:
                diff_node = _DIV (_SUB (_MUL (dL, cR),
                                   _MUL (cL, dR)),
                                   _MUL (cR, cR));
                break;

            case POW:
                diff_node = DiffPowNode (node, d_tree, diff_var_index);
                break;

            case SIN:
                diff_node = _MUL (dR, _COS (cR));
                break;

            case COS:
                diff_node = _MUL (_MUL (dR, _SIN (cR)), _CONST (-1));
                break;

            case LN:
                diff_node = _DIV (dR, cR);
                break;

            default:
                diff_node = nullptr;
                break;
        }

        if (diff_node)
        {
            PrintNodeToLatex (diff_node, d_tree, START_PRIORITY);
        }

        return diff_node;
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
        return _CONST (1);
    }

    else
    {
        return _CONST (0);
    }
}

static BinTree_node*
DiffPowNode (const BinTree_node*  const node,
                   BinTree*       const d_tree,
             const var_index_type       diff_var_index)
{
    assert (node);
    assert (d_tree);

    bool is_left_dependant  =
        IsNodeVarDependant (node->left,  diff_var_index);
    bool is_right_dependant =
        IsNodeVarDependant (node->right, diff_var_index);

    if (is_left_dependant & is_right_dependant)
    {
        BinTree_node* old_node =
            _POW (_CONST (EULER_CONSTANT), _MUL (cR, _LN (cL)));

        BinTree_node* diff_node = d (old_node);

        BinTree_DestroySubtree (old_node, d_tree);

        return diff_node;
    }

    else if (is_left_dependant)
    {
        return _MUL (cR, _MUL (dL, _POW (cL, _CONST (node->right->data.
                                             data_value.num_value - 1))));
    }

    else if (is_right_dependant)
    {
        return _MUL (dR, _MUL (_POW (cL, cR), _LN (cL)));
    }

    else
    {
        return _CONST (0);
    }
}

static bool
IsNodeVarDependant (const BinTree_node* const node,
                    const var_index_type      diff_var_index)
{
    assert (node);

    if (node->data.data_type == VARIABLE)
    {
        if (node->data.data_value.var_index == diff_var_index)
            return true;
    }

    bool left_dependence  = false;
    bool right_dependence = false;

    if (node->left)
    {
        left_dependence =
            IsNodeVarDependant (node->left, diff_var_index);

        if (left_dependence) return true;
    }

    if (node->right)
    {
        right_dependence =
            IsNodeVarDependant (node->right, diff_var_index);

        if (right_dependence) return true;
    }

    return false;
}

static void
MakeDiffVariablesArray (const BinTree* const tree,
                              BinTree* const d_tree)
{
    assert (tree);
    assert (d_tree);

    d_tree->var_number = tree->var_number;
    if (d_tree->var_number > d_tree->var_table_capacity)
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
