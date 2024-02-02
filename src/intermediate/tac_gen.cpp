#include "intermediate/tac_gen.hpp"
#include "util/error.hpp"
#include "util/names.hpp"
#include "ast/ast.hpp"
#include "ast/symbol_table.hpp"
#include "ast/c_ast.hpp"
#include "ast/tac_ast.hpp"
#include "semantic/type_check.hpp"

#include <inttypes.h>
#include <string>
#include <memory>
#include <vector>

// https://stackoverflow.com/questions/201718/concatenating-two-stdvectors

/**
cdef TacBinaryOp represent_binary_op(CBinaryOp node):
    # binary_operator = Add | Subtract | Multiply | Divide | Remainder | BitAnd | BitOr | BitXor
    #                 | BitShiftLeft | BitShiftRight | Equal | NotEqual | LessThan | LessOrEqual
    #                 | GreaterThan | GreaterOrEqual
    if isinstance(node, CAdd):
        return TacAdd()
    elif isinstance(node, CSubtract):
        return TacSubtract()
    elif isinstance(node, CMultiply):
        return TacMultiply()
    elif isinstance(node, CDivide):
        return TacDivide()
    elif isinstance(node, CRemainder):
        return TacRemainder()
    elif isinstance(node, CBitAnd):
        return TacBitAnd()
    elif isinstance(node, CBitOr):
        return TacBitOr()
    elif isinstance(node, CBitXor):
        return TacBitXor()
    elif isinstance(node, CBitShiftLeft):
        return TacBitShiftLeft()
    elif isinstance(node, CBitShiftRight):
        return TacBitShiftRight()
    elif isinstance(node, CEqual):
        return TacEqual()
    elif isinstance(node, CNotEqual):
        return TacNotEqual()
    elif isinstance(node, CLessThan):
        return TacLessThan()
    elif isinstance(node, CLessOrEqual):
        return TacLessOrEqual()
    elif isinstance(node, CGreaterThan):
        return TacGreaterThan()
    elif isinstance(node, CGreaterOrEqual):
        return TacGreaterOrEqual()
    else:

        raise RuntimeError(
            "An error occurred in three address code representation, not all nodes were visited")
*/
// binary_operator = Add | Subtract | Multiply | Divide | Remainder | BitAnd | BitOr | BitXor
//                 | BitShiftLeft | BitShiftRight | Equal | NotEqual | LessThan | LessOrEqual
//                 | GreaterThan | GreaterOrEqual
static std::unique_ptr<TacBinaryOp> represent_binary_op(CBinaryOp* node) {
    switch(node->type()) {
        case AST_T::CAdd_t:
            return std::make_unique<TacAdd>();
        case AST_T::CSubtract_t:
            return std::make_unique<TacSubtract>();
        case AST_T::CMultiply_t:
            return std::make_unique<TacMultiply>();
        case AST_T::CDivide_t:
            return std::make_unique<TacDivide>();
        case AST_T::CRemainder_t:
            return std::make_unique<TacRemainder>();
        case AST_T::CBitAnd_t:
            return std::make_unique<TacBitAnd>();
        case AST_T::CBitOr_t:
            return std::make_unique<TacBitOr>();
        case AST_T::CBitXor_t:
            return std::make_unique<TacBitXor>();
        case AST_T::CBitShiftLeft_t:
            return std::make_unique<TacBitShiftLeft>();
        case AST_T::CBitShiftRight_t:
            return std::make_unique<TacBitShiftRight>();
        case AST_T::CEqual_t:
            return std::make_unique<TacEqual>();
        case AST_T::CNotEqual_t:
            return std::make_unique<TacNotEqual>();
        case AST_T::CLessThan_t:
            return std::make_unique<TacLessThan>();
        case AST_T::CLessOrEqual_t:
            return std::make_unique<TacLessOrEqual>();
        case AST_T::CGreaterThan_t:
            return std::make_unique<TacGreaterThan>();
        case AST_T::CGreaterOrEqual_t:
            return std::make_unique<TacGreaterOrEqual>();
        default:
            raise_internal_error("An error occurred in three address code representation, "
                                 "not all nodes were visited");
    }
}

/**
cdef TacUnaryOp represent_unary_op(CUnaryOp node):
    # unary_operator = Complement | Negate | Not
    if isinstance(node, CComplement):
        return TacComplement()
    elif isinstance(node, CNegate):
        return TacNegate()
    elif isinstance(node, CNot):
        return TacNot()
    else:

        raise RuntimeError(
            "An error occurred in three address code representation, not all nodes were visited")
*/
// unary_operator = Complement | Negate | Not
static std::unique_ptr<TacUnaryOp> represent_unary_op(CUnaryOp* node) {
    switch(node->type()) {
        case AST_T::CComplement_t:
            return std::make_unique<TacComplement>();
        case AST_T::CNegate_t:
            return std::make_unique<TacNegate>();
        case AST_T::CNot_t:
            return std::make_unique<TacNot>();
        default:
            raise_internal_error("An error occurred in three address code representation, "
                                 "not all nodes were visited");
    }
}

/**
cdef TacVariable represent_variable_value(CVar node):
    cdef TIdentifier name
    name = copy_identifier(node.name)
    return TacVariable(name)
*/
static std::shared_ptr<TacVariable> represent_variable_value(CVar* node) {
    TIdentifier name = node->name;
    return std::make_shared<TacVariable>(std::move(name));
}

/**
cdef TacConstant represent_constant_value(CConstant node):
    cdef CConst constant = node.constant
    return TacConstant(constant)
*/
static std::shared_ptr<TacConstant> represent_constant_value(CConstant* node) {
    std::shared_ptr<CConst> constant = node->constant;
    return std::make_shared<TacConstant>(std::move(constant));
}

