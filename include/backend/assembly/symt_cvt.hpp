#ifndef _BACKEND_ASSEMBLY_SYMT_CVT_HPP
#define _BACKEND_ASSEMBLY_SYMT_CVT_HPP

#include <memory>

#include "ast/ast.hpp"

struct Type;
struct AssemblyType;
struct AsmProgram;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Symbol table conversion

TInt generate_type_alignment(Type* type);
std::shared_ptr<AssemblyType> convert_backend_assembly_type(TIdentifier name);
void convert_symbol_table(AsmProgram* node);

#endif
