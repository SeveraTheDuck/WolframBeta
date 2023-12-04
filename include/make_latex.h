#pragma once

#include "BinTree_struct.h"
#include "wolfram.h"

const char LATEX_OUTPUT_FILE_NAME [] = "latex_output.txt";

enum priority
{
    START_PRIORITY   = 0,
    ADD_SUB_PRIORITY = 1,
    MUL_DIV_PRIORITY = 2,
    POW_PRIORITY     = 3,
    UNARY_PRIORITY   = 4
};

void
PrintExpressionToLatex (const BinTree* const tree,
                              FILE*    const latex_out);

FILE*
OpenLatexFile ();

void
CloseLatexFile (FILE* const latex_out);

void
PrintNodeToLatex (const BinTree_node* const node,
                  const BinTree*      const tree,
                        FILE*         const latex_out,
                  const uint32_t             previous_priority);
