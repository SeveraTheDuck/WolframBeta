#pragma once

#include <ctype.h>
#include "BinTree_struct.h"
#include "wolfram.h"
#include "FileOpenLib.h"

enum number_sign
{
    POSITIVE =  1,
    NEGATIVE = -1
};

BinTree*
ReadTree (const char*    const input_file_name,
                BinTree* const tree);
