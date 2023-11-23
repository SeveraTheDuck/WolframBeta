#ifndef BINTREE_CONFIG_H
#define BINTREE_CONFIG_H

#include "wolfram_config.h"

union data_value
{
    double      numerical_value;
    operation   operation_number;
    char* const variable_name;
};

typedef struct data
{
    data_type  data_type;
    data_value data_value;
} BinTree_data_type;

#define BinTree_OUTPUT_F "%lg"

const double BinTree_POISON = -3.1415926;

const char* const BinTree_IMAGE_CONSTRUCT_FILE_NAME = "image_construct.dot";
const char* const BinTree_IMAGE_FILE_NAME = "tree.png";

const size_t INIT_NUM_OF_ALLOCATED_DATA = 50;

#endif /* BINTREE_CONFIG_H */
