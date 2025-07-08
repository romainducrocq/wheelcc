#include <vector>

#include "util/c_std.hpp"

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

CStringLiteral::CStringLiteral(std::vector<TChar>&& value) : value(std::move(value)) {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Identifiers

typedef IdentifierContext* Ctx;

TIdentifier string_to_hash(const char* string);

TIdentifier make_string_identifier(Ctx ctx, string_t* value) {
    TIdentifier identifier = string_to_hash(*value);
    if (ctx->hash_table.find(identifier) == ctx->hash_table.end()) {
        ctx->hash_table[identifier] = str_new(NULL);
        str_move(value, &ctx->hash_table[identifier]);
    }
    else {
        str_delete(*value);
    }
    return identifier;
}

TIdentifier make_label_identifier(Ctx ctx, string_t* name) {
    str_append(*name, UID_SEPARATOR);
    {
        string_t strto_uid = str_to_string(ctx->label_count);
        str_append(*name, strto_uid);
        str_delete(strto_uid);
    }
    ctx->label_count++;
    return make_string_identifier(ctx, name);
}

TIdentifier make_var_identifier(Ctx ctx, string_t* name) {
    str_append(*name, UID_SEPARATOR);
    {
        string_t strto_uid = str_to_string(ctx->var_count);
        str_append(*name, strto_uid);
        str_delete(strto_uid);
    }
    ctx->var_count++;
    return make_string_identifier(ctx, name);
}

TIdentifier make_struct_identifier(Ctx ctx, string_t* name) {
    str_append(*name, UID_SEPARATOR);
    {
        string_t strto_uid = str_to_string(ctx->struct_count);
        str_append(*name, strto_uid);
        str_delete(strto_uid);
    }
    ctx->struct_count++;
    return make_string_identifier(ctx, name);
}
