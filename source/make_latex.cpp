#include "../include/make_latex.h"

const char* MATH_PHRASES_ARRAY [] =
    {"Легко заметить, что:",
     "Следующий факт будем использовать без доказательства:",
     "Ёжику очевидно, что:",
     "Доказательство данного факта предлагается читателю в качестве упражнения:",
     "Пользуясь теоремой Коши номер раз, два, ... сколько их там, получим:",
     "По теореме о стреляющем в плохую погоду колене, получим:",
     "Используя тридцать три теоремы о циркуляции алкоголя в крови, выведем:",
     "По размерности сходится, поэтому:",
     "Из рандомного места уравнения получим:",
     "Ахалай махалай, решайся давай:",
     "В начале имеем:"};

const uint8_t lower_index = 0;
const uint8_t upper_index = sizeof (MATH_PHRASES_ARRAY) / sizeof (char*) - 1;

static uint32_t
GetOperationPriority (const op_code_type op_code);

static void
PrintOperationToLatex (const BinTree_node* const node,
                       const BinTree*      const tree,
                       const uint32_t            previous_priority);

static inline void
PrintNumberToLatex (const double      num_value,
                          FILE* const latex_out);

static inline void
PrintVariableToLatex (const char* const var_name,
                            FILE* const latex_out);

static void
PrintMathPhrase (const BinTree* const tree);

void
PrintExpressionToLatex (const BinTree* const tree)
{
    if (!tree)
    {
        fprintf (stderr, "Invalid tree struct pointer.\n");
        return;
    }

    if (!tree->latex_out)
    {
        fprintf (stderr, "Invalid pointer to latex_out\n");
        return;
    }

    PrintNodeToLatex (tree->root, tree, START_PRIORITY);
}

FILE*
OpenLatexFile (BinTree* const tree)
{
    if (!tree)
    {
        fprintf (stderr, "Invalid pointer to tree struct.\n");
        return nullptr;
    }

    FILE* latex_out = fopen (LATEX_OUTPUT_FILE_NAME, "wb");
    tree->latex_out = latex_out;
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
           "\\maketitle\n", latex_out);

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
                  const uint32_t            pre_priority)
{
    if (!node)
    {
        fprintf (stderr, "Invalid pointer to node.\n");
        return;
    }

    if (!tree->latex_out)
    {
        fprintf (stderr, "Invalid pointer to latex_out\n");
        return;
    }

    if (pre_priority == START_PRIORITY)
    {
        PrintMathPhrase (tree);
        fprintf (tree->latex_out, "$$");
    }

    if (IsOperation (node))
    {
        PrintOperationToLatex (node, tree, pre_priority);
    }

    else if (IsNumber (node))
    {
        PrintNumberToLatex (node->data.data_value.num_value,
                            tree->latex_out);
    }

    else if (IsVariable (node))
    {
        PrintVariableToLatex (tree->var_table [node->data.data_value.var_index]
                                   .var_name, tree->latex_out);
    }

    else
    {
        fprintf (stderr, "Wrong data type.\n");
    }

    if (pre_priority == START_PRIORITY)
    {
        fprintf (tree->latex_out, "$$");
    }
}

static void
PrintOperationToLatex (const BinTree_node* const node,
                       const BinTree*      const tree,
                       const uint32_t            pre_priority)
{
    assert (node);
    assert (tree);

    uint32_t cur_priority =
        GetOperationPriority (node->data.data_value.op_code);

    if (cur_priority < pre_priority) fputc ('(', tree->latex_out);

    switch (node->data.data_value.op_code)
    {
        case ADD:
            PrintNodeToLatex (node->left,  tree, cur_priority);

            fputc ('+', tree->latex_out);

            PrintNodeToLatex (node->right, tree, cur_priority);

            break;

        case SUB:
            PrintNodeToLatex (node->left,  tree, cur_priority);

            fputc ('-', tree->latex_out);

            PrintNodeToLatex (node->right, tree, cur_priority);

            break;

        case MUL:
            PrintNodeToLatex (node->left,  tree, cur_priority);

            fputs (" \\cdot ", tree->latex_out);

            PrintNodeToLatex (node->right, tree, cur_priority);

            break;

        case DIV:
            fprintf (tree->latex_out, "\\frac{");

            PrintNodeToLatex (node->left,  tree, cur_priority);
            fputs ("}{", tree->latex_out);
            PrintNodeToLatex (node->right, tree, cur_priority);

            fputc ('}', tree->latex_out);

            break;

        case POW:
            PrintNodeToLatex (node->left,  tree, cur_priority);

            fputc ('^', tree->latex_out);

            fputc ('{', tree->latex_out);
            PrintNodeToLatex (node->right, tree, ADD_SUB_PRIORITY);
            fputc ('}', tree->latex_out);

            break;

        case SIN:
            fputs ("sin", tree->latex_out);
            PrintNodeToLatex (node->right, tree, cur_priority);

            break;

        case COS:
            fputs ("cos", tree->latex_out);
            PrintNodeToLatex (node->right, tree, cur_priority);

            break;

        case LN:
            fputs ("ln", tree->latex_out);
            PrintNodeToLatex (node->right, tree, cur_priority);

            break;
    }

    if (cur_priority < pre_priority) fputc (')', tree->latex_out);
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

static void
PrintMathPhrase (const BinTree* const tree)
{
    assert (tree);

    int32_t random_number = rand () % (upper_index - lower_index + 1)
                           + lower_index;

    fprintf (tree->latex_out, "%s", MATH_PHRASES_ARRAY [random_number]);
}

static inline void
PrintNumberToLatex (const double      num_value,
                          FILE* const latex_out)
{
    assert (latex_out);

    if (fabs (num_value - EULER_CONSTANT) < WOLFRAM_EPS)
    {
        fprintf (latex_out, " e ");
    }

    else if (num_value < 0)
    {

        fprintf (latex_out, "( %lg )", num_value);
    }

    else
    {
        fprintf (latex_out, " %lg ", num_value);
    }
}

static inline void
PrintVariableToLatex (const char* const var_name,
                            FILE* const latex_out)
{
    assert (latex_out);
    fprintf (latex_out, " %s ", var_name);
}
