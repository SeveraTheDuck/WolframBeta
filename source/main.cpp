#include "BinTree_struct.h"
#include "BinTree_make_image.h"
#include "read_equation.h"
#include "wolfram.h"
#include "make_latex.h"
#include "simplifier.h"

int32_t main (const int32_t /* argc */, const char** argv)
{
    BinTree tree = {};
    BINTREE_CTOR (&tree);
    FILE* latex_out = OpenLatexFile (&tree);

// check reading functions
    ReadTree (argv [1], &tree);

    PrintExpressionToLatex (&tree);

    MakeFunctionGraph (&tree, "x", -3.1415, 3.1415);

    SimplifyExpression (&tree);

    BinTree_MakeTreeImage (&tree);
    printf (BinTree_OUTPUT_F "\n", Evaluate(&tree));

// check differential functions
    BinTree d_tree = {};
    BINTREE_CTOR (&d_tree);

    DifferentiateExpression (&tree, &d_tree, "x");
    PrintExpressionToLatex  (&d_tree);

    SimplifyExpression (&d_tree);

    MakeFunctionGraph (&d_tree, "x", -3.1415, 3.1415);

    PrintExpressionToLatex (&tree);
    PrintExpressionToLatex (&d_tree);

    BinTree_MakeTreeImage (&d_tree);
    printf (BinTree_OUTPUT_F "\n", Evaluate(&d_tree));

// dtor
    BINTREE_DTOR (&tree);
    BINTREE_DTOR (&d_tree);
    CloseLatexFile (latex_out);

    return 0;
}
