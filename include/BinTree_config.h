#ifndef BINTREE_CONFIG_H
#define BINTREE_CONFIG_H

#include "wolfram_config.h"

typedef uint8_t op_code_type;

union data_value
{
    double       num_value;
    op_code_type op_code;
    char* const  var_name;
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

const size_t INIT_NUM_OF_ALLOCATED_DATA = 50;

#endif /* BINTREE_CONFIG_H */
