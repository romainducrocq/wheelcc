#ifndef _OPTIMIZATION_OPTIM_TAC_H
#define _OPTIMIZATION_OPTIM_TAC_H

#include <inttypes.h>

typedef struct TacProgram TacProgram;
typedef struct FrontEndContext FrontEndContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Three address code optimization

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constant folding
// Unreachable code elimination
// Copy propagation
// Dead store elimination

void optimize_three_address_code(TacProgram* node, FrontEndContext* frontend, uint8_t optim_1_mask);

#endif