/**
cdef TacVariable represent_inner_exp_value(CExp node):
    cdef TIdentifier inner_name = represent_variable_identifier(node)
    cdef Type inner_type = node.exp_type
    cdef IdentifierAttr inner_attrs = LocalAttr()
    symbol_table[inner_name.str_t] = Symbol(inner_type, inner_attrs)
    return TacVariable(inner_name)
*/
static std::shared_ptr<TacVariable> represent_inner_exp_value(CExp* node) {
    TIdentifier inner_name = represent_variable_identifier(node);
    std::shared_ptr<Type> inner_type = node->exp_type;
    std::unique_ptr<IdentifierAttr> inner_attrs = std::make_unique<LocalAttr>();
    std::unique_ptr<Symbol> symbol = std::make_unique<Symbol>(std::move(inner_type),
                                                              std::move(inner_attrs));
    symbol_table[inner_name] = std::move(symbol);
    return std::make_shared<TacVariable>(std::move(inner_name));
}

/**
cdef TacValue represent_inner_value(CExp node):
    return represent_inner_exp_value(node)
*/
static std::shared_ptr<TacValue> represent_inner_value(CExp* node) {
    return represent_inner_exp_value(node);
}

/**
cdef TacValue represent_value(CExp node):
    # val = Constant(int) | Var(identifier)
    if isinstance(node, CVar):
        return represent_variable_value(node)
    elif isinstance(node, CConstant):
        return represent_constant_value(node)
    else:

        raise RuntimeError(
            "An error occurred in three address code representation, not all nodes were visited")
*/
// val = Constant(int) | Var(identifier)
static std::shared_ptr<TacValue> represent_value(CExp* node) {
    switch(node->type()) {
        case AST_T::CVar_t:
            return represent_variable_value(static_cast<CVar*>(node));
        case AST_T::CConstant_t:
            return represent_constant_value(static_cast<CConstant*>(node));
        default:
            raise_internal_error("An error occurred in three address code representation, "
                                 "not all nodes were visited");
    }
}

/**
cdef list[TacInstruction] instructions = []
*/
static std::vector<std::unique_ptr<TacInstruction>>* p_instructions;

static void push_instruction(std::unique_ptr<TacInstruction>&& instruction) {
    p_instructions->push_back(std::move(instruction));
}

static std::shared_ptr<TacValue> represent_exp_instructions(CExp* node);

/**
cdef TacConstant represent_exp_constant_instructions(CConstant node):
    return represent_value(node)
*/
static std::shared_ptr<TacValue> represent_exp_constant_instructions(CConstant* node) {
    return represent_value(node);
}

/**
cdef TacVariable represent_exp_var_instructions(CVar node):
    return represent_value(node)
*/
static std::shared_ptr<TacValue> represent_exp_var_instructions(CVar* node) {
    return represent_value(node);
}

/**
cdef TacValue represent_exp_fun_call_instructions(CFunctionCall node):
    cdef TIdentifier name = copy_identifier(node.name)
    cdef Py_ssize_t i
    cdef list[TacValue] args = []
    for i in range(len(node.args)):
        args.append(represent_exp_instructions(node.args[i]))
    cdef TacValue dst = represent_inner_value(node)
    instructions.append(TacFunCall(name, args, dst))
    return dst
*/
static std::shared_ptr<TacValue> represent_exp_fun_call_instructions(CFunctionCall* node) {
    TIdentifier name = node->name;
    std::vector<std::shared_ptr<TacValue>> args;
    for(size_t i = 0; i < node->args.size(); i++) {
        std::shared_ptr<TacValue> arg = represent_exp_instructions(node->args[i].get());
        args.push_back(std::move(arg));
    }
    std::shared_ptr<TacValue> dst = represent_inner_value(node);
    push_instruction(std::make_unique<TacFunCall>(std::move(name), std::move(args), dst));
    return dst;
}

/**
cdef TacValue represent_exp_cast_instructions(CCast node):
    cdef TacValue src = represent_exp_instructions(node.exp)
    if is_same_type(node.target_type, node.exp.exp_type):
        return src
    cdef TacValue dst = represent_inner_value(node)
    if isinstance(node.exp.exp_type, Double):
        if is_type_signed(node.target_type):
            instructions.append(TacDoubleToInt(src, dst))
        else:
            instructions.append(TacDoubleToUInt(src, dst))
        return dst
    elif isinstance(node.target_type, Double):
        if is_type_signed(node.exp.exp_type):
            instructions.append(TacIntToDouble(src, dst))
        else:
            instructions.append(TacUIntToDouble(src, dst))
        return dst
    cdef int32 target_type_size = get_type_size(node.target_type)
    cdef int32 inner_type_size = get_type_size(node.exp.exp_type)
    if target_type_size == inner_type_size:
        instructions.append(TacCopy(src, dst))
    elif target_type_size < inner_type_size:
        instructions.append(TacTruncate(src, dst))
    elif is_type_signed(node.exp.exp_type):
        instructions.append(TacSignExtend(src, dst))
    else:
        instructions.append(TacZeroExtend(src, dst))
    return dst
*/
static std::shared_ptr<TacValue> represent_exp_cast_instructions(CCast* node) {
    std::shared_ptr<TacValue> src = represent_exp_instructions(node->exp.get());
    if(is_same_type(node->target_type.get(), node->exp->exp_type.get())) {
        return src;
    }

    std::shared_ptr<TacValue> dst = represent_inner_value(node);
    if(node->exp->exp_type->type() == AST_T::Double_t) {
        if(is_type_signed(node->target_type.get())) {
            push_instruction(std::make_unique<TacDoubleToInt>(std::move(src), dst));
        }
        else {
            push_instruction(std::make_unique<TacDoubleToUInt>(std::move(src), dst));
        }
        return dst;
    }
    else if(node->target_type->type() == AST_T::Double_t) {
        if(is_type_signed(node->exp->exp_type.get())) {
            push_instruction(std::make_unique<TacIntToDouble>(std::move(src), dst));
        }
        else {
            push_instruction(std::make_unique<TacUIntToDouble>(std::move(src), dst));
        }
        return dst;
    }

    int32_t target_type_size = get_type_size(node->target_type.get());
    int32_t inner_type_size = get_type_size(node->exp->exp_type.get());
    if(target_type_size == inner_type_size) {
        push_instruction(std::make_unique<TacCopy>(std::move(src), dst));
    }
    else if(target_type_size < inner_type_size) {
        push_instruction(std::make_unique<TacTruncate>(std::move(src), dst));
    }
    else if(is_type_signed(node->exp->exp_type.get())) {
        push_instruction(std::make_unique<TacSignExtend>(std::move(src), dst));
    }
    else {
        push_instruction(std::make_unique<TacZeroExtend>(std::move(src), dst));
    }
    return dst;
}

