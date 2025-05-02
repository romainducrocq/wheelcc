#ifndef _BACKEND_ASSEMBLY_SYMT_CVT_HPP
#define _BACKEND_ASSEMBLY_SYMT_CVT_HPP

#include <memory>

#include "ast_t.hpp" // ast

struct Type;
struct AssemblyType;
struct AsmProgram;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Symbol table conversion

TInt gen_type_alignment(Type* type);
std::shared_ptr<AssemblyType> cvt_backend_asm_type(TIdentifier name);
void symbol_table_conversion(AsmProgram* node);

#endif
