#include "util/c_std.h"
#include "util/throw.h"

#include "ast/ast.h"
#include "ast/front_ast.h"
#include "ast/front_symt.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Frontend abstract syntax tree

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unique_ptr_t(CUnaryOp) make_CUnaryOp(void) {
    unique_ptr_t(CUnaryOp) self = uptr_new();
    uptr_alloc(CUnaryOp, self);
    self->type = AST_CUnaryOp_t;
    return self;
}

unique_ptr_t(CUnaryOp) make_CComplement(void) {
    unique_ptr_t(CUnaryOp) self = make_CUnaryOp();
    self->type = AST_CComplement_t;
    return self;
}

unique_ptr_t(CUnaryOp) make_CNegate(void) {
    unique_ptr_t(CUnaryOp) self = make_CUnaryOp();
    self->type = AST_CNegate_t;
    return self;
}

unique_ptr_t(CUnaryOp) make_CNot(void) {
    unique_ptr_t(CUnaryOp) self = make_CUnaryOp();
    self->type = AST_CNot_t;
    return self;
}

unique_ptr_t(CUnaryOp) make_CPrefix(void) {
    unique_ptr_t(CUnaryOp) self = make_CUnaryOp();
    self->type = AST_CPrefix_t;
    return self;
}

unique_ptr_t(CUnaryOp) make_CPostfix(void) {
    unique_ptr_t(CUnaryOp) self = make_CUnaryOp();
    self->type = AST_CPostfix_t;
    return self;
}

void free_CUnaryOp(unique_ptr_t(CUnaryOp) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_CUnaryOp_t:
        case AST_CComplement_t:
        case AST_CNegate_t:
        case AST_CNot_t:
        case AST_CPrefix_t:
        case AST_CPostfix_t:
            break;
        default:
            THROW_ABORT;
    }
    uptr_free(*self);
}

void move_CUnaryOp(unique_ptr_t(CUnaryOp) * self, unique_ptr_t(CUnaryOp) * other) {
    uptr_move(CUnaryOp, *self, *other);
}

unique_ptr_t(CBinaryOp) make_CBinaryOp(void) {
    unique_ptr_t(CBinaryOp) self = uptr_new();
    uptr_alloc(CBinaryOp, self);
    self->type = AST_CBinaryOp_t;
    return self;
}

unique_ptr_t(CBinaryOp) make_CAdd(void) {
    unique_ptr_t(CBinaryOp) self = make_CBinaryOp();
    self->type = AST_CAdd_t;
    return self;
}

unique_ptr_t(CBinaryOp) make_CSubtract(void) {
    unique_ptr_t(CBinaryOp) self = make_CBinaryOp();
    self->type = AST_CSubtract_t;
    return self;
}

unique_ptr_t(CBinaryOp) make_CMultiply(void) {
    unique_ptr_t(CBinaryOp) self = make_CBinaryOp();
    self->type = AST_CMultiply_t;
    return self;
}

unique_ptr_t(CBinaryOp) make_CDivide(void) {
    unique_ptr_t(CBinaryOp) self = make_CBinaryOp();
    self->type = AST_CDivide_t;
    return self;
}

unique_ptr_t(CBinaryOp) make_CRemainder(void) {
    unique_ptr_t(CBinaryOp) self = make_CBinaryOp();
    self->type = AST_CRemainder_t;
    return self;
}

unique_ptr_t(CBinaryOp) make_CBitAnd(void) {
    unique_ptr_t(CBinaryOp) self = make_CBinaryOp();
    self->type = AST_CBitAnd_t;
    return self;
}

unique_ptr_t(CBinaryOp) make_CBitOr(void) {
    unique_ptr_t(CBinaryOp) self = make_CBinaryOp();
    self->type = AST_CBitOr_t;
    return self;
}

unique_ptr_t(CBinaryOp) make_CBitXor(void) {
    unique_ptr_t(CBinaryOp) self = make_CBinaryOp();
    self->type = AST_CBitXor_t;
    return self;
}

unique_ptr_t(CBinaryOp) make_CBitShiftLeft(void) {
    unique_ptr_t(CBinaryOp) self = make_CBinaryOp();
    self->type = AST_CBitShiftLeft_t;
    return self;
}

unique_ptr_t(CBinaryOp) make_CBitShiftRight(void) {
    unique_ptr_t(CBinaryOp) self = make_CBinaryOp();
    self->type = AST_CBitShiftRight_t;
    return self;
}

unique_ptr_t(CBinaryOp) make_CBitShrArithmetic(void) {
    unique_ptr_t(CBinaryOp) self = make_CBinaryOp();
    self->type = AST_CBitShrArithmetic_t;
    return self;
}

unique_ptr_t(CBinaryOp) make_CAnd(void) {
    unique_ptr_t(CBinaryOp) self = make_CBinaryOp();
    self->type = AST_CAnd_t;
    return self;
}

unique_ptr_t(CBinaryOp) make_COr(void) {
    unique_ptr_t(CBinaryOp) self = make_CBinaryOp();
    self->type = AST_COr_t;
    return self;
}

unique_ptr_t(CBinaryOp) make_CEqual(void) {
    unique_ptr_t(CBinaryOp) self = make_CBinaryOp();
    self->type = AST_CEqual_t;
    return self;
}

unique_ptr_t(CBinaryOp) make_CNotEqual(void) {
    unique_ptr_t(CBinaryOp) self = make_CBinaryOp();
    self->type = AST_CNotEqual_t;
    return self;
}

unique_ptr_t(CBinaryOp) make_CLessThan(void) {
    unique_ptr_t(CBinaryOp) self = make_CBinaryOp();
    self->type = AST_CLessThan_t;
    return self;
}

unique_ptr_t(CBinaryOp) make_CLessOrEqual(void) {
    unique_ptr_t(CBinaryOp) self = make_CBinaryOp();
    self->type = AST_CLessOrEqual_t;
    return self;
}

unique_ptr_t(CBinaryOp) make_CGreaterThan(void) {
    unique_ptr_t(CBinaryOp) self = make_CBinaryOp();
    self->type = AST_CGreaterThan_t;
    return self;
}

