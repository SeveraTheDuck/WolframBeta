#include "read_equation.h"
#include "List_commands.h"

/*
 * Here is the description of grammar rules of the code.
 *
 * G stands for GetGrammar(),
 * E stands for GetExpression(),
 * T stands for GetTerm(),
 * U stands for GetUnary(),
 * P stands for GetPrimary(),
 * V stands for GetValue(),
 * N stands for GetNumber(),
 * R stands for GetVariable().
 *
 * G ::= E '\0'
 * E ::= T {[+ -] T}*
 * T ::= U {[* / ^] U}*
 * U ::= {[operations_array] P} | P
 * P ::= '(' E ')' | V
 * V ::= N | R
 * N ::= [0 - 9]+ Как обозначить единственность точки? [0 - 9]*
 * R ::= {[A - Z] | [a - z]} {[A - Z] | [a - z] | [0 - 9]}*
 */

static void
SeparateToTokens (const file_input* const input_parsed,
                        List*       const tokens_list);

static void
GetTokenData (const file_input* const input_parsed,
                    size_t*     const index,
                    token*      const cur_token);

static BinTree_node*
GetGrammar (const List*    const tokens_list,
                  size_t*  const token_index,
                  BinTree* const tree);

static BinTree_node*
GetExpression (const List_data_type* const tokens_array,
                     size_t*         const token_index,
                     BinTree*        const tree);

static BinTree_node*
GetTerm (const List_data_type* const tokens_array,
               size_t*         const token_index,
               BinTree*        const tree);

static BinTree_node*
GetUnary (const List_data_type* const tokens_array,
                size_t*         const token_index,
                BinTree*        const tree);

static BinTree_node*
GetPrimary (const List_data_type* const tokens_array,
                  size_t*         const token_index,
                  BinTree*        const tree);

static BinTree_node*
GetValue (const List_data_type* const tokens_array,
                size_t*         const token_index,
                BinTree*        const tree);

static BinTree_node*
GetVariable (const List_data_type* const tokens_array,
                   size_t*         const token_index,
                   BinTree*        const tree);

static void
syn_assert_func (const size_t n_line,
                 const bool expression);

#define syn_assert(expression)              \
    syn_assert_func (__LINE__, expression);

BinTree*
ReadTree (const char*    const input_file_name,
                BinTree* const tree)
{
    if (!tree)
    {
        fprintf (stderr, "Invalid pointer to tree struct\n");
        return nullptr;
    }

    file_input input_parsed = {};
    GetFileInput (input_file_name, &input_parsed, NOT_PARTED);
    input_parsed.buffer [input_parsed.buffer_size - 1] = '\0';

    if (input_parsed.buffer_size == 0) return nullptr;

    List tokens_list = {};
    List_Ctor (&tokens_list);

    SeparateToTokens (&input_parsed, &tokens_list);

    FreeFileInput    (&input_parsed);

    size_t token_index = 0;
    tree->root = GetGrammar (&tokens_list, &token_index, tree);

    /* null-termination check */
    syn_assert (tokens_list.list_data [token_index]
                .token_data_type == OPERATION &&
                tokens_list.list_data [token_index]
                .data.op_code == NULL_TERMINATE);

    SetParents (nullptr, tree->root);

    List_Dtor (&tokens_list);

    return tree;
}

static void
SeparateToTokens (const file_input* const input_parsed,
                        List*       const tokens_list)
{
    assert (input_parsed->buffer);
    assert (tokens_list);

    size_t index = 0;

    token cur_token = {.token_data_type = NUMBER,
                       .data.num_value  = BinTree_POISON};

    while (index < input_parsed->buffer_size)
    {
        if (input_parsed->buffer [index] == ' ')
        {
            index++;
            continue;
        }

        switch (input_parsed->buffer[index])
        {
            case '(':
                index++;
                cur_token.token_data_type = OPERATION;
                cur_token.data.op_code    = OPEN_PARENTHESIS;
                break;

            case ')':
                index++;
                cur_token.token_data_type = OPERATION;
                cur_token.data.op_code    = CLOSE_PARENTHESIS;
                break;

            case '\0':
                index++;
                cur_token.token_data_type = OPERATION;
                cur_token.data.op_code    = NULL_TERMINATE;
                break;

            default:
                GetTokenData (input_parsed, &index, &cur_token);
                break;
        }

        List_PushBack (&cur_token, tokens_list);
    }
}

