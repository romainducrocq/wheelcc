#include "util/c_std.h"
#include "util/throw.h"

#include "ast/ast.h"
#include "ast/front_symt.h"
#include "ast/interm_ast.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Intermediate abstract syntax tree

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

shared_ptr_t(TacValue) make_TacValue(void) {
    shared_ptr_t(TacValue) self = sptr_new();
    sptr_alloc(TacValue, self);
    self->type = AST_TacValue_t;
    return self;
}

shared_ptr_t(TacValue) make_TacConstant(shared_ptr_t(CConst) * constant) {
    shared_ptr_t(TacValue) self = make_TacValue();
    self->type = AST_TacConstant_t;
    self->get._TacConstant.constant = sptr_new();
    sptr_move(CConst, *constant, self->get._TacConstant.constant);
    return self;
}

shared_ptr_t(TacValue) make_TacVariable(TIdentifier name) {
    shared_ptr_t(TacValue) self = make_TacValue();
    self->type = AST_TacVariable_t;
    self->get._TacVariable.name = name;
    return self;
}

void free_TacValue(shared_ptr_t(TacValue) * self) {
    sptr_delete(*self);
    switch ((*self)->type) {
        case AST_TacValue_t:
            break;
        case AST_TacConstant_t:
            free_CConst(&(*self)->get._TacConstant.constant);
            break;
        case AST_TacVariable_t:
            break;
        default:
            THROW_ABORT;
    }
    sptr_free(*self);
}

unique_ptr_t(TacExpResult) make_TacExpResult(void) {
    unique_ptr_t(TacExpResult) self = uptr_new();
    uptr_alloc(TacExpResult, self);
    self->type = AST_TacExpResult_t;
    return self;
}

unique_ptr_t(TacExpResult) make_TacPlainOperand(shared_ptr_t(TacValue) * val) {
    unique_ptr_t(TacExpResult) self = make_TacExpResult();
    self->type = AST_TacPlainOperand_t;
    self->get._TacPlainOperand.val = sptr_new();
    sptr_move(TacValue, *val, self->get._TacPlainOperand.val);
    return self;
}

unique_ptr_t(TacExpResult) make_TacDereferencedPointer(shared_ptr_t(TacValue) * val) {
    unique_ptr_t(TacExpResult) self = make_TacExpResult();
    self->type = AST_TacDereferencedPointer_t;
    self->get._TacDereferencedPointer.val = sptr_new();
    sptr_move(TacValue, *val, self->get._TacDereferencedPointer.val);
    return self;
}

unique_ptr_t(TacExpResult) make_TacSubObject(TIdentifier base_name, TLong offset) {
    unique_ptr_t(TacExpResult) self = make_TacExpResult();
    self->type = AST_TacSubObject_t;
    self->get._TacSubObject.base_name = base_name;
    self->get._TacSubObject.offset = offset;
    return self;
}

void free_TacExpResult(unique_ptr_t(TacExpResult) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_TacExpResult_t:
            break;
        case AST_TacPlainOperand_t:
            free_TacValue(&(*self)->get._TacPlainOperand.val);
            break;
        case AST_TacDereferencedPointer_t:
            free_TacValue(&(*self)->get._TacDereferencedPointer.val);
            break;
        case AST_TacSubObject_t:
            break;
        default:
            THROW_ABORT;
    }
    uptr_free(*self);
}

unique_ptr_t(TacInstruction) make_TacInstruction(void) {
    unique_ptr_t(TacInstruction) self = uptr_new();
    uptr_alloc(TacInstruction, self);
    self->type = AST_TacInstruction_t;
    return self;
}

unique_ptr_t(TacInstruction) make_TacReturn(shared_ptr_t(TacValue) * val) {
    unique_ptr_t(TacInstruction) self = make_TacInstruction();
    self->type = AST_TacReturn_t;
    self->get._TacReturn.val = sptr_new();
    sptr_move(TacValue, *val, self->get._TacReturn.val);
    return self;
}

