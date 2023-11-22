#include "../include/BinTree_make_image.h"

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
                         "        bgcolor = \"#ffAAAA\";"
                         "        height  = 20"
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
                              FILE*             const image_file)
{
    static size_t  node_level = 1;
    static size_t  node_index = 0;
    size_t current_node_index = node_index;

    if (!node)
    {
        fprintf (image_file,
                 "        %zd [shape = \"Mrecord\", "
                              "label = \"nil\"]\n", node_index);
        ++node_index;
        --node_level;

        return;
    }

    fprintf (image_file, "        %zd [shape = \"Mrecord\", "
                                      "label = \""
                                      BinTree_OUTPUT_F
                                      "\"];\n",
                                      node_index, node->data);

    node_index++;
    node_level++;

    fprintf (image_file, "        %zd -> %zd [color = \"#FF0000\","
                                     "weight = %zd];\n",
                                     current_node_index, node_index,
                                     node_level);
    BinTree_PrintNodes (node->left,  image_file);

    node_level++;

    fprintf (image_file, "        %zd -> %zd [color = \"#FF0000\","
                                     "weight = %zd];\n",
                                     current_node_index, node_index,
                                     node_level);
    BinTree_PrintNodes (node->right, image_file);

    node_level--;
}
