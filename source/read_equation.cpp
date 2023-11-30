#include "read_equation.h"

// input_parced & line_index into one struct?
// FileOpenLib remake
// inline functions like IsOpenBracket

static BinTree_node*
ReadNode (const file_input* const input_parced,
                size_t*     const line_index,
                BinTree*    const tree);

static BinTree_node*
ReadSubtree (const file_input* const input_parced,
                   size_t*     const line_index,
                   BinTree*    const tree);

static BinTree_data_type*
ReadNodeData (const file_input* const input_parced,
                    size_t*     const line_index,
                    BinTree*    const tree);

static data_type
GetNumValue (const file_input*        const input_parced,
                   size_t*            const line_index,
                   BinTree_data_type* const new_data);

static data_type
GetOpCode (const file_input*        const input_parced,
                 size_t*            const line_index,
                 BinTree_data_type* const new_data);

static data_type
SetVariable (const file_input*        const input_parced,
                   size_t*            const line_index,
                   BinTree_data_type* const new_data,
                   BinTree*           const tree);

static inline bool
IsOpeningBracket (const char ch);

static inline bool
IsClosingBracket (const char ch);

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
    GetFileInput (input_file_name, &input_parced, PARTED);

    if (input_parced.buffer_size == 0) return nullptr;

    size_t line_index = 0;

    tree->root = ReadNode (&input_parced, &line_index, tree);

    FreeFileInput (&input_parced);

    return tree;
}

static BinTree_node*
ReadNode (const file_input* const input_parced,
                size_t*     const line_index,
                BinTree*    const tree)
{
    assert (input_parced);
    assert (line_index);
    assert (tree);

    if (IsOpeningBracket (input_parced->lines_array[*line_index].line[0]))
    {
        (*line_index)++;
    }

    else
    {
        fprintf (stderr, "No opening bracket.\n");
        return nullptr;
    }

    BinTree_node*      node_left  =
        ReadSubtree  (input_parced, line_index, tree);

    BinTree_data_type* node_data  =
        ReadNodeData (input_parced, line_index, tree);

    BinTree_node*      node_right =
        ReadSubtree  (input_parced, line_index, tree);

    if (IsClosingBracket (input_parced->lines_array[*line_index].line[0]))
    {
        (*line_index)++;
    }

    else
    {
        fprintf (stderr, "No closing bracket.\n");
        return nullptr;
    }

    BinTree_node* new_node =
        MakeNodeByData (node_left, node_data, node_right, tree);

    free (node_data);

    return new_node;
}

static BinTree_node*
ReadSubtree (const file_input* const input_parced,
                   size_t*     const line_index,
                   BinTree*    const tree)
{
    assert (input_parced);
    assert (line_index);
    assert (tree);

    if (IsOpeningBracket (input_parced->lines_array[*line_index].line[0]))
    {
        return ReadNode (input_parced, line_index, tree);
    }

    else
    {
        return nullptr;
    }
}

static BinTree_data_type*
ReadNodeData (const file_input* const input_parced,
                    size_t*     const line_index,
                    BinTree*    const tree)
{
    assert (input_parced);
    assert (line_index);
    assert (tree);

    BinTree_data_type* new_data =
        (BinTree_data_type*) calloc (1, sizeof (BinTree_data_type));
    if (!new_data)
    {
        perror ("new_data allocation error");
        return nullptr;
    }

    if (GetNumValue (input_parced, line_index, new_data))
    {
        return new_data;
    }

    if (GetOpCode   (input_parced, line_index, new_data))
    {
        return new_data;
    }

    if (SetVariable (input_parced, line_index, new_data, tree))
    {
        return new_data;
    }

    fprintf (stderr, "Unknown type of expression or no expression given.\n");

    return nullptr;
}

static data_type
GetNumValue (const file_input*        const input_parced,
                   size_t*            const line_index,
                   BinTree_data_type* const new_data)
{
    assert (input_parced);
    assert (line_index);
    assert (new_data);

    if (sscanf (input_parced->lines_array[*line_index].line, "%lg",
                &new_data->data_value.num_value))
    {
        new_data->data_type = NUMBER;
        (*line_index)++;

        return NUMBER;
    }

    else
    {
        return NO_TYPE;
    }
}

static data_type
GetOpCode (const file_input*        const input_parced,
                 size_t*            const line_index,
                 BinTree_data_type* const new_data)
{
    assert (input_parced);
    assert (line_index);
    assert (new_data);

    char op_name [OP_NAME_MAX_LEN] = {};

    sscanf (input_parced->lines_array[*line_index].line, "%s", op_name);

    for (op_code_type cur_op_code = 0;
                        cur_op_code < NUM_OF_OP;
                        cur_op_code++)
    {
        if (strcasecmp (op_name,
                        operations_array [cur_op_code]) == 0)
        {
            new_data->data_type = OPERATION;
            new_data->data_value.op_code = cur_op_code;

            (*line_index)++;
            return OPERATION;
        }
    }

    return NO_TYPE;
}

static data_type
SetVariable (const file_input*        const input_parced,
                   size_t*            const line_index,
                   BinTree_data_type* const new_data,
                   BinTree*           const tree)
{
    assert (input_parced);
    assert (line_index);
    assert (new_data);
    assert (tree);

    char var_name [VAR_NAME_MAX_LEN] = {};

    sscanf (input_parced->lines_array[*line_index].line, "%s", var_name);

    if (tree->var_number == tree->var_table_capacity)
    {
        tree->var_table_capacity *= VAR_TABLE_CAPACITY_MULTIPLIER;
        ReallocVarTable (tree);
    }

    tree->var_table[tree->var_number].var_name =
        (char*) calloc (sizeof (char), sizeof (VAR_NAME_MAX_LEN));
    if (!tree->var_table[tree->var_number].var_name)
    {
        perror ("var_name allocation error");
        return NO_TYPE;
    }

    strcpy (tree->var_table[tree->var_number].var_name, var_name);
    tree->var_table->status = ENABLED;

    new_data->data_type = VARIABLE;
    new_data->data_value.var_index = tree->var_number;
    tree->var_number++;

    (*line_index)++;

    return VARIABLE;
}

static inline bool
IsOpeningBracket (const char ch)
{
    if (ch == '(') return true;
    return false;
}

static inline bool
IsClosingBracket (const char ch)
{
    if (ch == ')') return true;
    return false;
}
