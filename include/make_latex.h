#pragma once

#include <time.h>
#include "BinTree_struct.h"
#include "wolfram.h"

const char LATEX_OUTPUT_FILE_NAME [] = "latex_output.txt";

/* Without file extension to generate multiple graphs */
const char GRAPH_OUTPUT_FILE_NAME [] = "gnuplot_test";
const size_t GRAPH_NUMBER_MAX_LEN = 2;

const size_t GRAPH_OUTPUT_FILE_NAME_LEN =
    strlen (GRAPH_OUTPUT_FILE_NAME) + GRAPH_NUMBER_MAX_LEN + strlen (".gpi ");

enum priority
{
    START_PRIORITY   = 0,
    ADD_SUB_PRIORITY = 1,
    MUL_DIV_PRIORITY = 2,
    UNARY_PRIORITY   = 3,
    POW_PRIORITY     = 4
};

void
PrintExpressionToLatex (const BinTree* const tree);

FILE*
OpenLatexFile (BinTree* const tree);

void
CloseLatexFile (FILE* const latex_out);

void
PrintNodeToLatex (const BinTree_node* const node,
                  const BinTree*      const tree,
                  const uint32_t             previous_priority);
