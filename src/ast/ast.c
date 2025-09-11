#include "util/c_std.h"
#include "util/throw.h"

#include "ast/ast.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Abstract syntax tree

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

shared_ptr_t(CConst) make_CConst(void) {
    shared_ptr_t(CConst) self = sptr_new();
    sptr_alloc(CConst, self);
    self->type = AST_CConst_t;
    return self;
}

shared_ptr_t(CConst) make_CConstInt(TInt value) {
    shared_ptr_t(CConst) self = make_CConst();
    self->type = AST_CConstInt_t;
    self->get._CConstInt.value = value;
    return self;
}

shared_ptr_t(CConst) make_CConstLong(TLong value) {
    shared_ptr_t(CConst) self = make_CConst();
    self->type = AST_CConstLong_t;
    self->get._CConstLong.value = value;
    return self;
}

shared_ptr_t(CConst) make_CConstUInt(TUInt value) {
    shared_ptr_t(CConst) self = make_CConst();
    self->type = AST_CConstUInt_t;
    self->get._CConstUInt.value = value;
    return self;
}

shared_ptr_t(CConst) make_CConstULong(TULong value) {
    shared_ptr_t(CConst) self = make_CConst();
    self->type = AST_CConstULong_t;
    self->get._CConstULong.value = value;
    return self;
}

shared_ptr_t(CConst) make_CConstDouble(TDouble value) {
    shared_ptr_t(CConst) self = make_CConst();
    self->type = AST_CConstDouble_t;
    self->get._CConstDouble.value = value;
    return self;
}

shared_ptr_t(CConst) make_CConstChar(TChar value) {
    shared_ptr_t(CConst) self = make_CConst();
    self->type = AST_CConstChar_t;
    self->get._CConstChar.value = value;
    return self;
}

shared_ptr_t(CConst) make_CConstUChar(TUChar value) {
    shared_ptr_t(CConst) self = make_CConst();
    self->type = AST_CConstUChar_t;
    self->get._CConstUChar.value = value;
    return self;
}

void free_CConst(shared_ptr_t(CConst) * self) {
    sptr_delete(*self);
    switch ((*self)->type) {
        case AST_CConst_t:
        case AST_CConstInt_t:
        case AST_CConstLong_t:
        case AST_CConstUInt_t:
        case AST_CConstULong_t:
        case AST_CConstDouble_t:
        case AST_CConstChar_t:
        case AST_CConstUChar_t:
            break;
        default:
            THROW_ABORT;
    }
    sptr_free(*self);
}

shared_ptr_t(CStringLiteral) make_CStringLiteral(vector_t(TChar) * value) {
    shared_ptr_t(CStringLiteral) self = sptr_new();
    sptr_alloc(CStringLiteral, self);
    self->type = AST_CStringLiteral_t;
    self->value = vec_new();
    vec_move(*value, self->value);
    return self;
}

void free_CStringLiteral(shared_ptr_t(CStringLiteral) * self) {
    sptr_delete(*self);
    switch ((*self)->type) {
        case AST_CStringLiteral_t:
            break;
        default:
            THROW_ABORT;
    }
    vec_delete((*self)->value);
    sptr_free(*self);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Identifiers

typedef IdentifierContext* Ctx;

TIdentifier make_string_identifier(Ctx ctx, string_t* value) {
    TIdentifier identifier = str_hash(*value);
    if (map_find(ctx->hash_table, identifier) == map_end()) {
        map_move_add(ctx->hash_table, identifier, *value);
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
