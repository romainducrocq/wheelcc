#include "ast/back_symt.hpp"
#include "ast/ast.hpp"

#include <memory>
#include <unordered_map>

AST_T AssemblyType::type() { return AST_T::AssemblyType_t; }
AST_T LongWord::type() { return AST_T::LongWord_t; }
AST_T QuadWord::type() { return AST_T::QuadWord_t; }
AST_T BackendDouble::type() { return AST_T::BackendDouble_t; }
AST_T BackendSymbol::type() { return AST_T::BackendSymbol_t; }
AST_T BackendObj::type() { return AST_T::BackendObj_t; }
AST_T BackendFun::type() { return AST_T::BackendFun_t; }

BackendObj::BackendObj(bool is_static, bool is_constant, std::shared_ptr<AssemblyType> assembly_type)
    : is_static(is_static), is_constant(is_constant), assembly_type(std::move(assembly_type)) {}

BackendFun::BackendFun(bool is_defined)
    : is_defined(is_defined) {}

std::unordered_map<TIdentifier, std::unique_ptr<BackendSymbol>> backend_symbol_table;