unique_ptr_t(CBinaryOp) make_CGreaterOrEqual(void) {
    unique_ptr_t(CBinaryOp) self = make_CBinaryOp();
    self->type = AST_CGreaterOrEqual_t;
    return self;
}

void free_CBinaryOp(unique_ptr_t(CBinaryOp) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_CBinaryOp_t:
        case AST_CAdd_t:
        case AST_CSubtract_t:
        case AST_CMultiply_t:
        case AST_CDivide_t:
        case AST_CRemainder_t:
        case AST_CBitAnd_t:
        case AST_CBitOr_t:
        case AST_CBitXor_t:
        case AST_CBitShiftLeft_t:
        case AST_CBitShiftRight_t:
        case AST_CBitShrArithmetic_t:
        case AST_CAnd_t:
        case AST_COr_t:
        case AST_CEqual_t:
        case AST_CNotEqual_t:
        case AST_CLessThan_t:
        case AST_CLessOrEqual_t:
        case AST_CGreaterThan_t:
        case AST_CGreaterOrEqual_t:
            break;
        default:
            THROW_ABORT;
    }
    uptr_free(*self);
}

void move_CBinaryOp(unique_ptr_t(CBinaryOp) * self, unique_ptr_t(CBinaryOp) * other) {
    uptr_move(CBinaryOp, *self, *other);
}

unique_ptr_t(CAbstractDeclarator) make_CAbstractDeclarator(void) {
    unique_ptr_t(CAbstractDeclarator) self = uptr_new();
    uptr_alloc(CAbstractDeclarator, self);
    self->type = AST_CAbstractDeclarator_t;
    return self;
}

unique_ptr_t(CAbstractDeclarator) make_CAbstractPointer(unique_ptr_t(CAbstractDeclarator) * abstract_decltor) {
    unique_ptr_t(CAbstractDeclarator) self = make_CAbstractDeclarator();
    self->type = AST_CAbstractPointer_t;
    self->get._CAbstractPointer.abstract_decltor = uptr_new();
    move_CAbstractDeclarator(abstract_decltor, &self->get._CAbstractPointer.abstract_decltor);
    return self;
}

unique_ptr_t(CAbstractDeclarator)
    make_CAbstractArray(TLong size, unique_ptr_t(CAbstractDeclarator) * abstract_decltor) {
    unique_ptr_t(CAbstractDeclarator) self = make_CAbstractDeclarator();
    self->type = AST_CAbstractArray_t;
    self->get._CAbstractArray.size = size;
    self->get._CAbstractArray.abstract_decltor = uptr_new();
    move_CAbstractDeclarator(abstract_decltor, &self->get._CAbstractArray.abstract_decltor);
    return self;
}

unique_ptr_t(CAbstractDeclarator) make_CAbstractBase(void) {
    unique_ptr_t(CAbstractDeclarator) self = make_CAbstractDeclarator();
    self->type = AST_CAbstractBase_t;
    return self;
}

void free_CAbstractDeclarator(unique_ptr_t(CAbstractDeclarator) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_CAbstractDeclarator_t:
            break;
        case AST_CAbstractPointer_t:
            free_CAbstractDeclarator(&(*self)->get._CAbstractPointer.abstract_decltor);
            break;
        case AST_CAbstractArray_t:
            free_CAbstractDeclarator(&(*self)->get._CAbstractArray.abstract_decltor);
            break;
        case AST_CAbstractBase_t:
            break;
        default:
            THROW_ABORT;
    }
    uptr_free(*self);
}

void move_CAbstractDeclarator(unique_ptr_t(CAbstractDeclarator) * self, unique_ptr_t(CAbstractDeclarator) * other) {
    uptr_move(CAbstractDeclarator, *self, *other);
}

unique_ptr_t(CParam) make_CParam(unique_ptr_t(CDeclarator) * decltor, shared_ptr_t(Type) * param_type) {
    unique_ptr_t(CParam) self = uptr_new();
    uptr_alloc(CParam, self);
    self->type = AST_CParam_t;
    self->decltor = uptr_new();
    move_CDeclarator(decltor, &self->decltor);
    self->param_type = sptr_new();
    copy_Type(param_type, &self->param_type);
    return self;
}

void free_CParam(unique_ptr_t(CParam) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_CParam_t:
            break;
        default:
            THROW_ABORT;
    }
    free_CDeclarator(&(*self)->decltor);
    free_Type(&(*self)->param_type);
    uptr_free(*self);
}

void move_CParam(unique_ptr_t(CParam) * self, unique_ptr_t(CParam) * other) { uptr_move(CParam, *self, *other); }

unique_ptr_t(CDeclarator) make_CDeclarator(void) {
    unique_ptr_t(CDeclarator) self = uptr_new();
    uptr_alloc(CDeclarator, self);
    self->type = AST_CDeclarator_t;
    return self;
}

unique_ptr_t(CDeclarator) make_CIdent(TIdentifier name) {
    unique_ptr_t(CDeclarator) self = make_CDeclarator();
    self->type = AST_CIdent_t;
    self->get._CIdent.name = name;
    return self;
}

unique_ptr_t(CDeclarator) make_CPointerDeclarator(unique_ptr_t(CDeclarator) * decltor) {
    unique_ptr_t(CDeclarator) self = make_CDeclarator();
    self->type = AST_CPointerDeclarator_t;
    self->get._CPointerDeclarator.decltor = uptr_new();
    move_CDeclarator(decltor, &self->get._CPointerDeclarator.decltor);
    return self;
}

unique_ptr_t(CDeclarator) make_CArrayDeclarator(TLong size, unique_ptr_t(CDeclarator) * decltor) {
    unique_ptr_t(CDeclarator) self = make_CDeclarator();
    self->type = AST_CArrayDeclarator_t;
    self->get._CArrayDeclarator.size = size;
    self->get._CArrayDeclarator.decltor = uptr_new();
    move_CDeclarator(decltor, &self->get._CArrayDeclarator.decltor);
    return self;
}

