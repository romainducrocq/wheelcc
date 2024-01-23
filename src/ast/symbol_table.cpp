#include "ast/symbol_table.hpp"
#include "ast/ast.hpp"

#include <memory>
#include <vector>

AST_T Type::type() { return AST_T::Type_t; }
AST_T Int::type() { return AST_T::Int_t; }
AST_T Long::type() { return AST_T::Long_t; }
AST_T Double::type() { return AST_T::Double_t; }
AST_T UInt::type() { return AST_T::UInt_t; }
AST_T ULong::type() { return AST_T::ULong_t; }
AST_T FunType::type() { return AST_T::FunType_t; }
AST_T StaticInit::type() { return AST_T::StaticInit_t; }
AST_T IntInit::type() { return AST_T::IntInit_t; }
AST_T LongInit::type() { return AST_T::LongInit_t; }
AST_T DoubleInit::type() { return AST_T::DoubleInit_t; }
AST_T UIntInit::type() { return AST_T::UIntInit_t; }
AST_T ULongInit::type() { return AST_T::ULongInit_t; }
AST_T InitialValue::type() { return AST_T::InitialValue_t; }
AST_T Tentative::type() { return AST_T::Tentative_t; }
AST_T Initial::type() { return AST_T::Initial_t; }
AST_T NoInitializer::type() { return AST_T::NoInitializer_t; }
AST_T IdentifierAttr::type() { return AST_T::IdentifierAttr_t; }
AST_T FunAttr::type() { return AST_T::FunAttr_t; }
AST_T StaticAttr::type() { return AST_T::StaticAttr_t; }
AST_T LocalAttr::type() { return AST_T::LocalAttr_t; }
AST_T Symbol::type() { return AST_T::Symbol_t; }

FunType::FunType(std::vector<std::unique_ptr<Type>>&& param_types, std::unique_ptr<Type>&& ret_type)
    : param_types(std::move(param_types)), ret_type(std::move(ret_type)) {}

IntInit::IntInit(TInt value)
    : value(value) {}

LongInit::LongInit(TLong value)
    : value(value) {}

DoubleInit::DoubleInit(TDouble value)
    : value(value) {}

UIntInit::UIntInit(TUInt value)
    : value(value) {}

ULongInit::ULongInit(TULong value)
    : value(value) {}

Initial::Initial(std::unique_ptr<StaticInit>&& static_init)
    : static_init(std::move(static_init)) {}

FunAttr::FunAttr(bool is_defined, bool is_global)
    : is_defined(is_defined), is_global(is_global) {}

StaticAttr::StaticAttr(std::unique_ptr<InitialValue>&& init, bool is_global)
    : init(std::move(init)), is_global(is_global) {}

Symbol::Symbol(std::unique_ptr<Type>&& type_t, std::unique_ptr<IdentifierAttr>&& attrs)
    : type_t(std::move(type_t)), attrs(std::move(attrs)) {}

std::unordered_map<std::string, std::unique_ptr<Symbol>> symbol_table;
