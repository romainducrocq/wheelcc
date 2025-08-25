#ifndef _AST_BACK_SYMT_H
#define _AST_BACK_SYMT_H

#include <memory>

#include "util/c_std.hpp"

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
    BackendObj(bool is_static, bool is_const, std::shared_ptr<AssemblyType>&& asm_type);

    bool is_static;
    bool is_const;
    std::shared_ptr<AssemblyType> asm_type;
};

struct AsmOperand;
struct BackendFun : BackendSymbol {
    AST_T type() override;
    BackendFun();
    BackendFun(bool is_def);
    ~BackendFun();

    bool is_def;
    vector_t(std::shared_ptr<AsmOperand>) callee_saved_regs;
};

/*
struct Dummy : Ast {
};
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef std::unique_ptr<BackendSymbol> UPtrBackendSymbol;
PairKeyValue(TIdentifier, UPtrBackendSymbol);

typedef struct BackEndContext {
    hashmap_t(TIdentifier, UPtrBackendSymbol) symbol_table;
} BackEndContext;

#endif
