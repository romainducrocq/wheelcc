#ifndef _AST_FRONT_SYMT_H
#define _AST_FRONT_SYMT_H

#include <inttypes.h>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ast/ast.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Frontend symbol table

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// type = Char
//      | SChar
//      | UChar
//      | Int
//      | Long
//      | UInt
//      | ULong
//      | Double
//      | Void
//      | FunType(int, int, type*, type)
//      | Pointer(type)
//      | Array(int, type)
//      | Structure(identifier, bool)
struct Type : Ast {
    AST_T type() override;
};

struct Char : Type {
    AST_T type() override;
};

struct SChar : Type {
    AST_T type() override;
};

struct UChar : Type {
    AST_T type() override;
};

struct Int : Type {
    AST_T type() override;
};

struct Long : Type {
    AST_T type() override;
};

struct UInt : Type {
    AST_T type() override;
};

struct ULong : Type {
    AST_T type() override;
};

struct Double : Type {
    AST_T type() override;
};

struct Void : Type {
    AST_T type() override;
};

struct FunType : Type {
    AST_T type() override;
    FunType() = default;
    FunType(std::vector<std::shared_ptr<Type>> param_types, std::shared_ptr<Type> ret_type);

    TULong param_reg_mask;
    TULong ret_reg_mask;
    std::vector<std::shared_ptr<Type>> param_types;
    std::shared_ptr<Type> ret_type;
};

struct Pointer : Type {
    AST_T type() override;
    Pointer() = default;
    Pointer(std::shared_ptr<Type> ref_type);

    std::shared_ptr<Type> ref_type;
};

struct Array : Type {
    AST_T type() override;
    Array() = default;
    Array(TLong size, std::shared_ptr<Type> elem_type);

    TLong size;
    std::shared_ptr<Type> elem_type;
};

struct Structure : Type {
    AST_T type() override;
    Structure() = default;
    Structure(TIdentifier tag, bool is_union);

    TIdentifier tag;
    bool is_union;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// static_init = IntInit(int)
//             | LongInit(int)
//             | UIntInit(int)
//             | ULongInit(int)
//             | CharInit(int)
//             | UCharInit(int)
//             | DoubleInit(string)
//             | ZeroInit(int)
//             | StringInit(identifier, bool, string)
//             | PointerInit(identifier)
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

struct CharInit : StaticInit {
    AST_T type() override;
    CharInit() = default;
    CharInit(TChar value);

    TChar value;
};

struct UCharInit : StaticInit {
    AST_T type() override;
    UCharInit() = default;
    UCharInit(TUChar value);

    TUChar value;
};

struct DoubleInit : StaticInit {
    AST_T type() override;
    DoubleInit() = default;
    DoubleInit(TIdentifier dbl_const);

    TIdentifier dbl_const;
};

struct ZeroInit : StaticInit {
    AST_T type() override;
    ZeroInit() = default;
    ZeroInit(TLong byte);

    TLong byte;
};

struct StringInit : StaticInit {
    AST_T type() override;
    StringInit() = default;
    StringInit(TIdentifier string_const, bool is_null_term, std::shared_ptr<CStringLiteral> literal);

    TIdentifier string_const;
    bool is_null_term;
    std::shared_ptr<CStringLiteral> literal;
};

struct PointerInit : StaticInit {
    AST_T type() override;
    PointerInit() = default;
    PointerInit(TIdentifier name);

    TIdentifier name;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// initial_value = Tentative
//               | Initial(static_init*)
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
    Initial(std::vector<std::shared_ptr<StaticInit>> static_inits);

    std::vector<std::shared_ptr<StaticInit>> static_inits;
};

struct NoInitializer : InitialValue {
    AST_T type() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// identifier_attrs = FunAttr(bool, bool)
//                  | StaticAttr(initial_value, bool)
//                  | ConstantAttr(static_init)
//                  | LocalAttr
struct IdentifierAttr : Ast {
    AST_T type() override;
};

struct FunAttr : IdentifierAttr {
    AST_T type() override;
    FunAttr() = default;
    FunAttr(bool is_def, bool is_glob);

    bool is_def;
    bool is_glob;
};

struct StaticAttr : IdentifierAttr {
    AST_T type() override;
    StaticAttr() = default;
    StaticAttr(bool is_glob, std::shared_ptr<InitialValue> init);

    bool is_glob;
    std::shared_ptr<InitialValue> init;
};

struct ConstantAttr : IdentifierAttr {
    AST_T type() override;
    ConstantAttr() = default;
    ConstantAttr(std::shared_ptr<StaticInit> static_init);

    std::shared_ptr<StaticInit> static_init;
};

struct LocalAttr : IdentifierAttr {
    AST_T type() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// symbol = Symbol(type, identifier_attrs)
struct Symbol : Ast {
    AST_T type() override;
    Symbol() = default;
    Symbol(std::shared_ptr<Type> type_t, std::unique_ptr<IdentifierAttr>&& attrs);

    std::shared_ptr<Type> type_t;
    std::unique_ptr<IdentifierAttr> attrs;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// struct_member = StructMember(int, type)
struct StructMember : Ast {
    AST_T type() override;
    StructMember() = default;
    StructMember(TLong offset, std::shared_ptr<Type> member_type);

    TLong offset;
    std::shared_ptr<Type> member_type;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// struct_typedef(int, int, identifier*, struct_member*)
struct StructTypedef : Ast {
    AST_T type() override;
    StructTypedef() = default;
    StructTypedef(TInt alignment, TLong size, std::vector<TIdentifier> member_names,
        std::unordered_map<TIdentifier, std::unique_ptr<StructMember>> members);

    TInt alignment;
    TLong size;
    std::vector<TIdentifier> member_names;
    std::unordered_map<TIdentifier, std::unique_ptr<StructMember>> members;
};

/*
struct Dummy : Ast {
};
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct FrontEndContext {
    std::unordered_map<TIdentifier, TIdentifier> string_const_table;
    std::unordered_map<TIdentifier, std::unique_ptr<StructTypedef>> struct_typedef_table;
    std::unordered_map<TIdentifier, std::unique_ptr<Symbol>> symbol_table;
    std::unordered_set<TIdentifier> addressed_set;
};

#define GET_STRUCT_TYPEDEF_MEMBER(X, Y) \
    ctx->frontend->struct_typedef_table[X]->members[ctx->frontend->struct_typedef_table[X]->member_names[Y]]
#define GET_STRUCT_TYPEDEF_BACK(X) \
    ctx->frontend->struct_typedef_table[X]->members[ctx->frontend->struct_typedef_table[X]->member_names.back()]

#endif