/**
cdef TacValue represent_exp_assignment_instructions(CAssignment node):
    cdef TacValue src = represent_exp_instructions(node.exp_right)
    cdef TacValue dst = represent_exp_instructions(node.exp_left)
    instructions.append(TacCopy(src, dst))
    return dst
*/
static std::shared_ptr<TacValue> represent_exp_assignment_instructions(CAssignment* node) {
    std::shared_ptr<TacValue> src = represent_exp_instructions(node->exp_right.get());
    std::shared_ptr<TacValue> dst = represent_exp_instructions(node->exp_left.get());
    push_instruction(std::make_unique<TacCopy>(std::move(src), dst));
    return dst;
}

/**
cdef TacValue represent_exp_conditional_instructions(CConditional node):
    cdef TIdentifier target_else = represent_label_identifier("ternary_else")
    cdef TacValue condition = represent_exp_instructions(node.condition)
    instructions.append(TacJumpIfZero(condition, target_else))
    cdef TacValue src_middle = represent_exp_instructions(node.exp_middle)
    cdef TacValue dst = represent_inner_value(node)
    instructions.append(TacCopy(src_middle, dst))
    cdef TIdentifier target_false = represent_label_identifier("ternary_false")
    instructions.append(TacJump(target_false))
    instructions.append(TacLabel(target_else))
    cdef TacValue src_right = represent_exp_instructions(node.exp_right)
    instructions.append(TacCopy(src_right, dst))
    instructions.append(TacLabel(target_false))
    return dst
*/
static std::shared_ptr<TacValue> represent_exp_conditional_instructions(CConditional* node) {
    TIdentifier target_else = represent_label_identifier("ternary_else");
    TIdentifier target_false = represent_label_identifier("ternary_false");
    std::shared_ptr<TacValue> dst = represent_inner_value(node);
    {
        std::shared_ptr<TacValue> condition = represent_exp_instructions(node->condition.get());
        push_instruction(std::make_unique<TacJumpIfZero>(target_else, std::move(condition)));
    }
    {
        std::shared_ptr<TacValue> src_middle = represent_exp_instructions(node->exp_middle.get());
        push_instruction(std::make_unique<TacCopy>(std::move(src_middle), dst));
    }
    push_instruction(std::make_unique<TacJump>(target_false));
    push_instruction(std::make_unique<TacLabel>(std::move(target_else)));
    {
        std::shared_ptr<TacValue> src_right = represent_exp_instructions(node->exp_right.get());
        push_instruction(std::make_unique<TacCopy>(std::move(src_right), dst));
    }
    push_instruction(std::make_unique<TacLabel>(std::move(target_false)));
    return dst;
}

/**
cdef TacValue represent_exp_unary_instructions(CUnary node):
    cdef TacValue src = represent_exp_instructions(node.exp)
    cdef TacValue dst = represent_inner_value(node)
    cdef TacUnaryOp unary_op = represent_unary_op(node.unary_op)
    instructions.append(TacUnary(unary_op, src, dst))
    return dst
*/
static std::shared_ptr<TacValue> represent_exp_unary_instructions(CUnary* node) {
    std::shared_ptr<TacValue> src = represent_exp_instructions(node->exp.get());
    std::shared_ptr<TacValue> dst = represent_inner_value(node);
    std::unique_ptr<TacUnaryOp> unary_op = represent_unary_op(node->unary_op.get());
    push_instruction(std::make_unique<TacUnary>(std::move(unary_op), std::move(src), dst));
    return dst;
}

/**
cdef TacValue represent_exp_binary_and_instructions(CBinary node):
    cdef TIdentifier target_false = represent_label_identifier("and_false")
    cdef TacValue condition_left = represent_exp_instructions(node.exp_left)
    instructions.append(TacJumpIfZero(condition_left, target_false))
    cdef TacValue condition_right = represent_exp_instructions(node.exp_right)
    instructions.append(TacJumpIfZero(condition_right, target_false))
    cdef TacValue src_true = TacConstant(CConstInt(TInt(1)))
    cdef TacValue src_false = TacConstant(CConstInt(TInt(0)))
    cdef TIdentifier target_true = represent_label_identifier("and_true")
    cdef TacValue dst = represent_inner_value(node)
    instructions.append(TacCopy(src_true, dst))
    instructions.append(TacJump(target_true))
    instructions.append(TacLabel(target_false))
    instructions.append(TacCopy(src_false, dst))
    instructions.append(TacLabel(target_true))
    return dst
*/
static std::shared_ptr<TacValue> represent_exp_binary_and_instructions(CBinary* node) {
    TIdentifier target_false = represent_label_identifier("and_false");
    TIdentifier target_true = represent_label_identifier("and_true");
    std::shared_ptr<TacValue> dst = represent_inner_value(node);
    {
        std::shared_ptr<TacValue> condition_left = represent_exp_instructions(node->exp_left.get());
        push_instruction(std::make_unique<TacJumpIfZero>(target_false,std::move(condition_left)));
    }
    {
        std::shared_ptr<TacValue> condition_right = represent_exp_instructions(node->exp_right.get());
        push_instruction(std::make_unique<TacJumpIfZero>(target_false,std::move(condition_right)));
    }
    {
        std::shared_ptr<CConst> constant = std::make_shared<CConstInt>(1);
        std::shared_ptr<TacValue> src_true = std::make_shared<TacConstant>(std::move(constant));
        push_instruction(std::make_unique<TacCopy>(std::move(src_true), dst));
    }
    push_instruction(std::make_unique<TacJump>(target_true));
    push_instruction(std::make_unique<TacLabel>(std::move(target_false)));
    {
        std::shared_ptr<CConst> constant = std::make_shared<CConstInt>(0);
        std::shared_ptr<TacValue> src_false = std::make_shared<TacConstant>(std::move(constant));
        push_instruction(std::make_unique<TacCopy>(std::move(src_false), dst));
    }
    push_instruction(std::make_unique<TacLabel>(std::move(target_true)));
    return dst;
}

