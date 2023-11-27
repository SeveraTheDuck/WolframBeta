#ifndef WOLFRAM_H
#define WOLFRAM_H

#include <math.h>
#include "BinTree_struct.h"
#include "wolfram_config.h"

double
Evaluate (BinTree* const tree);

void
FillVariables (BinTree* const tree);

#endif /* WOLFRAM_H */
