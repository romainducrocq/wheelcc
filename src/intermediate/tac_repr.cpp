#include "intermediate/tac_repr.hpp"
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

// binary_operator = Add | Subtract | Multiply | Divide | Remainder | BitAnd | BitOr | BitXor | BitShiftLeft
//                 | BitShiftRight | Equal | NotEqual | LessThan | LessOrEqual | GreaterThan | GreaterOrEqual
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
            RAISE_INTERNAL_ERROR;
    }
}

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
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<TacVariable> represent_variable_value(CVar* node) {
    TIdentifier name = node->name;
    return std::make_shared<TacVariable>(std::move(name));
}

static std::shared_ptr<TacConstant> represent_constant_value(CConstant* node) {
    std::shared_ptr<CConst> constant = node->constant;
    return std::make_shared<TacConstant>(std::move(constant));
}

static std::shared_ptr<TacVariable> represent_inner_exp_value(CExp* node) {
    TIdentifier inner_name = represent_variable_identifier(node);
    std::shared_ptr<Type> inner_type = node->exp_type;
    std::unique_ptr<IdentifierAttr> inner_attrs = std::make_unique<LocalAttr>();
    std::unique_ptr<Symbol> symbol = std::make_unique<Symbol>(std::move(inner_type), std::move(inner_attrs));
    symbol_table[inner_name] = std::move(symbol);
    return std::make_shared<TacVariable>(std::move(inner_name));
}

static std::shared_ptr<TacValue> represent_inner_value(CExp* node) {
    return represent_inner_exp_value(node);
}