/**
cdef TacValue represent_exp_binary_or_instructions(CBinary node):
    cdef TIdentifier target_true = represent_label_identifier("or_true")
    cdef TacValue condition_left = represent_exp_instructions(node.exp_left)
    instructions.append(TacJumpIfNotZero(condition_left, target_true))
    cdef TacValue condition_right = represent_exp_instructions(node.exp_right)
    instructions.append(TacJumpIfNotZero(condition_right, target_true))
    cdef TacValue src_true = TacConstant(CConstInt(TInt(1)))
    cdef TacValue src_false = TacConstant(CConstInt(TInt(0)))
    cdef TIdentifier target_false = represent_label_identifier("or_false")
    cdef TacValue dst = represent_inner_value(node)
    instructions.append(TacCopy(src_false, dst))
    instructions.append(TacJump(target_false))
    instructions.append(TacLabel(target_true))
    instructions.append(TacCopy(src_true, dst))
    instructions.append(TacLabel(target_false))
    return dst
*/
static std::shared_ptr<TacValue> represent_exp_binary_or_instructions(CBinary* node) {
    TIdentifier target_true = represent_label_identifier("or_true");
    TIdentifier target_false = represent_label_identifier("or_false");
    std::shared_ptr<TacValue> dst = represent_inner_value(node);
    {
        std::shared_ptr<TacValue> condition_left = represent_exp_instructions(node->exp_left.get());
        push_instruction(std::make_unique<TacJumpIfNotZero>(target_true,std::move(condition_left)));
    }
    {
        std::shared_ptr<TacValue> condition_right = represent_exp_instructions(node->exp_right.get());
        push_instruction(std::make_unique<TacJumpIfNotZero>(target_true, std::move(condition_right)));
    }
    {
        std::shared_ptr<CConst> constant = std::make_shared<CConstInt>(0);
        std::shared_ptr<TacValue> src_false = std::make_shared<TacConstant>(std::move(constant));
        push_instruction(std::make_unique<TacCopy>(std::move(src_false), dst));
    }
    push_instruction(std::make_unique<TacJump>(target_false));
    push_instruction(std::make_unique<TacLabel>(std::move(target_true)));
    {
        std::shared_ptr<CConst> constant = std::make_shared<CConstInt>(1);
        std::shared_ptr<TacValue> src_true = std::make_shared<TacConstant>(std::move(constant));
        push_instruction(std::make_unique<TacCopy>(std::move(src_true), dst));
    }
    push_instruction(std::make_unique<TacLabel>(std::move(target_false)));
    return dst;
}

/**
cdef TacValue represent_exp_binary_instructions(CBinary node):
    cdef TacValue src1 = represent_exp_instructions(node.exp_left)
    cdef TacValue src2 = represent_exp_instructions(node.exp_right)
    cdef TacValue dst = represent_inner_value(node)
    cdef TacBinaryOp binary_op = represent_binary_op(node.binary_op)
    instructions.append(TacBinary(binary_op, src1, src2, dst))
    return dst
*/
static std::shared_ptr<TacValue> represent_exp_binary_instructions(CBinary* node) {
    std::shared_ptr<TacValue> src1 = represent_exp_instructions(node->exp_left.get());
    std::shared_ptr<TacValue> src2 = represent_exp_instructions(node->exp_right.get());
    std::shared_ptr<TacValue> dst = represent_inner_value(node);
    std::unique_ptr<TacBinaryOp> binary_op = represent_binary_op(node->binary_op.get());
    push_instruction(std::make_unique<TacBinary>(std::move(binary_op), std::move(src1), std::move(src2), dst));
    return dst;
}

/**
cdef TacValue represent_exp_instructions(CExp node):
    if isinstance(node, CFunctionCall):
        return represent_exp_fun_call_instructions(node)
    elif isinstance(node, CVar):
        return represent_exp_var_instructions(node)
    elif isinstance(node, CConstant):
        return represent_exp_constant_instructions(node)
    elif isinstance(node, CCast):
        return represent_exp_cast_instructions(node)
    elif isinstance(node, CAssignment):
        return represent_exp_assignment_instructions(node)
    elif isinstance(node, CAssignmentCompound):
        return represent_exp_assignment_compound_instructions(node)
    elif isinstance(node, CConditional):
        return represent_exp_conditional_instructions(node)
    elif isinstance(node, CUnary):
        return represent_exp_unary_instructions(node)
    elif isinstance(node, CBinary):
        if isinstance(node.binary_op, CAnd):
            return represent_exp_binary_and_instructions(node)
        elif isinstance(node.binary_op, COr):
            return represent_exp_binary_or_instructions(node)
        elif isinstance(node.binary_op, (CEqual, CNotEqual, CLessThan, CLessOrEqual, CGreaterThan, CGreaterOrEqual,
                                       CAdd, CSubtract, CMultiply, CDivide, CRemainder, CBitAnd, CBitOr, CBitXor,
                                       CBitShiftLeft, CBitShiftRight)):
            return represent_exp_binary_instructions(node)
        else:

            raise RuntimeError(
                "An error occurred in three address code representation, not all nodes were visited")

    else:

        raise RuntimeError(
            "An error occurred in three address code representation, not all nodes were visited")
*/
static std::shared_ptr<TacValue> represent_exp_instructions(CExp* node) {
    switch(node->type()) {
        case AST_T::CFunctionCall_t:
            return represent_exp_fun_call_instructions(static_cast<CFunctionCall*>(node));
        case AST_T::CVar_t:
            return represent_exp_var_instructions(static_cast<CVar*>(node));
        case AST_T::CConstant_t:
            return represent_exp_constant_instructions(static_cast<CConstant*>(node));
        case AST_T::CCast_t:
            return represent_exp_cast_instructions(static_cast<CCast*>(node));
        case AST_T::CAssignment_t:
            return represent_exp_assignment_instructions(static_cast<CAssignment*>(node));
        case AST_T::CConditional_t:
            return represent_exp_conditional_instructions(static_cast<CConditional*>(node));
        case AST_T::CUnary_t:
            return represent_exp_unary_instructions(static_cast<CUnary*>(node));
        case AST_T::CBinary_t: {
            CBinary* p_node = static_cast<CBinary*>(node);
            switch(p_node->binary_op->type()) {
                case AST_T::CAnd_t:
                    return represent_exp_binary_and_instructions(p_node);
                case AST_T::COr_t:
                    return represent_exp_binary_or_instructions(p_node);
                default:
                    return represent_exp_binary_instructions(p_node);
            }
        }
        default:
            raise_internal_error("An error occurred in three address code representation, "
                                 "not all nodes were visited");
    }
}

