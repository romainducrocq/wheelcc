#include <inttypes.h>
#include <memory>
#include <string>
#include <vector>

#include "util/str2t.hpp"
#include "util/throw.hpp"

#include "ast/ast.hpp"
#include "ast/front_ast.hpp"
#include "ast/front_symt.hpp"
#include "ast/interm_ast.hpp"

#include "frontend/intermediate/names.hpp"
#include "frontend/intermediate/tac_repr.hpp"

static std::unique_ptr<TacReprContext> context;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Three address code representation

// unary_operator = Complement | Negate | Not
static std::unique_ptr<TacUnaryOp> represent_unary_op(CUnaryOp* node) {
    switch (node->type()) {
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

// binary_operator = Add | Subtract | Multiply | Divide | Remainder | BitAnd | BitOr | BitXor | BitShiftLeft
//                 | BitShiftRight | BitShrArithmetic | Equal | NotEqual | LessThan | LessOrEqual | GreaterThan |
//                 GreaterOrEqual
static std::unique_ptr<TacBinaryOp> represent_binary_op(CBinaryOp* node) {
    switch (node->type()) {
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
        case AST_T::CBitShrArithmetic_t:
            return std::make_unique<TacBitShrArithmetic>();
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

static std::shared_ptr<TacConstant> represent_constant_value(CConstant* node) {
    std::shared_ptr<CConst> constant = node->constant;
    return std::make_shared<TacConstant>(std::move(constant));
}

static std::shared_ptr<TacVariable> represent_variable_value(CVar* node) {
    TIdentifier name = node->name;
    return std::make_shared<TacVariable>(std::move(name));
}

static std::shared_ptr<TacVariable> represent_inner_exp_value(CExp* node, std::shared_ptr<Type>&& inner_type) {
    TIdentifier inner_name = represent_variable_identifier(node);
    std::unique_ptr<IdentifierAttr> inner_attrs = std::make_unique<LocalAttr>();
    frontend->symbol_table[inner_name] = std::make_unique<Symbol>(std::move(inner_type), std::move(inner_attrs));
    return std::make_shared<TacVariable>(std::move(inner_name));
}

static std::shared_ptr<TacValue> represent_plain_inner_value(CExp* node) {
    std::shared_ptr<Type> inner_type = node->exp_type;
    return represent_inner_exp_value(node, std::move(inner_type));
}

static std::shared_ptr<TacValue> represent_pointer_inner_value(CExp* node) {
    std::shared_ptr<Type> inner_type = std::make_shared<Long>();
    return represent_inner_exp_value(node, std::move(inner_type));
}

// val = Constant(int) | Var(identifier)
static std::shared_ptr<TacValue> represent_value(CExp* node) {
    switch (node->type()) {
        case AST_T::CConstant_t:
            return represent_constant_value(static_cast<CConstant*>(node));
        case AST_T::CVar_t:
            return represent_variable_value(static_cast<CVar*>(node));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void push_instruction(std::unique_ptr<TacInstruction>&& instruction) {
    context->p_instructions->push_back(std::move(instruction));
}

static std::unique_ptr<TacExpResult> represent_exp_result_instructions(CExp* node);
static std::shared_ptr<TacValue> represent_exp_instructions(CExp* node);

static std::unique_ptr<TacPlainOperand> represent_exp_result_constant_instructions(CConstant* node) {
    std::shared_ptr<TacValue> val = represent_value(node);
    return std::make_unique<TacPlainOperand>(std::move(val));
}

static std::unique_ptr<TacPlainOperand> represent_exp_result_string_instructions(CString* node) {
    TIdentifier string_constant_label;
    {
        TIdentifier string_constant;
        {
            std::string value = string_literal_to_string_constant(node->literal->value);
            string_constant = make_string_identifier(std::move(value));
        }
        if (frontend->string_constant_table.find(string_constant) != frontend->string_constant_table.end()) {
            string_constant_label = frontend->string_constant_table[string_constant];
        }
        else {
            string_constant_label = represent_label_identifier(LABEL_KIND::Lstring);
            frontend->string_constant_table[string_constant] = string_constant_label;
            std::shared_ptr<Type> constant_type;
            {
                TLong size = static_cast<TLong>(node->literal->value.size()) + 1l;
                std::shared_ptr<Type> elem_type = std::make_shared<Char>();
                constant_type = std::make_shared<Array>(std::move(size), std::move(elem_type));
            }
            std::unique_ptr<IdentifierAttr> constant_attrs;
            {
                std::shared_ptr<StaticInit> static_init;
                {
                    std::shared_ptr<CStringLiteral> literal = node->literal;
                    static_init = std::make_shared<StringInit>(true, std::move(string_constant), std::move(literal));
                }
                constant_attrs = std::make_unique<ConstantAttr>(std::move(static_init));
            }
            frontend->symbol_table[string_constant_label] =
                std::make_unique<Symbol>(std::move(constant_type), std::move(constant_attrs));
        }
    }
    std::shared_ptr<TacValue> val = std::make_shared<TacVariable>(std::move(string_constant_label));
    return std::make_unique<TacPlainOperand>(std::move(val));
}

static std::unique_ptr<TacPlainOperand> represent_exp_result_var_instructions(CVar* node) {
    std::shared_ptr<TacValue> val = represent_value(node);
    return std::make_unique<TacPlainOperand>(std::move(val));
}

static bool is_type_signed(Type* type) {
    switch (type->type()) {
        case AST_T::Char_t:
        case AST_T::SChar_t:
        case AST_T::Int_t:
        case AST_T::Long_t:
        case AST_T::Double_t:
            return true;
        default:
            return false;
    }
}

static TInt get_scalar_type_size(Type* type) {
    switch (type->type()) {
        case AST_T::Char_t:
        case AST_T::SChar_t:
        case AST_T::UChar_t:
            return 1;
        case AST_T::Int_t:
        case AST_T::UInt_t:
            return 4;
        case AST_T::Long_t:
        case AST_T::Double_t:
        case AST_T::ULong_t:
        case AST_T::Pointer_t:
            return 8;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static TLong get_type_scale(Type* type);

static TLong get_array_aggregate_type_scale(Array* arr_type) {
    TLong size = arr_type->size;
    while (arr_type->elem_type->type() == AST_T::Array_t) {
        arr_type = static_cast<Array*>(arr_type->elem_type.get());
        size *= arr_type->size;
    }
    return get_type_scale(arr_type->elem_type.get()) * size;
}

static TLong get_structure_aggregate_type_scale(Structure* struct_type) {
    return frontend->struct_typedef_table[struct_type->tag]->size;
}

static TLong get_type_scale(Type* type) {
    switch (type->type()) {
        case AST_T::Array_t:
            return get_array_aggregate_type_scale(static_cast<Array*>(type));
        case AST_T::Structure_t:
            return get_structure_aggregate_type_scale(static_cast<Structure*>(type));
        default:
            return get_scalar_type_size(type);
    }
}

static std::unique_ptr<TacPlainOperand> represent_complete_exp_result_cast_instructions(CCast* node) {
    std::shared_ptr<TacValue> src = represent_exp_instructions(node->exp.get());
    if (node->target_type->type() == node->exp->exp_type->type()) {
        return std::make_unique<TacPlainOperand>(std::move(src));
    }

    std::shared_ptr<TacValue> dst = represent_plain_inner_value(node);
    if (node->exp->exp_type->type() == AST_T::Double_t) {
        if (is_type_signed(node->target_type.get())) {
            push_instruction(std::make_unique<TacDoubleToInt>(std::move(src), dst));
        }
        else {
            push_instruction(std::make_unique<TacDoubleToUInt>(std::move(src), dst));
        }
        return std::make_unique<TacPlainOperand>(std::move(dst));
    }
    else if (node->target_type->type() == AST_T::Double_t) {
        if (is_type_signed(node->exp->exp_type.get())) {
            push_instruction(std::make_unique<TacIntToDouble>(std::move(src), dst));
        }
        else {
            push_instruction(std::make_unique<TacUIntToDouble>(std::move(src), dst));
        }
        return std::make_unique<TacPlainOperand>(std::move(dst));
    }

    TInt target_type_size = get_scalar_type_size(node->target_type.get());
    TInt inner_type_size = get_scalar_type_size(node->exp->exp_type.get());
    if (target_type_size == inner_type_size) {
        push_instruction(std::make_unique<TacCopy>(std::move(src), dst));
    }
    else if (target_type_size < inner_type_size) {
        push_instruction(std::make_unique<TacTruncate>(std::move(src), dst));
    }
    else if (is_type_signed(node->exp->exp_type.get())) {
        push_instruction(std::make_unique<TacSignExtend>(std::move(src), dst));
    }
    else {
        push_instruction(std::make_unique<TacZeroExtend>(std::move(src), dst));
    }
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacPlainOperand> represent_void_exp_result_cast_instructions(CCast* node) {
    represent_exp_instructions(node->exp.get());
    std::shared_ptr<TacValue> dst;
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacPlainOperand> represent_exp_result_cast_instructions(CCast* node) {
    if (node->target_type->type() == AST_T::Void_t) {
        return represent_void_exp_result_cast_instructions(node);
    }
    else {
        return represent_complete_exp_result_cast_instructions(node);
    }
}

static std::unique_ptr<TacPlainOperand> represent_exp_result_unary_instructions(CUnary* node) {
    std::shared_ptr<TacValue> src = represent_exp_instructions(node->exp.get());
    std::shared_ptr<TacValue> dst = represent_plain_inner_value(node);
    std::unique_ptr<TacUnaryOp> unary_op = represent_unary_op(node->unary_op.get());
    push_instruction(std::make_unique<TacUnary>(std::move(unary_op), std::move(src), dst));
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacPlainOperand> represent_exp_result_binary_any_instructions(CBinary* node);

static std::unique_ptr<TacPlainOperand> represent_exp_result_from_to_pointer_binary_add_instructions(CBinary* node) {
    TLong scale;
    std::shared_ptr<TacValue> src_ptr;
    std::shared_ptr<TacValue> index;
    if (node->exp_left->exp_type->type() == AST_T::Pointer_t) {
        scale = get_type_scale(static_cast<Pointer*>(node->exp_left->exp_type.get())->ref_type.get());
        src_ptr = represent_exp_instructions(node->exp_left.get());
        index = represent_exp_instructions(node->exp_right.get());
    }
    else {
        scale = get_type_scale(static_cast<Pointer*>(node->exp_right->exp_type.get())->ref_type.get());
        src_ptr = represent_exp_instructions(node->exp_right.get());
        index = represent_exp_instructions(node->exp_left.get());
    }
    std::shared_ptr<TacValue> dst = represent_pointer_inner_value(node);
    push_instruction(std::make_unique<TacAddPtr>(std::move(scale), std::move(src_ptr), std::move(index), dst));
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacPlainOperand> represent_exp_result_binary_add_instructions(CBinary* node) {
    if (node->exp_left->exp_type->type() == AST_T::Pointer_t || node->exp_right->exp_type->type() == AST_T::Pointer_t) {
        return represent_exp_result_from_to_pointer_binary_add_instructions(node);
    }
    else {
        return represent_exp_result_binary_any_instructions(node);
    }
}

static std::unique_ptr<TacPlainOperand> represent_exp_result_to_pointer_binary_subtract_instructions(CBinary* node) {
    TLong scale = get_type_scale(static_cast<Pointer*>(node->exp_left->exp_type.get())->ref_type.get());
    std::shared_ptr<TacValue> src_ptr = represent_exp_instructions(node->exp_left.get());
    std::shared_ptr<TacValue> index;
    {
        index = represent_exp_instructions(node->exp_right.get());
        std::shared_ptr<TacValue> dst = represent_plain_inner_value(node);
        std::unique_ptr<TacUnaryOp> unary_op = std::make_unique<TacNegate>();
        push_instruction(std::make_unique<TacUnary>(std::move(unary_op), std::move(index), dst));
        index = std::move(dst);
    }
    std::shared_ptr<TacValue> dst = represent_pointer_inner_value(node);
    push_instruction(std::make_unique<TacAddPtr>(std::move(scale), std::move(src_ptr), std::move(index), dst));
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacPlainOperand> represent_exp_result_pointer_binary_subtract_instructions(CBinary* node) {
    std::shared_ptr<TacValue> src_1;
    {
        src_1 = represent_exp_instructions(node->exp_left.get());
        std::shared_ptr<TacValue> src_2 = represent_exp_instructions(node->exp_right.get());
        std::shared_ptr<TacValue> dst = represent_plain_inner_value(node);
        std::unique_ptr<TacBinaryOp> binary_op = std::make_unique<TacSubtract>();
        push_instruction(std::make_unique<TacBinary>(std::move(binary_op), std::move(src_1), std::move(src_2), dst));
        src_1 = std::move(dst);
    }
    std::shared_ptr<TacValue> src_2;
    {
        TLong value = get_type_scale(static_cast<Pointer*>(node->exp_left->exp_type.get())->ref_type.get());
        std::shared_ptr<CConst> constant = std::make_shared<CConstLong>(std::move(value));
        src_2 = std::make_shared<TacConstant>(std::move(constant));
    }
    std::shared_ptr<TacValue> dst = represent_plain_inner_value(node);
    std::unique_ptr<TacBinaryOp> binary_op = std::make_unique<TacDivide>();
    push_instruction(std::make_unique<TacBinary>(std::move(binary_op), std::move(src_1), std::move(src_2), dst));
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacPlainOperand> represent_exp_result_binary_subtract_instructions(CBinary* node) {
    if (node->exp_left->exp_type->type() == AST_T::Pointer_t) {
        if (node->exp_right->exp_type->type() == AST_T::Pointer_t) {
            return represent_exp_result_pointer_binary_subtract_instructions(node);
        }
        else {
            return represent_exp_result_to_pointer_binary_subtract_instructions(node);
        }
    }
    else {
        return represent_exp_result_binary_any_instructions(node);
    }
}

static std::unique_ptr<TacPlainOperand> represent_exp_result_binary_and_instructions(CBinary* node) {
    TIdentifier target_false = represent_label_identifier(LABEL_KIND::Land_false);
    TIdentifier target_true = represent_label_identifier(LABEL_KIND::Land_true);
    std::shared_ptr<TacValue> dst = represent_plain_inner_value(node);
    {
        std::shared_ptr<TacValue> condition_left = represent_exp_instructions(node->exp_left.get());
        push_instruction(std::make_unique<TacJumpIfZero>(target_false, std::move(condition_left)));
    }
    {
        std::shared_ptr<TacValue> condition_right = represent_exp_instructions(node->exp_right.get());
        push_instruction(std::make_unique<TacJumpIfZero>(target_false, std::move(condition_right)));
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

static std::unique_ptr<TacPlainOperand> represent_exp_result_binary_or_instructions(CBinary* node) {
    TIdentifier target_true = represent_label_identifier(LABEL_KIND::Lor_true);
    TIdentifier target_false = represent_label_identifier(LABEL_KIND::Lor_false);
    std::shared_ptr<TacValue> dst = represent_plain_inner_value(node);
    {
        std::shared_ptr<TacValue> condition_left = represent_exp_instructions(node->exp_left.get());
        push_instruction(std::make_unique<TacJumpIfNotZero>(target_true, std::move(condition_left)));
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

static std::unique_ptr<TacPlainOperand> represent_exp_result_binary_any_instructions(CBinary* node) {
    std::shared_ptr<TacValue> src1 = represent_exp_instructions(node->exp_left.get());
    std::shared_ptr<TacValue> src2 = represent_exp_instructions(node->exp_right.get());
    std::shared_ptr<TacValue> dst = represent_plain_inner_value(node);
    std::unique_ptr<TacBinaryOp> binary_op = represent_binary_op(node->binary_op.get());
    push_instruction(std::make_unique<TacBinary>(std::move(binary_op), std::move(src1), std::move(src2), dst));
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacPlainOperand> represent_exp_result_binary_instructions(CBinary* node) {
    switch (node->binary_op->type()) {
        case AST_T::CAdd_t:
            return represent_exp_result_binary_add_instructions(node);
        case AST_T::CSubtract_t:
            return represent_exp_result_binary_subtract_instructions(node);
        case AST_T::CAnd_t:
            return represent_exp_result_binary_and_instructions(node);
        case AST_T::COr_t:
            return represent_exp_result_binary_or_instructions(node);
        default:
            return represent_exp_result_binary_any_instructions(node);
    }
}

static void represent_exp_plain_operand_postfix_assignment_instructions(
    TacPlainOperand* res, std::shared_ptr<TacValue>& dst) {
    std::shared_ptr<TacValue> src = res->val;
    push_instruction(std::make_unique<TacCopy>(std::move(src), dst));
}

static void represent_exp_dereference_pointer_postfix_assignment_instructions(
    TacDereferencedPointer* res, std::shared_ptr<TacValue>& dst) {
    std::shared_ptr<TacValue> src = res->val;
    push_instruction(std::make_unique<TacLoad>(std::move(src), dst));
}

static void represent_exp_sub_object_postfix_assignment_instructions(
    TacSubObject* res, std::shared_ptr<TacValue>& dst) {
    TIdentifier src_name = res->base_name;
    TLong offset = res->offset;
    push_instruction(std::make_unique<TacCopyFromOffset>(std::move(src_name), std::move(offset), dst));
}

static void represent_exp_result_plain_operand_assignment_instructions(
    TacPlainOperand* res, std::shared_ptr<TacValue> src) {
    std::shared_ptr<TacValue> dst = res->val;
    push_instruction(std::make_unique<TacCopy>(std::move(src), dst));
}

static std::unique_ptr<TacPlainOperand> represent_exp_result_dereference_pointer_assignment_instructions(
    TacDereferencedPointer* res, std::shared_ptr<TacValue> src) {
    std::shared_ptr<TacValue> dst = std::move(res->val);
    push_instruction(std::make_unique<TacStore>(src, std::move(dst)));
    return std::make_unique<TacPlainOperand>(std::move(src));
}

static std::unique_ptr<TacPlainOperand> represent_exp_result_sub_object_assignment_instructions(
    TacSubObject* res, std::shared_ptr<TacValue> src) {
    TIdentifier dst_name = std::move(res->base_name);
    TLong offset = std::move(res->offset);
    push_instruction(std::make_unique<TacCopyToOffset>(std::move(dst_name), std::move(offset), src));
    return std::make_unique<TacPlainOperand>(std::move(src));
}

static std::unique_ptr<TacExpResult> represent_exp_result_assignment_instructions(CAssignment* node) {
    std::shared_ptr<TacValue> src;
    std::unique_ptr<TacExpResult> res;
    std::unique_ptr<TacExpResult> res_postfix;
    if (node->exp_left) {
        src = represent_exp_instructions(node->exp_right.get());
        res = represent_exp_result_instructions(node->exp_left.get());
    }
    else {
        uint32_t label_counter_1 = frontend->label_counter;
        uint32_t variable_counter_1 = frontend->variable_counter;
        uint32_t structure_counter_1 = frontend->structure_counter;

        src = represent_exp_instructions(node->exp_right.get());

        uint32_t label_counter_2 = frontend->label_counter;
        uint32_t variable_counter_2 = frontend->variable_counter;
        uint32_t structure_counter_2 = frontend->structure_counter;

        frontend->label_counter = label_counter_1;
        frontend->variable_counter = variable_counter_1;
        frontend->structure_counter = structure_counter_1;

        {
            CExp* exp_left = node->exp_right.get();
            if (exp_left->type() == AST_T::CCast_t) {
                exp_left = static_cast<CCast*>(exp_left)->exp.get();
            }
            exp_left = static_cast<CBinary*>(exp_left)->exp_left.get();
            if (exp_left->type() == AST_T::CCast_t) {
                exp_left = static_cast<CCast*>(exp_left)->exp.get();
            }

            std::vector<std::unique_ptr<TacInstruction>> noeval_instructions;
            std::vector<std::unique_ptr<TacInstruction>>* p_instructions = context->p_instructions;
            context->p_instructions = &noeval_instructions;
            res = represent_exp_result_instructions(exp_left);
            context->p_instructions = p_instructions;

            frontend->label_counter = label_counter_2;
            frontend->variable_counter = variable_counter_2;
            frontend->structure_counter = structure_counter_2;

            if (node->unary_op && node->unary_op->type() == AST_T::CPostfix_t) {
                std::shared_ptr<TacValue> dst = represent_plain_inner_value(node);
                switch (res->type()) {
                    case AST_T::TacPlainOperand_t:
                        represent_exp_plain_operand_postfix_assignment_instructions(
                            static_cast<TacPlainOperand*>(res.get()), dst);
                        break;
                    case AST_T::TacDereferencedPointer_t:
                        represent_exp_dereference_pointer_postfix_assignment_instructions(
                            static_cast<TacDereferencedPointer*>(res.get()), dst);
                        break;
                    case AST_T::TacSubObject_t:
                        represent_exp_sub_object_postfix_assignment_instructions(
                            static_cast<TacSubObject*>(res.get()), dst);
                        break;
                    default:
                        RAISE_INTERNAL_ERROR;
                }
                res_postfix = std::make_unique<TacPlainOperand>(std::move(dst));
            }
        }
    }
    switch (res->type()) {
        case AST_T::TacPlainOperand_t:
            represent_exp_result_plain_operand_assignment_instructions(
                static_cast<TacPlainOperand*>(res.get()), std::move(src));
            break;
        case AST_T::TacDereferencedPointer_t: {
            res = represent_exp_result_dereference_pointer_assignment_instructions(
                static_cast<TacDereferencedPointer*>(res.get()), std::move(src));
            break;
        }
        case AST_T::TacSubObject_t: {
            res = represent_exp_result_sub_object_assignment_instructions(
                static_cast<TacSubObject*>(res.get()), std::move(src));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    if (node->unary_op && node->unary_op->type() == AST_T::CPostfix_t) {
        return res_postfix;
    }
    else {
        return res;
    }
}

static std::unique_ptr<TacPlainOperand> represent_complete_exp_result_conditional_instructions(CConditional* node) {
    TIdentifier target_else = represent_label_identifier(LABEL_KIND::Lternary_else);
    TIdentifier target_false = represent_label_identifier(LABEL_KIND::Lternary_false);
    std::shared_ptr<TacValue> dst = represent_plain_inner_value(node);
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

static std::unique_ptr<TacPlainOperand> represent_void_exp_result_conditional_instructions(CConditional* node) {
    TIdentifier target_else = represent_label_identifier(LABEL_KIND::Lternary_else);
    TIdentifier target_false = represent_label_identifier(LABEL_KIND::Lternary_false);
    std::shared_ptr<TacValue> dst;
    {
        std::shared_ptr<TacValue> condition = represent_exp_instructions(node->condition.get());
        push_instruction(std::make_unique<TacJumpIfZero>(target_else, std::move(condition)));
    }
    represent_exp_instructions(node->exp_middle.get());
    push_instruction(std::make_unique<TacJump>(target_false));
    push_instruction(std::make_unique<TacLabel>(std::move(target_else)));
    represent_exp_instructions(node->exp_right.get());
    push_instruction(std::make_unique<TacLabel>(std::move(target_false)));
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacPlainOperand> represent_exp_result_conditional_instructions(CConditional* node) {
    if (node->exp_middle->exp_type->type() == AST_T::Void_t) {
        return represent_void_exp_result_conditional_instructions(node);
    }
    else {
        return represent_complete_exp_result_conditional_instructions(node);
    }
}

static std::unique_ptr<TacPlainOperand> represent_exp_result_fun_call_instructions(CFunctionCall* node) {
    TIdentifier name = node->name;
    std::vector<std::shared_ptr<TacValue>> args;
    args.reserve(node->args.size());
    for (const auto& arg : node->args) {
        std::shared_ptr<TacValue> arg_1 = represent_exp_instructions(arg.get());
        args.push_back(std::move(arg_1));
    }
    std::shared_ptr<TacValue> dst;
    if (node->exp_type->type() != AST_T::Void_t) {
        dst = represent_plain_inner_value(node);
    }
    push_instruction(std::make_unique<TacFunCall>(std::move(name), std::move(args), dst));
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacDereferencedPointer> represent_exp_result_dereference_instructions(CDereference* node) {
    std::shared_ptr<TacValue> val = represent_exp_instructions(node->exp.get());
    return std::make_unique<TacDereferencedPointer>(std::move(val));
}

static void represent_exp_result_plain_operand_addrof_instructions(TacPlainOperand* res, CAddrOf* node) {
    std::shared_ptr<TacValue> src = std::move(res->val);
    std::shared_ptr<TacValue> dst = represent_pointer_inner_value(node);
    push_instruction(std::make_unique<TacGetAddress>(std::move(src), dst));
    res->val = std::move(dst);
}

static std::unique_ptr<TacPlainOperand> represent_exp_result_dereference_pointer_addrof_instructions(
    TacDereferencedPointer* res) {
    std::shared_ptr<TacValue> val = std::move(res->val);
    return std::make_unique<TacPlainOperand>(std::move(val));
}

static std::unique_ptr<TacPlainOperand> represent_exp_result_sub_object_addrof_instructions(
    TacSubObject* res, CAddrOf* node) {
    std::shared_ptr<TacValue> dst = represent_pointer_inner_value(node);
    {
        TIdentifier name = std::move(res->base_name);
        std::shared_ptr<TacValue> src = std::make_shared<TacVariable>(std::move(name));
        push_instruction(std::make_unique<TacGetAddress>(std::move(src), dst));
    }
    if (res->offset > 0l) {
        std::shared_ptr<TacValue> index;
        {
            TLong offset = std::move(res->offset);
            std::shared_ptr<CConst> constant = std::make_shared<CConstLong>(std::move(offset));
            index = std::make_shared<TacConstant>(std::move(constant));
        }
        push_instruction(std::make_unique<TacAddPtr>(1l, dst, std::move(index), dst));
    }
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacExpResult> represent_exp_result_addrof_instructions(CAddrOf* node) {
    std::unique_ptr<TacExpResult> res = represent_exp_result_instructions(node->exp.get());
    switch (res->type()) {
        case AST_T::TacPlainOperand_t:
            represent_exp_result_plain_operand_addrof_instructions(static_cast<TacPlainOperand*>(res.get()), node);
            break;
        case AST_T::TacDereferencedPointer_t: {
            res = represent_exp_result_dereference_pointer_addrof_instructions(
                static_cast<TacDereferencedPointer*>(res.get()));
            break;
        }
        case AST_T::TacSubObject_t: {
            res = represent_exp_result_sub_object_addrof_instructions(static_cast<TacSubObject*>(res.get()), node);
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return res;
}

static std::unique_ptr<TacDereferencedPointer> represent_exp_result_subscript_instructions(CSubscript* node) {
    TLong scale;
    std::shared_ptr<TacValue> src_ptr;
    std::shared_ptr<TacValue> index;
    if (node->primary_exp->exp_type->type() == AST_T::Pointer_t) {
        scale = get_type_scale(static_cast<Pointer*>(node->primary_exp->exp_type.get())->ref_type.get());
        src_ptr = represent_exp_instructions(node->primary_exp.get());
        index = represent_exp_instructions(node->subscript_exp.get());
    }
    else {
        scale = get_type_scale(static_cast<Pointer*>(node->subscript_exp->exp_type.get())->ref_type.get());
        src_ptr = represent_exp_instructions(node->subscript_exp.get());
        index = represent_exp_instructions(node->primary_exp.get());
    }
    std::shared_ptr<TacValue> dst = represent_pointer_inner_value(node);
    push_instruction(std::make_unique<TacAddPtr>(std::move(scale), std::move(src_ptr), std::move(index), dst));
    return std::make_unique<TacDereferencedPointer>(std::move(dst));
}

static std::unique_ptr<TacPlainOperand> represent_exp_result_sizeof_instructions(CSizeOf* node) {
    std::shared_ptr<CConst> constant;
    {
        TULong value = static_cast<TULong>(get_type_scale(node->exp->exp_type.get()));
        constant = std::make_shared<CConstULong>(std::move(value));
    }
    std::shared_ptr<TacValue> val = std::make_shared<TacConstant>(std::move(constant));
    return std::make_unique<TacPlainOperand>(std::move(val));
}

static std::unique_ptr<TacPlainOperand> represent_exp_result_sizeoft_instructions(CSizeOfT* node) {
    std::shared_ptr<CConst> constant;
    {
        TULong value = static_cast<TULong>(get_type_scale(node->target_type.get()));
        constant = std::make_shared<CConstULong>(std::move(value));
    }
    std::shared_ptr<TacValue> val = std::make_shared<TacConstant>(std::move(constant));
    return std::make_unique<TacPlainOperand>(std::move(val));
}

static std::unique_ptr<TacSubObject> represent_exp_result_plain_operand_dot_instructions(
    TacPlainOperand* res, TLong member_offset) {
    if (res->val->type() != AST_T::TacVariable_t) {
        RAISE_INTERNAL_ERROR;
    }
    TIdentifier base_name = static_cast<TacVariable*>(res->val.get())->name;
    TLong offset = member_offset;
    return std::make_unique<TacSubObject>(std::move(base_name), std::move(offset));
}

static void represent_exp_result_dereference_pointer_dot_instructions(
    TacDereferencedPointer* res, CDot* node, TLong member_offset) {
    if (member_offset > 0l) {
        std::shared_ptr<TacValue> src_ptr = std::move(res->val);
        std::shared_ptr<TacValue> index;
        {
            TLong offset = member_offset;
            std::shared_ptr<CConst> constant = std::make_shared<CConstLong>(std::move(offset));
            index = std::make_shared<TacConstant>(std::move(constant));
        }
        std::shared_ptr<TacValue> dst = represent_pointer_inner_value(node);
        push_instruction(std::make_unique<TacAddPtr>(1l, std::move(src_ptr), std::move(index), dst));
        res->val = std::move(dst);
    }
}

static void represent_exp_result_sub_object_dot_instructions(TacSubObject* res, TLong member_offset) {
    TLong offset = res->offset + member_offset;
    res->offset = std::move(offset);
}

static std::unique_ptr<TacExpResult> represent_exp_result_dot_instructions(CDot* node) {
    if (node->structure->exp_type->type() != AST_T::Structure_t) {
        RAISE_INTERNAL_ERROR;
    }
    Structure* struct_type = static_cast<Structure*>(node->structure->exp_type.get());
    TLong member_offset = frontend->struct_typedef_table[struct_type->tag]->members[node->member]->offset;
    std::unique_ptr<TacExpResult> res = represent_exp_result_instructions(node->structure.get());
    switch (res->type()) {
        case AST_T::TacPlainOperand_t: {
            res = represent_exp_result_plain_operand_dot_instructions(
                static_cast<TacPlainOperand*>(res.get()), member_offset);
            break;
        }
        case AST_T::TacDereferencedPointer_t:
            represent_exp_result_dereference_pointer_dot_instructions(
                static_cast<TacDereferencedPointer*>(res.get()), node, member_offset);
            break;
        case AST_T::TacSubObject_t:
            represent_exp_result_sub_object_dot_instructions(static_cast<TacSubObject*>(res.get()), member_offset);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
    return res;
}

static std::unique_ptr<TacDereferencedPointer> represent_exp_result_arrow_instructions(CArrow* node) {
    if (node->pointer->exp_type->type() != AST_T::Pointer_t) {
        RAISE_INTERNAL_ERROR;
    }
    Pointer* ptr_type = static_cast<Pointer*>(node->pointer->exp_type.get());
    if (ptr_type->ref_type->type() != AST_T::Structure_t) {
        RAISE_INTERNAL_ERROR;
    }
    Structure* struct_type = static_cast<Structure*>(ptr_type->ref_type.get());
    TLong member_offset = frontend->struct_typedef_table[struct_type->tag]->members[node->member]->offset;
    std::shared_ptr<TacValue> val = represent_exp_instructions(node->pointer.get());
    if (member_offset > 0l) {
        std::shared_ptr<TacValue> index;
        {
            TLong offset = member_offset;
            std::shared_ptr<CConst> constant = std::make_shared<CConstLong>(std::move(offset));
            index = std::make_shared<TacConstant>(std::move(constant));
        }
        std::shared_ptr<TacValue> dst = represent_pointer_inner_value(node);
        push_instruction(std::make_unique<TacAddPtr>(1l, std::move(val), std::move(index), dst));
        val = std::move(dst);
    }
    return std::make_unique<TacDereferencedPointer>(std::move(val));
}

static std::unique_ptr<TacExpResult> represent_exp_result_instructions(CExp* node) {
    switch (node->type()) {
        case AST_T::CConstant_t:
            return represent_exp_result_constant_instructions(static_cast<CConstant*>(node));
        case AST_T::CString_t:
            return represent_exp_result_string_instructions(static_cast<CString*>(node));
        case AST_T::CVar_t:
            return represent_exp_result_var_instructions(static_cast<CVar*>(node));
        case AST_T::CCast_t:
            return represent_exp_result_cast_instructions(static_cast<CCast*>(node));
        case AST_T::CUnary_t:
            return represent_exp_result_unary_instructions(static_cast<CUnary*>(node));
        case AST_T::CBinary_t:
            return represent_exp_result_binary_instructions(static_cast<CBinary*>(node));
        case AST_T::CAssignment_t:
            return represent_exp_result_assignment_instructions(static_cast<CAssignment*>(node));
        case AST_T::CConditional_t:
            return represent_exp_result_conditional_instructions(static_cast<CConditional*>(node));
        case AST_T::CFunctionCall_t:
            return represent_exp_result_fun_call_instructions(static_cast<CFunctionCall*>(node));
        case AST_T::CDereference_t:
            return represent_exp_result_dereference_instructions(static_cast<CDereference*>(node));
        case AST_T::CAddrOf_t:
            return represent_exp_result_addrof_instructions(static_cast<CAddrOf*>(node));
        case AST_T::CSubscript_t:
            return represent_exp_result_subscript_instructions(static_cast<CSubscript*>(node));
        case AST_T::CSizeOf_t:
            return represent_exp_result_sizeof_instructions(static_cast<CSizeOf*>(node));
        case AST_T::CSizeOfT_t:
            return represent_exp_result_sizeoft_instructions(static_cast<CSizeOfT*>(node));
        case AST_T::CDot_t:
            return represent_exp_result_dot_instructions(static_cast<CDot*>(node));
        case AST_T::CArrow_t:
            return represent_exp_result_arrow_instructions(static_cast<CArrow*>(node));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<TacValue> represent_exp_plain_operand_instructions(TacPlainOperand* res) {
    std::shared_ptr<TacValue> dst = std::move(res->val);
    return dst;
}

static std::shared_ptr<TacValue> represent_exp_dereferenced_pointer_instructions(
    TacDereferencedPointer* res, CExp* node) {
    std::shared_ptr<TacValue> src = std::move(res->val);
    std::shared_ptr<TacValue> dst = represent_plain_inner_value(node);
    push_instruction(std::make_unique<TacLoad>(std::move(src), dst));
    return dst;
}

static std::shared_ptr<TacValue> represent_exp_sub_object_instructions(TacSubObject* res, CExp* node) {
    TIdentifier src_name = std::move(res->base_name);
    TLong offset = std::move(res->offset);
    std::shared_ptr<TacValue> dst = represent_plain_inner_value(node);
    push_instruction(std::make_unique<TacCopyFromOffset>(std::move(src_name), std::move(offset), dst));
    return dst;
}

// exp_result = PlainOperand(val) | DereferencedPointer(val) | SubObject(val)
static std::shared_ptr<TacValue> represent_exp_instructions(CExp* node) {
    std::unique_ptr<TacExpResult> res = represent_exp_result_instructions(node);
    switch (res->type()) {
        case AST_T::TacPlainOperand_t:
            return represent_exp_plain_operand_instructions(static_cast<TacPlainOperand*>(res.get()));
        case AST_T::TacDereferencedPointer_t:
            return represent_exp_dereferenced_pointer_instructions(
                static_cast<TacDereferencedPointer*>(res.get()), node);
        case AST_T::TacSubObject_t:
            return represent_exp_sub_object_instructions(static_cast<TacSubObject*>(res.get()), node);
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void represent_block(CBlock* node);

static void represent_statement_instructions(CStatement* node);
static void represent_variable_declaration_instructions(CVariableDeclaration* node);

static void represent_statement_return_instructions(CReturn* node) {
    std::shared_ptr<TacValue> val;
    if (node->exp) {
        val = represent_exp_instructions(node->exp.get());
    }
    push_instruction(std::make_unique<TacReturn>(std::move(val)));
}

static void represent_statement_expression_instructions(CExpression* node) {
    represent_exp_result_instructions(node->exp.get());
}

static void represent_statement_if_only_instructions(CIf* node) {
    TIdentifier target_false = represent_label_identifier(LABEL_KIND::Lif_false);
    {
        std::shared_ptr<TacValue> condition = represent_exp_instructions(node->condition.get());
        push_instruction(std::make_unique<TacJumpIfZero>(target_false, std::move(condition)));
    }
    represent_statement_instructions(node->then.get());
    push_instruction(std::make_unique<TacLabel>(std::move(target_false)));
}

static void represent_statement_if_else_instructions(CIf* node) {
    TIdentifier target_else = represent_label_identifier(LABEL_KIND::Lif_else);
    TIdentifier target_false = represent_label_identifier(LABEL_KIND::Lif_false);
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

static void represent_statement_if_instructions(CIf* node) {
    if (node->else_fi) {
        represent_statement_if_else_instructions(node);
    }
    else {
        represent_statement_if_only_instructions(node);
    }
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

static void represent_statement_compound_instructions(CCompound* node) { represent_block(node->block.get()); }

static void represent_statement_while_instructions(CWhile* node) {
    TIdentifier target_break = represent_loop_identifier(LABEL_KIND::Lbreak, node->target);
    TIdentifier target_continue = represent_loop_identifier(LABEL_KIND::Lcontinue, node->target);
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
    TIdentifier target_do_while_start = represent_label_identifier(LABEL_KIND::Ldo_while_start);
    TIdentifier target_break = represent_loop_identifier(LABEL_KIND::Lbreak, node->target);
    TIdentifier target_continue = represent_loop_identifier(LABEL_KIND::Lcontinue, node->target);
    push_instruction(std::make_unique<TacLabel>(target_do_while_start));
    represent_statement_instructions(node->body.get());
    push_instruction(std::make_unique<TacLabel>(std::move(target_continue)));
    {
        std::shared_ptr<TacValue> condition = represent_exp_instructions(node->condition.get());
        push_instruction(std::make_unique<TacJumpIfNotZero>(std::move(target_do_while_start), std::move(condition)));
    }
    push_instruction(std::make_unique<TacLabel>(std::move(target_break)));
}

static void represent_for_init_decl_instructions(CInitDecl* node) {
    represent_variable_declaration_instructions(node->init.get());
}

static void represent_for_init_exp_instructions(CInitExp* node) {
    if (node->init) {
        represent_exp_result_instructions(node->init.get());
    }
}

static void represent_statement_for_init_instructions(CForInit* node) {
    switch (node->type()) {
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
    TIdentifier target_for_start = represent_label_identifier(LABEL_KIND::Lfor_start);
    TIdentifier target_break = represent_loop_identifier(LABEL_KIND::Lbreak, node->target);
    TIdentifier target_continue = represent_loop_identifier(LABEL_KIND::Lcontinue, node->target);
    represent_statement_for_init_instructions(node->init.get());
    push_instruction(std::make_unique<TacLabel>(target_for_start));
    if (node->condition) {
        std::shared_ptr<TacValue> condition = represent_exp_instructions(node->condition.get());
        push_instruction(std::make_unique<TacJumpIfZero>(target_break, std::move(condition)));
    }
    represent_statement_instructions(node->body.get());
    push_instruction(std::make_unique<TacLabel>(std::move(target_continue)));
    if (node->post) {
        represent_exp_result_instructions(node->post.get());
    }
    push_instruction(std::make_unique<TacJump>(std::move(target_for_start)));
    push_instruction(std::make_unique<TacLabel>(std::move(target_break)));
}

static void represent_statement_switch_instructions(CSwitch* node) {
    TIdentifier target_break = represent_loop_identifier(LABEL_KIND::Lbreak, node->target);
    {
        std::shared_ptr<TacValue> match = represent_exp_instructions(node->match.get());
        for (size_t i = 0; i < node->cases.size(); ++i) {
            TIdentifier target_case = represent_case_identifier(node->target, true, i);
            std::shared_ptr<TacValue> case_match;
            {
                std::shared_ptr<TacValue> esac = represent_exp_instructions(node->cases[i].get());
                case_match = represent_plain_inner_value(node->cases[i].get());
                std::unique_ptr<TacBinaryOp> binary_op = std::make_unique<TacEqual>();
                push_instruction(std::make_unique<TacBinary>(std::move(binary_op), match, std::move(esac), case_match));
            }
            push_instruction(std::make_unique<TacJumpIfNotZero>(std::move(target_case), std::move(case_match)));
        }
    }
    if (node->is_default) {
        TIdentifier target_default = represent_loop_identifier(LABEL_KIND::Ldefault, node->target);
        push_instruction(std::make_unique<TacJump>(std::move(target_default)));
        represent_statement_instructions(node->body.get());
    }
    else if (!node->cases.empty()) {
        push_instruction(std::make_unique<TacJump>(target_break));
        represent_statement_instructions(node->body.get());
    }
    push_instruction(std::make_unique<TacLabel>(std::move(target_break)));
}

static void represent_statement_case_instructions(CCase* node) {
    TIdentifier target_case = represent_loop_identifier(LABEL_KIND::Lcase, node->target);
    push_instruction(std::make_unique<TacLabel>(std::move(target_case)));
    represent_statement_instructions(node->jump_to.get());
}

static void represent_statement_default_instructions(CDefault* node) {
    TIdentifier target_default = represent_loop_identifier(LABEL_KIND::Ldefault, node->target);
    push_instruction(std::make_unique<TacLabel>(std::move(target_default)));
    represent_statement_instructions(node->jump_to.get());
}

static void represent_statement_break_instructions(CBreak* node) {
    TIdentifier target_break = represent_loop_identifier(LABEL_KIND::Lbreak, node->target);
    push_instruction(std::make_unique<TacJump>(std::move(target_break)));
}

static void represent_statement_continue_instructions(CContinue* node) {
    TIdentifier target_continue = represent_loop_identifier(LABEL_KIND::Lcontinue, node->target);
    push_instruction(std::make_unique<TacJump>(std::move(target_continue)));
}

static void represent_statement_instructions(CStatement* node) {
    switch (node->type()) {
        case AST_T::CReturn_t:
            represent_statement_return_instructions(static_cast<CReturn*>(node));
            break;
        case AST_T::CExpression_t:
            represent_statement_expression_instructions(static_cast<CExpression*>(node));
            break;
        case AST_T::CIf_t:
            represent_statement_if_instructions(static_cast<CIf*>(node));
            break;
        case AST_T::CGoto_t:
            represent_statement_goto_instructions(static_cast<CGoto*>(node));
            break;
        case AST_T::CLabel_t:
            represent_statement_label_instructions(static_cast<CLabel*>(node));
            break;
        case AST_T::CCompound_t:
            represent_statement_compound_instructions(static_cast<CCompound*>(node));
            break;
        case AST_T::CWhile_t:
            represent_statement_while_instructions(static_cast<CWhile*>(node));
            break;
        case AST_T::CDoWhile_t:
            represent_statement_do_while_instructions(static_cast<CDoWhile*>(node));
            break;
        case AST_T::CFor_t:
            represent_statement_for_instructions(static_cast<CFor*>(node));
            break;
        case AST_T::CSwitch_t:
            represent_statement_switch_instructions(static_cast<CSwitch*>(node));
            break;
        case AST_T::CCase_t:
            represent_statement_case_instructions(static_cast<CCase*>(node));
            break;
        case AST_T::CDefault_t:
            represent_statement_default_instructions(static_cast<CDefault*>(node));
            break;
        case AST_T::CBreak_t:
            represent_statement_break_instructions(static_cast<CBreak*>(node));
            break;
        case AST_T::CContinue_t:
            represent_statement_continue_instructions(static_cast<CContinue*>(node));
            break;
        case AST_T::CNull_t:
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void represent_compound_init_instructions(CInitializer* node, Type* init_type, TIdentifier symbol, TLong& size);

static void represent_array_single_init_string_instructions(
    CString* node, Array* arr_type, TIdentifier symbol, TLong size) {
    size_t byte_at = 0;

    size_t bytes_size = static_cast<size_t>(arr_type->size);
    size_t bytes_copy =
        arr_type->size > static_cast<TLong>(node->literal->value.size()) ? node->literal->value.size() : bytes_size;

    while (byte_at < bytes_copy) {
        TIdentifier dst_name = symbol;
        TLong offset = size + static_cast<TLong>(byte_at);
        std::shared_ptr<TacValue> src;
        {
            std::shared_ptr<CConst> constant;
            {
                size_t bytes_left = bytes_size - byte_at;
                if (bytes_left < 4) {
                    TChar value = string_literal_bytes_to_int8(node->literal->value, byte_at);
                    constant = std::make_shared<CConstChar>(std::move(value));
                    byte_at++;
                }
                else if (bytes_left < 8) {
                    TInt value = string_literal_bytes_to_int32(node->literal->value, byte_at);
                    constant = std::make_shared<CConstInt>(std::move(value));
                    byte_at += 4;
                }
                else {
                    TLong value = string_literal_bytes_to_int64(node->literal->value, byte_at);
                    constant = std::make_shared<CConstLong>(std::move(value));
                    byte_at += 8;
                }
            }
            src = std::make_shared<TacConstant>(std::move(constant));
        }
        push_instruction(std::make_unique<TacCopyToOffset>(std::move(dst_name), std::move(offset), std::move(src)));
    }

    while (byte_at < bytes_size) {
        TIdentifier dst_name = symbol;
        TLong offset = size + static_cast<TLong>(byte_at);
        std::shared_ptr<TacValue> src;
        {
            std::shared_ptr<CConst> constant;
            {
                size_t bytes_left = bytes_size - byte_at;
                if (bytes_left < 4) {
                    constant = std::make_shared<CConstChar>(0);
                    byte_at++;
                }
                else if (bytes_left < 8) {
                    constant = std::make_shared<CConstInt>(0);
                    byte_at += 4;
                }
                else {
                    constant = std::make_shared<CConstLong>(0l);
                    byte_at += 8;
                }
            }
            src = std::make_shared<TacConstant>(std::move(constant));
        }
        push_instruction(std::make_unique<TacCopyToOffset>(std::move(dst_name), std::move(offset), std::move(src)));
    }
}

static void represent_single_init_instructions(CSingleInit* node, Type* init_type, TIdentifier symbol) {
    if (node->exp->type() == AST_T::CString_t && init_type->type() == AST_T::Array_t) {
        represent_array_single_init_string_instructions(
            static_cast<CString*>(node->exp.get()), static_cast<Array*>(init_type), symbol, 0l);
    }
    else {
        std::shared_ptr<TacValue> src = represent_exp_instructions(node->exp.get());
        std::shared_ptr<TacValue> dst;
        {
            TIdentifier name = symbol;
            std::unique_ptr<CExp> exp = std::make_unique<CVar>(std::move(name), 0);
            dst = represent_value(exp.get());
        }
        push_instruction(std::make_unique<TacCopy>(std::move(src), std::move(dst)));
    }
}

static void represent_scalar_compound_init_instructions(
    CSingleInit* node, Type* init_type, TIdentifier symbol, TLong& size) {
    if (node->exp->type() == AST_T::CString_t && init_type->type() == AST_T::Array_t) {
        represent_array_single_init_string_instructions(
            static_cast<CString*>(node->exp.get()), static_cast<Array*>(init_type), symbol, size);
    }
    else {
        TIdentifier dst_name = symbol;
        TLong offset = size;
        std::shared_ptr<TacValue> src = represent_exp_instructions(node->exp.get());
        push_instruction(std::make_unique<TacCopyToOffset>(std::move(dst_name), std::move(offset), std::move(src)));
    }
}

static void represent_array_compound_init_instructions(
    CCompoundInit* node, Array* arr_type, TIdentifier symbol, TLong& size) {
    for (const auto& initializer : node->initializers) {
        represent_compound_init_instructions(initializer.get(), arr_type->elem_type.get(), symbol, size);
        if (initializer->type() == AST_T::CSingleInit_t) {
            size += get_type_scale(arr_type->elem_type.get());
        }
    }
}

static void represent_structure_compound_init_instructions(
    CCompoundInit* node, Structure* struct_type, TIdentifier symbol, TLong& size) {
    for (size_t i = node->initializers.size(); i-- > 0;) {
        const auto& member = GET_STRUCT_TYPEDEF_MEMBER(struct_type->tag, i);
        TLong offset = size + member->offset;
        represent_compound_init_instructions(node->initializers[i].get(), member->member_type.get(), symbol, offset);
    }
    size += get_type_scale(struct_type);
}

static void represent_aggregate_compound_init_instructions(
    CCompoundInit* node, Type* init_type, TIdentifier symbol, TLong& size) {
    switch (init_type->type()) {
        case AST_T::Array_t:
            represent_array_compound_init_instructions(node, static_cast<Array*>(init_type), symbol, size);
            break;
        case AST_T::Structure_t:
            represent_structure_compound_init_instructions(node, static_cast<Structure*>(init_type), symbol, size);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void represent_compound_init_instructions(CInitializer* node, Type* init_type, TIdentifier symbol, TLong& size) {
    switch (node->type()) {
        case AST_T::CSingleInit_t:
            represent_scalar_compound_init_instructions(static_cast<CSingleInit*>(node), init_type, symbol, size);
            break;
        case AST_T::CCompoundInit_t:
            represent_aggregate_compound_init_instructions(static_cast<CCompoundInit*>(node), init_type, symbol, size);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void represent_variable_declaration_instructions(CVariableDeclaration* node) {
    switch (node->init->type()) {
        case AST_T::CSingleInit_t:
            represent_single_init_instructions(static_cast<CSingleInit*>(node->init.get()),
                frontend->symbol_table[node->name]->type_t.get(), node->name);
            break;
        case AST_T::CCompoundInit_t: {
            TLong size = 0l;
            represent_aggregate_compound_init_instructions(static_cast<CCompoundInit*>(node->init.get()),
                frontend->symbol_table[node->name]->type_t.get(), node->name, size);
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void represent_declaration_var_decl_instructions(CVarDecl* node) {
    if (frontend->symbol_table[node->variable_decl->name]->attrs->type() == AST_T::StaticAttr_t) {
        return;
    }
    if (node->variable_decl->init) {
        represent_variable_declaration_instructions(node->variable_decl.get());
    }
}

static void represent_declaration_instructions(CDeclaration* node) {
    switch (node->type()) {
        case AST_T::CFunDecl_t:
        case AST_T::CStructDecl_t:
            break;
        case AST_T::CVarDecl_t:
            represent_declaration_var_decl_instructions(static_cast<CVarDecl*>(node));
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

// instruction = Return(val?) | SignExtend(val, val) | Truncate(val, val) | ZeroExtend(val, val)
//             | TacDoubleToInt(val, val) | TacDoubleToUInt(val, val) | TacIntToDouble(val, val)
//             | TacUIntToDouble(val, val) | FunCall(identifier, val*, val?) | Unary(unary_operator, val, val)
//             | Binary(binary_operator, val, val, val) | Copy(val, val) | GetAddress(val, val) | Load(val, val)
//             | Store(val, val) | AddPtr(int, val, val, val) | CopyToOffset(identifier, int, val)
//             | CopyFromOffset(identifier, int, val) | Jump(identifier) | JumpIfZero(val, identifier)
//             | JumpIfNotZero(val, identifier) | Label(identifier)
static void represent_list_instructions(const std::vector<std::unique_ptr<CBlockItem>>& list_node) {
    for (const auto& block_item : list_node) {
        switch (block_item->type()) {
            case AST_T::CS_t:
                represent_statement_instructions(static_cast<CS*>(block_item.get())->statement.get());
                break;
            case AST_T::CD_t:
                represent_declaration_instructions(static_cast<CD*>(block_item.get())->declaration.get());
                break;
            default:
                RAISE_INTERNAL_ERROR;
        }
    }
}

static void represent_block(CBlock* node) {
    switch (node->type()) {
        case AST_T::CB_t:
            represent_list_instructions(static_cast<CB*>(node)->block_items);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::unique_ptr<TacFunction> represent_function_top_level(CFunctionDeclaration* node) {
    TIdentifier name = node->name;
    bool is_global = static_cast<FunAttr*>(frontend->symbol_table[node->name]->attrs.get())->is_global;

    std::vector<TIdentifier> params(node->params.begin(), node->params.end());

    std::vector<std::unique_ptr<TacInstruction>> body;
    {
        context->p_instructions = &body;
        represent_block(node->body.get());
        {
            std::shared_ptr<CConst> constant = std::make_shared<CConstInt>(0);
            std::shared_ptr<TacValue> val = std::make_shared<TacConstant>(std::move(constant));
            push_instruction(std::make_unique<TacReturn>(std::move(val)));
        }
        context->p_instructions = nullptr;
    }

    return std::make_unique<TacFunction>(std::move(name), std::move(is_global), std::move(params), std::move(body));
}

static void push_top_level(std::unique_ptr<TacTopLevel>&& top_level) {
    context->p_top_levels->push_back(std::move(top_level));
}

static void represent_fun_decl_top_level(CFunDecl* node) {
    if (node->function_decl->body) {
        push_top_level(represent_function_top_level(node->function_decl.get()));
    }
}

// (function) top_level = Function(identifier, bool, identifier*, instruction*)
static void represent_declaration_top_level(CDeclaration* node) {
    switch (node->type()) {
        case AST_T::CFunDecl_t:
            represent_fun_decl_top_level(static_cast<CFunDecl*>(node));
            break;
        case AST_T::CVarDecl_t:
        case AST_T::CStructDecl_t:
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::vector<std::shared_ptr<StaticInit>> represent_tentative_static_variable_top_level(Type* static_init_type) {
    std::vector<std::shared_ptr<StaticInit>> static_inits;
    {
        TLong byte = get_type_scale(static_init_type);
        std::shared_ptr<StaticInit> static_init = std::make_shared<ZeroInit>(std::move(byte));
        static_inits.push_back(std::move(static_init));
    }
    return static_inits;
}

static std::vector<std::shared_ptr<StaticInit>> represent_initial_static_variable_top_level(Initial* node) {
    std::vector<std::shared_ptr<StaticInit>> static_inits(node->static_inits.begin(), node->static_inits.end());
    return static_inits;
}

static void represent_static_variable_top_level(Symbol* node, TIdentifier symbol) {
    StaticAttr* static_attr = static_cast<StaticAttr*>(node->attrs.get());
    if (static_attr->init->type() == AST_T::NoInitializer_t) {
        return;
    }

    TIdentifier name = symbol;
    bool is_global = static_attr->is_global;
    std::shared_ptr<Type> static_init_type = node->type_t;
    std::vector<std::shared_ptr<StaticInit>> static_inits;
    switch (static_attr->init->type()) {
        case AST_T::Tentative_t:
            static_inits = represent_tentative_static_variable_top_level(static_init_type.get());
            break;
        case AST_T::Initial_t:
            static_inits = represent_initial_static_variable_top_level(static_cast<Initial*>(static_attr->init.get()));
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }

    push_top_level(std::make_unique<TacStaticVariable>(
        std::move(name), std::move(is_global), std::move(static_init_type), std::move(static_inits)));
}

static void push_static_constant_top_levels(std::unique_ptr<TacTopLevel>&& static_constant_top_levels) {
    context->p_static_constant_top_levels->push_back(std::move(static_constant_top_levels));
}

static void represent_static_constant_top_level(Symbol* node, TIdentifier symbol) {
    TIdentifier name = symbol;
    std::shared_ptr<Type> static_init_type = node->type_t;
    std::shared_ptr<StaticInit> static_init = static_cast<ConstantAttr*>(node->attrs.get())->static_init;
    push_static_constant_top_levels(
        std::make_unique<TacStaticConstant>(std::move(name), std::move(static_init_type), std::move(static_init)));
}

// (static variable) top_level = StaticVariable(identifier, bool, type, static_init*)
// (static constant) top_level = StaticConstant(identifier, type, static_init)
static void represent_symbol_top_level(Symbol* node, TIdentifier symbol) {
    switch (node->attrs->type()) {
        case AST_T::StaticAttr_t:
            represent_static_variable_top_level(node, symbol);
            break;
        case AST_T::ConstantAttr_t:
            represent_static_constant_top_level(node, symbol);
            break;
        default:
            break;
    }
}

// AST = Program(top_level*, top_level*, top_level*)
static std::unique_ptr<TacProgram> represent_program(CProgram* node) {
    std::vector<std::unique_ptr<TacTopLevel>> function_top_levels;
    {
        context->p_top_levels = &function_top_levels;
        for (const auto& declaration : node->declarations) {
            represent_declaration_top_level(declaration.get());
        }
        context->p_top_levels = nullptr;
    }

    std::vector<std::unique_ptr<TacTopLevel>> static_variable_top_levels;
    std::vector<std::unique_ptr<TacTopLevel>> static_constant_top_levels;
    {
        context->p_top_levels = &static_variable_top_levels;
        context->p_static_constant_top_levels = &static_constant_top_levels;
        for (const auto& symbol : frontend->symbol_table) {
            represent_symbol_top_level(symbol.second.get(), symbol.first);
        }
        context->p_top_levels = nullptr;
        context->p_static_constant_top_levels = nullptr;
    }

    return std::make_unique<TacProgram>(
        std::move(static_constant_top_levels), std::move(static_variable_top_levels), std::move(function_top_levels));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<TacProgram> three_address_code_representation(std::unique_ptr<CProgram> c_ast) {
    context = std::make_unique<TacReprContext>();
    std::unique_ptr<TacProgram> tac_ast = represent_program(c_ast.get());
    context.reset();

    c_ast.reset();
    if (!tac_ast) {
        RAISE_INTERNAL_ERROR;
    }
    return tac_ast;
}