unique_ptr_t(CDeclarator)
    make_CFunDeclarator(vector_t(unique_ptr_t(CParam)) * param_list, unique_ptr_t(CDeclarator) * decltor) {
    unique_ptr_t(CDeclarator) self = make_CDeclarator();
    self->type = AST_CFunDeclarator_t;
    self->get._CFunDeclarator.param_list = vec_new();
    vec_move(*param_list, self->get._CFunDeclarator.param_list);
    self->get._CFunDeclarator.decltor = uptr_new();
    move_CDeclarator(decltor, &self->get._CFunDeclarator.decltor);
    return self;
}

void free_CDeclarator(unique_ptr_t(CDeclarator) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_CDeclarator_t:
            break;
        case AST_CIdent_t:
            break;
        case AST_CPointerDeclarator_t:
            free_CDeclarator(&(*self)->get._CPointerDeclarator.decltor);
            break;
        case AST_CArrayDeclarator_t:
            free_CDeclarator(&(*self)->get._CArrayDeclarator.decltor);
            break;
        case AST_CFunDeclarator_t:
            for (size_t i = 0; i < vec_size((*self)->get._CFunDeclarator.param_list); ++i) {
                free_CParam(&(*self)->get._CFunDeclarator.param_list[i]);
            }
            vec_delete((*self)->get._CFunDeclarator.param_list);
            free_CDeclarator(&(*self)->get._CFunDeclarator.decltor);
            break;
        default:
            THROW_ABORT;
    }
    uptr_free(*self);
}

void move_CDeclarator(unique_ptr_t(CDeclarator) * self, unique_ptr_t(CDeclarator) * other) {
    uptr_move(CDeclarator, *self, *other);
}

unique_ptr_t(CExp) make_CExp(size_t line) {
    unique_ptr_t(CExp) self = uptr_new();
    uptr_alloc(CExp, self);
    self->type = AST_CExp_t;
    self->exp_type = sptr_new();
    self->line = line;
    return self;
}

unique_ptr_t(CExp) make_CConstant(shared_ptr_t(CConst) * constant, size_t line) {
    unique_ptr_t(CExp) self = make_CExp(line);
    self->type = AST_CConstant_t;
    self->get._CConstant.constant = sptr_new();
    copy_CConst(constant, &self->get._CConstant.constant);
    return self;
}

unique_ptr_t(CExp) make_CString(shared_ptr_t(CStringLiteral) * literal, size_t line) {
    unique_ptr_t(CExp) self = make_CExp(line);
    self->type = AST_CString_t;
    self->get._CString.literal = sptr_new();
    copy_CStringLiteral(literal, &self->get._CString.literal);
    return self;
}

unique_ptr_t(CExp) make_CVar(TIdentifier name, size_t line) {
    unique_ptr_t(CExp) self = make_CExp(line);
    self->type = AST_CVar_t;
    self->get._CVar.name = name;
    return self;
}

unique_ptr_t(CExp) make_CCast(unique_ptr_t(CExp) * exp, shared_ptr_t(Type) * target_type, size_t line) {
    unique_ptr_t(CExp) self = make_CExp(line);
    self->type = AST_CCast_t;
    self->get._CCast.exp = uptr_new();
    move_CExp(exp, &self->get._CCast.exp);
    self->get._CCast.target_type = sptr_new();
    copy_Type(target_type, &self->get._CCast.target_type);
    return self;
}

unique_ptr_t(CExp) make_CUnary(unique_ptr_t(CUnaryOp) * unop, unique_ptr_t(CExp) * exp, size_t line) {
    unique_ptr_t(CExp) self = make_CExp(line);
    self->type = AST_CUnary_t;
    self->get._CUnary.unop = uptr_new();
    move_CUnaryOp(unop, &self->get._CUnary.unop);
    self->get._CUnary.exp = uptr_new();
    move_CExp(exp, &self->get._CUnary.exp);
    return self;
}

unique_ptr_t(CExp) make_CBinary(
    unique_ptr_t(CBinaryOp) * binop, unique_ptr_t(CExp) * exp_left, unique_ptr_t(CExp) * exp_right, size_t line) {
    unique_ptr_t(CExp) self = make_CExp(line);
    self->type = AST_CBinary_t;
    self->get._CBinary.binop = uptr_new();
    move_CBinaryOp(binop, &self->get._CBinary.binop);
    self->get._CBinary.exp_left = uptr_new();
    move_CExp(exp_left, &self->get._CBinary.exp_left);
    self->get._CBinary.exp_right = uptr_new();
    move_CExp(exp_right, &self->get._CBinary.exp_right);
    return self;
}

unique_ptr_t(CExp) make_CAssignment(
    unique_ptr_t(CUnaryOp) * unop, unique_ptr_t(CExp) * exp_left, unique_ptr_t(CExp) * exp_right, size_t line) {
    unique_ptr_t(CExp) self = make_CExp(line);
    self->type = AST_CAssignment_t;
    self->get._CAssignment.unop = uptr_new();
    move_CUnaryOp(unop, &self->get._CAssignment.unop);
    self->get._CAssignment.exp_left = uptr_new();
    move_CExp(exp_left, &self->get._CAssignment.exp_left);
    self->get._CAssignment.exp_right = uptr_new();
    move_CExp(exp_right, &self->get._CAssignment.exp_right);
    return self;
}

unique_ptr_t(CExp) make_CConditional(
    unique_ptr_t(CExp) * condition, unique_ptr_t(CExp) * exp_middle, unique_ptr_t(CExp) * exp_right, size_t line) {
    unique_ptr_t(CExp) self = make_CExp(line);
    self->type = AST_CConditional_t;
    self->get._CConditional.condition = uptr_new();
    move_CExp(condition, &self->get._CConditional.condition);
    self->get._CConditional.exp_middle = uptr_new();
    move_CExp(exp_middle, &self->get._CConditional.exp_middle);
    self->get._CConditional.exp_right = uptr_new();
    move_CExp(exp_right, &self->get._CConditional.exp_right);
    return self;
}

