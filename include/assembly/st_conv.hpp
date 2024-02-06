#ifndef _ASSEMBLY_ST_CONV_HPP
#define _ASSEMBLY_ST_CONV_HPP

#include "ast/ast.hpp"
#include "ast/backend_st.hpp"
#include "ast/asm_ast.hpp"

#include <memory>
#include <vector>

extern std::vector<std::unique_ptr<AsmTopLevel>> static_constant_top_levels;

std::shared_ptr<AssemblyType> convert_backend_assembly_type(const TIdentifier& name);
void convert_symbol_table();

#endif