static void represent_block(CBlock* node);

static void represent_statement_instructions(CStatement* node);
static void represent_variable_declaration_instructions(CVariableDeclaration* node);

/**
cdef void represent_statement_null_instructions(CNull node):
    pass
*/
static void represent_statement_null_instructions(CNull* /*node*/) {
    ;
}

/**
cdef void represent_statement_return_instructions(CReturn node):
    cdef TacValue val = represent_exp_instructions(node.exp)
    instructions.append(TacReturn(val))
*/
static void represent_statement_return_instructions(CReturn* node) {
    std::shared_ptr<TacValue> val = represent_exp_instructions(node->exp.get());
    push_instruction(std::make_unique<TacReturn>(std::move(val)));
}

/**
cdef void represent_statement_compound_instructions(CCompound node):
    represent_block(node.block)
*/
static void represent_statement_compound_instructions(CCompound* node) {
    represent_block(node->block.get());
}

/**
cdef void represent_statement_expression_instructions(CExpression node):
    _ = represent_exp_instructions(node.exp)
*/
static void represent_statement_expression_instructions(CExpression* node) {
    represent_exp_instructions(node->exp.get());
}

/**
cdef void represent_statement_if_instructions(CIf node):
    cdef TIdentifier target_false = represent_label_identifier("if_false")
    cdef TacValue condition = represent_exp_instructions(node.condition)
    instructions.append(TacJumpIfZero(condition, target_false))
    represent_statement_instructions(node.then)
    instructions.append(TacLabel(target_false))
*/
static void represent_statement_if_instructions(CIf* node) {
    TIdentifier target_false = represent_label_identifier("if_false");
    {
        std::shared_ptr<TacValue> condition = represent_exp_instructions(node->condition.get());
        push_instruction(std::make_unique<TacJumpIfZero>(target_false, std::move(condition)));
    }
    represent_statement_instructions(node->then.get());
    push_instruction(std::make_unique<TacLabel>(std::move(target_false)));
}

/**
cdef void represent_statement_if_else_instructions(CIf node):
    cdef TIdentifier target_else = represent_label_identifier("if_else")
    cdef TacValue condition = represent_exp_instructions(node.condition)
    instructions.append(TacJumpIfZero(condition, target_else))
    represent_statement_instructions(node.then)
    cdef TIdentifier target_false = represent_label_identifier("if_false")
    instructions.append(TacJump(target_false))
    instructions.append(TacLabel(target_else))
    represent_statement_instructions(node.else_fi)
    instructions.append(TacLabel(target_false))
*/
static void represent_statement_if_else_instructions(CIf* node) {
    TIdentifier target_else = represent_label_identifier("if_else");
    TIdentifier target_false = represent_label_identifier("if_false");
    {
        std::shared_ptr<TacValue> condition = represent_exp_instructions(node->condition.get());
        push_instruction(std::make_unique<TacJumpIfZero>(target_else, std::move(condition)));
    }
    represent_statement_instructions(node->then.get());
    push_instruction(std::make_unique<TacJump>(target_false));
    push_instruction(std::make_unique<TacLabel>(std::move(target_else)));
    represent_statement_instructions(node->else_fi.get());
    push_instruction(std::make_unique<TacLabel>(std::move(target_false)));
}

/**
cdef void represent_statement_while_instructions(CWhile node):
    cdef TIdentifier target_continue = TIdentifier("continue_" + node.target.str_t)
    instructions.append(TacLabel(target_continue))
    cdef TacValue condition = represent_exp_instructions(node.condition)
    cdef TIdentifier target_break = TIdentifier("break_" + node.target.str_t)
    instructions.append(TacJumpIfZero(condition, target_break))
    represent_statement_instructions(node.body)
    instructions.append(TacJump(target_continue))
    instructions.append(TacLabel(target_break))
*/
static void represent_statement_while_instructions(CWhile* node) {
    TIdentifier target_continue = "continue_" + node->target;
    TIdentifier target_break = "break_" + node->target;
    push_instruction(std::make_unique<TacLabel>(target_continue));
    {
        std::shared_ptr<TacValue> condition = represent_exp_instructions(node->condition.get());
        push_instruction(std::make_unique<TacJumpIfZero>(target_break, std::move(condition)));
    }
    represent_statement_instructions(node->body.get());
    push_instruction(std::make_unique<TacJump>(std::move(target_continue)));
    push_instruction(std::make_unique<TacLabel>(std::move(target_break)));
}

/**
cdef void represent_statement_do_while_instructions(CDoWhile node):
    cdef TIdentifier target_do_while_start = represent_label_identifier("do_while_start")
    instructions.append(TacLabel(target_do_while_start))
    represent_statement_instructions(node.body)
    cdef TIdentifier target_continue = TIdentifier("continue_" + node.target.str_t)
    instructions.append(TacLabel(target_continue))
    cdef TacValue condition = represent_exp_instructions(node.condition)
    instructions.append(TacJumpIfNotZero(condition, target_do_while_start))
    cdef TIdentifier target_break = TIdentifier("break_" + node.target.str_t)
    instructions.append(TacLabel(target_break))
*/
static void represent_statement_do_while_instructions(CDoWhile* node) {
    TIdentifier target_do_while_start = represent_label_identifier("do_while_start");
    TIdentifier target_continue = "continue_" + node->target;
    TIdentifier target_break = "break_" + node->target;
    push_instruction(std::make_unique<TacLabel>(target_do_while_start));
    represent_statement_instructions(node->body.get());
    push_instruction(std::make_unique<TacLabel>(std::move(target_continue)));
    {
        std::shared_ptr<TacValue> condition = represent_exp_instructions(node->condition.get());
        push_instruction(std::make_unique<TacJumpIfNotZero>(std::move(target_do_while_start),
                                                                      std::move(condition)));
    }
    push_instruction(std::make_unique<TacLabel>(std::move(target_break)));
}

