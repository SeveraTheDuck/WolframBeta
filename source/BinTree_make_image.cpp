#include "BinTree_make_image.h"

static void
BinTree_PrintNodes (const BinTree_node* const node,
                          FILE*         const image_file);

static void
BinTree_ConstructImage ();

void
BinTree_MakeTreeImage (const BinTree* const tree)
{
    FILE* image_file = fopen (BinTree_IMAGE_CONSTRUCT_FILE_NAME, "wb");

    fprintf (image_file, "digraph G\n{\n"
                         "    rankdir = UD;\n"
                         "    bgcolor = \"#ff8080\";\n"
                         "    node  [style = filled, color = white];\n"
                         "    edge  [color = \"darkgreen\", "
                                 "fontcolor = \"blue\", "
                                 "fontsize = 15];\n\n"

                         "    subgraph cluster0\n    {\n"
                         "        bgcolor = \"#ffAAAA\";\n"
                         "        height  = 20;\n"
                         "        style   = filled;\n"
                         "        label   = \"My bin tree\";\n\n");

    BinTree_PrintNodes (tree->root, image_file);

    fprintf (image_file, "    }\n}");

    fclose (image_file);

    BinTree_ConstructImage ();
}

static void
BinTree_ConstructImage ()
{
    size_t system_call_length =
        strlen ("dot ") +
        strlen (BinTree_IMAGE_CONSTRUCT_FILE_NAME) +
        strlen (" -Tpng -o ") +
        strlen (BinTree_IMAGE_FILE_NAME) + 1;

    char* system_call = (char*)
        calloc (sizeof (char), system_call_length);

    system_call = strncat (system_call, "dot ", sizeof ("dot "));
    system_call = strncat (system_call, BinTree_IMAGE_CONSTRUCT_FILE_NAME,
                                strlen (BinTree_IMAGE_CONSTRUCT_FILE_NAME));
    system_call = strncat (system_call, " -Tpng -o ", sizeof (" -Tpng -o "));
    system_call = strncat (system_call, BinTree_IMAGE_FILE_NAME,
                                strlen (BinTree_IMAGE_FILE_NAME));

    system (system_call);

    free (system_call);
}

static void
BinTree_PrintNodes (const BinTree_node* const node,
                          FILE*         const image_file)
{
    if (!node)
    {
        return;
    }

    fprintf (image_file, "        %lld  [shape = \"Mrecord\", "
                                        "fillcolor = \"#FFFFFF\", "
                                        "label = \"", (int64_t) node);

    switch (node->data.data_type)
    {
        case NO_TYPE:
        {
            fprintf (image_file, "none\", color = \"#FF0000\"]\n");

            break;
        }

        case NUMBER:
        {
            fprintf (image_file, BinTree_OUTPUT_F
                                 "\", color = \"#00FF00\"];\n",
                                 node->data.data_value
                                           .num_value);

            break;
        }

        case OPERATION:
        {
            switch (node->data.data_value.op_code)
            {
                case ADD:
                {
                    fputc ('+', image_file);
                    break;
                }

                case SUB:
                {
                    fputc ('-', image_file);
                    break;
                }

                case MUL:
                {
                    fputc ('*', image_file);
                    break;
                }

                case DIV:
                {
                    fputc ('/', image_file);
                    break;
                }

                case SIN:
                {
                    fprintf (image_file, "sin");
                    break;
                }

                default:
                {
                    fputc ('0', image_file);
                    break;
                }
            }

            fprintf (image_file, "\", color = \"#0000FF\"];\n");

            break;
        }

        case VARIABLE:
        {
            fputs (node->data.data_value.var_name, image_file);
            fputs ("\", color = \"#00FFFF\"];\n", image_file);
        }
    }

    if (node->left)
    {
        fprintf (image_file, "        %lld -> %lld [color = \"#FF0000\", "
                                                   "weight = 10];\n",
                                                   (int64_t) node,
                                                   (int64_t) node->left);
        BinTree_PrintNodes (node->left,  image_file);
    }

    if (node->right)
    {
        fprintf (image_file, "        %lld -> %lld [color = \"#FF0000\","
                                                   "weight = 10];\n",
                                                   (int64_t) node,
                                                   (int64_t) node->right);
        BinTree_PrintNodes (node->right, image_file);
    }
}
