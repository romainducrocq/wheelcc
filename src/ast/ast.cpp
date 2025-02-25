#include <memory>
#include <vector>

#include "ast/ast.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Abstract syntax tree

Ast::~Ast() {}

AST_T CConst::type() { return AST_T::CConst_t; }
AST_T CConstInt::type() { return AST_T::CConstInt_t; }
AST_T CConstLong::type() { return AST_T::CConstLong_t; }
AST_T CConstUInt::type() { return AST_T::CConstUInt_t; }
AST_T CConstULong::type() { return AST_T::CConstULong_t; }
AST_T CConstDouble::type() { return AST_T::CConstDouble_t; }
AST_T CConstChar::type() { return AST_T::CConstChar_t; }
AST_T CConstUChar::type() { return AST_T::CConstUChar_t; }
AST_T CStringLiteral::type() { return AST_T::CStringLiteral_t; }

CConstInt::CConstInt(TInt value) : value(value) {}

CConstLong::CConstLong(TLong value) : value(value) {}

CConstUInt::CConstUInt(TUInt value) : value(value) {}

CConstULong::CConstULong(TULong value) : value(value) {}

CConstDouble::CConstDouble(TDouble value) : value(value) {}

CConstChar::CConstChar(TChar value) : value(value) {}

CConstUChar::CConstUChar(TUChar value) : value(value) {}

CStringLiteral::CStringLiteral(std::vector<TChar> value) : value(std::move(value)) {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

IdentifierContext::IdentifierContext() : label_counter(0), variable_counter(0), structure_counter(0) {}

std::unique_ptr<IdentifierContext> identifiers;