unique_ptr_t(CExp) make_CFunctionCall(TIdentifier name, vector_t(unique_ptr_t(CExp)) * args, size_t line) {
    unique_ptr_t(CExp) self = make_CExp(line);
    self->type = AST_CFunctionCall_t;
    self->get._CFunctionCall.name = name;
    self->get._CFunctionCall.args = vec_new();
    vec_move(*args, self->get._CFunctionCall.args);
    return self;
}

unique_ptr_t(CExp) make_CDereference(unique_ptr_t(CExp) * exp, size_t line) {
    unique_ptr_t(CExp) self = make_CExp(line);
    self->type = AST_CDereference_t;
    self->get._CDereference.exp = uptr_new();
    move_CExp(exp, &self->get._CDereference.exp);
    return self;
}

unique_ptr_t(CExp) make_CAddrOf(unique_ptr_t(CExp) * exp, size_t line) {
    unique_ptr_t(CExp) self = make_CExp(line);
    self->type = AST_CAddrOf_t;
    self->get._CAddrOf.exp = uptr_new();
    move_CExp(exp, &self->get._CAddrOf.exp);
    return self;
}

unique_ptr_t(CExp) make_CSubscript(unique_ptr_t(CExp) * primary_exp, unique_ptr_t(CExp) * subscript_exp, size_t line) {
    unique_ptr_t(CExp) self = make_CExp(line);
    self->type = AST_CSubscript_t;
    self->get._CSubscript.primary_exp = uptr_new();
    move_CExp(primary_exp, &self->get._CSubscript.primary_exp);
    self->get._CSubscript.subscript_exp = uptr_new();
    move_CExp(subscript_exp, &self->get._CSubscript.subscript_exp);
    return self;
}

unique_ptr_t(CExp) make_CSizeOf(unique_ptr_t(CExp) * exp, size_t line) {
    unique_ptr_t(CExp) self = make_CExp(line);
    self->type = AST_CSizeOf_t;
    self->get._CSizeOf.exp = uptr_new();
    move_CExp(exp, &self->get._CSizeOf.exp);
    return self;
}

unique_ptr_t(CExp) make_CSizeOfT(shared_ptr_t(Type) * target_type, size_t line) {
    unique_ptr_t(CExp) self = make_CExp(line);
    self->type = AST_CSizeOfT_t;
    self->get._CSizeOfT.target_type = sptr_new();
    copy_Type(target_type, &self->get._CSizeOfT.target_type);
    return self;
}

unique_ptr_t(CExp) make_CDot(TIdentifier member, unique_ptr_t(CExp) * structure, size_t line) {
    unique_ptr_t(CExp) self = make_CExp(line);
    self->type = AST_CDot_t;
    self->get._CDot.member = member;
    self->get._CDot.structure = uptr_new();
    move_CExp(structure, &self->get._CDot.structure);
    return self;
}

unique_ptr_t(CExp) make_CArrow(TIdentifier member, unique_ptr_t(CExp) * pointer, size_t line) {
    unique_ptr_t(CExp) self = make_CExp(line);
    self->type = AST_CArrow_t;
    self->get._CArrow.member = member;
    self->get._CArrow.pointer = uptr_new();
    move_CExp(pointer, &self->get._CArrow.pointer);
    return self;
}

void free_CExp(unique_ptr_t(CExp) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_CExp_t:
            break;
        case AST_CConstant_t:
            free_CConst(&(*self)->get._CConstant.constant);
            break;
        case AST_CString_t:
            free_CStringLiteral(&(*self)->get._CString.literal);
            break;
        case AST_CVar_t:
            break;
        case AST_CCast_t:
            free_CExp(&(*self)->get._CCast.exp);
            free_Type(&(*self)->get._CCast.target_type);
            break;
        case AST_CUnary_t:
            free_CUnaryOp(&(*self)->get._CUnary.unop);
            free_CExp(&(*self)->get._CUnary.exp);
            break;
        case AST_CBinary_t:
            free_CBinaryOp(&(*self)->get._CBinary.binop);
            free_CExp(&(*self)->get._CBinary.exp_left);
            free_CExp(&(*self)->get._CBinary.exp_right);
            break;
        case AST_CAssignment_t:
            free_CUnaryOp(&(*self)->get._CAssignment.unop);
            free_CExp(&(*self)->get._CAssignment.exp_left);
            free_CExp(&(*self)->get._CAssignment.exp_right);
            break;
        case AST_CConditional_t:
            free_CExp(&(*self)->get._CConditional.condition);
            free_CExp(&(*self)->get._CConditional.exp_middle);
            free_CExp(&(*self)->get._CConditional.exp_right);
            break;
        case AST_CFunctionCall_t:
            for (size_t i = 0; i < vec_size((*self)->get._CFunctionCall.args); ++i) {
                free_CExp(&(*self)->get._CFunctionCall.args[i]);
            }
            vec_delete((*self)->get._CFunctionCall.args);
            break;
        case AST_CDereference_t:
            free_CExp(&(*self)->get._CDereference.exp);
            break;
        case AST_CAddrOf_t:
            free_CExp(&(*self)->get._CAddrOf.exp);
            break;
        case AST_CSubscript_t:
            free_CExp(&(*self)->get._CSubscript.primary_exp);
            free_CExp(&(*self)->get._CSubscript.subscript_exp);
            break;
        case AST_CSizeOf_t:
            free_CExp(&(*self)->get._CSizeOf.exp);
            break;
        case AST_CSizeOfT_t:
            free_Type(&(*self)->get._CSizeOfT.target_type);
            break;
        case AST_CDot_t:
            free_CExp(&(*self)->get._CDot.structure);
            break;
        case AST_CArrow_t:
            free_CExp(&(*self)->get._CArrow.pointer);
            break;
        default:
            THROW_ABORT;
    }
    free_Type(&(*self)->exp_type);
    uptr_free(*self);
}

void move_CExp(unique_ptr_t(CExp) * self, unique_ptr_t(CExp) * other) { uptr_move(CExp, *self, *other); }