/**
cdef void represent_for_init_decl_instructions(CInitDecl node):
    represent_variable_declaration_instructions(node.init)
*/
static void represent_for_init_decl_instructions(CInitDecl* node) {
    represent_variable_declaration_instructions(node->init.get());
}

/**
cdef void represent_for_init_exp_instructions(CInitExp node):
    if node.init:
        _ = represent_exp_instructions(node.init)
*/
static void represent_for_init_exp_instructions(CInitExp* node) {
    if(node->init) {
        represent_exp_instructions(node->init.get());
    }
}

/**
cdef void represent_statement_for_init_instructions(CForInit node):
    if isinstance(node, CInitDecl):
        represent_for_init_decl_instructions(node)
    elif isinstance(node, CInitExp):
        represent_for_init_exp_instructions(node)
    else:

        raise RuntimeError(
            "An error occurred in three address code representation, not all nodes were visited")
*/
static void represent_statement_for_init_instructions(CForInit* node) {
    switch(node->type()) {
        case AST_T::CInitDecl_t:
            represent_for_init_decl_instructions(static_cast<CInitDecl*>(node));
            break;
        case AST_T::CInitExp_t:
            represent_for_init_exp_instructions(static_cast<CInitExp*>(node));
            break;
        default:
            raise_internal_error("An error occurred in three address code representation, "
                                 "not all nodes were visited");
    }
}

/**
cdef void represent_statement_for_instructions(CFor node):
    represent_statement_for_init_instructions(node.init)
    cdef TIdentifier target_for_start = represent_label_identifier("for_start")
    instructions.append(TacLabel(target_for_start))
    cdef TIdentifier target_break = TIdentifier("break_" + node.target.str_t)
    cdef TacValue condition
    if node.condition:
        condition = represent_exp_instructions(node.condition)
        instructions.append(TacJumpIfZero(condition, target_break))
    represent_statement_instructions(node.body)
    cdef TIdentifier target_continue = TIdentifier("continue_" + node.target.str_t)
    instructions.append(TacLabel(target_continue))
    if node.post:
        _ = represent_exp_instructions(node.post)
    instructions.append(TacJump(target_for_start))
    instructions.append(TacLabel(target_break))
*/
static void represent_statement_for_instructions(CFor* node) {
    TIdentifier target_for_start = represent_label_identifier("for_start");
    TIdentifier target_break = "break_" + node->target;
    TIdentifier target_continue = "continue_" + node->target;
    represent_statement_for_init_instructions(node->init.get());
    push_instruction(std::make_unique<TacLabel>(target_for_start));
    if(node->condition) {
        std::shared_ptr<TacValue> condition = represent_exp_instructions(node->condition.get());
        push_instruction(std::make_unique<TacJumpIfZero>(target_break, std::move(condition)));
    }
    represent_statement_instructions(node->body.get());
    push_instruction(std::make_unique<TacLabel>(std::move(target_continue)));
    if(node->post) {
        represent_exp_instructions(node->post.get());
    }
    push_instruction(std::make_unique<TacJump>(std::move(target_for_start)));
    push_instruction(std::make_unique<TacLabel>(std::move(target_break)));
}

/**
cdef void represent_statement_break_instructions(CBreak node):
    cdef TIdentifier target_break = TIdentifier("break_" + node.target.str_t)
    instructions.append(TacJump(target_break))
*/
static void represent_statement_break_instructions(CBreak* node) {
    TIdentifier target_break = "break_" + node->target;
    push_instruction(std::make_unique<TacJump>(std::move(target_break)));
}

/**
cdef void represent_statement_continue_instructions(CContinue node):
    cdef TIdentifier target_continue = TIdentifier("continue_" + node.target.str_t)
    instructions.append(TacJump(target_continue))
*/
static void represent_statement_continue_instructions(CContinue* node) {
    TIdentifier target_continue = "continue_" + node->target;
    push_instruction(std::make_unique<TacJump>(std::move(target_continue)));
}

/**
cdef void represent_statement_goto_instructions(CGoto node):
    cdef TIdentifier target_label = node.target
    instructions.append(TacJump(target_label))
*/
static void represent_statement_goto_instructions(CGoto* node) {
    TIdentifier target_label = node->target;
    push_instruction(std::make_unique<TacJump>(std::move(target_label)));
}

/**
cdef void represent_statement_label_instructions(CLabel node):
    cdef TIdentifier target_label = node.target
    instructions.append(TacLabel(target_label))
    represent_statement_instructions(node.jump_to)
*/
static void represent_statement_label_instructions(CLabel* node) {
    TIdentifier target_label = node->target;
    push_instruction(std::make_unique<TacLabel>(std::move(target_label)));
    represent_statement_instructions(node->jump_to.get());
}

