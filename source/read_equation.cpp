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
GetOpCode (BinTree_data_type* const new_data,
           const char*        const str_input);

static var_index_type
SetVariable (const char*    const var_name,
                   BinTree* const tree);

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

// inline functions?
    if (input_parced->lines_array[*line_index].line[0] == '(')
    {
        (*line_index)++;
    }

    else
    {
        return nullptr;
    }

    BinTree_node*      node_left  =
        ReadSubtree  (input_parced, line_index, tree);

    BinTree_data_type* node_data  =
        ReadNodeData (input_parced, line_index, tree);

    BinTree_node*      node_right =
        ReadSubtree  (input_parced, line_index, tree);

    if (input_parced->lines_array[*line_index].line[0] == ')')
    {
        (*line_index)++;
    }
    else
    {
        fprintf (stderr, "No closing bracket.\n");
        return nullptr;
    }

    BinTree_node* new_node =
        BinTree_CtorNode (node_data, node_left, node_right, tree);

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

    if (input_parced->lines_array[*line_index].line[0] == '(')
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

    char* const str_input =
        (char*) calloc (VAR_NAME_MAX_LEN, sizeof (char));

    if (sscanf (input_parced->lines_array[*line_index].line, "%lg",
                &new_data->data_value.num_value))
    {
        new_data->data_type = NUMBER;
    }

    else if (sscanf (input_parced->lines_array[*line_index].line, "%s",
                     str_input))
    {
        GetOpCode (new_data, str_input);
    }

    if (new_data->data_type == VARIABLE)
    {
        new_data->data_value.var_index =
            SetVariable (str_input, tree);

        if (new_data->data_value.var_index != VAR_INDEX_POISON)
        {
            tree->var_number++;
        }
    }

    (*line_index)++;
    free (str_input);

    return new_data;
}

static data_type
GetOpCode (BinTree_data_type* const new_data,
           const char*        const str_input)
{
    assert (new_data);
    assert (str_input);

    new_data->data_type = VARIABLE;

    for (op_code_type cur_op_code = 0;
                      cur_op_code < NUM_OF_OP;
                      cur_op_code++)
    {
        if (strcasecmp (str_input,
                        operations_array [cur_op_code]) == 0)
        {
            new_data->data_type = OPERATION;

            new_data->data_value.op_code =
                        cur_op_code;
        }
    }

    return new_data->data_type;
}

static var_index_type
SetVariable (const char*    const var_name,
                   BinTree* const tree)
{
    assert (var_name);
    assert (tree);

    tree->var_table[tree->var_number].var_name =
        (char*) calloc (sizeof (char), sizeof (VAR_NAME_MAX_LEN));
    if (!tree->var_table[tree->var_number].var_name)
    {
        perror ("var_name allocation error");
        return VAR_INDEX_POISON;
    }

    strcpy (tree->var_table[tree->var_number].var_name,
            var_name);

    return tree->var_number;
}
