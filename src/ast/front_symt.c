#include "util/c_std.h"
#include "util/throw.h"

#include "ast/ast.h"
#include "ast/front_symt.h"

#include "assembly/regs.h" // backend

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Frontend symbol table

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

shared_ptr_t(Type) make_Type(void) {
    shared_ptr_t(Type) self = sptr_new();
    sptr_alloc(Type, self);
    self->type = AST_Type_t;
    return self;
}

shared_ptr_t(Type) make_Char(void) {
    shared_ptr_t(Type) self = make_Type();
    self->type = AST_Char_t;
    return self;
}

shared_ptr_t(Type) make_SChar(void) {
    shared_ptr_t(Type) self = make_Type();
    self->type = AST_SChar_t;
    return self;
}

shared_ptr_t(Type) make_UChar(void) {
    shared_ptr_t(Type) self = make_Type();
    self->type = AST_UChar_t;
    return self;
}

shared_ptr_t(Type) make_Int(void) {
    shared_ptr_t(Type) self = make_Type();
    self->type = AST_Int_t;
    return self;
}

shared_ptr_t(Type) make_Long(void) {
    shared_ptr_t(Type) self = make_Type();
    self->type = AST_Long_t;
    return self;
}

shared_ptr_t(Type) make_UInt(void) {
    shared_ptr_t(Type) self = make_Type();
    self->type = AST_UInt_t;
    return self;
}

shared_ptr_t(Type) make_ULong(void) {
    shared_ptr_t(Type) self = make_Type();
    self->type = AST_ULong_t;
    return self;
}

shared_ptr_t(Type) make_Double(void) {
    shared_ptr_t(Type) self = make_Type();
    self->type = AST_Double_t;
    return self;
}

shared_ptr_t(Type) make_Void(void) {
    shared_ptr_t(Type) self = make_Type();
    self->type = AST_Void_t;
    return self;
}

shared_ptr_t(Type) make_FunType(vector_t(shared_ptr_t(Type)) * param_types, shared_ptr_t(Type) * ret_type) {
    shared_ptr_t(Type) self = make_Type();
    self->type = AST_FunType_t;
    self->get._FunType.param_reg_mask = 0ul;
    self->get._FunType.ret_reg_mask = 0ul;
    self->get._FunType.param_types = vec_new();
    vec_move(*param_types, self->get._FunType.param_types);
    self->get._FunType.ret_type = sptr_new();
    sptr_move(Type, *ret_type, self->get._FunType.ret_type);
    self->get._FunType.param_reg_mask = NULL_REGISTER_MASK;
    self->get._FunType.ret_reg_mask = NULL_REGISTER_MASK;
    return self;
}

shared_ptr_t(Type) make_Pointer(shared_ptr_t(Type) * ref_type) {
    shared_ptr_t(Type) self = make_Type();
    self->type = AST_Pointer_t;
    self->get._Pointer.ref_type = sptr_new();
    sptr_move(Type, *ref_type, self->get._Pointer.ref_type);
    return self;
}

shared_ptr_t(Type) make_Array(TLong size, shared_ptr_t(Type) * elem_type) {
    shared_ptr_t(Type) self = make_Type();
    self->type = AST_Array_t;
    self->get._Array.size = size;
    self->get._Array.elem_type = sptr_new();
    sptr_move(Type, *elem_type, self->get._Array.elem_type);
    return self;
}

shared_ptr_t(Type) make_Structure(TIdentifier tag, bool is_union) {
    shared_ptr_t(Type) self = make_Type();
    self->type = AST_Structure_t;
    self->get._Structure.tag = tag;
    self->get._Structure.is_union = is_union;
    return self;
}

void free_Type(shared_ptr_t(Type) * self) {
    sptr_delete(*self);
    switch ((*self)->type) {
        case AST_Type_t:
        case AST_Char_t:
        case AST_SChar_t:
        case AST_UChar_t:
        case AST_Int_t:
        case AST_Long_t:
        case AST_UInt_t:
        case AST_ULong_t:
        case AST_Double_t:
        case AST_Void_t:
            break;
        case AST_FunType_t:
            for (size_t i = 0; i < vec_size((*self)->get._FunType.param_types); ++i) {
                free_Type(&(*self)->get._FunType.param_types[i]);
            }
            vec_delete((*self)->get._FunType.param_types);
            free_Type(&(*self)->get._FunType.ret_type);
            break;
        case AST_Pointer_t:
            free_Type(&(*self)->get._Pointer.ref_type);
            break;
        case AST_Array_t:
            free_Type(&(*self)->get._Array.elem_type);
            break;
        case AST_Structure_t:
            break;
        default:
            THROW_ABORT;
    }
    sptr_free(*self);
}

shared_ptr_t(StaticInit) make_StaticInit(void) {
    shared_ptr_t(StaticInit) self = sptr_new();
    sptr_alloc(StaticInit, self);
    self->type = AST_StaticInit_t;
    return self;
}

shared_ptr_t(StaticInit) make_IntInit(TInt value) {
    shared_ptr_t(StaticInit) self = make_StaticInit();
    self->type = AST_IntInit_t;
    self->get._IntInit.value = value;
    return self;
}

