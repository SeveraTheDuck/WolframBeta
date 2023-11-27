#ifndef BINTREE_CONFIG_H
#define BINTREE_CONFIG_H

#include "wolfram_config.h"

typedef uint8_t op_code_type;
typedef  int8_t var_index_type;

union data_value
{
    double         num_value;
    op_code_type   op_code;
    var_index_type var_index;
};

struct data
{
    data_type  data_type;
    data_value data_value;
};

typedef data BinTree_data_type;

#define BinTree_OUTPUT_F "%lg"

const double BinTree_POISON = -3.1415926;

const char* const BinTree_IMAGE_CONSTRUCT_FILE_NAME = "image_construct.dot";
const char* const BinTree_IMAGE_FILE_NAME = "tree.png";

const size_t VAR_NAME_MAX_LEN = 50;
const size_t INIT_VAR_NUMBER  = 10;
const var_index_type VAR_INDEX_POISON = -1;
const var_index_type VAR_TABLE_CAPACITY_MULTIPLIER = 2;

#endif /* BINTREE_CONFIG_H */
