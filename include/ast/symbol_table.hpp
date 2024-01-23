#ifndef _AST_SYMBOL_TABLE_HPP
#define _AST_SYMBOL_TABLE_HPP

#include "ast/ast.hpp"

#include <memory>
#include <vector>
#include <unordered_map>


// type = Int
//      | Long
//      | Double
//      | UInt
//      | ULong
//      | FunType(type*, type)
struct Type : Ast {
    AST_T type() override;
};

struct Int : Type {
    AST_T type() override;
};

struct Long : Type {
    AST_T type() override;
};

struct Double : Type {
    AST_T type() override;
};

struct UInt : Type {
    AST_T type() override;
};

struct ULong : Type {
    AST_T type() override;
};

struct FunType : Type {
    AST_T type() override;
    FunType() = default;
    FunType(std::vector<std::unique_ptr<Type>>&& param_types, std::unique_ptr<Type>&& ret_type);

    std::vector<std::unique_ptr<Type>> param_types;
    std::unique_ptr<Type> ret_type;
};

// static_init = IntInit(int)
//             | LongInit(int)
//             | DoubleInit(double)
//             | UIntInit(int)
//             | ULongInit(int)
struct StaticInit : Ast {
    AST_T type() override;
};

struct IntInit : StaticInit {
    AST_T type() override;
    IntInit() = default;
    IntInit(TInt value);

    TInt value;
};

struct LongInit : StaticInit {
    AST_T type() override;
    LongInit() = default;
    LongInit(TLong value);

    TLong value;
};

struct DoubleInit : StaticInit {
    AST_T type() override;
    DoubleInit() = default;
    DoubleInit(TDouble value);

    TDouble value;
};

struct UIntInit : StaticInit {
    AST_T type() override;
    UIntInit() = default;
    UIntInit(TUInt value);

    TUInt value;
};

struct ULongInit : StaticInit {
    AST_T type() override;
    ULongInit() = default;
    ULongInit(TULong value);

    TULong value;
};

// initial_value = Tentative
//               | Initial(static_init)
//               | NoInitializer
struct InitialValue : Ast {
    AST_T type() override;
};

struct Tentative : InitialValue {
    AST_T type() override;
};

struct Initial : InitialValue {
    AST_T type() override;
    Initial() = default;
    Initial(std::unique_ptr<StaticInit>&& static_init);

    std::unique_ptr<StaticInit> static_init;
};

struct NoInitializer : InitialValue {
    AST_T type() override;
};

// identifier_attrs = FunAttr(bool defined, bool global)
//                  | StaticAttr(initial_value init, bool global)
//                  | LocalAttr
struct IdentifierAttr : Ast {
    AST_T type() override;
};

struct FunAttr : IdentifierAttr {
    AST_T type() override;
    FunAttr() = default;
    FunAttr(bool is_defined, bool is_global);

    bool is_defined;
    bool is_global;
};

struct StaticAttr : IdentifierAttr {
    AST_T type() override;
    StaticAttr() = default;
    StaticAttr(std::unique_ptr<InitialValue>&& init, bool is_global);

    std::unique_ptr<InitialValue> init;
    bool is_global;
};

struct LocalAttr : IdentifierAttr {
    AST_T type() override;
};

// symbol = Symbol(type type_t, identifier_attrs attrs)
struct Symbol : Ast {
    AST_T type() override;
    Symbol() = default;
    Symbol(std::unique_ptr<Type>&& type_t, std::unique_ptr<IdentifierAttr>&& attrs);

    std::unique_ptr<Type> type_t;
    std::unique_ptr<IdentifierAttr> attrs;
};

extern std::unordered_map<std::string, std::unique_ptr<Symbol>> symbol_table;

#endif