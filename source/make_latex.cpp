#include "../include/make_latex.h"

static uint32_t
GetOperationPriority (const op_code_type op_code);

static void
PrintOperationToLatex (const BinTree_node* const node,
                       const BinTree*      const tree,
                             FILE*         const latex_out,
                       const uint32_t            previous_priority);

static inline void
PrintNumberToLatex (const double      num_value,
                          FILE* const latex_out);

static inline void
PrintVariableToLatex (const char* const var_name,
                            FILE* const latex_out);

void
PrintExpressionToLatex (const BinTree* const tree,
                              FILE*    const latex_out)
{
    if (!tree)
    {
        fprintf (stderr, "Invalid tree struct pointer.\n");
        return;
    }

    if (!latex_out)
    {
        fprintf (stderr, "Invalid pointer to latex_out\n");
        return;
    }

    fprintf (latex_out, "$$");

    PrintNodeToLatex (tree->root, tree, latex_out, START_PRIORITY);

    fprintf (latex_out, "$$");
}

FILE*
OpenLatexFile ()
{
    FILE* latex_out = fopen (LATEX_OUTPUT_FILE_NAME, "wb");
    if (!latex_out)
    {
        perror ("fopen() for latex_out failed");
        return nullptr;
    }

    fputs ("\\documentclass{article}\n"
           "\\usepackage{graphicx} % Required for inserting images\n"
           "\\usepackage[russian]{babel}\n"
           "\\usepackage{amssymb}\n"
           "\\usepackage{amsmath}\n"
           "\\usepackage{amsfonts}\n"
           "\\usepackage[a4paper,top=2cm,"
                                "bottom=2cm,"
                                "left=3cm,"
                                "right=3cm,"
                                "marginparwidth=1.75cm]{geometry}\n"

           "\\title{Differentiator output}\n"
           "\\author{SeveraTheDuck}\n"

           "\\begin{document}\n"
           "\\maketitle", latex_out);

    return latex_out;
}

void
CloseLatexFile (FILE* const latex_out)
{
    if (!latex_out)
    {
        fprintf (stderr, "Invalid pointer to latex_out "
                         "in CloseLatexFile().\n");
        return;
    }

    fputs ("\\end{document}", latex_out);

    fclose (latex_out);
}

void
PrintNodeToLatex (const BinTree_node* const node,
                  const BinTree*      const tree,
                        FILE*         const latex_out,
                  const uint32_t            previous_priority)
{
    if (!node)
    {
        fprintf (stderr, "Invalid pointer to node.\n");
        return;
    }

    if (!latex_out)
    {
        fprintf (stderr, "Invalid pointer to latex_out\n");
        return;
    }

    if (IsOperation (node))
    {
        PrintOperationToLatex (node, tree, latex_out, previous_priority);
    }

    else if (IsNumber (node))
    {
        PrintNumberToLatex (node->data.data_value.num_value, latex_out);
    }

    else if (IsVariable (node))
    {
        PrintVariableToLatex (tree->var_table [node->data.data_value.var_index]
                                   .var_name, latex_out);
    }

    else
    {
        fprintf (stderr, "Wrong data type.\n");
    }
}

static void
PrintOperationToLatex (const BinTree_node* const node,
                       const BinTree*      const tree,
                             FILE*         const latex_out,
                       const uint32_t            pre_priority)
{
    assert (node);
    assert (tree);
    assert (latex_out);

    uint32_t cur_priority =
        GetOperationPriority (node->data.data_value.op_code);

    if (cur_priority < pre_priority) fputc ('(', latex_out);

    switch (node->data.data_value.op_code)
    {
        case ADD:
            PrintNodeToLatex (node->left,  tree, latex_out, cur_priority);

            fputc ('+', latex_out);

            PrintNodeToLatex (node->right, tree, latex_out, cur_priority);

            break;

        case SUB:
            PrintNodeToLatex (node->left,  tree, latex_out, cur_priority);

            fputc ('-', latex_out);

            PrintNodeToLatex (node->right, tree, latex_out, cur_priority);

            break;

        case MUL:
            PrintNodeToLatex (node->left,  tree, latex_out, cur_priority);

            fputc ('*', latex_out);

            PrintNodeToLatex (node->right, tree, latex_out, cur_priority);

            break;

        case DIV:
            fprintf (latex_out, "\\frac{");

            PrintNodeToLatex (node->left,  tree, latex_out, cur_priority);
            fputs ("}{", latex_out);
            PrintNodeToLatex (node->right, tree, latex_out, cur_priority);

            fputc ('}', latex_out);

            break;

        case POW:
            PrintNodeToLatex (node->left,  tree, latex_out, cur_priority);

            fputc ('^', latex_out);

            fputc ('{', latex_out);
            PrintNodeToLatex (node->right, tree, latex_out, cur_priority);
            fputc ('}', latex_out);

            break;

        case SIN:
            fputs ("sin", latex_out);
            PrintNodeToLatex (node->right, tree, latex_out, cur_priority);

            break;

        case COS:
            fputs ("cos", latex_out);
            PrintNodeToLatex (node->right, tree, latex_out, cur_priority);

            break;

        case LN:
            fputs ("ln", latex_out);
            PrintNodeToLatex (node->right, tree, latex_out, cur_priority);

            break;
    }

    if (cur_priority < pre_priority) fputc (')', latex_out);
}

static uint32_t
GetOperationPriority (const op_code_type op_code)
{
    switch (op_code)
    {
        case ADD:
            return ADD_SUB_PRIORITY;
        case SUB:
            return ADD_SUB_PRIORITY;
        case MUL:
            return MUL_DIV_PRIORITY;
        case DIV:
            return MUL_DIV_PRIORITY;
        case POW:
            return POW_PRIORITY;

        default:
            return UNARY_PRIORITY;
    }
}

static inline void
PrintNumberToLatex (const double      num_value,
                          FILE* const latex_out)
{
    assert (latex_out);
    fprintf (latex_out, " %lg ", num_value);
}

static inline void
PrintVariableToLatex (const char* const var_name,
                            FILE* const latex_out)
{
    assert (latex_out);
    fprintf (latex_out, " %s ", var_name);
}