// val = Constant(int) | Var(identifier)
static std::shared_ptr<TacValue> represent_value(CExp* node) {
    switch(node->type()) {
        case AST_T::CVar_t:
            return represent_variable_value(static_cast<CVar*>(node));
        case AST_T::CConstant_t:
            return represent_constant_value(static_cast<CConstant*>(node));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::vector<std::unique_ptr<TacInstruction>>* p_instructions;

static void push_instruction(std::unique_ptr<TacInstruction>&& instruction) {
    p_instructions->push_back(std::move(instruction));
}

static std::unique_ptr<TacExpResult> represent_exp_result_instructions(CExp* node);
static std::shared_ptr<TacValue> represent_exp_instructions(CExp* node);

static std::unique_ptr<TacExpResult> represent_exp_result_constant_instructions(CConstant* node) {
    std::shared_ptr<TacValue> val = represent_value(node);
    return std::make_unique<TacPlainOperand>(std::move(val));
}

static std::unique_ptr<TacExpResult> represent_exp_result_var_instructions(CVar* node) {
    std::shared_ptr<TacValue> val = represent_value(node);
    return std::make_unique<TacPlainOperand>(std::move(val));
}

static std::unique_ptr<TacExpResult> represent_exp_result_dereference_instructions(CDereference* node) {
    std::shared_ptr<TacValue> val = represent_exp_instructions(node->exp.get());
    return std::make_unique<TacDereferencedPointer>(std::move(val));
}

static std::unique_ptr<TacExpResult> represent_exp_result_plain_operand_addrof_instructions(TacPlainOperand* res,
                                                                                            CAddrOf* node) {
    std::shared_ptr<TacValue> src = std::move(res->val);
    std::shared_ptr<TacValue> dst = represent_inner_value(node);
    push_instruction(std::make_unique<TacGetAddress>(std::move(src), dst));
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacExpResult> represent_exp_result_dereference_pointer_addrof_instructions(
                                                                                          TacDereferencedPointer* res) {
    std::shared_ptr<TacValue> val = std::move(res->val);
    return std::make_unique<TacPlainOperand>(std::move(val));
}

static std::unique_ptr<TacExpResult> represent_exp_result_addrof_instructions(CAddrOf* node) {
    std::unique_ptr<TacExpResult> res = represent_exp_result_instructions(node->exp.get());
    switch(res->type()) {
        case AST_T::TacPlainOperand_t:
            return represent_exp_result_plain_operand_addrof_instructions(static_cast<TacPlainOperand*>(res.get()),
                                                                          node);
        case AST_T::TacDereferencedPointer_t:
            return represent_exp_result_dereference_pointer_addrof_instructions(
                                                                   static_cast<TacDereferencedPointer*>(res.get()));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::unique_ptr<TacExpResult> represent_exp_result_fun_call_instructions(CFunctionCall* node) {
    TIdentifier name = node->name;
    std::vector<std::shared_ptr<TacValue>> args;
    for(size_t i = 0; i < node->args.size(); i++) {
        std::shared_ptr<TacValue> arg = represent_exp_instructions(node->args[i].get());
        args.push_back(std::move(arg));
    }
    std::shared_ptr<TacValue> dst = represent_inner_value(node);
    push_instruction(std::make_unique<TacFunCall>(std::move(name), std::move(args), dst));
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacExpResult> represent_exp_result_cast_instructions(CCast* node) {
    std::shared_ptr<TacValue> src = represent_exp_instructions(node->exp.get());
    if(is_same_type(node->target_type.get(), node->exp->exp_type.get())) {
        return std::make_unique<TacPlainOperand>(std::move(src));
    }

    std::shared_ptr<TacValue> dst = represent_inner_value(node);
    if(node->exp->exp_type->type() == AST_T::Double_t) {
        if(is_type_signed(node->target_type.get())) {
            push_instruction(std::make_unique<TacDoubleToInt>(std::move(src), dst));
        }
        else {
            push_instruction(std::make_unique<TacDoubleToUInt>(std::move(src), dst));
        }
        return std::make_unique<TacPlainOperand>(std::move(dst));
    }
    else if(node->target_type->type() == AST_T::Double_t) {
        if(is_type_signed(node->exp->exp_type.get())) {
            push_instruction(std::make_unique<TacIntToDouble>(std::move(src), dst));
        }
        else {
            push_instruction(std::make_unique<TacUIntToDouble>(std::move(src), dst));
        }
        return std::make_unique<TacPlainOperand>(std::move(dst));
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
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacExpResult> represent_exp_result_plain_operand_assignment_instructions(
                                                                                      std::unique_ptr<TacExpResult> res,
                                                                                      std::shared_ptr<TacValue> src) {
    std::shared_ptr<TacValue> dst = static_cast<TacPlainOperand*>(res.get())->val;
    push_instruction(std::make_unique<TacCopy>(std::move(src), dst));
    return res;
}

static std::unique_ptr<TacExpResult> represent_exp_result_dereference_pointer_assignment_instructions(
                                                                                        TacDereferencedPointer* res,
                                                                                        std::shared_ptr<TacValue> src) {
    std::shared_ptr<TacValue> dst = std::move(res->val);
    push_instruction(std::make_unique<TacStore>(std::move(src), dst));
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacExpResult> represent_exp_result_assignment_instructions(CAssignment* node) {
    std::shared_ptr<TacValue> src = represent_exp_instructions(node->exp_right.get());
    std::unique_ptr<TacExpResult> res;
    if(node->exp_left) {
        res = represent_exp_result_instructions(node->exp_left.get());
    }
    else {
        CExp* exp_left = node->exp_right.get();
        if(exp_left->type() == AST_T::CCast_t) {
            exp_left = static_cast<CCast*>(exp_left)->exp.get();
        }
        exp_left = static_cast<CBinary*>(exp_left)->exp_left.get();
        if(exp_left->type() == AST_T::CCast_t) {
            exp_left = static_cast<CCast*>(exp_left)->exp.get();
        }
        res = represent_exp_result_instructions(exp_left);
    }
    switch(res->type()) {
        case AST_T::TacPlainOperand_t:
            return represent_exp_result_plain_operand_assignment_instructions(std::move(res), std::move(src));
        case AST_T::TacDereferencedPointer_t:
            return represent_exp_result_dereference_pointer_assignment_instructions(
                                                                    static_cast<TacDereferencedPointer*>(res.get()),
                                                                    std::move(src));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::unique_ptr<TacExpResult> represent_exp_result_conditional_instructions(CConditional* node) {
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
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacExpResult> represent_exp_result_unary_instructions(CUnary* node) {
    std::shared_ptr<TacValue> src = represent_exp_instructions(node->exp.get());
    std::shared_ptr<TacValue> dst = represent_inner_value(node);
    std::unique_ptr<TacUnaryOp> unary_op = represent_unary_op(node->unary_op.get());
    push_instruction(std::make_unique<TacUnary>(std::move(unary_op), std::move(src), dst));
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacExpResult> represent_exp_result_binary_and_instructions(CBinary* node) {
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
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacExpResult> represent_exp_result_binary_or_instructions(CBinary* node) {
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
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacExpResult> represent_exp_result_binary_instructions(CBinary* node) {
    std::shared_ptr<TacValue> src1 = represent_exp_instructions(node->exp_left.get());
    std::shared_ptr<TacValue> src2 = represent_exp_instructions(node->exp_right.get());
    std::shared_ptr<TacValue> dst = represent_inner_value(node);
    std::unique_ptr<TacBinaryOp> binary_op = represent_binary_op(node->binary_op.get());
    push_instruction(std::make_unique<TacBinary>(std::move(binary_op), std::move(src1), std::move(src2), dst));
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacExpResult> represent_exp_result_instructions(CExp* node) {
    switch(node->type()) {
        case AST_T::CConstant_t:
            return represent_exp_result_constant_instructions(static_cast<CConstant*>(node));
        case AST_T::CVar_t:
            return represent_exp_result_var_instructions(static_cast<CVar*>(node));
        case AST_T::CDereference_t:
            return represent_exp_result_dereference_instructions(static_cast<CDereference*>(node));
        case AST_T::CAddrOf_t:
            return represent_exp_result_addrof_instructions(static_cast<CAddrOf*>(node));
        case AST_T::CFunctionCall_t:
            return represent_exp_result_fun_call_instructions(static_cast<CFunctionCall*>(node));
        case AST_T::CCast_t:
            return represent_exp_result_cast_instructions(static_cast<CCast*>(node));
        case AST_T::CAssignment_t:
            return represent_exp_result_assignment_instructions(static_cast<CAssignment*>(node));
        case AST_T::CConditional_t:
            return represent_exp_result_conditional_instructions(static_cast<CConditional*>(node));
        case AST_T::CUnary_t:
            return represent_exp_result_unary_instructions(static_cast<CUnary*>(node));
        case AST_T::CBinary_t: {
            CBinary* p_node = static_cast<CBinary*>(node);
            switch(p_node->binary_op->type()) {
                case AST_T::CAnd_t:
                    return represent_exp_result_binary_and_instructions(p_node);
                case AST_T::COr_t:
                    return represent_exp_result_binary_or_instructions(p_node);
                default:
                    return represent_exp_result_binary_instructions(p_node);
            }
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<TacValue> represent_exp_plain_operand_instructions(TacPlainOperand* res) {
    std::shared_ptr<TacValue> dst = std::move(res->val);
    return dst;
}

static std::shared_ptr<TacValue> represent_exp_dereferenced_pointer_instructions(TacDereferencedPointer* res,
                                                                                 CExp* node) {
    std::shared_ptr<TacValue> src = std::move(res->val);
    std::shared_ptr<TacValue> dst = represent_inner_value(node);
    push_instruction(std::make_unique<TacLoad>(std::move(src), dst));
    return dst;
}

static std::shared_ptr<TacValue> represent_exp_instructions(CExp* node) {
    std::unique_ptr<TacExpResult> res = represent_exp_result_instructions(node);
    switch(res->type()) {
        case AST_T::TacPlainOperand_t:
            return represent_exp_plain_operand_instructions(static_cast<TacPlainOperand*>(res.get()));
        case AST_T::TacDereferencedPointer_t:
            return represent_exp_dereferenced_pointer_instructions(static_cast<TacDereferencedPointer*>(res.get()),
                                                                   node);
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void represent_block(CBlock* node);

static void represent_statement_instructions(CStatement* node);
static void represent_variable_declaration_instructions(CVariableDeclaration* node);

static void represent_statement_return_instructions(CReturn* node) {
    std::shared_ptr<TacValue> val = represent_exp_instructions(node->exp.get());
    push_instruction(std::make_unique<TacReturn>(std::move(val)));
}

static void represent_statement_compound_instructions(CCompound* node) {
    represent_block(node->block.get());
}

static void represent_statement_expression_instructions(CExpression* node) {
    represent_exp_instructions(node->exp.get());
}

static void represent_statement_if_instructions(CIf* node) {
    TIdentifier target_false = represent_label_identifier("if_false");
    {
        std::shared_ptr<TacValue> condition = represent_exp_instructions(node->condition.get());
        push_instruction(std::make_unique<TacJumpIfZero>(target_false, std::move(condition)));
    }
    represent_statement_instructions(node->then.get());
    push_instruction(std::make_unique<TacLabel>(std::move(target_false)));
}

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

static void represent_for_init_decl_instructions(CInitDecl* node) {
    represent_variable_declaration_instructions(node->init.get());
}

static void represent_for_init_exp_instructions(CInitExp* node) {
    if(node->init) {
        represent_exp_instructions(node->init.get());
    }
}

static void represent_statement_for_init_instructions(CForInit* node) {
    switch(node->type()) {
        case AST_T::CInitDecl_t:
            represent_for_init_decl_instructions(static_cast<CInitDecl*>(node));
            break;
        case AST_T::CInitExp_t:
            represent_for_init_exp_instructions(static_cast<CInitExp*>(node));
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

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

static void represent_statement_break_instructions(CBreak* node) {
    TIdentifier target_break = "break_" + node->target;
    push_instruction(std::make_unique<TacJump>(std::move(target_break)));
}

static void represent_statement_continue_instructions(CContinue* node) {
    TIdentifier target_continue = "continue_" + node->target;
    push_instruction(std::make_unique<TacJump>(std::move(target_continue)));
}

static void represent_statement_goto_instructions(CGoto* node) {
    TIdentifier target_label = node->target;
    push_instruction(std::make_unique<TacJump>(std::move(target_label)));
}

static void represent_statement_label_instructions(CLabel* node) {
    TIdentifier target_label = node->target;
    push_instruction(std::make_unique<TacLabel>(std::move(target_label)));
    represent_statement_instructions(node->jump_to.get());
}

static void represent_statement_instructions(CStatement* node) {
    switch(node->type()) {
        case AST_T::CNull_t:
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
            RAISE_INTERNAL_ERROR;
    }
}

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

static void represent_declaration_var_decl_instructions(CVarDecl* node) {
    if(symbol_table[node->variable_decl->name]->attrs->type() == AST_T::StaticAttr_t) {
        return;
    }
    if(node->variable_decl->init) {
        represent_variable_declaration_instructions(node->variable_decl.get());
    }
}

static void represent_declaration_instructions(CDeclaration* node) {
    switch(node->type()) {
        case AST_T::CFunDecl_t:
            break;
        case AST_T::CVarDecl_t:
            represent_declaration_var_decl_instructions(static_cast<CVarDecl*>(node));
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

// instruction = Return(val) | SignExtend(val, val) | Truncate(val, val) | ZeroExtend(val, val)
//             | TacDoubleToInt(val, val) | TacDoubleToUInt(val, val) | TacIntToDouble(val, val)
//             | TacUIntToDouble(val, val) | FunCall(identifier, val*, val) | Unary(unary_operator, val, val)
//             | Binary(binary_operator, val, val, val) | Copy(val, val) | Jump(identifier)
//             | JumpIfZero(val, identifier) | JumpIfNotZero(val, identifier) | Label(identifier)
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
                RAISE_INTERNAL_ERROR;
        }
    }
}

static void represent_block(CBlock* node) {
    switch(node->type()) {
        case AST_T::CB_t:
            represent_list_instructions(static_cast<CB*>(node)->block_items);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

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

    return std::make_unique<TacFunction>(std::move(name), std::move(is_global), std::move(params), std::move(body));
}

static std::vector<std::unique_ptr<TacTopLevel>>* p_top_levels;

static void push_top_level(std::unique_ptr<TacTopLevel>&& top_level) {
    p_top_levels->push_back(std::move(top_level));
}

static void represent_fun_decl_top_level(CFunDecl* node) {
    if(node->function_decl->body) {
        push_top_level(represent_function_top_level(node->function_decl.get()));
    }
}

// (function) top_level = Function(identifier, bool, identifier*, instruction*)
static void represent_declaration_top_level(CDeclaration* node) {
    switch(node->type()) {
        case AST_T::CFunDecl_t:
            represent_fun_decl_top_level(static_cast<CFunDecl*>(node));
            break;
        case AST_T::CVarDecl_t:
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<StaticInit> represent_tentative_static_init(Type* static_init_type) {
    switch(static_init_type->type()) {
        case AST_T::Int_t:
            return std::make_shared<IntInit>(0);
        case AST_T::Long_t:
            return std::make_shared<LongInit>(0l);
        case AST_T::Double_t:
            return std::make_shared<DoubleInit>(0.0);
        case AST_T::UInt_t:
            return std::make_shared<UIntInit>(0u);
        case AST_T::ULong_t:
        case AST_T::Pointer_t:
            return std::make_shared<ULongInit>(0ul);
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void represent_static_variable_top_level(Symbol* node, const TIdentifier& symbol) {
    StaticAttr* static_attr = static_cast<StaticAttr*>(node->attrs.get());
    if(static_attr->init->type() == AST_T::NoInitializer_t) {
        return;
    }

    TIdentifier name = symbol;
    bool is_global = static_attr->is_global;
    std::shared_ptr<Type> static_init_type = node->type_t;
    std::shared_ptr<StaticInit> initial_value;
    switch(static_attr->init->type()) {
        case AST_T::Initial_t:
            initial_value = static_cast<Initial*>(static_attr->init.get())->static_init;
            break;
        case AST_T::Tentative_t:
            initial_value = represent_tentative_static_init(static_init_type.get());
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }

    push_top_level(std::make_unique<TacStaticVariable>(std::move(name), std::move(is_global),
                                                               std::move(static_init_type), std::move(initial_value)));
}

// (static variable) top_level = StaticVariable(identifier, bool global, int init)
static void represent_symbol_top_level(Symbol* node, const TIdentifier& symbol) {
    if(node->attrs->type() == AST_T::StaticAttr_t) {
        represent_static_variable_top_level(node, symbol);
    }
}

// AST = Program(top_level*, top_level*)
static std::unique_ptr<TacProgram> represent_program(CProgram* node) {
    std::vector<std::unique_ptr<TacTopLevel>> function_top_levels;
    p_top_levels = &function_top_levels;
    for(size_t declaration = 0; declaration < node->declarations.size(); declaration++) {
        represent_declaration_top_level(node->declarations[declaration].get());
    }
    p_top_levels = nullptr;

    std::vector<std::unique_ptr<TacTopLevel>> static_variable_top_levels;
    p_top_levels = &static_variable_top_levels;
    for(const auto& symbol: symbol_table) {
        represent_symbol_top_level(symbol.second.get(), symbol.first);
    }
    p_top_levels = nullptr;

    return std::make_unique<TacProgram>(std::move(static_variable_top_levels), std::move(function_top_levels));
}

std::unique_ptr<TacProgram> three_address_code_representation(std::unique_ptr<CProgram> c_ast) {
    std::unique_ptr<TacProgram> tac_ast = represent_program(c_ast.get());
    c_ast.reset();
    if(!tac_ast) {
        RAISE_INTERNAL_ERROR;
    }
    return tac_ast;
}
