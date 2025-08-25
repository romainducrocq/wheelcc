#ifndef _OPTIMIZATION_REG_ALLOC_H
#define _OPTIMIZATION_REG_ALLOC_H

#include <inttypes.h>

typedef struct AsmProgram AsmProgram;
typedef struct BackEndContext BackEndContext;
typedef struct FrontEndContext FrontEndContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Register allocation

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Register allocation
// Register coalescing

void allocate_registers(AsmProgram* node, BackEndContext* backend, FrontEndContext* frontend, uint8_t optim_2_code);

#endif
