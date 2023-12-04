#include "read_equation.h"

// FileOpenLib remake

#define _CONST(value) BinTree_CtorNode (NUMBER, value, nullptr, nullptr, nullptr, tree)
#define _ADD(left, right) BinTree_CtorNode (OPERATION, ADD, left, right, nullptr, tree)
#define _SUB(left, right) BinTree_CtorNode (OPERATION, SUB, left, right, nullptr, tree)
#define _MUL(left, right) BinTree_CtorNode (OPERATION, MUL, left, right, nullptr, tree)
#define _DIV(left, right) BinTree_CtorNode (OPERATION, DIV, left, right, nullptr, tree)
#define _POW(left, right) BinTree_CtorNode (OPERATION, POW, left, right, nullptr, tree)
#define _SIN(right)       BinTree_CtorNode (OPERATION, SIN, nullptr, right, nullptr, tree)
#define _COS(right)       BinTree_CtorNode (OPERATION, COS, nullptr, right, nullptr, tree)
#define _LN(right)        BinTree_CtorNode (OPERATION, LN,  nullptr, right, nullptr, tree)

static BinTree_node*
GetExpression (const char*          buffer,
                     size_t*  const index,
                     BinTree* const tree);

static BinTree_node*
GetTerm (const char*          buffer,
               size_t*  const index,
               BinTree* const tree);

static BinTree_node*
GetUnary (const char*          buffer,
                size_t*  const index,
                BinTree* const tree);

static BinTree_node*
GetPrimary (const char*          buffer,
                  size_t*  const index,
                  BinTree* const tree);

static BinTree_node*
GetValue (const char*          buffer,
                size_t*  const index,
                BinTree* const tree);

static BinTree_node*
GetVariable (const char*          buffer,
                   size_t*  const index,
                   BinTree* const tree);

static BinTree_node*
GetNumber (const char*          buffer,
                 size_t*  const index,
                 BinTree* const tree);

static void
syn_assert (const bool expression);

BinTree*
ReadTree (const char*    const input_file_name,
                BinTree* const tree)
{
    if (!tree)
    {
        fprintf (stderr, "Invalid pointer to tree struct\n");
        return nullptr;
    }

    file_input input_parced = {};
    GetFileInput (input_file_name, &input_parced, NOT_PARTED);

    if (input_parced.buffer_size == 0) return nullptr;

    size_t index = 0;
    tree->root = GetExpression (input_parced.buffer, &index, tree);

    SetParents (nullptr, tree->root);

    fprintf (tree->latex_out, "$$%s$$\n", input_parced.buffer);

    FreeFileInput (&input_parced);

    return tree;
}

static BinTree_node*
GetExpression (const char*          buffer,
                     size_t*  const index,
                     BinTree* const tree)
{
    assert (buffer);
    assert (index);
    assert (tree);

    BinTree_node* left_value  = GetTerm (buffer, index, tree);
    BinTree_node* right_value = nullptr;
    BinTree_node* new_node    = nullptr;

    op_code_type operation = 0;

    while (buffer [*index] == operations_array [ADD] [0] ||
           buffer [*index] == operations_array [SUB] [0])
    {
        if (strncmp (buffer + *index, operations_array [ADD],
                     strlen (operations_array [ADD])) == 0)
        {
            operation = ADD;
        }
        else
        {
            operation = SUB;
        }

        (*index)++;

        right_value = GetTerm (buffer, index, tree);

        switch (operation)
        {
            case ADD:
                new_node = _ADD (left_value, right_value);
                break;

            case SUB:
                new_node = _SUB (left_value, right_value);
                break;

            default:
                fprintf (stderr, "Not add or sub op_code\n");
                abort ();
                break;
        }

        left_value = new_node;
    }

    return left_value;
}

static BinTree_node*
GetTerm (const char*          buffer,
               size_t*  const index,
               BinTree* const tree)
{
    assert (buffer);
    assert (index);
    assert (tree);

    BinTree_node* left_value  = GetUnary (buffer, index, tree);
    BinTree_node* right_value = nullptr;
    BinTree_node* new_node    = nullptr;

    op_code_type operation = 0;

    while (buffer [*index] == operations_array [MUL] [0] ||
           buffer [*index] == operations_array [DIV] [0] ||
           buffer [*index] == operations_array [POW] [0])
    {
        if (strncmp (buffer + *index, operations_array [MUL],
                     strlen (operations_array [MUL])) == 0)
        {
            operation = MUL;
        }

        else if (strncmp (buffer + *index, operations_array [DIV],
                          strlen (operations_array [DIV])) == 0)
        {
            operation = DIV;
        }

        else if (strncmp (buffer + *index, operations_array [POW],
                          strlen (operations_array [POW])) == 0)
        {
            operation = POW;
        }

        else
        {
            syn_assert (0);
        }

        (*index)++;

        right_value = GetUnary (buffer, index, tree);

        switch (operation)
        {
            case MUL:
                new_node = _MUL (left_value, right_value);
                break;

            case DIV:
                new_node = _DIV (left_value, right_value);
                break;

            case POW:
                new_node = _POW (left_value, right_value);
                break;

            default:
                fprintf (stderr, "Not mul, div or pow op_code\n");
                abort ();
                break;
        }

        left_value = new_node;
    }

    return left_value;
}

