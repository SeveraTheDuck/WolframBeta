#include "BinTree_struct.h"
#include "BinTree_make_image.h"
#include "wolfram.h"

int32_t main (int32_t /*argc*/, char** /*argv*/)
{
    BinTree tree = {};
    BINTREE_CTOR (&tree);

    BinTree_data_type data1 = {.data_type = OPERATION, .data_value.operation_number = DIV};
    tree.root = BinTree_CtorNode (&data1, nullptr, nullptr, nullptr, &tree);

    BinTree_data_type data2 = {.data_type = NUMBER, .data_value.numerical_value = 10};
    tree.root->left = BinTree_CtorNode (&data2, nullptr, nullptr, nullptr, &tree);

    BinTree_data_type data3 = {.data_type = VARIABLE, .data_value.variable_name = "x"};
    tree.root->right = BinTree_CtorNode (&data3, nullptr, nullptr, nullptr, &tree);

    BinTree_MakeTreeImage (&tree);

    printf (BinTree_OUTPUT_F, Evaluate(&tree, 15));

    BINTREE_DTOR (&tree);

    return 0;
}
