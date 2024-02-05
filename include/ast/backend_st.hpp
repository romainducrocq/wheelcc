#ifndef _AST_BACKEND_ST_HPP
#define _AST_BACKEND_ST_HPP

#include "ast/ast.hpp"

#include <memory>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// assembly_type = LongWord
//               | QuadWord
//               | BackendDouble
struct AssemblyType : Ast {
    AST_T type() override;
};

struct LongWord : AssemblyType {
    AST_T type() override;
};

struct QuadWord : AssemblyType {
    AST_T type() override;
};

struct BackendDouble : AssemblyType {
    AST_T type() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// symbol = Obj(type assembly_type, bool is_static, bool is_constant)
//        | Fun(bool defined)
struct BackendSymbol : Ast {
    AST_T type() override;
};

struct BackendObj: BackendSymbol {
    AST_T type() override;
    BackendObj() = default;
    BackendObj(bool is_static, bool is_constant, std::shared_ptr<AssemblyType> assembly_type);

    bool is_static;
    bool is_constant;
    std::shared_ptr<AssemblyType> assembly_type;
};

struct BackendFun: BackendSymbol {
    AST_T type() override;
    BackendFun() = default;
    BackendFun(bool is_defined);

    bool is_defined;
};

/*
struct Dummy : Ast {
};
*/

extern std::unordered_map<TIdentifier, std::unique_ptr<BackendSymbol>> backend_symbol_table;

#endif
