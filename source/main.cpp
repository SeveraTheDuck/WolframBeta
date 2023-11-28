#include "BinTree_struct.h"
#include "BinTree_make_image.h"
#include "read_equation.h"
#include "wolfram.h"

int32_t main (const int32_t /*argc*/, const char** argv)
{
    BinTree tree = {};
    BINTREE_CTOR (&tree);

// check reading functions
    ReadTree (argv [1], &tree);

    BinTree_MakeTreeImage (&tree);
    printf (BinTree_OUTPUT_F "\n", Evaluate(&tree));

// check differential functions
    BinTree d_tree = {};
    BINTREE_CTOR (&d_tree);

    DifferentiateTree (&tree, &d_tree, "x");

    BinTree_MakeTreeImage (&d_tree);
    printf (BinTree_OUTPUT_F "\n", Evaluate(&d_tree));

// dtor
    BINTREE_DTOR (&tree);
    BINTREE_DTOR (&d_tree);

    return 0;
}
