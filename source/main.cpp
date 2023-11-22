#include "BinTree_struct.h"
#include "BinTree_make_image.h"
#include "wolfram.h"

int32_t main (int32_t /*argc*/, char** /*argv*/)
{
    BinTree tree = {};
    BinTree_Ctor (&tree);

    BinTree_data_type data1 = {.data_type = OPERATION, .data_value.operation_number = SIN};
    tree.root = BinTree_CtorNode (&data1, nullptr, nullptr, nullptr);

    BinTree_data_type data2 = {.data_type = NUMBER, .data_value.numerical_value = 10};
    tree.root->left = BinTree_CtorNode (&data2, nullptr, nullptr, nullptr);

    BinTree_data_type data3 = {.data_type = NUMBER, .data_value.numerical_value = 20};
    tree.root->right = BinTree_CtorNode (&data3, nullptr, nullptr, nullptr);

    BinTree_MakeTreeImage (&tree);

    printf (BinTree_OUTPUT_F, Evaluate(tree.root));

    BinTree_DestroySubtree (tree.root);

    return 0;
}
