#include "ast/ast.hpp"

Ast::~Ast() {}

AST_T CConst::type() { return AST_T::CConst_t; }
AST_T CConstInt::type() { return AST_T::CConstInt_t; }
AST_T CConstLong::type() { return AST_T::CConstLong_t; }
AST_T CConstDouble::type() { return AST_T::CConstDouble_t; }
AST_T CConstUInt::type() { return AST_T::CConstUInt_t; }
AST_T CConstULong::type() { return AST_T::CConstULong_t; }

CConstInt::CConstInt(TInt value)
    : value(value) {}

CConstLong::CConstLong(TLong value)
    : value(value) {}

CConstDouble::CConstDouble(TDouble value)
    : value(value) {}

CConstUInt::CConstUInt(TUInt value)
    : value(value) {}

CConstULong::CConstULong(TULong value)
    : value(value) {}
