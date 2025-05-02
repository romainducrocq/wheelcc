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

#include "frontend/intermediate/idents.hpp"
#include "frontend/intermediate/tac_repr.hpp"

struct TacReprContext {
    std::vector<std::unique_ptr<TacInstruction>>* p_instructions;
    std::vector<std::unique_ptr<TacTopLevel>>* p_top_levels;
    std::vector<std::unique_ptr<TacTopLevel>>* p_static_constant_top_levels;
};

static std::unique_ptr<TacReprContext> context;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Three address code representation

// unary_operator = Complement | Negate | Not
static std::unique_ptr<TacUnaryOp> repr_unop(CUnaryOp* node) {
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
static std::unique_ptr<TacBinaryOp> repr_binop(CBinaryOp* node) {
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

static std::shared_ptr<TacConstant> const_value(CConstant* node) {
    std::shared_ptr<CConst> constant = node->constant;
    return std::make_shared<TacConstant>(std::move(constant));
}

static std::shared_ptr<TacVariable> var_value(CVar* node) {
    TIdentifier name = node->name;
    return std::make_shared<TacVariable>(std::move(name));
}

static std::shared_ptr<TacVariable> exp_inner_value(CExp* node, std::shared_ptr<Type>&& inner_type) {
    TIdentifier inner_name = repr_var_identifier(node);
    std::unique_ptr<IdentifierAttr> inner_attrs = std::make_unique<LocalAttr>();
    frontend->symbol_table[inner_name] = std::make_unique<Symbol>(std::move(inner_type), std::move(inner_attrs));
    return std::make_shared<TacVariable>(std::move(inner_name));
}

static std::shared_ptr<TacValue> plain_inner_value(CExp* node) {
    std::shared_ptr<Type> inner_type = node->exp_type;
    return exp_inner_value(node, std::move(inner_type));
}

static std::shared_ptr<TacValue> ptr_inner_value(CExp* node) {
    std::shared_ptr<Type> inner_type = std::make_shared<Long>();
    return exp_inner_value(node, std::move(inner_type));
}

// val = Constant(int) | Var(identifier)
static std::shared_ptr<TacValue> repr_value(CExp* node) {
    switch (node->type()) {
        case AST_T::CConstant_t:
            return const_value(static_cast<CConstant*>(node));
        case AST_T::CVar_t:
            return var_value(static_cast<CVar*>(node));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void push_instr(std::unique_ptr<TacInstruction>&& instruction) {
    context->p_instructions->push_back(std::move(instruction));
}

static std::unique_ptr<TacExpResult> repr_res_instr(CExp* node);
static std::shared_ptr<TacValue> repr_exp_instr(CExp* node);

static std::unique_ptr<TacPlainOperand> const_res_instr(CConstant* node) {
    std::shared_ptr<TacValue> val = repr_value(node);
    return std::make_unique<TacPlainOperand>(std::move(val));
}

static std::unique_ptr<TacPlainOperand> string_res_instr(CString* node) {
    TIdentifier string_constant_label;
    {
        TIdentifier string_constant;
        {
            std::string value = string_literal_to_const(node->literal->value);
            string_constant = make_string_identifier(std::move(value));
        }
        if (frontend->string_constant_table.find(string_constant) != frontend->string_constant_table.end()) {
            string_constant_label = frontend->string_constant_table[string_constant];
        }
        else {
            string_constant_label = repr_label_identifier(LABEL_KIND::Lstring);
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
                    static_init = std::make_shared<StringInit>(std::move(string_constant), true, std::move(literal));
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

static std::unique_ptr<TacPlainOperand> var_res_instr(CVar* node) {
    std::shared_ptr<TacValue> val = repr_value(node);
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

static TInt get_scalar_size(Type* type) {
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

static TLong get_arr_scale(Array* arr_type) {
    TLong size = arr_type->size;
    while (arr_type->elem_type->type() == AST_T::Array_t) {
        arr_type = static_cast<Array*>(arr_type->elem_type.get());
        size *= arr_type->size;
    }
    return get_type_scale(arr_type->elem_type.get()) * size;
}

static TLong get_struct_scale(Structure* struct_type) { return frontend->struct_typedef_table[struct_type->tag]->size; }

static TLong get_type_scale(Type* type) {
    switch (type->type()) {
        case AST_T::Array_t:
            return get_arr_scale(static_cast<Array*>(type));
        case AST_T::Structure_t:
            return get_struct_scale(static_cast<Structure*>(type));
        default:
            return get_scalar_size(type);
    }
}

static std::unique_ptr<TacPlainOperand> cast_complete_res_instr(CCast* node) {
    std::shared_ptr<TacValue> src = repr_exp_instr(node->exp.get());
    if (node->target_type->type() == node->exp->exp_type->type()) {
        return std::make_unique<TacPlainOperand>(std::move(src));
    }

    std::shared_ptr<TacValue> dst = plain_inner_value(node);
    if (node->exp->exp_type->type() == AST_T::Double_t) {
        if (is_type_signed(node->target_type.get())) {
            push_instr(std::make_unique<TacDoubleToInt>(std::move(src), dst));
        }
        else {
            push_instr(std::make_unique<TacDoubleToUInt>(std::move(src), dst));
        }
        return std::make_unique<TacPlainOperand>(std::move(dst));
    }
    else if (node->target_type->type() == AST_T::Double_t) {
        if (is_type_signed(node->exp->exp_type.get())) {
            push_instr(std::make_unique<TacIntToDouble>(std::move(src), dst));
        }
        else {
            push_instr(std::make_unique<TacUIntToDouble>(std::move(src), dst));
        }
        return std::make_unique<TacPlainOperand>(std::move(dst));
    }

    TInt target_type_size = get_scalar_size(node->target_type.get());
    TInt inner_type_size = get_scalar_size(node->exp->exp_type.get());
    if (target_type_size == inner_type_size) {
        push_instr(std::make_unique<TacCopy>(std::move(src), dst));
    }
    else if (target_type_size < inner_type_size) {
        push_instr(std::make_unique<TacTruncate>(std::move(src), dst));
    }
    else if (is_type_signed(node->exp->exp_type.get())) {
        push_instr(std::make_unique<TacSignExtend>(std::move(src), dst));
    }
    else {
        push_instr(std::make_unique<TacZeroExtend>(std::move(src), dst));
    }
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacPlainOperand> cast_void_res_instr(CCast* node) {
    repr_exp_instr(node->exp.get());
    std::shared_ptr<TacValue> dst;
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacPlainOperand> cast_res_instr(CCast* node) {
    if (node->target_type->type() == AST_T::Void_t) {
        return cast_void_res_instr(node);
    }
    else {
        return cast_complete_res_instr(node);
    }
}

static std::unique_ptr<TacPlainOperand> unary_res_instr(CUnary* node) {
    std::shared_ptr<TacValue> src = repr_exp_instr(node->exp.get());
    std::shared_ptr<TacValue> dst = plain_inner_value(node);
    std::unique_ptr<TacUnaryOp> unary_op = repr_unop(node->unary_op.get());
    push_instr(std::make_unique<TacUnary>(std::move(unary_op), std::move(src), dst));
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacPlainOperand> binary_any_res_instr(CBinary* node);

static std::unique_ptr<TacPlainOperand> binary_add_ptr_res_instr(CBinary* node) {
    TLong scale;
    std::shared_ptr<TacValue> src_ptr;
    std::shared_ptr<TacValue> index;
    if (node->exp_left->exp_type->type() == AST_T::Pointer_t) {
        scale = get_type_scale(static_cast<Pointer*>(node->exp_left->exp_type.get())->ref_type.get());
        src_ptr = repr_exp_instr(node->exp_left.get());
        index = repr_exp_instr(node->exp_right.get());
    }
    else {
        scale = get_type_scale(static_cast<Pointer*>(node->exp_right->exp_type.get())->ref_type.get());
        src_ptr = repr_exp_instr(node->exp_right.get());
        index = repr_exp_instr(node->exp_left.get());
    }
    std::shared_ptr<TacValue> dst = ptr_inner_value(node);
    push_instr(std::make_unique<TacAddPtr>(std::move(scale), std::move(src_ptr), std::move(index), dst));
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacPlainOperand> binary_add_res_instr(CBinary* node) {
    if (node->exp_left->exp_type->type() == AST_T::Pointer_t || node->exp_right->exp_type->type() == AST_T::Pointer_t) {
        return binary_add_ptr_res_instr(node);
    }
    else {
        return binary_any_res_instr(node);
    }
}

static std::unique_ptr<TacPlainOperand> binary_sub_to_ptr_res_instr(CBinary* node) {
    TLong scale = get_type_scale(static_cast<Pointer*>(node->exp_left->exp_type.get())->ref_type.get());
    std::shared_ptr<TacValue> src_ptr = repr_exp_instr(node->exp_left.get());
    std::shared_ptr<TacValue> index;
    {
        index = repr_exp_instr(node->exp_right.get());
        std::shared_ptr<TacValue> dst = plain_inner_value(node);
        std::unique_ptr<TacUnaryOp> unary_op = std::make_unique<TacNegate>();
        push_instr(std::make_unique<TacUnary>(std::move(unary_op), std::move(index), dst));
        index = std::move(dst);
    }
    std::shared_ptr<TacValue> dst = ptr_inner_value(node);
    push_instr(std::make_unique<TacAddPtr>(std::move(scale), std::move(src_ptr), std::move(index), dst));
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacPlainOperand> binary_subtract_ptr_res_instr(CBinary* node) {
    std::shared_ptr<TacValue> src_1;
    {
        src_1 = repr_exp_instr(node->exp_left.get());
        std::shared_ptr<TacValue> src_2 = repr_exp_instr(node->exp_right.get());
        std::shared_ptr<TacValue> dst = plain_inner_value(node);
        std::unique_ptr<TacBinaryOp> binary_op = std::make_unique<TacSubtract>();
        push_instr(std::make_unique<TacBinary>(std::move(binary_op), std::move(src_1), std::move(src_2), dst));
        src_1 = std::move(dst);
    }
    std::shared_ptr<TacValue> src_2;
    {
        TLong value = get_type_scale(static_cast<Pointer*>(node->exp_left->exp_type.get())->ref_type.get());
        std::shared_ptr<CConst> constant = std::make_shared<CConstLong>(std::move(value));
        src_2 = std::make_shared<TacConstant>(std::move(constant));
    }
    std::shared_ptr<TacValue> dst = plain_inner_value(node);
    std::unique_ptr<TacBinaryOp> binary_op = std::make_unique<TacDivide>();
    push_instr(std::make_unique<TacBinary>(std::move(binary_op), std::move(src_1), std::move(src_2), dst));
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacPlainOperand> binary_subtract_res_instr(CBinary* node) {
    if (node->exp_left->exp_type->type() == AST_T::Pointer_t) {
        if (node->exp_right->exp_type->type() == AST_T::Pointer_t) {
            return binary_subtract_ptr_res_instr(node);
        }
        else {
            return binary_sub_to_ptr_res_instr(node);
        }
    }
    else {
        return binary_any_res_instr(node);
    }
}

static std::unique_ptr<TacPlainOperand> binary_and_res_instr(CBinary* node) {
    TIdentifier target_false = repr_label_identifier(LABEL_KIND::Land_false);
    TIdentifier target_true = repr_label_identifier(LABEL_KIND::Land_true);
    std::shared_ptr<TacValue> dst = plain_inner_value(node);
    {
        std::shared_ptr<TacValue> condition_left = repr_exp_instr(node->exp_left.get());
        push_instr(std::make_unique<TacJumpIfZero>(target_false, std::move(condition_left)));
    }
    {
        std::shared_ptr<TacValue> condition_right = repr_exp_instr(node->exp_right.get());
        push_instr(std::make_unique<TacJumpIfZero>(target_false, std::move(condition_right)));
    }
    {
        std::shared_ptr<CConst> constant = std::make_shared<CConstInt>(1);
        std::shared_ptr<TacValue> src_true = std::make_shared<TacConstant>(std::move(constant));
        push_instr(std::make_unique<TacCopy>(std::move(src_true), dst));
    }
    push_instr(std::make_unique<TacJump>(target_true));
    push_instr(std::make_unique<TacLabel>(std::move(target_false)));
    {
        std::shared_ptr<CConst> constant = std::make_shared<CConstInt>(0);
        std::shared_ptr<TacValue> src_false = std::make_shared<TacConstant>(std::move(constant));
        push_instr(std::make_unique<TacCopy>(std::move(src_false), dst));
    }
    push_instr(std::make_unique<TacLabel>(std::move(target_true)));
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacPlainOperand> binary_or_res_instr(CBinary* node) {
    TIdentifier target_true = repr_label_identifier(LABEL_KIND::Lor_true);
    TIdentifier target_false = repr_label_identifier(LABEL_KIND::Lor_false);
    std::shared_ptr<TacValue> dst = plain_inner_value(node);
    {
        std::shared_ptr<TacValue> condition_left = repr_exp_instr(node->exp_left.get());
        push_instr(std::make_unique<TacJumpIfNotZero>(target_true, std::move(condition_left)));
    }
    {
        std::shared_ptr<TacValue> condition_right = repr_exp_instr(node->exp_right.get());
        push_instr(std::make_unique<TacJumpIfNotZero>(target_true, std::move(condition_right)));
    }
    {
        std::shared_ptr<CConst> constant = std::make_shared<CConstInt>(0);
        std::shared_ptr<TacValue> src_false = std::make_shared<TacConstant>(std::move(constant));
        push_instr(std::make_unique<TacCopy>(std::move(src_false), dst));
    }
    push_instr(std::make_unique<TacJump>(target_false));
    push_instr(std::make_unique<TacLabel>(std::move(target_true)));
    {
        std::shared_ptr<CConst> constant = std::make_shared<CConstInt>(1);
        std::shared_ptr<TacValue> src_true = std::make_shared<TacConstant>(std::move(constant));
        push_instr(std::make_unique<TacCopy>(std::move(src_true), dst));
    }
    push_instr(std::make_unique<TacLabel>(std::move(target_false)));
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacPlainOperand> binary_any_res_instr(CBinary* node) {
    std::shared_ptr<TacValue> src1 = repr_exp_instr(node->exp_left.get());
    std::shared_ptr<TacValue> src2 = repr_exp_instr(node->exp_right.get());
    std::shared_ptr<TacValue> dst = plain_inner_value(node);
    std::unique_ptr<TacBinaryOp> binary_op = repr_binop(node->binary_op.get());
    push_instr(std::make_unique<TacBinary>(std::move(binary_op), std::move(src1), std::move(src2), dst));
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacPlainOperand> binary_res_instr(CBinary* node) {
    switch (node->binary_op->type()) {
        case AST_T::CAdd_t:
            return binary_add_res_instr(node);
        case AST_T::CSubtract_t:
            return binary_subtract_res_instr(node);
        case AST_T::CAnd_t:
            return binary_and_res_instr(node);
        case AST_T::COr_t:
            return binary_or_res_instr(node);
        default:
            return binary_any_res_instr(node);
    }
}

static void plain_op_postfix_exp_instr(TacPlainOperand* res, std::shared_ptr<TacValue>& dst) {
    std::shared_ptr<TacValue> src = res->val;
    push_instr(std::make_unique<TacCopy>(std::move(src), dst));
}

static void deref_ptr_postfix_exp_instr(TacDereferencedPointer* res, std::shared_ptr<TacValue>& dst) {
    std::shared_ptr<TacValue> src = res->val;
    push_instr(std::make_unique<TacLoad>(std::move(src), dst));
}

static void sub_obj_postfix_exp_instr(TacSubObject* res, std::shared_ptr<TacValue>& dst) {
    TIdentifier src_name = res->base_name;
    TLong offset = res->offset;
    push_instr(std::make_unique<TacCopyFromOffset>(std::move(src_name), std::move(offset), dst));
}

static void plain_op_assign_res_instr(TacPlainOperand* res, std::shared_ptr<TacValue> src) {
    std::shared_ptr<TacValue> dst = res->val;
    push_instr(std::make_unique<TacCopy>(std::move(src), dst));
}

static std::unique_ptr<TacPlainOperand> deref_ptr_assign_res_instr(
    TacDereferencedPointer* res, std::shared_ptr<TacValue> src) {
    std::shared_ptr<TacValue> dst = std::move(res->val);
    push_instr(std::make_unique<TacStore>(src, std::move(dst)));
    return std::make_unique<TacPlainOperand>(std::move(src));
}

static std::unique_ptr<TacPlainOperand> sub_obj_assign_res_instr(TacSubObject* res, std::shared_ptr<TacValue> src) {
    TIdentifier dst_name = std::move(res->base_name);
    TLong offset = std::move(res->offset);
    push_instr(std::make_unique<TacCopyToOffset>(std::move(dst_name), std::move(offset), src));
    return std::make_unique<TacPlainOperand>(std::move(src));
}

static std::unique_ptr<TacExpResult> assign_res_instr(CAssignment* node) {
    std::shared_ptr<TacValue> src;
    std::unique_ptr<TacExpResult> res;
    std::unique_ptr<TacExpResult> res_postfix;
    if (node->exp_left) {
        src = repr_exp_instr(node->exp_right.get());
        res = repr_res_instr(node->exp_left.get());
    }
    else {
        uint32_t label_counter_1 = identifiers->label_counter;
        uint32_t variable_counter_1 = identifiers->variable_counter;
        uint32_t structure_counter_1 = identifiers->structure_counter;

        src = repr_exp_instr(node->exp_right.get());

        uint32_t label_counter_2 = identifiers->label_counter;
        uint32_t variable_counter_2 = identifiers->variable_counter;
        uint32_t structure_counter_2 = identifiers->structure_counter;

        identifiers->label_counter = label_counter_1;
        identifiers->variable_counter = variable_counter_1;
        identifiers->structure_counter = structure_counter_1;

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
            res = repr_res_instr(exp_left);
            context->p_instructions = p_instructions;

            identifiers->label_counter = label_counter_2;
            identifiers->variable_counter = variable_counter_2;
            identifiers->structure_counter = structure_counter_2;

            if (node->unary_op && node->unary_op->type() == AST_T::CPostfix_t) {
                std::shared_ptr<TacValue> dst = plain_inner_value(node);
                switch (res->type()) {
                    case AST_T::TacPlainOperand_t:
                        plain_op_postfix_exp_instr(static_cast<TacPlainOperand*>(res.get()), dst);
                        break;
                    case AST_T::TacDereferencedPointer_t:
                        deref_ptr_postfix_exp_instr(static_cast<TacDereferencedPointer*>(res.get()), dst);
                        break;
                    case AST_T::TacSubObject_t:
                        sub_obj_postfix_exp_instr(static_cast<TacSubObject*>(res.get()), dst);
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
            plain_op_assign_res_instr(static_cast<TacPlainOperand*>(res.get()), std::move(src));
            break;
        case AST_T::TacDereferencedPointer_t: {
            res = deref_ptr_assign_res_instr(static_cast<TacDereferencedPointer*>(res.get()), std::move(src));
            break;
        }
        case AST_T::TacSubObject_t: {
            res = sub_obj_assign_res_instr(static_cast<TacSubObject*>(res.get()), std::move(src));
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

static std::unique_ptr<TacPlainOperand> conditional_complete_res_instr(CConditional* node) {
    TIdentifier target_else = repr_label_identifier(LABEL_KIND::Lternary_else);
    TIdentifier target_false = repr_label_identifier(LABEL_KIND::Lternary_false);
    std::shared_ptr<TacValue> dst = plain_inner_value(node);
    {
        std::shared_ptr<TacValue> condition = repr_exp_instr(node->condition.get());
        push_instr(std::make_unique<TacJumpIfZero>(target_else, std::move(condition)));
    }
    {
        std::shared_ptr<TacValue> src_middle = repr_exp_instr(node->exp_middle.get());
        push_instr(std::make_unique<TacCopy>(std::move(src_middle), dst));
    }
    push_instr(std::make_unique<TacJump>(target_false));
    push_instr(std::make_unique<TacLabel>(std::move(target_else)));
    {
        std::shared_ptr<TacValue> src_right = repr_exp_instr(node->exp_right.get());
        push_instr(std::make_unique<TacCopy>(std::move(src_right), dst));
    }
    push_instr(std::make_unique<TacLabel>(std::move(target_false)));
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacPlainOperand> conditional_void_res_instr(CConditional* node) {
    TIdentifier target_else = repr_label_identifier(LABEL_KIND::Lternary_else);
    TIdentifier target_false = repr_label_identifier(LABEL_KIND::Lternary_false);
    std::shared_ptr<TacValue> dst;
    {
        std::shared_ptr<TacValue> condition = repr_exp_instr(node->condition.get());
        push_instr(std::make_unique<TacJumpIfZero>(target_else, std::move(condition)));
    }
    repr_exp_instr(node->exp_middle.get());
    push_instr(std::make_unique<TacJump>(target_false));
    push_instr(std::make_unique<TacLabel>(std::move(target_else)));
    repr_exp_instr(node->exp_right.get());
    push_instr(std::make_unique<TacLabel>(std::move(target_false)));
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacPlainOperand> conditional_res_instr(CConditional* node) {
    if (node->exp_middle->exp_type->type() == AST_T::Void_t) {
        return conditional_void_res_instr(node);
    }
    else {
        return conditional_complete_res_instr(node);
    }
}

static std::unique_ptr<TacPlainOperand> call_res_instr(CFunctionCall* node) {
    TIdentifier name = node->name;
    std::vector<std::shared_ptr<TacValue>> args;
    args.reserve(node->args.size());
    for (const auto& arg : node->args) {
        std::shared_ptr<TacValue> arg_1 = repr_exp_instr(arg.get());
        args.push_back(std::move(arg_1));
    }
    std::shared_ptr<TacValue> dst;
    if (node->exp_type->type() != AST_T::Void_t) {
        dst = plain_inner_value(node);
    }
    push_instr(std::make_unique<TacFunCall>(std::move(name), std::move(args), dst));
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacDereferencedPointer> deref_res_instr(CDereference* node) {
    std::shared_ptr<TacValue> val = repr_exp_instr(node->exp.get());
    return std::make_unique<TacDereferencedPointer>(std::move(val));
}

static void plain_op_addrof_res_instr(TacPlainOperand* res, CAddrOf* node) {
    std::shared_ptr<TacValue> src = std::move(res->val);
    std::shared_ptr<TacValue> dst = ptr_inner_value(node);
    push_instr(std::make_unique<TacGetAddress>(std::move(src), dst));
    res->val = std::move(dst);
}

static std::unique_ptr<TacPlainOperand> deref_ptr_addrof_res_instr(TacDereferencedPointer* res) {
    std::shared_ptr<TacValue> val = std::move(res->val);
    return std::make_unique<TacPlainOperand>(std::move(val));
}

static std::unique_ptr<TacPlainOperand> sub_obj_addrof_res_instr(TacSubObject* res, CAddrOf* node) {
    std::shared_ptr<TacValue> dst = ptr_inner_value(node);
    {
        TIdentifier name = std::move(res->base_name);
        std::shared_ptr<TacValue> src = std::make_shared<TacVariable>(std::move(name));
        push_instr(std::make_unique<TacGetAddress>(std::move(src), dst));
    }
    if (res->offset > 0l) {
        std::shared_ptr<TacValue> index;
        {
            TLong offset = std::move(res->offset);
            std::shared_ptr<CConst> constant = std::make_shared<CConstLong>(std::move(offset));
            index = std::make_shared<TacConstant>(std::move(constant));
        }
        push_instr(std::make_unique<TacAddPtr>(1l, dst, std::move(index), dst));
    }
    return std::make_unique<TacPlainOperand>(std::move(dst));
}

static std::unique_ptr<TacExpResult> addrof_res_instr(CAddrOf* node) {
    std::unique_ptr<TacExpResult> res = repr_res_instr(node->exp.get());
    switch (res->type()) {
        case AST_T::TacPlainOperand_t:
            plain_op_addrof_res_instr(static_cast<TacPlainOperand*>(res.get()), node);
            break;
        case AST_T::TacDereferencedPointer_t: {
            res = deref_ptr_addrof_res_instr(static_cast<TacDereferencedPointer*>(res.get()));
            break;
        }
        case AST_T::TacSubObject_t: {
            res = sub_obj_addrof_res_instr(static_cast<TacSubObject*>(res.get()), node);
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return res;
}

static std::unique_ptr<TacDereferencedPointer> subscript_res_instr(CSubscript* node) {
    TLong scale;
    std::shared_ptr<TacValue> src_ptr;
    std::shared_ptr<TacValue> index;
    if (node->primary_exp->exp_type->type() == AST_T::Pointer_t) {
        scale = get_type_scale(static_cast<Pointer*>(node->primary_exp->exp_type.get())->ref_type.get());
        src_ptr = repr_exp_instr(node->primary_exp.get());
        index = repr_exp_instr(node->subscript_exp.get());
    }
    else {
        scale = get_type_scale(static_cast<Pointer*>(node->subscript_exp->exp_type.get())->ref_type.get());
        src_ptr = repr_exp_instr(node->subscript_exp.get());
        index = repr_exp_instr(node->primary_exp.get());
    }
    std::shared_ptr<TacValue> dst = ptr_inner_value(node);
    push_instr(std::make_unique<TacAddPtr>(std::move(scale), std::move(src_ptr), std::move(index), dst));
    return std::make_unique<TacDereferencedPointer>(std::move(dst));
}

static std::unique_ptr<TacPlainOperand> sizeof_res_instr(CSizeOf* node) {
    std::shared_ptr<CConst> constant;
    {
        TULong value = static_cast<TULong>(get_type_scale(node->exp->exp_type.get()));
        constant = std::make_shared<CConstULong>(std::move(value));
    }
    std::shared_ptr<TacValue> val = std::make_shared<TacConstant>(std::move(constant));
    return std::make_unique<TacPlainOperand>(std::move(val));
}

static std::unique_ptr<TacPlainOperand> sizeoft_res_instr(CSizeOfT* node) {
    std::shared_ptr<CConst> constant;
    {
        TULong value = static_cast<TULong>(get_type_scale(node->target_type.get()));
        constant = std::make_shared<CConstULong>(std::move(value));
    }
    std::shared_ptr<TacValue> val = std::make_shared<TacConstant>(std::move(constant));
    return std::make_unique<TacPlainOperand>(std::move(val));
}

static std::unique_ptr<TacSubObject> plain_op_dot_res_instr(TacPlainOperand* res, TLong member_offset) {
    if (res->val->type() != AST_T::TacVariable_t) {
        RAISE_INTERNAL_ERROR;
    }
    TIdentifier base_name = static_cast<TacVariable*>(res->val.get())->name;
    TLong offset = member_offset;
    return std::make_unique<TacSubObject>(std::move(base_name), std::move(offset));
}

static void deref_ptr_dot_res_instr(TacDereferencedPointer* res, CDot* node, TLong member_offset) {
    if (member_offset > 0l) {
        std::shared_ptr<TacValue> src_ptr = std::move(res->val);
        std::shared_ptr<TacValue> index;
        {
            TLong offset = member_offset;
            std::shared_ptr<CConst> constant = std::make_shared<CConstLong>(std::move(offset));
            index = std::make_shared<TacConstant>(std::move(constant));
        }
        std::shared_ptr<TacValue> dst = ptr_inner_value(node);
        push_instr(std::make_unique<TacAddPtr>(1l, std::move(src_ptr), std::move(index), dst));
        res->val = std::move(dst);
    }
}

static void sub_obj_dot_res_instr(TacSubObject* res, TLong member_offset) {
    TLong offset = res->offset + member_offset;
    res->offset = std::move(offset);
}

static std::unique_ptr<TacExpResult> dot_res_instr(CDot* node) {
    if (node->structure->exp_type->type() != AST_T::Structure_t) {
        RAISE_INTERNAL_ERROR;
    }
    Structure* struct_type = static_cast<Structure*>(node->structure->exp_type.get());
    TLong member_offset = frontend->struct_typedef_table[struct_type->tag]->members[node->member]->offset;
    std::unique_ptr<TacExpResult> res = repr_res_instr(node->structure.get());
    switch (res->type()) {
        case AST_T::TacPlainOperand_t: {
            res = plain_op_dot_res_instr(static_cast<TacPlainOperand*>(res.get()), member_offset);
            break;
        }
        case AST_T::TacDereferencedPointer_t:
            deref_ptr_dot_res_instr(static_cast<TacDereferencedPointer*>(res.get()), node, member_offset);
            break;
        case AST_T::TacSubObject_t:
            sub_obj_dot_res_instr(static_cast<TacSubObject*>(res.get()), member_offset);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
    return res;
}

static std::unique_ptr<TacDereferencedPointer> arrow_res_instr(CArrow* node) {
    if (node->pointer->exp_type->type() != AST_T::Pointer_t) {
        RAISE_INTERNAL_ERROR;
    }
    Pointer* ptr_type = static_cast<Pointer*>(node->pointer->exp_type.get());
    if (ptr_type->ref_type->type() != AST_T::Structure_t) {
        RAISE_INTERNAL_ERROR;
    }
    Structure* struct_type = static_cast<Structure*>(ptr_type->ref_type.get());
    TLong member_offset = frontend->struct_typedef_table[struct_type->tag]->members[node->member]->offset;
    std::shared_ptr<TacValue> val = repr_exp_instr(node->pointer.get());
    if (member_offset > 0l) {
        std::shared_ptr<TacValue> index;
        {
            TLong offset = member_offset;
            std::shared_ptr<CConst> constant = std::make_shared<CConstLong>(std::move(offset));
            index = std::make_shared<TacConstant>(std::move(constant));
        }
        std::shared_ptr<TacValue> dst = ptr_inner_value(node);
        push_instr(std::make_unique<TacAddPtr>(1l, std::move(val), std::move(index), dst));
        val = std::move(dst);
    }
    return std::make_unique<TacDereferencedPointer>(std::move(val));
}

static std::unique_ptr<TacExpResult> repr_res_instr(CExp* node) {
    switch (node->type()) {
        case AST_T::CConstant_t:
            return const_res_instr(static_cast<CConstant*>(node));
        case AST_T::CString_t:
            return string_res_instr(static_cast<CString*>(node));
        case AST_T::CVar_t:
            return var_res_instr(static_cast<CVar*>(node));
        case AST_T::CCast_t:
            return cast_res_instr(static_cast<CCast*>(node));
        case AST_T::CUnary_t:
            return unary_res_instr(static_cast<CUnary*>(node));
        case AST_T::CBinary_t:
            return binary_res_instr(static_cast<CBinary*>(node));
        case AST_T::CAssignment_t:
            return assign_res_instr(static_cast<CAssignment*>(node));
        case AST_T::CConditional_t:
            return conditional_res_instr(static_cast<CConditional*>(node));
        case AST_T::CFunctionCall_t:
            return call_res_instr(static_cast<CFunctionCall*>(node));
        case AST_T::CDereference_t:
            return deref_res_instr(static_cast<CDereference*>(node));
        case AST_T::CAddrOf_t:
            return addrof_res_instr(static_cast<CAddrOf*>(node));
        case AST_T::CSubscript_t:
            return subscript_res_instr(static_cast<CSubscript*>(node));
        case AST_T::CSizeOf_t:
            return sizeof_res_instr(static_cast<CSizeOf*>(node));
        case AST_T::CSizeOfT_t:
            return sizeoft_res_instr(static_cast<CSizeOfT*>(node));
        case AST_T::CDot_t:
            return dot_res_instr(static_cast<CDot*>(node));
        case AST_T::CArrow_t:
            return arrow_res_instr(static_cast<CArrow*>(node));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<TacValue> plain_op_exp_instr(TacPlainOperand* res) {
    std::shared_ptr<TacValue> dst = std::move(res->val);
    return dst;
}

static std::shared_ptr<TacValue> deref_ptr_exp_instr(TacDereferencedPointer* res, CExp* node) {
    std::shared_ptr<TacValue> src = std::move(res->val);
    std::shared_ptr<TacValue> dst = plain_inner_value(node);
    push_instr(std::make_unique<TacLoad>(std::move(src), dst));
    return dst;
}

static std::shared_ptr<TacValue> sub_obj_exp_instr(TacSubObject* res, CExp* node) {
    TIdentifier src_name = std::move(res->base_name);
    TLong offset = std::move(res->offset);
    std::shared_ptr<TacValue> dst = plain_inner_value(node);
    push_instr(std::make_unique<TacCopyFromOffset>(std::move(src_name), std::move(offset), dst));
    return dst;
}

// exp_result = PlainOperand(val) | DereferencedPointer(val) | SubObject(val)
static std::shared_ptr<TacValue> repr_exp_instr(CExp* node) {
    std::unique_ptr<TacExpResult> res = repr_res_instr(node);
    switch (res->type()) {
        case AST_T::TacPlainOperand_t:
            return plain_op_exp_instr(static_cast<TacPlainOperand*>(res.get()));
        case AST_T::TacDereferencedPointer_t:
            return deref_ptr_exp_instr(static_cast<TacDereferencedPointer*>(res.get()), node);
        case AST_T::TacSubObject_t:
            return sub_obj_exp_instr(static_cast<TacSubObject*>(res.get()), node);
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void repr_block(CBlock* node);

static void statement_instr(CStatement* node);
static void var_decl_instr(CVariableDeclaration* node);

static void ret_statement_instr(CReturn* node) {
    std::shared_ptr<TacValue> val;
    if (node->exp) {
        val = repr_exp_instr(node->exp.get());
    }
    push_instr(std::make_unique<TacReturn>(std::move(val)));
}

static void exp_statement_instr(CExpression* node) { repr_res_instr(node->exp.get()); }

static void if_only_statement_instr(CIf* node) {
    TIdentifier target_false = repr_label_identifier(LABEL_KIND::Lif_false);
    {
        std::shared_ptr<TacValue> condition = repr_exp_instr(node->condition.get());
        push_instr(std::make_unique<TacJumpIfZero>(target_false, std::move(condition)));
    }
    statement_instr(node->then.get());
    push_instr(std::make_unique<TacLabel>(std::move(target_false)));
}

static void if_else_statement_instr(CIf* node) {
    TIdentifier target_else = repr_label_identifier(LABEL_KIND::Lif_else);
    TIdentifier target_false = repr_label_identifier(LABEL_KIND::Lif_false);
    {
        std::shared_ptr<TacValue> condition = repr_exp_instr(node->condition.get());
        push_instr(std::make_unique<TacJumpIfZero>(target_else, std::move(condition)));
    }
    statement_instr(node->then.get());
    push_instr(std::make_unique<TacJump>(target_false));
    push_instr(std::make_unique<TacLabel>(std::move(target_else)));
    statement_instr(node->else_fi.get());
    push_instr(std::make_unique<TacLabel>(std::move(target_false)));
}

static void if_statement_instr(CIf* node) {
    if (node->else_fi) {
        if_else_statement_instr(node);
    }
    else {
        if_only_statement_instr(node);
    }
}

static void goto_statement_instr(CGoto* node) {
    TIdentifier target_label = node->target;
    push_instr(std::make_unique<TacJump>(std::move(target_label)));
}

static void label_statement_instr(CLabel* node) {
    TIdentifier target_label = node->target;
    push_instr(std::make_unique<TacLabel>(std::move(target_label)));
    statement_instr(node->jump_to.get());
}

static void statement_compound_instr(CCompound* node) { repr_block(node->block.get()); }

static void while_statement_instr(CWhile* node) {
    TIdentifier target_break = repr_loop_identifier(LABEL_KIND::Lbreak, node->target);
    TIdentifier target_continue = repr_loop_identifier(LABEL_KIND::Lcontinue, node->target);
    push_instr(std::make_unique<TacLabel>(target_continue));
    {
        std::shared_ptr<TacValue> condition = repr_exp_instr(node->condition.get());
        push_instr(std::make_unique<TacJumpIfZero>(target_break, std::move(condition)));
    }
    statement_instr(node->body.get());
    push_instr(std::make_unique<TacJump>(std::move(target_continue)));
    push_instr(std::make_unique<TacLabel>(std::move(target_break)));
}

static void do_while_statement_instr(CDoWhile* node) {
    TIdentifier target_do_while_start = repr_label_identifier(LABEL_KIND::Ldo_while_start);
    TIdentifier target_break = repr_loop_identifier(LABEL_KIND::Lbreak, node->target);
    TIdentifier target_continue = repr_loop_identifier(LABEL_KIND::Lcontinue, node->target);
    push_instr(std::make_unique<TacLabel>(target_do_while_start));
    statement_instr(node->body.get());
    push_instr(std::make_unique<TacLabel>(std::move(target_continue)));
    {
        std::shared_ptr<TacValue> condition = repr_exp_instr(node->condition.get());
        push_instr(std::make_unique<TacJumpIfNotZero>(std::move(target_do_while_start), std::move(condition)));
    }
    push_instr(std::make_unique<TacLabel>(std::move(target_break)));
}

static void for_init_decl_instr(CInitDecl* node) { var_decl_instr(node->init.get()); }

static void for_init_exp_instr(CInitExp* node) {
    if (node->init) {
        repr_res_instr(node->init.get());
    }
}

static void for_init_statement_instr(CForInit* node) {
    switch (node->type()) {
        case AST_T::CInitDecl_t:
            for_init_decl_instr(static_cast<CInitDecl*>(node));
            break;
        case AST_T::CInitExp_t:
            for_init_exp_instr(static_cast<CInitExp*>(node));
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void for_statement_instr(CFor* node) {
    TIdentifier target_for_start = repr_label_identifier(LABEL_KIND::Lfor_start);
    TIdentifier target_break = repr_loop_identifier(LABEL_KIND::Lbreak, node->target);
    TIdentifier target_continue = repr_loop_identifier(LABEL_KIND::Lcontinue, node->target);
    for_init_statement_instr(node->init.get());
    push_instr(std::make_unique<TacLabel>(target_for_start));
    if (node->condition) {
        std::shared_ptr<TacValue> condition = repr_exp_instr(node->condition.get());
        push_instr(std::make_unique<TacJumpIfZero>(target_break, std::move(condition)));
    }
    statement_instr(node->body.get());
    push_instr(std::make_unique<TacLabel>(std::move(target_continue)));
    if (node->post) {
        repr_res_instr(node->post.get());
    }
    push_instr(std::make_unique<TacJump>(std::move(target_for_start)));
    push_instr(std::make_unique<TacLabel>(std::move(target_break)));
}

static void switch_statement_instr(CSwitch* node) {
    TIdentifier target_break = repr_loop_identifier(LABEL_KIND::Lbreak, node->target);
    {
        std::shared_ptr<TacValue> match = repr_exp_instr(node->match.get());
        for (size_t i = 0; i < node->cases.size(); ++i) {
            TIdentifier target_case = repr_case_identifier(node->target, true, i);
            std::shared_ptr<TacValue> case_match;
            {
                std::shared_ptr<TacValue> esac = repr_exp_instr(node->cases[i].get());
                case_match = plain_inner_value(node->cases[i].get());
                std::unique_ptr<TacBinaryOp> binary_op = std::make_unique<TacEqual>();
                push_instr(std::make_unique<TacBinary>(std::move(binary_op), match, std::move(esac), case_match));
            }
            push_instr(std::make_unique<TacJumpIfNotZero>(std::move(target_case), std::move(case_match)));
        }
    }
    if (node->is_default) {
        TIdentifier target_default = repr_loop_identifier(LABEL_KIND::Ldefault, node->target);
        push_instr(std::make_unique<TacJump>(std::move(target_default)));
        statement_instr(node->body.get());
    }
    else if (!node->cases.empty()) {
        push_instr(std::make_unique<TacJump>(target_break));
        statement_instr(node->body.get());
    }
    push_instr(std::make_unique<TacLabel>(std::move(target_break)));
}

static void case_statement_instr(CCase* node) {
    TIdentifier target_case = repr_loop_identifier(LABEL_KIND::Lcase, node->target);
    push_instr(std::make_unique<TacLabel>(std::move(target_case)));
    statement_instr(node->jump_to.get());
}

static void default_statement_instr(CDefault* node) {
    TIdentifier target_default = repr_loop_identifier(LABEL_KIND::Ldefault, node->target);
    push_instr(std::make_unique<TacLabel>(std::move(target_default)));
    statement_instr(node->jump_to.get());
}

static void break_statement_instr(CBreak* node) {
    TIdentifier target_break = repr_loop_identifier(LABEL_KIND::Lbreak, node->target);
    push_instr(std::make_unique<TacJump>(std::move(target_break)));
}

static void continue_statement_instr(CContinue* node) {
    TIdentifier target_continue = repr_loop_identifier(LABEL_KIND::Lcontinue, node->target);
    push_instr(std::make_unique<TacJump>(std::move(target_continue)));
}

static void statement_instr(CStatement* node) {
    switch (node->type()) {
        case AST_T::CReturn_t:
            ret_statement_instr(static_cast<CReturn*>(node));
            break;
        case AST_T::CExpression_t:
            exp_statement_instr(static_cast<CExpression*>(node));
            break;
        case AST_T::CIf_t:
            if_statement_instr(static_cast<CIf*>(node));
            break;
        case AST_T::CGoto_t:
            goto_statement_instr(static_cast<CGoto*>(node));
            break;
        case AST_T::CLabel_t:
            label_statement_instr(static_cast<CLabel*>(node));
            break;
        case AST_T::CCompound_t:
            statement_compound_instr(static_cast<CCompound*>(node));
            break;
        case AST_T::CWhile_t:
            while_statement_instr(static_cast<CWhile*>(node));
            break;
        case AST_T::CDoWhile_t:
            do_while_statement_instr(static_cast<CDoWhile*>(node));
            break;
        case AST_T::CFor_t:
            for_statement_instr(static_cast<CFor*>(node));
            break;
        case AST_T::CSwitch_t:
            switch_statement_instr(static_cast<CSwitch*>(node));
            break;
        case AST_T::CCase_t:
            case_statement_instr(static_cast<CCase*>(node));
            break;
        case AST_T::CDefault_t:
            default_statement_instr(static_cast<CDefault*>(node));
            break;
        case AST_T::CBreak_t:
            break_statement_instr(static_cast<CBreak*>(node));
            break;
        case AST_T::CContinue_t:
            continue_statement_instr(static_cast<CContinue*>(node));
            break;
        case AST_T::CNull_t:
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void compound_init_instr(CInitializer* node, Type* init_type, TIdentifier symbol, TLong& size);

static void string_single_init_instr(CString* node, Array* arr_type, TIdentifier symbol, TLong size) {
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
                    TChar value = string_bytes_to_int8(node->literal->value, byte_at);
                    constant = std::make_shared<CConstChar>(std::move(value));
                    byte_at++;
                }
                else if (bytes_left < 8) {
                    TInt value = string_bytes_to_int32(node->literal->value, byte_at);
                    constant = std::make_shared<CConstInt>(std::move(value));
                    byte_at += 4;
                }
                else {
                    TLong value = string_bytes_to_int64(node->literal->value, byte_at);
                    constant = std::make_shared<CConstLong>(std::move(value));
                    byte_at += 8;
                }
            }
            src = std::make_shared<TacConstant>(std::move(constant));
        }
        push_instr(std::make_unique<TacCopyToOffset>(std::move(dst_name), std::move(offset), std::move(src)));
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
        push_instr(std::make_unique<TacCopyToOffset>(std::move(dst_name), std::move(offset), std::move(src)));
    }
}

static void single_init_instr(CSingleInit* node, Type* init_type, TIdentifier symbol) {
    if (node->exp->type() == AST_T::CString_t && init_type->type() == AST_T::Array_t) {
        string_single_init_instr(static_cast<CString*>(node->exp.get()), static_cast<Array*>(init_type), symbol, 0l);
    }
    else {
        std::shared_ptr<TacValue> src = repr_exp_instr(node->exp.get());
        std::shared_ptr<TacValue> dst;
        {
            TIdentifier name = symbol;
            std::unique_ptr<CExp> exp = std::make_unique<CVar>(std::move(name), 0);
            dst = repr_value(exp.get());
        }
        push_instr(std::make_unique<TacCopy>(std::move(src), std::move(dst)));
    }
}

static void scalar_compound_init_instr(CSingleInit* node, Type* init_type, TIdentifier symbol, TLong& size) {
    if (node->exp->type() == AST_T::CString_t && init_type->type() == AST_T::Array_t) {
        string_single_init_instr(static_cast<CString*>(node->exp.get()), static_cast<Array*>(init_type), symbol, size);
    }
    else {
        TIdentifier dst_name = symbol;
        TLong offset = size;
        std::shared_ptr<TacValue> src = repr_exp_instr(node->exp.get());
        push_instr(std::make_unique<TacCopyToOffset>(std::move(dst_name), std::move(offset), std::move(src)));
    }
}

static void arr_compound_init_instr(CCompoundInit* node, Array* arr_type, TIdentifier symbol, TLong& size) {
    for (const auto& initializer : node->initializers) {
        compound_init_instr(initializer.get(), arr_type->elem_type.get(), symbol, size);
        if (initializer->type() == AST_T::CSingleInit_t) {
            size += get_type_scale(arr_type->elem_type.get());
        }
    }
}

static void struct_compound_init_instr(CCompoundInit* node, Structure* struct_type, TIdentifier symbol, TLong& size) {
    for (size_t i = node->initializers.size(); i-- > 0;) {
        const auto& member = GET_STRUCT_TYPEDEF_MEMBER(struct_type->tag, i);
        TLong offset = size + member->offset;
        compound_init_instr(node->initializers[i].get(), member->member_type.get(), symbol, offset);
    }
    size += get_type_scale(struct_type);
}

static void aggr_compound_init_instr(CCompoundInit* node, Type* init_type, TIdentifier symbol, TLong& size) {
    switch (init_type->type()) {
        case AST_T::Array_t:
            arr_compound_init_instr(node, static_cast<Array*>(init_type), symbol, size);
            break;
        case AST_T::Structure_t:
            struct_compound_init_instr(node, static_cast<Structure*>(init_type), symbol, size);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void compound_init_instr(CInitializer* node, Type* init_type, TIdentifier symbol, TLong& size) {
    switch (node->type()) {
        case AST_T::CSingleInit_t:
            scalar_compound_init_instr(static_cast<CSingleInit*>(node), init_type, symbol, size);
            break;
        case AST_T::CCompoundInit_t:
            aggr_compound_init_instr(static_cast<CCompoundInit*>(node), init_type, symbol, size);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void var_decl_instr(CVariableDeclaration* node) {
    switch (node->init->type()) {
        case AST_T::CSingleInit_t:
            single_init_instr(static_cast<CSingleInit*>(node->init.get()),
                frontend->symbol_table[node->name]->type_t.get(), node->name);
            break;
        case AST_T::CCompoundInit_t: {
            TLong size = 0l;
            aggr_compound_init_instr(static_cast<CCompoundInit*>(node->init.get()),
                frontend->symbol_table[node->name]->type_t.get(), node->name, size);
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void var_declaration_instr(CVarDecl* node) {
    if (frontend->symbol_table[node->variable_decl->name]->attrs->type() != AST_T::StaticAttr_t
        && node->variable_decl->init) {
        var_decl_instr(node->variable_decl.get());
    }
}

static void declaration_instr(CDeclaration* node) {
    switch (node->type()) {
        case AST_T::CFunDecl_t:
        case AST_T::CStructDecl_t:
            break;
        case AST_T::CVarDecl_t:
            var_declaration_instr(static_cast<CVarDecl*>(node));
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
static void repr_instr_list(const std::vector<std::unique_ptr<CBlockItem>>& list_node) {
    for (const auto& block_item : list_node) {
        switch (block_item->type()) {
            case AST_T::CS_t:
                statement_instr(static_cast<CS*>(block_item.get())->statement.get());
                break;
            case AST_T::CD_t:
                declaration_instr(static_cast<CD*>(block_item.get())->declaration.get());
                break;
            default:
                RAISE_INTERNAL_ERROR;
        }
    }
}

static void repr_block(CBlock* node) {
    switch (node->type()) {
        case AST_T::CB_t:
            repr_instr_list(static_cast<CB*>(node)->block_items);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::unique_ptr<TacFunction> repr_fun_toplvl(CFunctionDeclaration* node) {
    TIdentifier name = node->name;
    bool is_global = static_cast<FunAttr*>(frontend->symbol_table[node->name]->attrs.get())->is_global;

    std::vector<TIdentifier> params(node->params.begin(), node->params.end());

    std::vector<std::unique_ptr<TacInstruction>> body;
    {
        context->p_instructions = &body;
        repr_block(node->body.get());
        {
            std::shared_ptr<CConst> constant = std::make_shared<CConstInt>(0);
            std::shared_ptr<TacValue> val = std::make_shared<TacConstant>(std::move(constant));
            push_instr(std::make_unique<TacReturn>(std::move(val)));
        }
        context->p_instructions = nullptr;
    }

    return std::make_unique<TacFunction>(std::move(name), std::move(is_global), std::move(params), std::move(body));
}

static void push_toplvl(std::unique_ptr<TacTopLevel>&& top_level) {
    context->p_top_levels->push_back(std::move(top_level));
}

static void fun_decl_toplvl(CFunDecl* node) {
    if (node->function_decl->body) {
        push_toplvl(repr_fun_toplvl(node->function_decl.get()));
    }
}

// (function) top_level = Function(identifier, bool, identifier*, instruction*)
static void declaration_toplvl(CDeclaration* node) {
    switch (node->type()) {
        case AST_T::CFunDecl_t:
            fun_decl_toplvl(static_cast<CFunDecl*>(node));
            break;
        case AST_T::CVarDecl_t:
        case AST_T::CStructDecl_t:
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::vector<std::shared_ptr<StaticInit>> tentative_static_toplvl(Type* static_init_type) {
    std::vector<std::shared_ptr<StaticInit>> static_inits;
    {
        TLong byte = get_type_scale(static_init_type);
        std::shared_ptr<StaticInit> static_init = std::make_shared<ZeroInit>(std::move(byte));
        static_inits.push_back(std::move(static_init));
    }
    return static_inits;
}

static std::vector<std::shared_ptr<StaticInit>> initial_static_toplvl(Initial* node) {
    std::vector<std::shared_ptr<StaticInit>> static_inits(node->static_inits.begin(), node->static_inits.end());
    return static_inits;
}

static void repr_static_var_toplvl(Symbol* node, TIdentifier symbol) {
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
            static_inits = tentative_static_toplvl(static_init_type.get());
            break;
        case AST_T::Initial_t:
            static_inits = initial_static_toplvl(static_cast<Initial*>(static_attr->init.get()));
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }

    push_toplvl(std::make_unique<TacStaticVariable>(
        std::move(name), std::move(is_global), std::move(static_init_type), std::move(static_inits)));
}

static void push_static_const_toplvl(std::unique_ptr<TacTopLevel>&& static_constant_top_levels) {
    context->p_static_constant_top_levels->push_back(std::move(static_constant_top_levels));
}

static void repr_static_const_toplvl(Symbol* node, TIdentifier symbol) {
    TIdentifier name = symbol;
    std::shared_ptr<Type> static_init_type = node->type_t;
    std::shared_ptr<StaticInit> static_init = static_cast<ConstantAttr*>(node->attrs.get())->static_init;
    push_static_const_toplvl(
        std::make_unique<TacStaticConstant>(std::move(name), std::move(static_init_type), std::move(static_init)));
}

// (static variable) top_level = StaticVariable(identifier, bool, type, static_init*)
// (static constant) top_level = StaticConstant(identifier, type, static_init)
static void symbol_toplvl(Symbol* node, TIdentifier symbol) {
    switch (node->attrs->type()) {
        case AST_T::StaticAttr_t:
            repr_static_var_toplvl(node, symbol);
            break;
        case AST_T::ConstantAttr_t:
            repr_static_const_toplvl(node, symbol);
            break;
        default:
            break;
    }
}

// AST = Program(top_level*, top_level*, top_level*)
static std::unique_ptr<TacProgram> repr_program(CProgram* node) {
    std::vector<std::unique_ptr<TacTopLevel>> function_top_levels;
    {
        context->p_top_levels = &function_top_levels;
        for (const auto& declaration : node->declarations) {
            declaration_toplvl(declaration.get());
        }
        context->p_top_levels = nullptr;
    }

    std::vector<std::unique_ptr<TacTopLevel>> static_variable_top_levels;
    std::vector<std::unique_ptr<TacTopLevel>> static_constant_top_levels;
    {
        context->p_top_levels = &static_variable_top_levels;
        context->p_static_constant_top_levels = &static_constant_top_levels;
        for (const auto& symbol : frontend->symbol_table) {
            symbol_toplvl(symbol.second.get(), symbol.first);
        }
        context->p_top_levels = nullptr;
        context->p_static_constant_top_levels = nullptr;
    }

    return std::make_unique<TacProgram>(
        std::move(static_constant_top_levels), std::move(static_variable_top_levels), std::move(function_top_levels));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<TacProgram> tac_representation(std::unique_ptr<CProgram> c_ast) {
    context = std::make_unique<TacReprContext>();
    std::unique_ptr<TacProgram> tac_ast = repr_program(c_ast.get());
    context.reset();

    c_ast.reset();
    if (!tac_ast) {
        RAISE_INTERNAL_ERROR;
    }
    return tac_ast;
}
