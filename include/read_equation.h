#pragma once

#include <ctype.h>
#include "BinTree_struct.h"
#include "wolfram.h"
#include "FileOpenLib.h"

enum number_sign
{
    POSITIVE =  1,
    NEGATIVE = -1
};

enum parse_op_code
{
    OPEN_PARENTHESIS  = -1,
    CLOSE_PARENTHESIS = -2,
    NULL_TERMINATE    = -3
};

const int32_t PARSE_START_OP_CODE = NULL_TERMINATE;

union token_data
{
    double num_value;
    int8_t op_code;
    char   var_name [VAR_NAME_MAX_LEN];
};

struct token
{
    data_type  token_data_type;
    token_data data;
};

#include "List_struct.h"

BinTree*
ReadTree (const char*    const input_file_name,
                BinTree* const tree);
