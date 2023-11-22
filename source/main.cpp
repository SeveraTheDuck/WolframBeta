#include "BinTree_struct.h"
#include "BinTree_make_image.h"

int main (int /*argc*/, char** /*argv*/)
{
    BinTree tree = {};
    BinTree_Ctor (&tree);
    tree.root = BinTree_CtorNode (10, nullptr, nullptr, nullptr);

    BinTree_node* const root = tree.root;

    BinTree_InsertSorted (&tree, 5);
    BinTree_InsertSorted (&tree, 17);
    BinTree_InsertSorted (&tree, 15);
    BinTree_InsertSorted (&tree, 20);
    BinTree_InsertSorted (&tree, 12);

    // BinTree_DestroySubtree (root->left, root);
    BinTree_DestroySubtree (root->left);

    BinTree_MakeTreeImage (&tree);

    BinTree_DestroySubtree (root);

    return 0;
}
