#pragma once

#include "BinTree_struct.h"
#include "wolfram.h"
#include "FileOpenLib.h"

BinTree*
ReadTree (const char*    const input_file_name,
                BinTree* const tree);