unique_ptr_t(TacInstruction) make_TacSignExtend(shared_ptr_t(TacValue) * src, shared_ptr_t(TacValue) * dst) {
    unique_ptr_t(TacInstruction) self = make_TacInstruction();
    self->type = AST_TacSignExtend_t;
    self->get._TacSignExtend.src = sptr_new();
    sptr_move(TacValue, *src, self->get._TacSignExtend.src);
    self->get._TacSignExtend.dst = sptr_new();
    sptr_move(TacValue, *dst, self->get._TacSignExtend.dst);
    return self;
}

unique_ptr_t(TacInstruction) make_TacTruncate(shared_ptr_t(TacValue) * src, shared_ptr_t(TacValue) * dst) {
    unique_ptr_t(TacInstruction) self = make_TacInstruction();
    self->type = AST_TacTruncate_t;
    self->get._TacTruncate.src = sptr_new();
    sptr_move(TacValue, *src, self->get._TacTruncate.src);
    self->get._TacTruncate.dst = sptr_new();
    sptr_move(TacValue, *dst, self->get._TacTruncate.dst);
    return self;
}

unique_ptr_t(TacInstruction) make_TacZeroExtend(shared_ptr_t(TacValue) * src, shared_ptr_t(TacValue) * dst) {
    unique_ptr_t(TacInstruction) self = make_TacInstruction();
    self->type = AST_TacZeroExtend_t;
    self->get._TacZeroExtend.src = sptr_new();
    sptr_move(TacValue, *src, self->get._TacZeroExtend.src);
    self->get._TacZeroExtend.dst = sptr_new();
    sptr_move(TacValue, *dst, self->get._TacZeroExtend.dst);
    return self;
}

unique_ptr_t(TacInstruction) make_TacDoubleToInt(shared_ptr_t(TacValue) * src, shared_ptr_t(TacValue) * dst) {
    unique_ptr_t(TacInstruction) self = make_TacInstruction();
    self->type = AST_TacDoubleToInt_t;
    self->get._TacDoubleToInt.src = sptr_new();
    sptr_move(TacValue, *src, self->get._TacDoubleToInt.src);
    self->get._TacDoubleToInt.dst = sptr_new();
    sptr_move(TacValue, *dst, self->get._TacDoubleToInt.dst);
    return self;
}

unique_ptr_t(TacInstruction) make_TacDoubleToUInt(shared_ptr_t(TacValue) * src, shared_ptr_t(TacValue) * dst) {
    unique_ptr_t(TacInstruction) self = make_TacInstruction();
    self->type = AST_TacDoubleToUInt_t;
    self->get._TacDoubleToUInt.src = sptr_new();
    sptr_move(TacValue, *src, self->get._TacDoubleToUInt.src);
    self->get._TacDoubleToUInt.dst = sptr_new();
    sptr_move(TacValue, *dst, self->get._TacDoubleToUInt.dst);
    return self;
}

unique_ptr_t(TacInstruction) make_TacIntToDouble(shared_ptr_t(TacValue) * src, shared_ptr_t(TacValue) * dst) {
    unique_ptr_t(TacInstruction) self = make_TacInstruction();
    self->type = AST_TacIntToDouble_t;
    self->get._TacIntToDouble.src = sptr_new();
    sptr_move(TacValue, *src, self->get._TacIntToDouble.src);
    self->get._TacIntToDouble.dst = sptr_new();
    sptr_move(TacValue, *dst, self->get._TacIntToDouble.dst);
    return self;
}

unique_ptr_t(TacInstruction) make_TacUIntToDouble(shared_ptr_t(TacValue) * src, shared_ptr_t(TacValue) * dst) {
    unique_ptr_t(TacInstruction) self = make_TacInstruction();
    self->type = AST_TacUIntToDouble_t;
    self->get._TacUIntToDouble.src = sptr_new();
    sptr_move(TacValue, *src, self->get._TacUIntToDouble.src);
    self->get._TacUIntToDouble.dst = sptr_new();
    sptr_move(TacValue, *dst, self->get._TacUIntToDouble.dst);
    return self;
}

unique_ptr_t(TacInstruction)
    make_TacFunCall(TIdentifier name, vector_t(shared_ptr_t(TacValue)) * args, shared_ptr_t(TacValue) * dst) {
    unique_ptr_t(TacInstruction) self = make_TacInstruction();
    self->type = AST_TacFunCall_t;
    self->get._TacFunCall.name = name;
    self->get._TacFunCall.args = vec_new();
    vec_move(*args, self->get._TacFunCall.args);
    self->get._TacFunCall.dst = sptr_new();
    sptr_move(TacValue, *dst, self->get._TacFunCall.dst);
    return self;
}

