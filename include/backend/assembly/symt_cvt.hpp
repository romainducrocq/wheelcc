#ifndef _BACKEND_ASSEMBLY_SYMT_CVT_HPP
#define _BACKEND_ASSEMBLY_SYMT_CVT_HPP

#include "ast/ast.hpp"
#include "ast/front_symt.hpp"
#include "ast/back_symt.hpp"
#include "ast/back_ast.hpp"

#include <memory>

TInt generate_type_alignment(Type* type_1);
std::shared_ptr<AssemblyType> convert_backend_assembly_type(const TIdentifier& name);
void convert_symbol_table(AsmProgram* node);

#endif
