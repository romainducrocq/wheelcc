#ifndef _FRONT_INTERMEDIATE_TAC_REPR_H
#define _FRONT_INTERMEDIATE_TAC_REPR_H

#include <memory>

struct CProgram;
struct TacProgram;
struct IdentifierContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Three address code representation

std::unique_ptr<TacProgram> represent_three_address_code(
    std::unique_ptr<CProgram> c_ast, IdentifierContext* identifiers);

#endif
