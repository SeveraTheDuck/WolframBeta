#pragma once

#include <math.h>
#include "BinTree_struct.h"

const double WOLFRAM_EPS = 1e-9;

inline bool
IsNumber (const BinTree_node* const node);

inline bool
IsOperation (const BinTree_node* const node);

inline bool
IsVariable (const BinTree_node* const node);

inline bool
IsNullValue (const BinTree_node* const node);

inline bool
IsOneValue (const BinTree_node* const node);

inline bool
IsNumber (const BinTree_node* const node)
{
    if (node->data.data_type == NUMBER) return true;
    return false;
}

inline bool
IsOperation (const BinTree_node* const node)
{
    if (node->data.data_type == OPERATION) return true;
    return false;
}

inline bool
IsVariable (const BinTree_node* const node)
{
    if (node->data.data_type == VARIABLE) return true;
    return false;
}

inline bool
IsNullValue (const BinTree_node* const node)
{
    if (!node) return false;

    if (!IsNumber (node)) return false;

    if (fabs (node->data.data_value.num_value) < WOLFRAM_EPS)
    {
        return true;
    }

    return false;
}

inline bool
IsOneValue (const BinTree_node* const node)
{
    if (!node) return false;

    if (!IsNumber (node)) return false;

    if (fabs (node->data.data_value.num_value - 1) < WOLFRAM_EPS)
    {
        return true;
    }

    return false;
}