unique_ptr_t(TacInstruction)
    make_TacUnary(TacUnaryOp* unop, shared_ptr_t(TacValue) * src, shared_ptr_t(TacValue) * dst) {
    unique_ptr_t(TacInstruction) self = make_TacInstruction();
    self->type = AST_TacUnary_t;
    self->get._TacUnary.unop = *unop;
    self->get._TacUnary.src = sptr_new();
    sptr_move(TacValue, *src, self->get._TacUnary.src);
    self->get._TacUnary.dst = sptr_new();
    sptr_move(TacValue, *dst, self->get._TacUnary.dst);
    return self;
}

unique_ptr_t(TacInstruction) make_TacBinary(
    TacBinaryOp* binop, shared_ptr_t(TacValue) * src1, shared_ptr_t(TacValue) * src2, shared_ptr_t(TacValue) * dst) {
    unique_ptr_t(TacInstruction) self = make_TacInstruction();
    self->type = AST_TacBinary_t;
    self->get._TacBinary.binop = *binop;
    self->get._TacBinary.src1 = sptr_new();
    sptr_move(TacValue, *src1, self->get._TacBinary.src1);
    self->get._TacBinary.src2 = sptr_new();
    sptr_move(TacValue, *src2, self->get._TacBinary.src2);
    self->get._TacBinary.dst = sptr_new();
    sptr_move(TacValue, *dst, self->get._TacBinary.dst);
    return self;
}

unique_ptr_t(TacInstruction) make_TacCopy(shared_ptr_t(TacValue) * src, shared_ptr_t(TacValue) * dst) {
    unique_ptr_t(TacInstruction) self = make_TacInstruction();
    self->type = AST_TacCopy_t;
    self->get._TacCopy.src = sptr_new();
    sptr_move(TacValue, *src, self->get._TacCopy.src);
    self->get._TacCopy.dst = sptr_new();
    sptr_move(TacValue, *dst, self->get._TacCopy.dst);
    return self;
}

unique_ptr_t(TacInstruction) make_TacGetAddress(shared_ptr_t(TacValue) * src, shared_ptr_t(TacValue) * dst) {
    unique_ptr_t(TacInstruction) self = make_TacInstruction();
    self->type = AST_TacGetAddress_t;
    self->get._TacGetAddress.src = sptr_new();
    sptr_move(TacValue, *src, self->get._TacGetAddress.src);
    self->get._TacGetAddress.dst = sptr_new();
    sptr_move(TacValue, *dst, self->get._TacGetAddress.dst);
    return self;
}

unique_ptr_t(TacInstruction) make_TacLoad(shared_ptr_t(TacValue) * src_ptr, shared_ptr_t(TacValue) * dst) {
    unique_ptr_t(TacInstruction) self = make_TacInstruction();
    self->type = AST_TacLoad_t;
    self->get._TacLoad.src_ptr = sptr_new();
    sptr_move(TacValue, *src_ptr, self->get._TacLoad.src_ptr);
    self->get._TacLoad.dst = sptr_new();
    sptr_move(TacValue, *dst, self->get._TacLoad.dst);
    return self;
}

unique_ptr_t(TacInstruction) make_TacStore(shared_ptr_t(TacValue) * src, shared_ptr_t(TacValue) * dst_ptr) {
    unique_ptr_t(TacInstruction) self = make_TacInstruction();
    self->type = AST_TacStore_t;
    self->get._TacStore.src = sptr_new();
    sptr_move(TacValue, *src, self->get._TacStore.src);
    self->get._TacStore.dst_ptr = sptr_new();
    sptr_move(TacValue, *dst_ptr, self->get._TacStore.dst_ptr);
    return self;
}

