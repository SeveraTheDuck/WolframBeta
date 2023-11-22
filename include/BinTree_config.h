#ifndef BINTREE_CONFIG_H
#define BINTREE_CONFIG_H

typedef int32_t BinTree_data_type;
#define BinTree_OUTPUT_F "%d"

const BinTree_data_type BinTree_POISON = -31415926;

const char* const BinTree_IMAGE_CONSTRUCT_FILE_NAME = "image_construct.dot";
const char* const BinTree_IMAGE_FILE_NAME = "tree.png";

const size_t MAX_NUM_OF_ALLOCATED_DATA = 50;

const size_t DESCRIPTION_MAX_LEN = 200;

#endif
