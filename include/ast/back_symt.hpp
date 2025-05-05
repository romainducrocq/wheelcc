#ifndef _AST_BACK_SYMT_H
#define _AST_BACK_SYMT_H

#include <memory>
#include <unordered_map>

#include "ast/ast.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Backend symbol table

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// assembly_type = Byte
//               | LongWord
//               | QuadWord
//               | BackendDouble
//               | ByteArray(int, int)
struct AssemblyType : Ast {
    AST_T type() override;
};

struct Byte : AssemblyType {
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

struct ByteArray : AssemblyType {
    AST_T type() override;
    ByteArray() = default;
    ByteArray(TLong size, TInt alignment);

    TLong size;
    TInt alignment;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// symbol = Obj(assembly_type, bool, bool)
//        | Fun(bool, operand*)
struct BackendSymbol : Ast {
    AST_T type() override;
};

struct BackendObj : BackendSymbol {
    AST_T type() override;
    BackendObj() = default;
    BackendObj(bool is_static, bool is_constant, std::shared_ptr<AssemblyType> assembly_type);

    bool is_static;
    bool is_constant;
    std::shared_ptr<AssemblyType> assembly_type;
};

struct AsmOperand;
struct BackendFun : BackendSymbol {
    AST_T type() override;
    BackendFun() = default;
    BackendFun(bool is_defined);

    bool is_defined;
    std::vector<std::shared_ptr<AsmOperand>> callee_saved_registers;
};

/*
struct Dummy : Ast {
};
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct BackEndContext {
    std::unordered_map<TIdentifier, std::unique_ptr<BackendSymbol>> backend_symbol_table;
};

extern std::unique_ptr<BackEndContext> backend;
#define INIT_BACKEND_CTX backend = std::make_unique<BackEndContext>()
#define FREE_BACKEND_CTX backend.reset()

#endif
