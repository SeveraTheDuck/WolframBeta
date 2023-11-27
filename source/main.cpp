#include "BinTree_struct.h"
#include "BinTree_make_image.h"
#include "read_equation.h"
#include "wolfram.h"

int32_t main (const int32_t /*argc*/, const char** argv)
{
    BinTree tree = {};
    BINTREE_CTOR (&tree);

    ReadTree (argv [1], &tree);

    BinTree_MakeTreeImage (&tree);

    printf (BinTree_OUTPUT_F, Evaluate(&tree, 0));

    BINTREE_DTOR (&tree);

    return 0;
}
