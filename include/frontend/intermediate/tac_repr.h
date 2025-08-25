#ifndef _FRONT_INTERMEDIATE_TAC_REPR_H
#define _FRONT_INTERMEDIATE_TAC_REPR_H

#include <memory>

typedef struct CProgram CProgram;
typedef struct TacProgram TacProgram;
typedef struct FrontEndContext FrontEndContext;
typedef struct IdentifierContext IdentifierContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Three address code representation

#ifdef __cplusplus
extern "C" {
#endif
std::unique_ptr<TacProgram> represent_three_address_code(
    std::unique_ptr<CProgram>* c_ast, FrontEndContext* frontend, IdentifierContext* identifiers);
#ifdef __cplusplus
}
#endif

#endif