unique_ptr_t(CStatement) make_CStatement(void) {
    unique_ptr_t(CStatement) self = uptr_new();
    uptr_alloc(CStatement, self);
    self->type = AST_CStatement_t;
    return self;
}

unique_ptr_t(CStatement) make_CReturn(unique_ptr_t(CExp) * exp, size_t line) {
    unique_ptr_t(CStatement) self = make_CStatement();
    self->type = AST_CReturn_t;
    self->get._CReturn.exp = uptr_new();
    move_CExp(exp, &self->get._CReturn.exp);
    self->get._CReturn.line = line;
    return self;
}

unique_ptr_t(CStatement) make_CExpression(unique_ptr_t(CExp) * exp) {
    unique_ptr_t(CStatement) self = make_CStatement();
    self->type = AST_CExpression_t;
    self->get._CExpression.exp = uptr_new();
    move_CExp(exp, &self->get._CExpression.exp);
    return self;
}

unique_ptr_t(CStatement)
    make_CIf(unique_ptr_t(CExp) * condition, unique_ptr_t(CStatement) * then, unique_ptr_t(CStatement) * else_fi) {
    unique_ptr_t(CStatement) self = make_CStatement();
    self->type = AST_CIf_t;
    self->get._CIf.condition = uptr_new();
    move_CExp(condition, &self->get._CIf.condition);
    self->get._CIf.then = uptr_new();
    move_CStatement(then, &self->get._CIf.then);
    self->get._CIf.else_fi = uptr_new();
    move_CStatement(else_fi, &self->get._CIf.else_fi);
    return self;
}

unique_ptr_t(CStatement) make_CGoto(TIdentifier target, size_t line) {
    unique_ptr_t(CStatement) self = make_CStatement();
    self->type = AST_CGoto_t;
    self->get._CGoto.target = target;
    self->get._CGoto.line = line;
    return self;
}

unique_ptr_t(CStatement) make_CLabel(TIdentifier target, unique_ptr_t(CStatement) * jump_to, size_t line) {
    unique_ptr_t(CStatement) self = make_CStatement();
    self->type = AST_CLabel_t;
    self->get._CLabel.target = target;
    self->get._CLabel.jump_to = uptr_new();
    move_CStatement(jump_to, &self->get._CLabel.jump_to);
    self->get._CLabel.line = line;
    return self;
}

unique_ptr_t(CStatement) make_CCompound(unique_ptr_t(CBlock) * block) {
    unique_ptr_t(CStatement) self = make_CStatement();
    self->type = AST_CCompound_t;
    self->get._CCompound.block = uptr_new();
    move_CBlock(block, &self->get._CCompound.block);
    return self;
}

unique_ptr_t(CStatement) make_CWhile(unique_ptr_t(CExp) * condition, unique_ptr_t(CStatement) * body) {
    unique_ptr_t(CStatement) self = make_CStatement();
    self->type = AST_CWhile_t;
    self->get._CWhile.target = 0;
    self->get._CWhile.condition = uptr_new();
    move_CExp(condition, &self->get._CWhile.condition);
    self->get._CWhile.body = uptr_new();
    move_CStatement(body, &self->get._CWhile.body);
    return self;
}

unique_ptr_t(CStatement) make_CDoWhile(unique_ptr_t(CExp) * condition, unique_ptr_t(CStatement) * body) {
    unique_ptr_t(CStatement) self = make_CStatement();
    self->type = AST_CDoWhile_t;
    self->get._CDoWhile.target = 0;
    self->get._CDoWhile.condition = uptr_new();
    move_CExp(condition, &self->get._CDoWhile.condition);
    self->get._CDoWhile.body = uptr_new();
    move_CStatement(body, &self->get._CDoWhile.body);
    return self;
}

unique_ptr_t(CStatement) make_CFor(unique_ptr_t(CForInit) * init, unique_ptr_t(CExp) * condition,
    unique_ptr_t(CExp) * post, unique_ptr_t(CStatement) * body) {
    unique_ptr_t(CStatement) self = make_CStatement();
    self->type = AST_CFor_t;
    self->get._CFor.target = 0;
    self->get._CFor.init = uptr_new();
    move_CForInit(init, &self->get._CFor.init);
    self->get._CFor.condition = uptr_new();
    move_CExp(condition, &self->get._CFor.condition);
    self->get._CFor.post = uptr_new();
    move_CExp(post, &self->get._CFor.post);
    self->get._CFor.body = uptr_new();
    move_CStatement(body, &self->get._CFor.body);
    return self;
}

unique_ptr_t(CStatement) make_CSwitch(unique_ptr_t(CExp) * match, unique_ptr_t(CStatement) * body) {
    unique_ptr_t(CStatement) self = make_CStatement();
    self->type = AST_CSwitch_t;
    self->get._CSwitch.target = 0;
    self->get._CSwitch.is_default = 0;
    self->get._CSwitch.match = uptr_new();
    move_CExp(match, &self->get._CSwitch.match);
    self->get._CSwitch.body = uptr_new();
    move_CStatement(body, &self->get._CSwitch.body);
    self->get._CSwitch.cases = vec_new();
    return self;
}

unique_ptr_t(CStatement) make_CCase(unique_ptr_t(CExp) * value, unique_ptr_t(CStatement) * jump_to) {
    unique_ptr_t(CStatement) self = make_CStatement();
    self->type = AST_CCase_t;
    self->get._CCase.target = 0;
    self->get._CCase.value = uptr_new();
    move_CExp(value, &self->get._CCase.value);
    self->get._CCase.jump_to = uptr_new();
    move_CStatement(jump_to, &self->get._CCase.jump_to);
    return self;
}

unique_ptr_t(CStatement) make_CDefault(unique_ptr_t(CStatement) * jump_to, size_t line) {
    unique_ptr_t(CStatement) self = make_CStatement();
    self->type = AST_CDefault_t;
    self->get._CDefault.target = 0;
    self->get._CDefault.jump_to = uptr_new();
    move_CStatement(jump_to, &self->get._CDefault.jump_to);
    self->get._CDefault.line = line;
    return self;
}