unique_ptr_t(TacInstruction) make_TacAddPtr(
    TLong scale, shared_ptr_t(TacValue) * src_ptr, shared_ptr_t(TacValue) * idx, shared_ptr_t(TacValue) * dst) {
    unique_ptr_t(TacInstruction) self = make_TacInstruction();
    self->type = AST_TacAddPtr_t;
    self->get._TacAddPtr.scale = scale;
    self->get._TacAddPtr.src_ptr = sptr_new();
    sptr_move(TacValue, *src_ptr, self->get._TacAddPtr.src_ptr);
    self->get._TacAddPtr.idx = sptr_new();
    sptr_move(TacValue, *idx, self->get._TacAddPtr.idx);
    self->get._TacAddPtr.dst = sptr_new();
    sptr_move(TacValue, *dst, self->get._TacAddPtr.dst);
    return self;
}

unique_ptr_t(TacInstruction) make_TacCopyToOffset(TIdentifier dst_name, TLong offset, shared_ptr_t(TacValue) * src) {
    unique_ptr_t(TacInstruction) self = make_TacInstruction();
    self->type = AST_TacCopyToOffset_t;
    self->get._TacCopyToOffset.dst_name = dst_name;
    self->get._TacCopyToOffset.offset = offset;
    self->get._TacCopyToOffset.src = sptr_new();
    sptr_move(TacValue, *src, self->get._TacCopyToOffset.src);
    return self;
}

unique_ptr_t(TacInstruction) make_TacCopyFromOffset(TIdentifier src_name, TLong offset, shared_ptr_t(TacValue) * dst) {
    unique_ptr_t(TacInstruction) self = make_TacInstruction();
    self->type = AST_TacCopyFromOffset_t;
    self->get._TacCopyFromOffset.src_name = src_name;
    self->get._TacCopyFromOffset.offset = offset;
    self->get._TacCopyFromOffset.dst = sptr_new();
    sptr_move(TacValue, *dst, self->get._TacCopyFromOffset.dst);
    return self;
}

unique_ptr_t(TacInstruction) make_TacJump(TIdentifier target) {
    unique_ptr_t(TacInstruction) self = make_TacInstruction();
    self->type = AST_TacJump_t;
    self->get._TacJump.target = target;
    return self;
}

unique_ptr_t(TacInstruction) make_TacJumpIfZero(TIdentifier target, shared_ptr_t(TacValue) * condition) {
    unique_ptr_t(TacInstruction) self = make_TacInstruction();
    self->type = AST_TacJumpIfZero_t;
    self->get._TacJumpIfZero.target = target;
    self->get._TacJumpIfZero.condition = sptr_new();
    sptr_move(TacValue, *condition, self->get._TacJumpIfZero.condition);
    return self;
}

unique_ptr_t(TacInstruction) make_TacJumpIfNotZero(TIdentifier target, shared_ptr_t(TacValue) * condition) {
    unique_ptr_t(TacInstruction) self = make_TacInstruction();
    self->type = AST_TacJumpIfNotZero_t;
    self->get._TacJumpIfNotZero.target = target;
    self->get._TacJumpIfNotZero.condition = sptr_new();
    sptr_move(TacValue, *condition, self->get._TacJumpIfNotZero.condition);
    return self;
}

unique_ptr_t(TacInstruction) make_TacLabel(TIdentifier name) {
    unique_ptr_t(TacInstruction) self = make_TacInstruction();
    self->type = AST_TacLabel_t;
    self->get._TacLabel.name = name;
    return self;
}

