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
                    size_t*     const line_index);

static BinTree_node*
FillNewNode (BinTree_node* const node_left,
             BinTree_data_type* const node_data,
             BinTree_node* const node_right);

BinTree*
ReadTree (const char*    const input_file_name,
                BinTree* const tree)
{
    if (!tree)
    {
        fprintf (stderr, "Invalid pointer to tree struct");
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
    if (!input_parced || !line_index)
    {
        fprintf (stderr, "Invalid input_parced pointers.\n");
        return nullptr;
    }

    if (!tree)
    {
        fprintf (stderr, "Invalid tree pointer.\n");
        return nullptr;
    }

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
        ReadNodeData (input_parced, line_index);

    BinTree_node*      node_right =
        ReadSubtree  (input_parced, line_index, tree);

    if (input_parced->lines_array[*line_index].line[0] == ')')
    {
        (*line_index)++;
        tree->n_elem++;
    }
    else
    {
        fprintf (stderr, "No closing bracket.\n");
        return nullptr;
    }

    BinTree_node* new_node = FillNewNode (node_left, node_data, node_right);

    free (node_data);

    return new_node;
}

static BinTree_node*
ReadSubtree (const file_input* const input_parced,
                   size_t*     const line_index,
                   BinTree*    const tree)
{
    if (!input_parced || !line_index)
    {
        fprintf (stderr, "Invalid input_parced pointers\n");
        return nullptr;
    }

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
                    size_t*     const line_index)
{
    if (!input_parced || !line_index)
    {
        fprintf (stderr, "Invalid input_parced pointers\n");
        return nullptr;
    }

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
        // SetVariable
    }

    (*line_index)++;
    free (str_input);

    return new_data;
}

static BinTree_node*
FillNewNode (BinTree_node*      const node_left,
             BinTree_data_type* const node_data,
             BinTree_node*      const node_right)
{
    if (!node_data)
    {
        fprintf (stderr, "node_data wrong input.\n");
        return nullptr;
    }

    BinTree_node* new_node = (BinTree_node*) calloc (1, sizeof (BinTree_node));
    if (!new_node)
    {
        perror ("new_node allocation error");
        return nullptr;
    }

    new_node->left  = node_left;
    memcpy (&new_node->data, node_data, sizeof (BinTree_data_type));
    new_node->right = node_right;

    return new_node;
}