shared_ptr_t(StaticInit) make_LongInit(TLong value) {
    shared_ptr_t(StaticInit) self = make_StaticInit();
    self->type = AST_LongInit_t;
    self->get._LongInit.value = value;
    return self;
}

shared_ptr_t(StaticInit) make_UIntInit(TUInt value) {
    shared_ptr_t(StaticInit) self = make_StaticInit();
    self->type = AST_UIntInit_t;
    self->get._UIntInit.value = value;
    return self;
}

shared_ptr_t(StaticInit) make_ULongInit(TULong value) {
    shared_ptr_t(StaticInit) self = make_StaticInit();
    self->type = AST_ULongInit_t;
    self->get._ULongInit.value = value;
    return self;
}

shared_ptr_t(StaticInit) make_CharInit(TChar value) {
    shared_ptr_t(StaticInit) self = make_StaticInit();
    self->type = AST_CharInit_t;
    self->get._CharInit.value = value;
    return self;
}

shared_ptr_t(StaticInit) make_UCharInit(TUChar value) {
    shared_ptr_t(StaticInit) self = make_StaticInit();
    self->type = AST_UCharInit_t;
    self->get._UCharInit.value = value;
    return self;
}

shared_ptr_t(StaticInit) make_DoubleInit(TIdentifier dbl_const) {
    shared_ptr_t(StaticInit) self = make_StaticInit();
    self->type = AST_DoubleInit_t;
    self->get._DoubleInit.dbl_const = dbl_const;
    return self;
}

shared_ptr_t(StaticInit) make_ZeroInit(TLong byte) {
    shared_ptr_t(StaticInit) self = make_StaticInit();
    self->type = AST_ZeroInit_t;
    self->get._ZeroInit.byte = byte;
    return self;
}

shared_ptr_t(StaticInit)
    make_StringInit(TIdentifier string_const, bool is_null_term, shared_ptr_t(CStringLiteral) * literal) {
    shared_ptr_t(StaticInit) self = make_StaticInit();
    self->type = AST_StringInit_t;
    self->get._StringInit.string_const = string_const;
    self->get._StringInit.is_null_term = is_null_term;
    self->get._StringInit.literal = sptr_new();
    sptr_move(CStringLiteral, *literal, self->get._StringInit.literal);
    return self;
}

shared_ptr_t(StaticInit) make_PointerInit(TIdentifier name) {
    shared_ptr_t(StaticInit) self = make_StaticInit();
    self->type = AST_PointerInit_t;
    self->get._PointerInit.name = name;
    return self;
}

void free_StaticInit(shared_ptr_t(StaticInit) * self) {
    sptr_delete(*self);
    switch ((*self)->type) {
        case AST_StaticInit_t:
        case AST_IntInit_t:
        case AST_LongInit_t:
        case AST_UIntInit_t:
        case AST_ULongInit_t:
        case AST_CharInit_t:
        case AST_UCharInit_t:
            break;
        case AST_DoubleInit_t:
            break;
        case AST_ZeroInit_t:
            break;
        case AST_StringInit_t:
            free_CStringLiteral(&(*self)->get._StringInit.literal);
            break;
        case AST_PointerInit_t:
            break;
        default:
            THROW_ABORT;
    }
    sptr_free(*self);
}

shared_ptr_t(InitialValue) make_InitialValue(void) {
    shared_ptr_t(InitialValue) self = sptr_new();
    sptr_alloc(InitialValue, self);
    self->type = AST_InitialValue_t;
    return self;
}

shared_ptr_t(InitialValue) make_Tentative(void) {
    shared_ptr_t(InitialValue) self = make_InitialValue();
    self->type = AST_Tentative_t;
    return self;
}

shared_ptr_t(InitialValue) make_Initial(vector_t(shared_ptr_t(StaticInit)) * static_inits) {
    shared_ptr_t(InitialValue) self = make_InitialValue();
    self->type = AST_Initial_t;
    self->get._Initial.static_inits = vec_new();
    vec_move(*static_inits, self->get._Initial.static_inits);
    return self;
}

shared_ptr_t(InitialValue) make_NoInitializer(void) {
    shared_ptr_t(InitialValue) self = make_InitialValue();
    self->type = AST_NoInitializer_t;
    return self;
}

void free_InitialValue(shared_ptr_t(InitialValue) * self) {
    sptr_delete(*self);
    switch ((*self)->type) {
        case AST_InitialValue_t:
        case AST_Tentative_t:
            break;
        case AST_Initial_t:
            for (size_t i = 0; i < vec_size((*self)->get._Initial.static_inits); ++i) {
                free_StaticInit(&(*self)->get._Initial.static_inits[i]);
            }
            vec_delete((*self)->get._Initial.static_inits);
            break;
        case AST_NoInitializer_t:
            break;
        default:
            THROW_ABORT;
    }
    sptr_free(*self);
}