unique_ptr_t(CStatement) make_CBreak(size_t line) {
    unique_ptr_t(CStatement) self = make_CStatement();
    self->type = AST_CBreak_t;
    self->get._CBreak.target = 0;
    self->get._CBreak.line = line;
    return self;
}

unique_ptr_t(CStatement) make_CContinue(size_t line) {
    unique_ptr_t(CStatement) self = make_CStatement();
    self->type = AST_CContinue_t;
    self->get._CContinue.target = 0;
    self->get._CContinue.line = line;
    return self;
}

unique_ptr_t(CStatement) make_CNull(void) {
    unique_ptr_t(CStatement) self = make_CStatement();
    self->type = AST_CNull_t;
    return self;
}

void free_CStatement(unique_ptr_t(CStatement) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_CStatement_t:
            break;
        case AST_CReturn_t:
            free_CExp(&(*self)->get._CReturn.exp);
            break;
        case AST_CExpression_t:
            free_CExp(&(*self)->get._CExpression.exp);
            break;
        case AST_CIf_t:
            free_CExp(&(*self)->get._CIf.condition);
            free_CStatement(&(*self)->get._CIf.then);
            free_CStatement(&(*self)->get._CIf.else_fi);
            break;
        case AST_CGoto_t:
            break;
        case AST_CLabel_t:
            free_CStatement(&(*self)->get._CLabel.jump_to);
            break;
        case AST_CCompound_t:
            free_CBlock(&(*self)->get._CCompound.block);
            break;
        case AST_CWhile_t:
            free_CExp(&(*self)->get._CWhile.condition);
            free_CStatement(&(*self)->get._CWhile.body);
            break;
        case AST_CDoWhile_t:
            free_CExp(&(*self)->get._CDoWhile.condition);
            free_CStatement(&(*self)->get._CDoWhile.body);
            break;
        case AST_CFor_t:
            free_CForInit(&(*self)->get._CFor.init);
            free_CExp(&(*self)->get._CFor.condition);
            free_CExp(&(*self)->get._CFor.post);
            free_CStatement(&(*self)->get._CFor.body);
            break;
        case AST_CSwitch_t:
            free_CExp(&(*self)->get._CSwitch.match);
            free_CStatement(&(*self)->get._CSwitch.body);
            for (size_t i = 0; i < vec_size((*self)->get._CSwitch.cases); ++i) {
                free_CExp(&(*self)->get._CSwitch.cases[i]);
            }
            vec_delete((*self)->get._CSwitch.cases);
            break;
        case AST_CCase_t:
            free_CExp(&(*self)->get._CCase.value);
            free_CStatement(&(*self)->get._CCase.jump_to);
            break;
        case AST_CDefault_t:
            free_CStatement(&(*self)->get._CDefault.jump_to);
            break;
        case AST_CBreak_t:
            break;
        case AST_CContinue_t:
            break;
        case AST_CNull_t:
            break;
        default:
            THROW_ABORT;
    }
    uptr_free(*self);
}

void move_CStatement(unique_ptr_t(CStatement) * self, unique_ptr_t(CStatement) * other) {
    uptr_move(CStatement, *self, *other);
}

unique_ptr_t(CForInit) make_CForInit(void) {
    unique_ptr_t(CForInit) self = uptr_new();
    uptr_alloc(CForInit, self);
    self->type = AST_CForInit_t;
    return self;
}

unique_ptr_t(CForInit) make_CInitDecl(unique_ptr_t(CVariableDeclaration) * init) {
    unique_ptr_t(CForInit) self = make_CForInit();
    self->type = AST_CInitDecl_t;
    self->get._CInitDecl.init = uptr_new();
    move_CVariableDeclaration(init, &self->get._CInitDecl.init);
    return self;
}

unique_ptr_t(CForInit) make_CInitExp(unique_ptr_t(CExp) * init) {
    unique_ptr_t(CForInit) self = make_CForInit();
    self->type = AST_CInitExp_t;
    self->get._CInitExp.init = uptr_new();
    move_CExp(init, &self->get._CInitExp.init);
    return self;
}

void free_CForInit(unique_ptr_t(CForInit) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_CForInit_t:
            break;
        case AST_CInitDecl_t:
            free_CVariableDeclaration(&(*self)->get._CInitDecl.init);
            break;
        case AST_CInitExp_t:
            free_CExp(&(*self)->get._CInitExp.init);
            break;
        default:
            THROW_ABORT;
    }
    uptr_free(*self);
}

void move_CForInit(unique_ptr_t(CForInit) * self, unique_ptr_t(CForInit) * other) {
    uptr_move(CForInit, *self, *other);
}

unique_ptr_t(CBlock) make_CBlock(void) {
    unique_ptr_t(CBlock) self = uptr_new();
    uptr_alloc(CBlock, self);
    self->type = AST_CBlock_t;
    return self;
}

unique_ptr_t(CBlock) make_CB(vector_t(unique_ptr_t(CBlockItem)) * block_items) {
    unique_ptr_t(CBlock) self = make_CBlock();
    self->type = AST_CB_t;
    self->get._CB.block_items = vec_new();
    vec_move(*block_items, self->get._CB.block_items);
    return self;
}

void free_CBlock(unique_ptr_t(CBlock) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_CBlock_t:
            break;
        case AST_CB_t:
            for (size_t i = 0; i < vec_size((*self)->get._CB.block_items); ++i) {
                free_CBlockItem(&(*self)->get._CB.block_items[i]);
            }
            vec_delete((*self)->get._CB.block_items);
            break;
        default:
            THROW_ABORT;
    }
    uptr_free(*self);
}

void move_CBlock(unique_ptr_t(CBlock) * self, unique_ptr_t(CBlock) * other) { uptr_move(CBlock, *self, *other); }

unique_ptr_t(CBlockItem) make_CBlockItem(void) {
    unique_ptr_t(CBlockItem) self = uptr_new();
    uptr_alloc(CBlockItem, self);
    self->type = AST_CBlockItem_t;
    return self;
}