static BinTree_node*
GetUnary (const char*          buffer,
                size_t*  const index,
                BinTree* const tree)
{
    assert (buffer);
    assert (index);
    assert (tree);

    for (size_t op_code = NUM_OF_BIN_OP;
                op_code < NUM_OF_OP;
                op_code++)
    {
        if (strncasecmp (buffer + *index, operations_array [op_code],
                                  strlen (operations_array [op_code])) == 0)
        {
            *index += strlen (operations_array [op_code]);

            BinTree_node* right_value =
                GetPrimary (buffer, index, tree);

            return BinTree_CtorNode (OPERATION, op_code, nullptr,
                                     right_value, nullptr, tree);
        }
    }

    return GetPrimary (buffer, index, tree);
}

static BinTree_node*
GetPrimary (const char*          buffer,
                  size_t*  const index,
                  BinTree* const tree)
{
    assert (buffer);
    assert (index);
    assert (tree);

    BinTree_node* node = nullptr;

    if (buffer [*index] == '(')
    {
        (*index)++;

        node = GetExpression (buffer, index, tree);

        syn_assert (buffer [*index] == ')');

        (*index)++;

        return node;
    }

    return GetValue (buffer, index, tree);
}

static BinTree_node*
GetValue (const char*          buffer,
                size_t*  const index,
                BinTree* const tree)
{
    assert (buffer);
    assert (index);
    assert (tree);

    if (isdigit (buffer [*index]))
    {
        return GetNumber (buffer, index, tree);
    }

    else
    {
        return GetVariable (buffer, index, tree);
    }
}

static BinTree_node*
GetVariable (const char*          buffer,
                   size_t*  const index,
                   BinTree* const tree)
{
    assert (buffer);
    assert (index);
    assert (tree);

    char* const var_name =
        (char*) calloc (VAR_NAME_MAX_LEN, sizeof (char));
    if (!var_name)
    {
        perror ("var_name allocation error");
        return nullptr;
    }

    size_t var_name_index = 0;

    while (isalnum (buffer [*index]))
    {
        var_name [var_name_index++] = buffer [*index];
        (*index)++;
    }

    for (var_index_type i = 0; i < tree->var_number; ++i)
    {
        if (strcmp (var_name, tree->var_table[i].var_name) == 0)
        {
            return BinTree_CtorNode (VARIABLE, i, nullptr,
                                     nullptr, nullptr, tree);
        }
    }

    if (tree->var_number == tree->var_table_capacity)
    {
        tree->var_table_capacity *= VAR_TABLE_CAPACITY_MULTIPLIER;
        ReallocVarTable (tree);
    }

    tree->var_table [tree->var_number] .var_name = var_name;
    tree->var_table [tree->var_number] .status   = ENABLED;
    tree->var_number++;

    return BinTree_CtorNode (VARIABLE, tree->var_number - 1,
                             nullptr, nullptr, nullptr, tree);
}

static BinTree_node*
GetNumber (const char*          buffer,
                 size_t*  const index,
                 BinTree* const tree)
{
    assert (buffer);
    assert (index);
    assert (tree);

    int32_t  int_part       = 0;
    double   float_part     = 0;
    uint32_t n_float_digits = 0;

    bool after_dot   = false;
    number_sign sign = POSITIVE;

    const size_t old_index = *index;

    if (buffer [*index] == '-')
    {
        sign = NEGATIVE;
    }

    while (('0' <= buffer [*index] && buffer [*index] <= '9') ||
                   buffer [*index] == '.')
    {
        if (buffer [*index] == '.' && !after_dot)
        {
            after_dot = true;
        }

        else if (!after_dot)
        {
            int_part = int_part * 10 + buffer [*index] - '0';
        }

        else
        {
            n_float_digits++;
            float_part += (buffer [*index] - '0' ) /
                          pow (10, n_float_digits);
        }

        (*index)++;
    }

    syn_assert (*index > old_index);
    return _CONST (sign * (int_part + float_part));
}

static void
syn_assert (const bool expression)
{
    if (!expression) abort ();
}

#undef _ADD
#undef _SUB
#undef _MUL
#undef _DIV
#undef _POW
#undef _SIN
#undef _COS