static void
GetTokenData (const file_input* const input_parsed,
                    size_t*     const index,
                    token*      const cur_token)
{
    assert (input_parsed);
    assert (index);
    assert (cur_token);

    switch (input_parsed->buffer [*index])
    {
        case '+':
            (*index)++;
            cur_token->token_data_type = OPERATION;
            cur_token->data.op_code    = ADD;
            return;

        case '-':
            (*index)++;
            cur_token->token_data_type = OPERATION;
            cur_token->data.op_code    = SUB;
            return;

        case '*':
            (*index)++;
            cur_token->token_data_type = OPERATION;
            cur_token->data.op_code    = MUL;
            return;

        case '/':
            (*index)++;
            cur_token->token_data_type = OPERATION;
            cur_token->data.op_code    = DIV;
            return;

        case '^':
            (*index)++;
            cur_token->token_data_type = OPERATION;
            cur_token->data.op_code    = POW;
            return;
    }

    if (isnumber (input_parsed->buffer [*index]))
    {
        int32_t n_read_elem = 0;

        sscanf (input_parsed->buffer + *index, "%lg%n",
                &cur_token->data.num_value, &n_read_elem);

        *index += (size_t) n_read_elem;

        cur_token->token_data_type = NUMBER;
    }

    else
    {
        size_t str_length = 0;

        while (isalnum (input_parsed->buffer [*index]))
        {
            cur_token->data.var_name [str_length] =
                input_parsed->buffer [*index];

            (*index)++;
            str_length++;
        }
        cur_token->data.var_name [str_length] = '\0';

        /* start count from first unary operation */
        for (size_t i = NUM_OF_BIN_OP; i < NUM_OF_OP; ++i)
        {
            if (strcasecmp (cur_token->data.var_name,
                            operations_array [i]) == 0)
            {
                cur_token->token_data_type = OPERATION;
                cur_token->data.op_code    = (op_code_type) i;

                return;
            }
        }

        cur_token->token_data_type = VARIABLE;
    }
}

static BinTree_node*
GetGrammar (const List*    const tokens_list,
                  size_t*  const token_index,
                  BinTree* const tree)
{
    assert (tokens_list);
    assert (token_index);
    assert (tree);

    /* token_index start value not zero because of dummy_elem in list */
    *token_index = 1;
    return GetExpression (tokens_list->list_data, token_index, tree);
}

static BinTree_node*
GetExpression (const List_data_type* const tokens_array,
                     size_t*         const token_index,
                     BinTree*        const tree)
{
    assert (tokens_array);
    assert (token_index);
    assert (tree);

    BinTree_node* left_value  = GetTerm (tokens_array, token_index, tree);
    BinTree_node* right_value = nullptr;
    BinTree_node* new_node    = nullptr;

    while (tokens_array [*token_index] .token_data_type == OPERATION &&
          (tokens_array [*token_index] .data .op_code   == ADD ||
           tokens_array [*token_index] .data .op_code   == SUB))
    {
        op_code_type op_code =
            tokens_array [(*token_index)++] .data .op_code;

        right_value = GetTerm (tokens_array, token_index, tree);

        new_node = BinTree_CtorNode (OPERATION, op_code, left_value,
                                     right_value, nullptr, tree);

        left_value = new_node;
    }

    return left_value;
}