/**
cdef void represent_statement_instructions(CStatement node):
    if isinstance(node, CNull):
        represent_statement_null_instructions(node)
    elif isinstance(node, CReturn):
        represent_statement_return_instructions(node)
    elif isinstance(node, CCompound):
        represent_statement_compound_instructions(node)
    elif isinstance(node, CExpression):
        represent_statement_expression_instructions(node)
    elif isinstance(node, CIf):
        if node.else_fi:
            represent_statement_if_else_instructions(node)
        else:
            represent_statement_if_instructions(node)
    elif isinstance(node, CWhile):
        represent_statement_while_instructions(node)
    elif isinstance(node, CDoWhile):
        represent_statement_do_while_instructions(node)
    elif isinstance(node, CFor):
        represent_statement_for_instructions(node)
    elif isinstance(node, CBreak):
        represent_statement_break_instructions(node)
    elif isinstance(node, CContinue):
        represent_statement_continue_instructions(node)
    elif isinstance(node, CGoto):
        represent_statement_goto_instructions(node)
    elif isinstance(node, CLabel):
        represent_statement_label_instructions(node)
    else:

        raise RuntimeError(
            "An error occurred in three address code representation, not all nodes were visited")
*/
static void represent_statement_instructions(CStatement* node) {
    switch(node->type()) {
        case AST_T::CNull_t:
            represent_statement_null_instructions(static_cast<CNull*>(node));
            break;
        case AST_T::CReturn_t:
            represent_statement_return_instructions(static_cast<CReturn*>(node));
            break;
        case AST_T::CCompound_t:
            represent_statement_compound_instructions(static_cast<CCompound*>(node));
            break;
        case AST_T::CExpression_t:
            represent_statement_expression_instructions(static_cast<CExpression*>(node));
            break;
        case AST_T::CIf_t: {
            CIf* p_node = static_cast<CIf*>(node);
            if(p_node->else_fi) {
                represent_statement_if_else_instructions(p_node);
            }
            else {
                represent_statement_if_instructions(p_node);
            }
            break;
        }
        case AST_T::CWhile_t:
            represent_statement_while_instructions(static_cast<CWhile*>(node));
            break;
        case AST_T::CDoWhile_t:
            represent_statement_do_while_instructions(static_cast<CDoWhile*>(node));
            break;
        case AST_T::CFor_t:
            represent_statement_for_instructions(static_cast<CFor*>(node));
            break;
        case AST_T::CBreak_t:
            represent_statement_break_instructions(static_cast<CBreak*>(node));
            break;
        case AST_T::CContinue_t:
            represent_statement_continue_instructions(static_cast<CContinue*>(node));
            break;
        case AST_T::CGoto_t:
            represent_statement_goto_instructions(static_cast<CGoto*>(node));
            break;
        case AST_T::CLabel_t:
            represent_statement_label_instructions(static_cast<CLabel*>(node));
            break;
        default:
            raise_internal_error("An error occurred in three address code representation, "
                                 "not all nodes were visited");
    }
}

/**
cdef void represent_variable_declaration_instructions(CVariableDeclaration node):
    cdef TacValue src = represent_exp_instructions(node.init)
    cdef TacValue dst = represent_value(CVar(node.name))
    instructions.append(TacCopy(src, dst))
*/
static void represent_variable_declaration_instructions(CVariableDeclaration* node) {
    std::shared_ptr<TacValue> src = represent_exp_instructions(node->init.get());
    std::shared_ptr<TacValue> dst;
    {
        TIdentifier name = node->name;
        std::unique_ptr<CExp> exp = std::make_unique<CVar>(std::move(name));
        dst = represent_value(exp.get());
    }
    push_instruction(std::make_unique<TacCopy>(std::move(src), std::move(dst)));
}

/**
cdef void represent_declaration_var_decl_instructions(CVarDecl node):
    if isinstance(symbol_table[node.variable_decl.name.str_t].attrs, StaticAttr):
        return
    if node.variable_decl.init:
        represent_variable_declaration_instructions(node.variable_decl)
*/
static void represent_declaration_var_decl_instructions(CVarDecl* node) {
    if(symbol_table[node->variable_decl->name]->attrs->type() == AST_T::StaticAttr_t) {
        return;
    }
    if(node->variable_decl->init) {
        represent_variable_declaration_instructions(node->variable_decl.get());
    }
}

/**
cdef void represent_declaration_fun_decl_instructions(CFunDecl node):
    pass
*/
static void represent_declaration_fun_decl_instructions(CFunDecl* /*node*/) {
    ;
}

/**
cdef void represent_declaration_instructions(CDeclaration node):
    if isinstance(node, CFunDecl):
        represent_declaration_fun_decl_instructions(node)
    elif isinstance(node, CVarDecl):
        represent_declaration_var_decl_instructions(node)
    else:

        raise RuntimeError(
            "An error occurred in three address code representation, not all nodes were visited")
*/
static void represent_declaration_instructions(CDeclaration* node) {
    switch(node->type()) {
        case AST_T::CFunDecl_t:
            represent_declaration_fun_decl_instructions(static_cast<CFunDecl*>(node));
            break;
        case AST_T::CVarDecl_t:
            represent_declaration_var_decl_instructions(static_cast<CVarDecl*>(node));
            break;
        default:
            raise_internal_error("An error occurred in three address code representation, "
                                 "not all nodes were visited");
    }
}

/**
cdef void represent_list_instructions(list[CBlockItem] list_node):
    # instruction = Return(val) | FunCall(identifier fun_name, val* args, val dst)
    #             | Unary(unary_operator, val src, val dst) | Binary(binary_operator, val src1, val src2, val dst)
    #             | Copy(val src, val dst) | Jump(identifier target) | JumpIfZero(val condition, identifier target)
    #             | JumpIfNotZero(val condition, identifier target) | Label(identifier name)

    cdef Py_ssize_t block_item
    for block_item in range(len(list_node)):
        if isinstance(list_node[block_item], CS):
            represent_statement_instructions(list_node[block_item].statement)
        elif isinstance(list_node[block_item], CD):
            represent_declaration_instructions(list_node[block_item].declaration)
        else:

            raise RuntimeError(
                "An error occurred in three address code representation, not all nodes were visited")
*/
// instruction = Return(val) | FunCall(identifier fun_name, val* args, val dst)
//             | Unary(unary_operator, val src, val dst) | Binary(binary_operator, val src1, val src2, val dst)
//             | Copy(val src, val dst) | Jump(identifier target) | JumpIfZero(val condition, identifier target)
//             | JumpIfNotZero(val condition, identifier target) | Label(identifier name)
static void represent_list_instructions(std::vector<std::unique_ptr<CBlockItem>>& list_node) {
    for(size_t block_item = 0; block_item < list_node.size(); block_item++) {
        switch(list_node[block_item]->type()) {
            case AST_T::CS_t:
                represent_statement_instructions(static_cast<CS*>(list_node[block_item].get())->statement.get());
                break;
            case AST_T::CD_t:
                represent_declaration_instructions(static_cast<CD*>(list_node[block_item].get())->declaration.get());
                break;
            default:
                raise_internal_error("An error occurred in three address code representation, "
                                     "not all nodes were visited");
        }
    }
}

