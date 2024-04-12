#include "ast/front_symt.hpp"
#include "ast/ast.hpp"

#include <inttypes.h>
#include <memory>
#include <vector>
#include <unordered_map>

AST_T Type::type() { return AST_T::Type_t; }
AST_T Char::type() { return AST_T::Char_t; }
AST_T SChar::type() { return AST_T::SChar_t; }
AST_T UChar::type() { return AST_T::UChar_t; }
AST_T Int::type() { return AST_T::Int_t; }
AST_T Long::type() { return AST_T::Long_t; }
AST_T UInt::type() { return AST_T::UInt_t; }
AST_T ULong::type() { return AST_T::ULong_t; }
AST_T Double::type() { return AST_T::Double_t; }
AST_T Void::type() { return AST_T::Void_t; }
AST_T FunType::type() { return AST_T::FunType_t; }
AST_T Pointer::type() { return AST_T::Pointer_t; }
AST_T Array::type() { return AST_T::Array_t; }
AST_T Structure::type() { return AST_T::Structure_t; }
AST_T StaticInit::type() { return AST_T::StaticInit_t; }
AST_T IntInit::type() { return AST_T::IntInit_t; }
AST_T LongInit::type() { return AST_T::LongInit_t; }
AST_T UIntInit::type() { return AST_T::UIntInit_t; }
AST_T ULongInit::type() { return AST_T::ULongInit_t; }
AST_T CharInit::type() { return AST_T::CharInit_t; }
AST_T UCharInit::type() { return AST_T::UCharInit_t; }
AST_T DoubleInit::type() { return AST_T::DoubleInit_t; }
AST_T ZeroInit::type() { return AST_T::ZeroInit_t; }
AST_T StringInit::type() { return AST_T::StringInit_t; }
AST_T PointerInit::type() { return AST_T::PointerInit_t; }
AST_T InitialValue::type() { return AST_T::InitialValue_t; }
AST_T Tentative::type() { return AST_T::Tentative_t; }
AST_T Initial::type() { return AST_T::Initial_t; }
AST_T NoInitializer::type() { return AST_T::NoInitializer_t; }
AST_T IdentifierAttr::type() { return AST_T::IdentifierAttr_t; }
AST_T FunAttr::type() { return AST_T::FunAttr_t; }
AST_T StaticAttr::type() { return AST_T::StaticAttr_t; }
AST_T ConstantAttr::type() { return AST_T::ConstantAttr_t; }
AST_T LocalAttr::type() { return AST_T::LocalAttr_t; }
AST_T Symbol::type() { return AST_T::Symbol_t; }
AST_T StructMember::type() { return AST_T::StructMember_t; }
AST_T StructTypedef::type() { return AST_T::StructTypedef_t; }

FunType::FunType(std::vector<std::shared_ptr<Type>> param_types, std::shared_ptr<Type> ret_type)
    : param_types(std::move(param_types)), ret_type(std::move(ret_type)) {}

Pointer::Pointer(std::shared_ptr<Type> ref_type)
    : ref_type(std::move(ref_type)) {}

Array::Array(TLong size, std::shared_ptr<Type> elem_type)
    : size(size), elem_type(std::move(elem_type)) {}

Structure::Structure(TIdentifier tag)
    : tag(std::move(tag)) {}

IntInit::IntInit(TInt value)
    : value(value) {}

LongInit::LongInit(TLong value)
    : value(value) {}

UIntInit::UIntInit(TUInt value)
    : value(value) {}

ULongInit::ULongInit(TULong value)
    : value(value) {}

CharInit::CharInit(TChar value)
    : value(value) {}

UCharInit::UCharInit(TUChar value)
    : value(value) {}

DoubleInit::DoubleInit(TDouble value, TULong binary)
    : value(value), binary(binary) {}

ZeroInit::ZeroInit(TLong byte)
    : byte(byte) {}

StringInit::StringInit(bool is_null_terminated, TIdentifier string_constant,
                       std::shared_ptr<CStringLiteral> literal)
    : is_null_terminated(is_null_terminated), string_constant(std::move(string_constant)),
      literal(std::move(literal)) {}

PointerInit::PointerInit(TIdentifier name)
    : name(std::move(name)) {}

Initial::Initial(std::vector<std::shared_ptr<StaticInit>> static_inits)
    : static_inits(std::move(static_inits)) {}

FunAttr::FunAttr(bool is_defined, bool is_global)
    : is_defined(is_defined), is_global(is_global) {}

StaticAttr::StaticAttr(bool is_global, std::shared_ptr<InitialValue> init)
    : is_global(is_global), init(std::move(init)) {}

ConstantAttr::ConstantAttr(std::shared_ptr<StaticInit> static_init)
    : static_init(std::move(static_init)) {}

Symbol::Symbol(std::shared_ptr<Type> type_t, std::unique_ptr<IdentifierAttr> attrs)
    : type_t(std::move(type_t)), attrs(std::move(attrs)) {}

StructMember::StructMember(TLong offset, std::shared_ptr<Type> member_type)
    : offset(offset), member_type(std::move(member_type)) {}

StructTypedef::StructTypedef(TInt alignment, TLong size, std::vector<TIdentifier> member_names,
                             std::unordered_map<TIdentifier, std::unique_ptr<StructMember>> members)
    : alignment(alignment), size(size), member_names(std::move(member_names)), members(std::move(members)) {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<FrontEndContext> frontend;

FrontEndContext::FrontEndContext()
    : label_counter(0), variable_counter(0), structure_counter(0) {}

std::unique_ptr<std::unordered_map<TIdentifier, std::unique_ptr<Symbol>>> symbol_table;
