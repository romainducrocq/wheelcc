#include "util/c_std.h"
#include "util/throw.h"

#include "ast/ast.h"
#include "ast/back_ast.h"
#include "ast/back_symt.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Backend symbol table

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

shared_ptr_t(AssemblyType) make_AssemblyType(void) {
    shared_ptr_t(AssemblyType) self = sptr_new();
    sptr_alloc(AssemblyType, self);
    self->type = AST_AssemblyType_t;
    return self;
}

shared_ptr_t(AssemblyType) make_Byte(void) {
    shared_ptr_t(AssemblyType) self = make_AssemblyType();
    self->type = AST_Byte_t;
    return self;
}

shared_ptr_t(AssemblyType) make_LongWord(void) {
    shared_ptr_t(AssemblyType) self = make_AssemblyType();
    self->type = AST_LongWord_t;
    return self;
}

shared_ptr_t(AssemblyType) make_QuadWord(void) {
    shared_ptr_t(AssemblyType) self = make_AssemblyType();
    self->type = AST_QuadWord_t;
    return self;
}

shared_ptr_t(AssemblyType) make_BackendDouble(void) {
    shared_ptr_t(AssemblyType) self = make_AssemblyType();
    self->type = AST_BackendDouble_t;
    return self;
}

shared_ptr_t(AssemblyType) make_ByteArray(TLong size, TInt alignment) {
    shared_ptr_t(AssemblyType) self = make_AssemblyType();
    self->type = AST_ByteArray_t;
    self->get._ByteArray.size = size;
    self->get._ByteArray.alignment = alignment;
    return self;
}

void free_AssemblyType(shared_ptr_t(AssemblyType) * self) {
    sptr_delete(*self);
    switch ((*self)->type) {
        case AST_AssemblyType_t:
        case AST_Byte_t:
        case AST_LongWord_t:
        case AST_QuadWord_t:
        case AST_BackendDouble_t:
        case AST_ByteArray_t:
            break;
        default:
            THROW_ABORT;
    }
    sptr_free(*self);
}

unique_ptr_t(BackendSymbol) make_BackendSymbol(void) {
    unique_ptr_t(BackendSymbol) self = uptr_new();
    uptr_alloc(BackendSymbol, self);
    self->type = AST_BackendSymbol_t;
    return self;
}

unique_ptr_t(BackendSymbol) make_BackendObj(bool is_static, bool is_const, shared_ptr_t(AssemblyType) * asm_type) {
    unique_ptr_t(BackendSymbol) self = make_BackendSymbol();
    self->type = AST_BackendObj_t;
    self->get._BackendObj.is_static = is_static;
    self->get._BackendObj.is_const = is_const;
    self->get._BackendObj.asm_type = sptr_new();
    sptr_move(AssemblyType, *asm_type, self->get._BackendObj.asm_type);
    return self;
}

unique_ptr_t(BackendSymbol) make_BackendFun(bool is_def) {
    unique_ptr_t(BackendSymbol) self = make_BackendSymbol();
    self->type = AST_BackendFun_t;
    self->get._BackendFun.is_def = is_def;
    self->get._BackendFun.callee_saved_regs = vec_new();
    return self;
}

void free_BackendSymbol(unique_ptr_t(BackendSymbol) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_BackendSymbol_t:
            break;
        case AST_BackendObj_t:
            free_AssemblyType(&(*self)->get._BackendObj.asm_type);
            break;
        case AST_BackendFun_t:
            for (size_t i = 0; i < vec_size((*self)->get._BackendFun.callee_saved_regs); ++i) {
                free_AsmOperand(&(*self)->get._BackendFun.callee_saved_regs[i]);
            }
            vec_delete((*self)->get._BackendFun.callee_saved_regs);
            break;
        default:
            THROW_ABORT;
    }
    uptr_free(*self);
}