unique_ptr_t(CBlockItem) make_CS(unique_ptr_t(CStatement) * statement) {
    unique_ptr_t(CBlockItem) self = make_CBlockItem();
    self->type = AST_CS_t;
    self->get._CS.statement = uptr_new();
    move_CStatement(statement, &self->get._CS.statement);
    return self;
}

unique_ptr_t(CBlockItem) make_CD(unique_ptr_t(CDeclaration) * declaration) {
    unique_ptr_t(CBlockItem) self = make_CBlockItem();
    self->type = AST_CD_t;
    self->get._CD.declaration = uptr_new();
    move_CDeclaration(declaration, &self->get._CD.declaration);
    return self;
}

void free_CBlockItem(unique_ptr_t(CBlockItem) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_CBlockItem_t:
            break;
        case AST_CS_t:
            free_CStatement(&(*self)->get._CS.statement);
            break;
        case AST_CD_t:
            free_CDeclaration(&(*self)->get._CD.declaration);
            break;
        default:
            THROW_ABORT;
    }
    uptr_free(*self);
}

void move_CBlockItem(unique_ptr_t(CBlockItem) * self, unique_ptr_t(CBlockItem) * other) {
    uptr_move(CBlockItem, *self, *other);
}

unique_ptr_t(CStorageClass) make_CStorageClass(void) {
    unique_ptr_t(CStorageClass) self = uptr_new();
    uptr_alloc(CStorageClass, self);
    self->type = AST_CStorageClass_t;
    return self;
}

unique_ptr_t(CStorageClass) make_CStatic(void) {
    unique_ptr_t(CStorageClass) self = make_CStorageClass();
    self->type = AST_CStatic_t;
    return self;
}

unique_ptr_t(CStorageClass) make_CExtern(void) {
    unique_ptr_t(CStorageClass) self = make_CStorageClass();
    self->type = AST_CExtern_t;
    return self;
}

void free_CStorageClass(unique_ptr_t(CStorageClass) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_CStorageClass_t:
        case AST_CStatic_t:
        case AST_CExtern_t:
            break;
        default:
            THROW_ABORT;
    }
    uptr_free(*self);
}

void move_CStorageClass(unique_ptr_t(CStorageClass) * self, unique_ptr_t(CStorageClass) * other) {
    uptr_move(CStorageClass, *self, *other);
}

unique_ptr_t(CInitializer) make_CInitializer(void) {
    unique_ptr_t(CInitializer) self = uptr_new();
    uptr_alloc(CInitializer, self);
    self->type = AST_CInitializer_t;
    self->init_type = sptr_new();
    return self;
}

unique_ptr_t(CInitializer) make_CSingleInit(unique_ptr_t(CExp) * exp) {
    unique_ptr_t(CInitializer) self = make_CInitializer();
    self->type = AST_CSingleInit_t;
    self->get._CSingleInit.exp = uptr_new();
    move_CExp(exp, &self->get._CSingleInit.exp);
    return self;
}

unique_ptr_t(CInitializer) make_CCompoundInit(vector_t(unique_ptr_t(CInitializer)) * initializers) {
    unique_ptr_t(CInitializer) self = make_CInitializer();
    self->type = AST_CCompoundInit_t;
    self->get._CCompoundInit.initializers = vec_new();
    vec_move(*initializers, self->get._CCompoundInit.initializers);
    return self;
}

void free_CInitializer(unique_ptr_t(CInitializer) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_CInitializer_t:
            break;
        case AST_CSingleInit_t:
            free_CExp(&(*self)->get._CSingleInit.exp);
            break;
        case AST_CCompoundInit_t:
            for (size_t i = 0; i < vec_size((*self)->get._CCompoundInit.initializers); ++i) {
                free_CInitializer(&(*self)->get._CCompoundInit.initializers[i]);
            }
            vec_delete((*self)->get._CCompoundInit.initializers);
            break;
        default:
            THROW_ABORT;
    }
    free_Type(&(*self)->init_type);
    uptr_free(*self);
}

void move_CInitializer(unique_ptr_t(CInitializer) * self, unique_ptr_t(CInitializer) * other) {
    uptr_move(CInitializer, *self, *other);
}

unique_ptr_t(CMemberDeclaration)
    make_CMemberDeclaration(TIdentifier member_name, shared_ptr_t(Type) * member_type, size_t line) {
    unique_ptr_t(CMemberDeclaration) self = uptr_new();
    uptr_alloc(CMemberDeclaration, self);
    self->type = AST_CMemberDeclaration_t;
    self->member_name = member_name;
    self->member_type = sptr_new();
    copy_Type(member_type, &self->member_type);
    self->line = line;
    return self;
}

void free_CMemberDeclaration(unique_ptr_t(CMemberDeclaration) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_CMemberDeclaration_t:
            break;
        default:
            THROW_ABORT;
    }
    free_Type(&(*self)->member_type);
    uptr_free(*self);
}

void move_CMemberDeclaration(unique_ptr_t(CMemberDeclaration) * self, unique_ptr_t(CMemberDeclaration) * other) {
    uptr_move(CMemberDeclaration, *self, *other);
}

unique_ptr_t(CStructDeclaration) make_CStructDeclaration(
    TIdentifier tag, bool is_union, vector_t(unique_ptr_t(CMemberDeclaration)) * members, size_t line) {
    unique_ptr_t(CStructDeclaration) self = uptr_new();
    uptr_alloc(CStructDeclaration, self);
    self->type = AST_CStructDeclaration_t;
    self->tag = tag;
    self->is_union = is_union;
    self->members = vec_new();
    vec_move(*members, self->members);
    self->line = line;
    return self;
}

void free_CStructDeclaration(unique_ptr_t(CStructDeclaration) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_CStructDeclaration_t:
            break;
        default:
            THROW_ABORT;
    }
    for (size_t i = 0; i < vec_size((*self)->members); ++i) {
        free_CMemberDeclaration(&(*self)->members[i]);
    }
    vec_delete((*self)->members);
    uptr_free(*self);
}

void move_CStructDeclaration(unique_ptr_t(CStructDeclaration) * self, unique_ptr_t(CStructDeclaration) * other) {
    uptr_move(CStructDeclaration, *self, *other);
}

