#include <memory>
#include <string>
#include <vector>

#include "ast/ast.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Abstract syntax tree

Ast::~Ast() {}

AST_T CConst::type() { return AST_CConst_t; }
AST_T CConstInt::type() { return AST_CConstInt_t; }
AST_T CConstLong::type() { return AST_CConstLong_t; }
AST_T CConstUInt::type() { return AST_CConstUInt_t; }
AST_T CConstULong::type() { return AST_CConstULong_t; }
AST_T CConstDouble::type() { return AST_CConstDouble_t; }
AST_T CConstChar::type() { return AST_CConstChar_t; }
AST_T CConstUChar::type() { return AST_CConstUChar_t; }
AST_T CStringLiteral::type() { return AST_CStringLiteral_t; }

CConstInt::CConstInt(TInt value) : value(value) {}

CConstLong::CConstLong(TLong value) : value(value) {}

CConstUInt::CConstUInt(TUInt value) : value(value) {}

CConstULong::CConstULong(TULong value) : value(value) {}

CConstDouble::CConstDouble(TDouble value) : value(value) {}

CConstChar::CConstChar(TChar value) : value(value) {}

CConstUChar::CConstUChar(TUChar value) : value(value) {}

CStringLiteral::CStringLiteral(std::vector<TChar> value) : value(std::move(value)) {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Identifiers

typedef IdentifierContext* Ctx;

TIdentifier string_to_hash(const std::string& string);

TIdentifier make_string_identifier(Ctx ctx, std::string&& value) {
    TIdentifier identifier = string_to_hash(value);
    ctx->hash_table[identifier] = std::move(value);
    return identifier;
}

TIdentifier make_label_identifier(Ctx ctx, std::string&& name) {
    name += UID_SEPARATOR;
    name += std::to_string(ctx->label_count);
    ctx->label_count++;
    return make_string_identifier(ctx, std::move(name));
}

TIdentifier make_var_identifier(Ctx ctx, std::string&& name) {
    name += UID_SEPARATOR;
    name += std::to_string(ctx->var_count);
    ctx->var_count++;
    return make_string_identifier(ctx, std::move(name));
}

TIdentifier make_struct_identifier(Ctx ctx, std::string&& name) {
    name += UID_SEPARATOR;
    name += std::to_string(ctx->struct_count);
    ctx->struct_count++;
    return make_string_identifier(ctx, std::move(name));
}