void free_TacInstruction(unique_ptr_t(TacInstruction) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_TacInstruction_t:
            break;
        case AST_TacReturn_t:
            free_TacValue(&(*self)->get._TacReturn.val);
            break;
        case AST_TacSignExtend_t:
            free_TacValue(&(*self)->get._TacSignExtend.src);
            free_TacValue(&(*self)->get._TacSignExtend.dst);
            break;
        case AST_TacTruncate_t:
            free_TacValue(&(*self)->get._TacTruncate.src);
            free_TacValue(&(*self)->get._TacTruncate.dst);
            break;
        case AST_TacZeroExtend_t:
            free_TacValue(&(*self)->get._TacZeroExtend.src);
            free_TacValue(&(*self)->get._TacZeroExtend.dst);
            break;
        case AST_TacDoubleToInt_t:
            free_TacValue(&(*self)->get._TacDoubleToInt.src);
            free_TacValue(&(*self)->get._TacDoubleToInt.dst);
            break;
        case AST_TacDoubleToUInt_t:
            free_TacValue(&(*self)->get._TacDoubleToUInt.src);
            free_TacValue(&(*self)->get._TacDoubleToUInt.dst);
            break;
        case AST_TacIntToDouble_t:
            free_TacValue(&(*self)->get._TacIntToDouble.src);
            free_TacValue(&(*self)->get._TacIntToDouble.dst);
            break;
        case AST_TacUIntToDouble_t:
            free_TacValue(&(*self)->get._TacUIntToDouble.src);
            free_TacValue(&(*self)->get._TacUIntToDouble.dst);
            break;
        case AST_TacFunCall_t:
            for (size_t i = 0; i < vec_size((*self)->get._TacFunCall.args); ++i) {
                free_TacValue(&(*self)->get._TacFunCall.args[i]);
            }
            vec_delete((*self)->get._TacFunCall.args);
            free_TacValue(&(*self)->get._TacFunCall.dst);
            break;
        case AST_TacUnary_t:
            free_TacValue(&(*self)->get._TacUnary.src);
            free_TacValue(&(*self)->get._TacUnary.dst);
            break;
        case AST_TacBinary_t:
            free_TacValue(&(*self)->get._TacBinary.src1);
            free_TacValue(&(*self)->get._TacBinary.src2);
            free_TacValue(&(*self)->get._TacBinary.dst);
            break;
        case AST_TacCopy_t:
            free_TacValue(&(*self)->get._TacCopy.src);
            free_TacValue(&(*self)->get._TacCopy.dst);
            break;
        case AST_TacGetAddress_t:
            free_TacValue(&(*self)->get._TacGetAddress.src);
            free_TacValue(&(*self)->get._TacGetAddress.dst);
            break;
        case AST_TacLoad_t:
            free_TacValue(&(*self)->get._TacLoad.src_ptr);
            free_TacValue(&(*self)->get._TacLoad.dst);
            break;
        case AST_TacStore_t:
            free_TacValue(&(*self)->get._TacStore.src);
            free_TacValue(&(*self)->get._TacStore.dst_ptr);
            break;
        case AST_TacAddPtr_t:
            free_TacValue(&(*self)->get._TacAddPtr.src_ptr);
            free_TacValue(&(*self)->get._TacAddPtr.idx);
            free_TacValue(&(*self)->get._TacAddPtr.dst);
            break;
        case AST_TacCopyToOffset_t:
            free_TacValue(&(*self)->get._TacCopyToOffset.src);
            break;
        case AST_TacCopyFromOffset_t:
            free_TacValue(&(*self)->get._TacCopyFromOffset.dst);
            break;
        case AST_TacJump_t:
            break;
        case AST_TacJumpIfZero_t:
            free_TacValue(&(*self)->get._TacJumpIfZero.condition);
            break;
        case AST_TacJumpIfNotZero_t:
            free_TacValue(&(*self)->get._TacJumpIfNotZero.condition);
            break;
        case AST_TacLabel_t:
            break;
        default:
            THROW_ABORT;
    }
    uptr_free(*self);
}

unique_ptr_t(TacTopLevel) make_TacTopLevel(void) {
    unique_ptr_t(TacTopLevel) self = uptr_new();
    uptr_alloc(TacTopLevel, self);
    self->type = AST_TacTopLevel_t;
    return self;
}

unique_ptr_t(TacTopLevel) make_TacFunction(
    TIdentifier name, bool is_glob, vector_t(TIdentifier) * params, vector_t(unique_ptr_t(TacInstruction)) * body) {
    unique_ptr_t(TacTopLevel) self = make_TacTopLevel();
    self->type = AST_TacFunction_t;
    self->get._TacFunction.name = name;
    self->get._TacFunction.is_glob = is_glob;
    self->get._TacFunction.params = vec_new();
    vec_move(*params, self->get._TacFunction.params);
    self->get._TacFunction.body = vec_new();
    vec_move(*body, self->get._TacFunction.body);
    return self;
}