unique_ptr_t(CFunctionDeclaration)
    make_CFunctionDeclaration(TIdentifier name, vector_t(TIdentifier) * params, unique_ptr_t(CBlock) * body,
        shared_ptr_t(Type) * fun_type, unique_ptr_t(CStorageClass) * storage_class, size_t line) {
    unique_ptr_t(CFunctionDeclaration) self = uptr_new();
    uptr_alloc(CFunctionDeclaration, self);
    self->type = AST_CFunctionDeclaration_t;
    self->name = name;
    self->params = vec_new();
    vec_move(*params, self->params);
    self->body = uptr_new();
    move_CBlock(body, &self->body);
    self->fun_type = sptr_new();
    copy_Type(fun_type, &self->fun_type);
    self->storage_class = uptr_new();
    move_CStorageClass(storage_class, &self->storage_class);
    self->line = line;
    return self;
}

void free_CFunctionDeclaration(unique_ptr_t(CFunctionDeclaration) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_CFunctionDeclaration_t:
            break;
        default:
            THROW_ABORT;
    }
    vec_delete((*self)->params);
    free_CBlock(&(*self)->body);
    free_Type(&(*self)->fun_type);
    free_CStorageClass(&(*self)->storage_class);
    uptr_free(*self);
}

void move_CFunctionDeclaration(unique_ptr_t(CFunctionDeclaration) * self, unique_ptr_t(CFunctionDeclaration) * other) {
    uptr_move(CFunctionDeclaration, *self, *other);
}

unique_ptr_t(CVariableDeclaration) make_CVariableDeclaration(TIdentifier name, unique_ptr_t(CInitializer) * init,
    shared_ptr_t(Type) * var_type, unique_ptr_t(CStorageClass) * storage_class, size_t line) {
    unique_ptr_t(CVariableDeclaration) self = uptr_new();
    uptr_alloc(CVariableDeclaration, self);
    self->type = AST_CVariableDeclaration_t;
    self->name = name;
    self->init = uptr_new();
    move_CInitializer(init, &self->init);
    self->var_type = sptr_new();
    copy_Type(var_type, &self->var_type);
    self->storage_class = uptr_new();
    move_CStorageClass(storage_class, &self->storage_class);
    self->line = line;
    return self;
}

void free_CVariableDeclaration(unique_ptr_t(CVariableDeclaration) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_CVariableDeclaration_t:
            break;
        default:
            THROW_ABORT;
    }
    free_CInitializer(&(*self)->init);
    free_Type(&(*self)->var_type);
    free_CStorageClass(&(*self)->storage_class);
    uptr_free(*self);
}

void move_CVariableDeclaration(unique_ptr_t(CVariableDeclaration) * self, unique_ptr_t(CVariableDeclaration) * other) {
    uptr_move(CVariableDeclaration, *self, *other);
}

unique_ptr_t(CDeclaration) make_CDeclaration(void) {
    unique_ptr_t(CDeclaration) self = uptr_new();
    uptr_alloc(CDeclaration, self);
    self->type = AST_CDeclaration_t;
    return self;
}

unique_ptr_t(CDeclaration) make_CFunDecl(unique_ptr_t(CFunctionDeclaration) * fun_decl) {
    unique_ptr_t(CDeclaration) self = make_CDeclaration();
    self->type = AST_CFunDecl_t;
    self->get._CFunDecl.fun_decl = uptr_new();
    move_CFunctionDeclaration(fun_decl, &self->get._CFunDecl.fun_decl);
    return self;
}

unique_ptr_t(CDeclaration) make_CVarDecl(unique_ptr_t(CVariableDeclaration) * var_decl) {
    unique_ptr_t(CDeclaration) self = make_CDeclaration();
    self->type = AST_CVarDecl_t;
    self->get._CVarDecl.var_decl = uptr_new();
    move_CVariableDeclaration(var_decl, &self->get._CVarDecl.var_decl);
    return self;
}

unique_ptr_t(CDeclaration) make_CStructDecl(unique_ptr_t(CStructDeclaration) * struct_decl) {
    unique_ptr_t(CDeclaration) self = make_CDeclaration();
    self->type = AST_CStructDecl_t;
    self->get._CStructDecl.struct_decl = uptr_new();
    move_CStructDeclaration(struct_decl, &self->get._CStructDecl.struct_decl);
    return self;
}

void free_CDeclaration(unique_ptr_t(CDeclaration) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_CDeclaration_t:
            break;
        case AST_CFunDecl_t:
            free_CFunctionDeclaration(&(*self)->get._CFunDecl.fun_decl);
            break;
        case AST_CVarDecl_t:
            free_CVariableDeclaration(&(*self)->get._CVarDecl.var_decl);
            break;
        case AST_CStructDecl_t:
            free_CStructDeclaration(&(*self)->get._CStructDecl.struct_decl);
            break;
        default:
            THROW_ABORT;
    }
    uptr_free(*self);
}

void move_CDeclaration(unique_ptr_t(CDeclaration) * self, unique_ptr_t(CDeclaration) * other) {
    uptr_move(CDeclaration, *self, *other);
}

unique_ptr_t(CProgram) make_CProgram(vector_t(unique_ptr_t(CDeclaration)) * declarations) {
    unique_ptr_t(CProgram) self = uptr_new();
    uptr_alloc(CProgram, self);
    self->type = AST_CProgram_t;
    self->declarations = vec_new();
    vec_move(*declarations, self->declarations);
    return self;
}

void free_CProgram(unique_ptr_t(CProgram) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_CProgram_t:
            break;
        default:
            THROW_ABORT;
    }
    for (size_t i = 0; i < vec_size((*self)->declarations); ++i) {
        free_CDeclaration(&(*self)->declarations[i]);
    }
    vec_delete((*self)->declarations);
    uptr_free(*self);
}

void move_CProgram(unique_ptr_t(CProgram) * self, unique_ptr_t(CProgram) * other) {
    uptr_move(CProgram, *self, *other);
}