static BinTree_node*
GetTerm (const List_data_type* const tokens_array,
               size_t*         const token_index,
               BinTree*        const tree)
{
    assert (tokens_array);
    assert (token_index);
    assert (tree);

    BinTree_node* left_value  = GetUnary (tokens_array, token_index, tree);
    BinTree_node* right_value = nullptr;
    BinTree_node* new_node    = nullptr;

    while (tokens_array [*token_index] .token_data_type == OPERATION &&
          (tokens_array [*token_index] .data .op_code   == MUL ||
           tokens_array [*token_index] .data .op_code   == DIV ||
           tokens_array [*token_index] .data .op_code   == POW))
    {
        op_code_type op_code =
            tokens_array [(*token_index)++] .data .op_code;

        right_value = GetUnary (tokens_array, token_index, tree);

        new_node = BinTree_CtorNode (OPERATION, op_code, left_value,
                                     right_value, nullptr, tree);

        left_value = new_node;
    }

    return left_value;
}

static BinTree_node*
GetUnary (const List_data_type* const tokens_array,
                size_t*         const token_index,
                BinTree*        const tree)
{
    assert (tokens_array);
    assert (token_index);
    assert (tree);

    /*
     * op_code >= NUM_OF_BIN_OP because bin
     * operations have already been checked
     */

    if (tokens_array [*token_index] .token_data_type == OPERATION &&
        tokens_array [*token_index] .data .op_code >= NUM_OF_BIN_OP)
    {
        op_code_type op_code =
            tokens_array [(*token_index)++] .data .op_code;

        BinTree_node* right_value =
            GetPrimary (tokens_array, token_index, tree);

        return BinTree_CtorNode (OPERATION, op_code, nullptr,
                                 right_value, nullptr, tree);
    }

    return GetPrimary (tokens_array, token_index, tree);
}

static BinTree_node*
GetPrimary (const List_data_type* const tokens_array,
                  size_t*         const token_index,
                  BinTree*        const tree)
{
    assert (tokens_array);
    assert (token_index);
    assert (tree);

    BinTree_node* node = nullptr;

    if (tokens_array [*token_index] .token_data_type == OPERATION &&
        tokens_array [*token_index] .data .op_code   == OPEN_PARENTHESIS)
    {
        (*token_index)++;

        node = GetExpression (tokens_array, token_index, tree);

        syn_assert
        (
            tokens_array [*token_index] .token_data_type == OPERATION &&
            tokens_array [*token_index] .data .op_code   == CLOSE_PARENTHESIS
        );

        (*token_index)++;

        return node;
    }

    return GetValue (tokens_array, token_index, tree);
}

static BinTree_node*
GetValue (const List_data_type* const tokens_array,
                size_t*         const token_index,
                BinTree*        const tree)
{
    assert (tokens_array);
    assert (token_index);
    assert (tree);

    if (tokens_array [*token_index] .token_data_type == NUMBER)
    {
        return BinTree_CtorNode (NUMBER, tokens_array [(*token_index)++]
                                 .data .num_value, nullptr,
                                 nullptr, nullptr, tree);
    }

    else if (tokens_array [*token_index] .token_data_type == VARIABLE)
    {
        return GetVariable (tokens_array, token_index, tree);
    }

    else
    {
        syn_assert (NO_TYPE);
        return nullptr;
    }
}

static BinTree_node*
GetVariable (const List_data_type* const tokens_array,
                   size_t*         const token_index,
                   BinTree*        const tree)
{
    assert (tokens_array);
    assert (token_index);
    assert (tree);

    char* const var_name =
        (char*) calloc (VAR_NAME_MAX_LEN, sizeof (char));
    if (!var_name)
    {
        perror ("var_name allocation error");
        return nullptr;
    }

    strcpy (var_name, tokens_array [(*token_index)++] .data .var_name);

    for (var_index_type i = 0; i < tree->var_number; ++i)
    {
        if (strcmp (var_name, tree->var_table [i] .var_name) == 0)
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

static void
syn_assert_func (const size_t n_line,
                 const bool expression)
{
    if (!expression)
    {
        fprintf (stderr, "Aborting line %zd\n", n_line);
        abort ();
    }
}

#undef syn_assert