unique_ptr_t(IdentifierAttr) make_IdentifierAttr(void) {
    unique_ptr_t(IdentifierAttr) self = uptr_new();
    uptr_alloc(IdentifierAttr, self);
    self->type = AST_IdentifierAttr_t;
    return self;
}

unique_ptr_t(IdentifierAttr) make_FunAttr(bool is_def, bool is_glob) {
    unique_ptr_t(IdentifierAttr) self = make_IdentifierAttr();
    self->type = AST_FunAttr_t;
    self->get._FunAttr.is_def = is_def;
    self->get._FunAttr.is_glob = is_glob;
    return self;
}

unique_ptr_t(IdentifierAttr) make_StaticAttr(bool is_glob, shared_ptr_t(InitialValue) * init) {
    unique_ptr_t(IdentifierAttr) self = make_IdentifierAttr();
    self->type = AST_StaticAttr_t;
    self->get._StaticAttr.is_glob = is_glob;
    self->get._StaticAttr.init = sptr_new();
    sptr_move(InitialValue, *init, self->get._StaticAttr.init);
    return self;
}

unique_ptr_t(IdentifierAttr) make_ConstantAttr(shared_ptr_t(StaticInit) * static_init) {
    unique_ptr_t(IdentifierAttr) self = make_IdentifierAttr();
    self->type = AST_ConstantAttr_t;
    self->get._ConstantAttr.static_init = sptr_new();
    sptr_move(StaticInit, *static_init, self->get._ConstantAttr.static_init);
    return self;
}

unique_ptr_t(IdentifierAttr) make_LocalAttr(void) {
    unique_ptr_t(IdentifierAttr) self = make_IdentifierAttr();
    self->type = AST_LocalAttr_t;
    return self;
}

void free_IdentifierAttr(unique_ptr_t(IdentifierAttr) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_IdentifierAttr_t:
        case AST_FunAttr_t:
            break;
        case AST_StaticAttr_t:
            free_InitialValue(&(*self)->get._StaticAttr.init);
            break;
        case AST_ConstantAttr_t:
            free_StaticInit(&(*self)->get._ConstantAttr.static_init);
            break;
        case AST_LocalAttr_t:
            break;
        default:
            THROW_ABORT;
    }
    uptr_free(*self);
}

unique_ptr_t(Symbol) make_Symbol(shared_ptr_t(Type) * type_t, unique_ptr_t(IdentifierAttr) * attrs) {
    unique_ptr_t(Symbol) self = uptr_new();
    uptr_alloc(Symbol, self);
    self->type = AST_Symbol_t;
    self->type_t = sptr_new();
    sptr_move(Type, *type_t, self->type_t);
    self->attrs = uptr_new();
    uptr_move(IdentifierAttr, *attrs, self->attrs);
    return self;
}

void free_Symbol(unique_ptr_t(Symbol) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_Symbol_t:
            break;
        default:
            THROW_ABORT;
    }
    free_Type(&(*self)->type_t);
    free_IdentifierAttr(&(*self)->attrs);
    uptr_free(*self);
}

unique_ptr_t(StructMember) make_StructMember(TLong offset, shared_ptr_t(Type) * member_type) {
    unique_ptr_t(StructMember) self = uptr_new();
    uptr_alloc(StructMember, self);
    self->type = AST_StructMember_t;
    self->offset = offset;
    self->member_type = sptr_new();
    sptr_move(Type, *member_type, self->member_type);
    return self;
}

void free_StructMember(unique_ptr_t(StructMember) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_StructMember_t:
            break;
        default:
            THROW_ABORT;
    }
    free_Type(&(*self)->member_type);
    uptr_free(*self);
}

unique_ptr_t(StructTypedef) make_StructTypedef(TInt alignment, TLong size, vector_t(TIdentifier) * member_names,
    hashmap_t(TIdentifier, UPtrStructMember) * members) {
    unique_ptr_t(StructTypedef) self = uptr_new();
    uptr_alloc(StructTypedef, self);
    self->type = AST_StructTypedef_t;
    self->alignment = alignment;
    self->size = size;
    self->member_names = vec_new();
    vec_move(*member_names, self->member_names);
    self->members = map_new();
    map_move(*members, self->members);
    return self;
}

void free_StructTypedef(unique_ptr_t(StructTypedef) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_StructTypedef_t:
            break;
        default:
            THROW_ABORT;
    }
    vec_delete((*self)->member_names);
    for (size_t i = 0; i < map_size((*self)->members); ++i) {
        free_StructMember(&pair_second((*self)->members[i]));
    }
    map_delete((*self)->members);
    uptr_free(*self);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef FrontEndContext* Ctx;

StructMember* get_struct_typedef_member(Ctx ctx, TIdentifier tag, TIdentifier member_name) {
    StructTypedef* struct_typedef = map_get(ctx->struct_typedef_table, tag);
    return map_get(struct_typedef->members, struct_typedef->member_names[member_name]);
}

const StructMember* get_struct_typedef_back(Ctx ctx, TIdentifier tag) {
    StructTypedef* struct_typedef = map_get(ctx->struct_typedef_table, tag);
    return map_get(struct_typedef->members, vec_back(struct_typedef->member_names));
}
