#ifndef _BACK_ASSEMBLY_SYMT_CVT_H
#define _BACK_ASSEMBLY_SYMT_CVT_H

#include <memory>

#include "ast_t.hpp" // ast

struct Type;
struct AssemblyType;
struct AsmProgram;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Symbol table conversion

TInt gen_type_alignment(Type* type);
std::shared_ptr<AssemblyType> cvt_backend_asm_type(TIdentifier name);
void convert_symbol_table(AsmProgram* node);

#endif