/**
cdef void represent_block(CBlock node):
    # block = Block(block_item* block_items)
    if isinstance(node, CB):
        represent_list_instructions(node.block_items)
    else:

        raise RuntimeError(
            "An error occurred in three address code representation, not all nodes were visited")
*/
// block = Block(block_item* block_items)
static void represent_block(CBlock* node) {
    switch(node->type()) {
        case AST_T::CB_t:
            represent_list_instructions(static_cast<CB*>(node)->block_items);
            break;
        default:
            raise_internal_error("An error occurred in three address code representation, "
                                 "not all nodes were visited");
    }
}

/**
cdef TacFunction represent_function_top_level(CFunctionDeclaration node):
    global instructions

    cdef TIdentifier name = copy_identifier(node.name)
    cdef bint is_global = symbol_table[node.name.str_t].attrs.is_global
    cdef Py_ssize_t param
    cdef list[TIdentifier] params = []
    for param in range(len(node.params)):
        params.append(copy_identifier(node.params[param]))
    cdef list[TacInstruction] body = []
    instructions = body
    represent_block(node.body)
    instructions.append(TacReturn(TacConstant(CConstInt(TInt(0)))))
    return TacFunction(name, is_global, params, body)
*/
static std::unique_ptr<TacFunction> represent_function_top_level(CFunctionDeclaration* node) {
    TIdentifier name = node->name;
    bool is_global = static_cast<FunAttr*>(symbol_table[node->name]->attrs.get())->is_global;

    std::vector<TIdentifier> params;
    for(size_t param = 0; param < node->params.size(); param++) {
        TIdentifier identifier = node->params[param];
        params.push_back(std::move(identifier));
    }

    std::vector<std::unique_ptr<TacInstruction>> body;
    p_instructions = &body;
    represent_block(node->body.get());
    {
        std::shared_ptr<CConst> constant = std::make_shared<CConstInt>(0);
        std::shared_ptr<TacValue> val = std::make_shared<TacConstant>(std::move(constant));
        push_instruction(std::make_unique<TacReturn>(std::move(val)));
    }
    p_instructions = nullptr;

    return std::make_unique<TacFunction>(std::move(name), is_global, std::move(params), std::move(body));
}

/** TODO
cdef list[TacTopLevel] function_top_levels = []
*/

/** TODO
cdef void represent_fun_decl_top_level(CFunDecl node):
    if node.function_decl.body:
        function_top_levels.append(represent_function_top_level(node.function_decl))
*/

/** TODO
cdef void represent_var_decl_top_level(CVarDecl node):
    pass
*/

/** TODO
cdef void represent_declaration_top_level(CDeclaration node):
    # top_level = Function(identifier, bool global, identifier* params, instruction* body)
    if isinstance(node, CFunDecl):
        represent_fun_decl_top_level(node)
    elif isinstance(node, CVarDecl):
        represent_var_decl_top_level(node)
    else:

        raise RuntimeError(
            "An error occurred in three address code representation, not all nodes were visited")
*/

/** TODO
cdef list[TacTopLevel] static_variable_top_levels = []
*/

/** TODO
cdef StaticInit represent_tentative_static_init(Type static_init_type):
    if isinstance(static_init_type, Int):
        return IntInit(TInt(0))
    elif isinstance(static_init_type, Long):
        return LongInit(TLong(0))
    elif isinstance(static_init_type, Double):
        return DoubleInit(TDouble(0.0))
    elif isinstance(static_init_type, UInt):
        return UIntInit(TUInt(0))
    elif isinstance(static_init_type, ULong):
        return ULongInit(TULong(0))
    else:

        raise RuntimeError(
            "An error occurred in three address code representation, not all nodes were visited")
*/

/** TODO
cdef void represent_static_variable_top_level(StaticAttr node, Type static_init_type, str symbol):
    if isinstance(node.init, NoInitializer):
        return

    cdef TIdentifier name = TIdentifier(symbol)
    cdef bint is_global = node.is_global
    cdef StaticInit initial_value
    if isinstance(node.init, Initial):
        initial_value = node.init.static_init
    elif isinstance(node.init, Tentative):
        initial_value = represent_tentative_static_init(static_init_type)
    else:

        raise RuntimeError(
            "An error occurred in three address code representation, top level variable has invalid initializer")

    static_variable_top_levels.append(TacStaticVariable(name, is_global, static_init_type, initial_value))
*/

/** TODO
cdef void represent_symbol_top_level(Symbol node, str symbol):
    # top_level = StaticVariable(identifier, bool global, int init)
    if isinstance(node.attrs, StaticAttr):
        represent_static_variable_top_level(node.attrs, node.type_t, symbol)
*/

/** TODO
cdef TacProgram represent_program(CProgram node):
    # program = Program(top_level*)
    global function_top_levels

    cdef list[TacTopLevel] top_levels = []
    function_top_levels = top_levels
    cdef Py_ssize_t declaration
    for declaration in range(len(node.declarations)):
        represent_declaration_top_level(node.declarations[declaration])

    static_variable_top_levels.clear()
    cdef str symbol
    for symbol in symbol_table:
        represent_symbol_top_level(symbol_table[symbol], symbol)
    top_levels = static_variable_top_levels + top_levels

    return TacProgram(top_levels)
*/

/** TODO
cdef TacProgram three_address_code_representation(CProgram c_ast):

    cdef TacProgram tac_ast = represent_program(c_ast)

    if not tac_ast:
        raise RuntimeError(
            "An error occurred in three address code representation, Asm was not generated")

    return tac_ast
*/