unique_ptr_t(TacTopLevel) make_TacStaticVariable(TIdentifier name, bool is_glob, shared_ptr_t(Type) * static_init_type,
    vector_t(shared_ptr_t(StaticInit)) * static_inits) {
    unique_ptr_t(TacTopLevel) self = make_TacTopLevel();
    self->type = AST_TacStaticVariable_t;
    self->get._TacStaticVariable.name = name;
    self->get._TacStaticVariable.is_glob = is_glob;
    self->get._TacStaticVariable.static_init_type = sptr_new();
    sptr_move(Type, *static_init_type, self->get._TacStaticVariable.static_init_type);
    self->get._TacStaticVariable.static_inits = vec_new();
    vec_move(*static_inits, self->get._TacStaticVariable.static_inits);
    return self;
}

unique_ptr_t(TacTopLevel) make_TacStaticConstant(
    TIdentifier name, shared_ptr_t(Type) * static_init_type, shared_ptr_t(StaticInit) * static_init) {
    unique_ptr_t(TacTopLevel) self = make_TacTopLevel();
    self->type = AST_TacStaticConstant_t;
    self->get._TacStaticConstant.name = name;
    self->get._TacStaticConstant.static_init_type = sptr_new();
    sptr_move(Type, *static_init_type, self->get._TacStaticConstant.static_init_type);
    self->get._TacStaticConstant.static_init = sptr_new();
    sptr_move(StaticInit, *static_init, self->get._TacStaticConstant.static_init);
    return self;
}

void free_TacTopLevel(unique_ptr_t(TacTopLevel) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_TacTopLevel_t:
            break;
        case AST_TacFunction_t:
            vec_delete((*self)->get._TacFunction.params);
            for (size_t i = 0; i < vec_size((*self)->get._TacFunction.body); ++i) {
                free_TacInstruction(&(*self)->get._TacFunction.body[i]);
            }
            vec_delete((*self)->get._TacFunction.body);
            break;
        case AST_TacStaticVariable_t:
            free_Type(&(*self)->get._TacStaticVariable.static_init_type);
            for (size_t i = 0; i < vec_size((*self)->get._TacStaticVariable.static_inits); ++i) {
                free_StaticInit(&(*self)->get._TacStaticVariable.static_inits[i]);
            }
            vec_delete((*self)->get._TacStaticVariable.static_inits);
            break;
        case AST_TacStaticConstant_t:
            free_Type(&(*self)->get._TacStaticConstant.static_init_type);
            free_StaticInit(&(*self)->get._TacStaticConstant.static_init);
            break;
        default:
            THROW_ABORT;
    }
    uptr_free(*self);
}

unique_ptr_t(TacProgram) make_TacProgram(vector_t(unique_ptr_t(TacTopLevel)) * static_const_toplvls,
    vector_t(unique_ptr_t(TacTopLevel)) * static_var_toplvls, vector_t(unique_ptr_t(TacTopLevel)) * fun_toplvls) {
    unique_ptr_t(TacProgram) self = uptr_new();
    uptr_alloc(TacProgram, self);
    self->type = AST_TacProgram_t;
    self->static_const_toplvls = vec_new();
    vec_move(*static_const_toplvls, self->static_const_toplvls);
    self->static_var_toplvls = vec_new();
    vec_move(*static_var_toplvls, self->static_var_toplvls);
    self->fun_toplvls = vec_new();
    vec_move(*fun_toplvls, self->fun_toplvls);
    return self;
}

void free_TacProgram(unique_ptr_t(TacProgram) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_TacProgram_t:
            break;
        default:
            THROW_ABORT;
    }
    for (size_t i = 0; i < vec_size((*self)->static_const_toplvls); ++i) {
        free_TacTopLevel(&(*self)->static_const_toplvls[i]);
    }
    vec_delete((*self)->static_const_toplvls);
    for (size_t i = 0; i < vec_size((*self)->static_var_toplvls); ++i) {
        free_TacTopLevel(&(*self)->static_var_toplvls[i]);
    }
    vec_delete((*self)->static_var_toplvls);
    for (size_t i = 0; i < vec_size((*self)->fun_toplvls); ++i) {
        free_TacTopLevel(&(*self)->fun_toplvls[i]);
    }
    vec_delete((*self)->fun_toplvls);
    uptr_free(*self);
}
