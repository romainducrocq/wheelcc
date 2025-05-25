#include <memory>
#include <unordered_map>
#include <vector>

#include "ast/ast.hpp"
#include "ast/front_symt.hpp"

#include "assembly/regs.hpp" // backend

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Frontend symbol table

AST_T Type::type() { return AST_Type_t; }
AST_T Char::type() { return AST_Char_t; }
AST_T SChar::type() { return AST_SChar_t; }
AST_T UChar::type() { return AST_UChar_t; }
AST_T Int::type() { return AST_Int_t; }
AST_T Long::type() { return AST_Long_t; }
AST_T UInt::type() { return AST_UInt_t; }
AST_T ULong::type() { return AST_ULong_t; }
AST_T Double::type() { return AST_Double_t; }
AST_T Void::type() { return AST_Void_t; }
AST_T FunType::type() { return AST_FunType_t; }
AST_T Pointer::type() { return AST_Pointer_t; }
AST_T Array::type() { return AST_Array_t; }
AST_T Structure::type() { return AST_Structure_t; }
AST_T StaticInit::type() { return AST_StaticInit_t; }
AST_T IntInit::type() { return AST_IntInit_t; }
AST_T LongInit::type() { return AST_LongInit_t; }
AST_T UIntInit::type() { return AST_UIntInit_t; }
AST_T ULongInit::type() { return AST_ULongInit_t; }
AST_T CharInit::type() { return AST_CharInit_t; }
AST_T UCharInit::type() { return AST_UCharInit_t; }
AST_T DoubleInit::type() { return AST_DoubleInit_t; }
AST_T ZeroInit::type() { return AST_ZeroInit_t; }
AST_T StringInit::type() { return AST_StringInit_t; }
AST_T PointerInit::type() { return AST_PointerInit_t; }
AST_T InitialValue::type() { return AST_InitialValue_t; }
AST_T Tentative::type() { return AST_Tentative_t; }
AST_T Initial::type() { return AST_Initial_t; }
AST_T NoInitializer::type() { return AST_NoInitializer_t; }
AST_T IdentifierAttr::type() { return AST_IdentifierAttr_t; }
AST_T FunAttr::type() { return AST_FunAttr_t; }
AST_T StaticAttr::type() { return AST_StaticAttr_t; }
AST_T ConstantAttr::type() { return AST_ConstantAttr_t; }
AST_T LocalAttr::type() { return AST_LocalAttr_t; }
AST_T Symbol::type() { return AST_Symbol_t; }
AST_T StructMember::type() { return AST_StructMember_t; }
AST_T StructTypedef::type() { return AST_StructTypedef_t; }

FunType::FunType(std::vector<std::shared_ptr<Type>>&& param_types, std::shared_ptr<Type> ret_type) :
    param_reg_mask(NULL_REGISTER_MASK), ret_reg_mask(NULL_REGISTER_MASK), param_types(std::move(param_types)),
    ret_type(std::move(ret_type)) {}

Pointer::Pointer(std::shared_ptr<Type> ref_type) : ref_type(std::move(ref_type)) {}

Array::Array(TLong size, std::shared_ptr<Type> elem_type) : size(size), elem_type(std::move(elem_type)) {}

Structure::Structure(TIdentifier tag, bool is_union) : tag(tag), is_union(is_union) {}

IntInit::IntInit(TInt value) : value(value) {}

LongInit::LongInit(TLong value) : value(value) {}

UIntInit::UIntInit(TUInt value) : value(value) {}

ULongInit::ULongInit(TULong value) : value(value) {}

CharInit::CharInit(TChar value) : value(value) {}

UCharInit::UCharInit(TUChar value) : value(value) {}

DoubleInit::DoubleInit(TIdentifier dbl_const) : dbl_const(dbl_const) {}

ZeroInit::ZeroInit(TLong byte) : byte(byte) {}

StringInit::StringInit(TIdentifier string_const, bool is_null_term, std::shared_ptr<CStringLiteral> literal) :
    string_const(string_const), is_null_term(is_null_term), literal(std::move(literal)) {}

PointerInit::PointerInit(TIdentifier name) : name(name) {}

Initial::Initial(std::vector<std::shared_ptr<StaticInit>>&& static_inits) : static_inits(std::move(static_inits)) {}

FunAttr::FunAttr(bool is_def, bool is_glob) : is_def(is_def), is_glob(is_glob) {}

StaticAttr::StaticAttr(bool is_glob, std::shared_ptr<InitialValue> init) : is_glob(is_glob), init(std::move(init)) {}

ConstantAttr::ConstantAttr(std::shared_ptr<StaticInit> static_init) : static_init(std::move(static_init)) {}

Symbol::Symbol(std::shared_ptr<Type> type_t, std::unique_ptr<IdentifierAttr>&& attrs) :
    type_t(std::move(type_t)), attrs(std::move(attrs)) {}

StructMember::StructMember(TLong offset, std::shared_ptr<Type> member_type) :
    offset(offset), member_type(std::move(member_type)) {}

StructTypedef::StructTypedef(TInt alignment, TLong size, std::vector<TIdentifier>&& member_names,
    std::unordered_map<TIdentifier, std::unique_ptr<StructMember>>&& members) :
    alignment(alignment),
    size(size), member_names(std::move(member_names)), members(std::move(members)) {}
