#ifndef _ASSEMBLY_ST_CONV_HPP
#define _ASSEMBLY_ST_CONV_HPP

#include "ast/ast.hpp"
#include "ast/backend_st.hpp"
#include "ast/asm_ast.hpp"

#include <memory>

std::shared_ptr<AssemblyType> convert_backend_assembly_type(const TIdentifier& name);
void convert_symbol_table(AsmProgram* node);

#endif
