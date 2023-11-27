#ifndef READ_EQUATION_H
#define READ_EQUATION_H

#include "BinTree_struct.h"
#include "wolfram.h"
#include "FileOpenLib.h"

BinTree*
ReadTree (const char*    const input_file_name,
                BinTree* const tree);

#endif /* READ_EQUATION_H */
