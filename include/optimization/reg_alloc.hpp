#ifndef _OPTIMIZATION_REG_ALLOC_HPP
#define _OPTIMIZATION_REG_ALLOC_HPP

#include <inttypes.h>

#include "ast/back_ast.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Register allocation

struct RegAllocContext {};

void register_allocation(AsmProgram* node, uint8_t optim_2_code);

#endif
