#include <string>

#include "util/throw.hpp"

#include "frontend/parser/errors.hpp"
#include "frontend/parser/lexer.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Errors

std::string get_token_kind_hr(TOKEN_KIND token_kind) {
    switch (token_kind) {
        case TOKEN_KIND::assignment_bitshiftleft:
            return "<<=";
        case TOKEN_KIND::assignment_bitshiftright:
            return ">>=";
        case TOKEN_KIND::unop_decrement:
            return "--";
        case TOKEN_KIND::binop_bitshiftleft:
            return "<<";
        case TOKEN_KIND::binop_bitshiftright:
            return ">>";
        case TOKEN_KIND::binop_and:
            return "&&";
        case TOKEN_KIND::binop_or:
            return "||";
        case TOKEN_KIND::binop_equalto:
            return "==";
        case TOKEN_KIND::binop_notequal:
            return "!=";
        case TOKEN_KIND::binop_lessthanorequal:
            return "<=";
        case TOKEN_KIND::binop_greaterthanorequal:
            return ">=";
        case TOKEN_KIND::assignment_plus:
            return "+=";
        case TOKEN_KIND::assignment_difference:
            return "-=";
        case TOKEN_KIND::assignment_product:
            return "*=";
        case TOKEN_KIND::assignment_quotient:
            return "/=";
        case TOKEN_KIND::assignment_remainder:
            return "%=";
        case TOKEN_KIND::assignment_bitand:
            return "&=";
        case TOKEN_KIND::assignment_bitor:
            return "|=";
        case TOKEN_KIND::assignment_bitxor:
            return "^=";
        case TOKEN_KIND::structop_pointer:
            return "->";
        case TOKEN_KIND::parenthesis_open:
            return "(";
        case TOKEN_KIND::parenthesis_close:
            return ")";
        case TOKEN_KIND::brace_open:
            return "{";
        case TOKEN_KIND::brace_close:
            return "}";
        case TOKEN_KIND::brackets_open:
            return "[";
        case TOKEN_KIND::brackets_close:
            return "]";
        case TOKEN_KIND::semicolon:
            return ";";
        case TOKEN_KIND::unop_complement:
            return "~";
        case TOKEN_KIND::unop_negation:
            return "-";
        case TOKEN_KIND::unop_not:
            return "!";
        case TOKEN_KIND::binop_addition:
            return "+";
        case TOKEN_KIND::binop_multiplication:
            return "*";
        case TOKEN_KIND::binop_division:
            return "/";
        case TOKEN_KIND::binop_remainder:
            return "%";
        case TOKEN_KIND::binop_bitand:
            return "&";
        case TOKEN_KIND::binop_bitor:
            return "|";
        case TOKEN_KIND::binop_bitxor:
            return "^";
        case TOKEN_KIND::binop_lessthan:
            return "<";
        case TOKEN_KIND::binop_greaterthan:
            return ">";
        case TOKEN_KIND::assignment_simple:
            return "=";
        case TOKEN_KIND::ternary_if:
            return "?";
        case TOKEN_KIND::ternary_else:
            return ":";
        case TOKEN_KIND::separator_comma:
            return ",";
        case TOKEN_KIND::structop_member:
            return ".";
        case TOKEN_KIND::key_char:
            return "char";
        case TOKEN_KIND::key_int:
            return "int";
        case TOKEN_KIND::key_long:
            return "long";
        case TOKEN_KIND::key_double:
            return "double";
        case TOKEN_KIND::key_signed:
            return "signed";
        case TOKEN_KIND::key_unsigned:
            return "unsigned";
        case TOKEN_KIND::key_void:
            return "void";
        case TOKEN_KIND::key_struct:
            return "struct";
        case TOKEN_KIND::key_sizeof:
            return "sizeof";
        case TOKEN_KIND::key_return:
            return "return";
        case TOKEN_KIND::key_if:
            return "if";
        case TOKEN_KIND::key_else:
            return "else";
        case TOKEN_KIND::key_goto:
            return "goto";
        case TOKEN_KIND::key_do:
            return "do";
        case TOKEN_KIND::key_while:
            return "while";
        case TOKEN_KIND::key_for:
            return "for";
        case TOKEN_KIND::key_break:
            return "break";
        case TOKEN_KIND::key_continue:
            return "continue";
        case TOKEN_KIND::key_static:
            return "static";
        case TOKEN_KIND::key_extern:
            return "extern";
        case TOKEN_KIND::identifier:
            return "identifier";
        case TOKEN_KIND::string_literal:
            return "string literal";
        case TOKEN_KIND::char_constant:
            return "const char";
        case TOKEN_KIND::float_constant:
            return "const double";
        case TOKEN_KIND::unsigned_long_constant:
            return "const unsigned long";
        case TOKEN_KIND::unsigned_constant:
            return "const unsigned int";
        case TOKEN_KIND::long_constant:
            return "const long";
        case TOKEN_KIND::constant:
            return "const int";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

std::string get_name_hr(const TIdentifier& name) { return name.substr(0, name.find('.')); }

std::string get_struct_name_hr(const TIdentifier& name) {
    std::string type_hr = get_name_hr(name);
    return "struct " + type_hr;
}

std::string get_type_hr(Type* type);

static std::string get_function_type_hr(FunType* fun_type) {
    std::string param_type_hr = "";
    for (const auto& param_type : fun_type->param_types) {
        param_type_hr += get_type_hr(param_type.get()) + ", ";
    }
    if (!fun_type->param_types.empty()) {
        param_type_hr.pop_back();
        param_type_hr.pop_back();
    }
    std::string type_hr = get_type_hr(fun_type->ret_type.get());
    return "(" + type_hr + ")(" + param_type_hr + ")";
}

static std::string get_pointer_type_hr(Pointer* ptr_type) {
    std::string ptr_type_hr = "*";
    while (ptr_type->ref_type->type() == Pointer_t) {
        ptr_type = static_cast<Pointer*>(ptr_type->ref_type.get());
        ptr_type_hr += "*";
    }
    std::string type_hr = get_type_hr(ptr_type->ref_type.get());
    return type_hr + ptr_type_hr;
}

static std::string get_array_type_hr(Array* arr_type) {
    std::string array_type_hr = "[" + std::to_string(arr_type->size) + "]";
    while (arr_type->elem_type->type() == Array_t) {
        arr_type = static_cast<Array*>(arr_type->elem_type.get());
        array_type_hr += "[" + std::to_string(arr_type->size) + "]";
    }
    std::string type_hr = get_type_hr(arr_type->elem_type.get());
    return type_hr + array_type_hr;
}

static std::string get_structure_type_hr(Structure* struct_type) { return get_struct_name_hr(struct_type->tag); }

std::string get_type_hr(Type* type) {
    switch (type->type()) {
        case AST_T::Char_t:
            return "char";
        case AST_T::SChar_t:
            return "signed char";
        case AST_T::UChar_t:
            return "unsigned char";
        case AST_T::Int_t:
            return "int";
        case AST_T::Long_t:
            return "long";
        case AST_T::UInt_t:
            return "unsigned int";
        case AST_T::ULong_t:
            return "unsigned long";
        case AST_T::Double_t:
            return "double";
        case AST_T::Void_t:
            return "void";
        case AST_T::FunType_t:
            return get_function_type_hr(static_cast<FunType*>(type));
        case AST_T::Pointer_t:
            return get_pointer_type_hr(static_cast<Pointer*>(type));
        case AST_T::Array_t:
            return get_array_type_hr(static_cast<Array*>(type));
        case AST_T::Structure_t:
            return get_structure_type_hr(static_cast<Structure*>(type));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

std::string get_const_hr(CConst* node) {
    switch (node->type()) {
        case AST_T::CConstInt_t:
            return "int";
        case AST_T::CConstLong_t:
            return "long";
        case AST_T::CConstUInt_t:
            return "unsigned int";
        case AST_T::CConstULong_t:
            return "unsigned long";
        case AST_T::CConstDouble_t:
            return "double";
        case AST_T::CConstChar_t:
            return "char";
        case AST_T::CConstUChar_t:
            return "unsigned char";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

std::string get_storage_class_hr(CStorageClass* node) {
    switch (node->type()) {
        case AST_T::CStatic_t:
            return "static";
        case AST_T::CExtern_t:
            return "extern";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

std::string get_unary_op_hr(CUnaryOp* node) {
    switch (node->type()) {
        case AST_T::CComplement_t:
            return "~";
        case AST_T::CNegate_t:
            return "-";
        case AST_T::CNot_t:
            return "!";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

std::string get_binary_op_hr(CBinaryOp* node) {
    switch (node->type()) {
        case CAdd_t:
            return "+";
        case CSubtract_t:
            return "-";
        case CMultiply_t:
            return "*";
        case CDivide_t:
            return "/";
        case CRemainder_t:
            return "%";
        case CBitAnd_t:
            return "&";
        case CBitOr_t:
            return "|";
        case CBitXor_t:
            return "^";
        case CBitShiftLeft_t:
            return "<<";
        case CBitShiftRight_t:
            return ">>";
        case CBitShrArithmetic_t:
            return ">>";
        case CAnd_t:
            return "&&";
        case COr_t:
            return "||";
        case CEqual_t:
            return "==";
        case CNotEqual_t:
            return "!=";
        case CLessThan_t:
            return "<";
        case CLessOrEqual_t:
            return "<=";
        case CGreaterThan_t:
            return ">";
        case CGreaterOrEqual_t:
            return ">=";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

std::string get_assignment_hr(CBinaryOp* node) {
    if (!node) {
        return "=";
    }
    else {
        switch (node->type()) {
            case CAdd_t:
                return "+=";
            case CSubtract_t:
                return "-=";
            case CMultiply_t:
                return "*=";
            case CDivide_t:
                return "/=";
            case CRemainder_t:
                return "%=";
            case CBitAnd_t:
                return "&=";
            case CBitOr_t:
                return "|=";
            case CBitXor_t:
                return "^=";
            case CBitShiftLeft_t:
                return "<<=";
            case CBitShiftRight_t:
                return ">>=";
            case CBitShrArithmetic_t:
                return ">>=";
            default:
                RAISE_INTERNAL_ERROR;
        }
    }
}

static std::string get_what_message(ERROR_MESSAGE message) {
    switch (message) {
        case ERROR_MESSAGE::runtime_error:
            return "";

        // Main
        case ERROR_MESSAGE::no_option_code:
            return "no option code";
        case ERROR_MESSAGE::invalid_option_code:
            return "invalid option code %s";
        case ERROR_MESSAGE::no_input_files:
            return "no input files";

        // Util
        case ERROR_MESSAGE::fail_open_rb_file:
            return "%s: No such file or directory";
        case ERROR_MESSAGE::fail_open_wb_file:
            return "%s: Failed to create output file";
        case ERROR_MESSAGE::string_not_integer:
            return "string %s not an integer";
        case ERROR_MESSAGE::string_not_unsigned:
            return "string %s not an unsigned integer";
        case ERROR_MESSAGE::string_not_float:
            return "string %s not a floating-point number";

        // Lexer
        case ERROR_MESSAGE::invalid_token:
            return "found invalid token %s";

        // Parser
        case ERROR_MESSAGE::invalid_next_token:
            return "found token %s, but expected %s next";
        case ERROR_MESSAGE::reach_end_of_input:
            return "reached end of file, but expected declaration or statement next";
        case ERROR_MESSAGE::out_of_bound_constant:
            return "cannot represent %s as a 64 bits signed integer constant, very large number";
        case ERROR_MESSAGE::out_of_bound_unsigned:
            return "cannot represent %s as a 64 bits unsigned integer constant, very large number";
        case ERROR_MESSAGE::invalid_arr_size_type:
            return "illegal array size %s, requires a constant integer";
        case ERROR_MESSAGE::invalid_unary_op:
            return "found token %s, but expected " + em(get_token_kind_hr(TOKEN_KIND::unop_complement)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::unop_negation)) + " or "
                   + em(get_token_kind_hr(TOKEN_KIND::unop_not)) + " next";
        case ERROR_MESSAGE::invalid_binary_op:
            return "found token %s, but expected " + em(get_token_kind_hr(TOKEN_KIND::binop_addition)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::assignment_plus)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::unop_negation)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::assignment_difference)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_multiplication)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::assignment_product)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_division)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::assignment_quotient)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_remainder)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::assignment_remainder)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_bitand)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::assignment_bitand)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_bitor)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::assignment_bitor)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_bitxor)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::assignment_bitxor)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_bitshiftleft)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::assignment_bitshiftleft)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_bitshiftright)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::assignment_bitshiftright)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_and)) + ", " + em(get_token_kind_hr(TOKEN_KIND::binop_or))
                   + ", " + em(get_token_kind_hr(TOKEN_KIND::binop_equalto)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_notequal)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_lessthan)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_lessthanorequal)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_greaterthan)) + " or "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_greaterthanorequal)) + " next";
        case ERROR_MESSAGE::invalid_abstract_declarator:
            return "found token %s, but expected " + em(get_token_kind_hr(TOKEN_KIND::binop_multiplication)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::parenthesis_open)) + " or "
                   + em(get_token_kind_hr(TOKEN_KIND::brackets_open)) + " next";
        case ERROR_MESSAGE::invalid_pointer_unary_factor:
            return "found token %s, but expected " + em(get_token_kind_hr(TOKEN_KIND::binop_multiplication)) + " or "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_bitand)) + " next";
        case ERROR_MESSAGE::invalid_primary_exp_factor:
            return "found token %s, but expected " + em(get_token_kind_hr(TOKEN_KIND::constant)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::long_constant)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::char_constant)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::float_constant)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::unsigned_constant)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::unsigned_long_constant)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::identifier)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::identifier) + get_token_kind_hr(TOKEN_KIND::parenthesis_open))
                   + ", " + em(get_token_kind_hr(TOKEN_KIND::string_literal)) + " or "
                   + em(get_token_kind_hr(TOKEN_KIND::parenthesis_open)) + " next";
        case ERROR_MESSAGE::invalid_exp:
            return "found token %s, but expected " + em(get_token_kind_hr(TOKEN_KIND::binop_addition)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::unop_negation)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_multiplication)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_division)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_remainder)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_bitand)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_bitor)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_bitxor)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_bitshiftleft)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_bitshiftright)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_lessthan)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_lessthanorequal)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_greaterthan)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_greaterthanorequal)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_equalto)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_notequal)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_and)) + ", " + em(get_token_kind_hr(TOKEN_KIND::binop_or))
                   + ", " + em(get_token_kind_hr(TOKEN_KIND::assignment_simple)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::assignment_plus)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::assignment_difference)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::assignment_product)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::assignment_quotient)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::assignment_remainder)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::assignment_bitand)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::assignment_bitor)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::assignment_bitxor)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::assignment_bitshiftleft)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::assignment_bitshiftright)) + " or "
                   + em(get_token_kind_hr(TOKEN_KIND::ternary_if)) + " next";
        case ERROR_MESSAGE::invalid_for_loop_decl_type:
            return "function %s declared in " + em("for") + " loop initial declaration";
        case ERROR_MESSAGE::invalid_type_specifier:
            return "found token %s, but expected " + em(get_token_kind_hr(TOKEN_KIND::identifier)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::parenthesis_close)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::key_char)) + ", " + em(get_token_kind_hr(TOKEN_KIND::key_int))
                   + ", " + em(get_token_kind_hr(TOKEN_KIND::key_long)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::key_double)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::key_unsigned)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::key_signed)) + ", " + em(get_token_kind_hr(TOKEN_KIND::key_void))
                   + ", " + em(get_token_kind_hr(TOKEN_KIND::key_struct)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::key_static)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::key_extern)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_multiplication)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::parenthesis_open)) + " or "
                   + em(get_token_kind_hr(TOKEN_KIND::brackets_open)) + " next";
        case ERROR_MESSAGE::invalid_type_specifier_list:
            return "found tokens %s, but expected valid list of unique type specifiers next";
        case ERROR_MESSAGE::invalid_storage_class:
            return "found token %s, but expected " + em(get_token_kind_hr(TOKEN_KIND::key_static)) + " or "
                   + em(get_token_kind_hr(TOKEN_KIND::key_extern)) + " next";
        case ERROR_MESSAGE::empty_compound_initializer:
            return "empty compound initializer requires at least one initializer";
        case ERROR_MESSAGE::many_fun_type_derivation:
            return "cannot apply further type derivation to function declaration";
        case ERROR_MESSAGE::invalid_simple_declarator:
            return "found token %s, but expected " + em(get_token_kind_hr(TOKEN_KIND::identifier)) + " or "
                   + em(get_token_kind_hr(TOKEN_KIND::parenthesis_open)) + " next";
        case ERROR_MESSAGE::invalid_param_list:
            return "found token %s, but expected " + em(get_token_kind_hr(TOKEN_KIND::key_void)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::key_char)) + ", " + em(get_token_kind_hr(TOKEN_KIND::key_int))
                   + ", " + em(get_token_kind_hr(TOKEN_KIND::key_long)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::key_double)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::key_unsigned)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::key_signed)) + " or "
                   + em(get_token_kind_hr(TOKEN_KIND::key_struct)) + " next";
        case ERROR_MESSAGE::invalid_member_decl_storage:
            return "structure type declared with member %s with %s storage class";
        case ERROR_MESSAGE::invalid_member_decl_fun_type:
            return "structure type declared with member %s as a function";

        // Semantic
        case ERROR_MESSAGE::array_of_incomplete_type:
            return "array type %s of incomplete type %s, requires a complete type";
        case ERROR_MESSAGE::joint_pointer_type_mismatch:
            return "pointer type mismatch %s and %s in operator";
        case ERROR_MESSAGE::function_used_as_variable:
            return "function %s used as a variable";
        case ERROR_MESSAGE::cannot_convert_from_to:
            return "illegal cast, cannot convert expression from type %s to %s";
        case ERROR_MESSAGE::cannot_apply_unop_on_type:
            return "cannot apply unary operator %s on operand type %s";
        case ERROR_MESSAGE::cannot_apply_binop_on_type:
            return "cannot apply binary operator %s on operand type %s";
        case ERROR_MESSAGE::cannot_apply_binop_on_types:
            return "cannot apply binary operator %s on operand types %s and %s";
        case ERROR_MESSAGE::wrong_lhs_assignment_type:
            return "cannot assign " + em("=") + " to left operand type " + em("void");
        case ERROR_MESSAGE::invalid_lvalue_lhs_assignment:
            return "assignment %s requires lvalue left operand, but got rvalue";
        case ERROR_MESSAGE::wrong_cond_type_conditional:
            return "cannot apply conditional " + em("?") + " on condition operand type %s";
        case ERROR_MESSAGE::conditional_type_mismatch:
            return "cannot apply ternary operator " + em(":") + " on operand types %s and %s";
        case ERROR_MESSAGE::variable_used_as_function:
            return "variable %s used as a function";
        case ERROR_MESSAGE::wrong_number_of_arguments:
            return "function %s called with %s arguments instead of %s";
        case ERROR_MESSAGE::cannot_dereference_non_pointer:
            return "cannot apply dereference operator " + em("*") + " on non-pointer type %s";
        case ERROR_MESSAGE::invalid_lvalue_address_of:
            return "addresssing " + em("&") + " requires lvalue operand, but got rvalue";
        case ERROR_MESSAGE::invalid_array_subscript_types:
            return "cannot subscript array with operand types %s and %s, requires a complete pointer and an "
                   "integer types";
        case ERROR_MESSAGE::size_of_incomplete_type:
            return "cannot get size with " + em("sizeof") + " operator on incomplete type %s";
        case ERROR_MESSAGE::access_member_non_struct:
            return "cannot access structure member %s with dot operator " + em(".") + " on non-structure type %s";
        case ERROR_MESSAGE::struct_has_no_member_named:
            return "structure type %s has no member named %s";
        case ERROR_MESSAGE::access_member_non_pointer:
            return "cannot access structure member %s with arrow operator " + em("->")
                   + " on non-pointer-to-structure type %s";
        case ERROR_MESSAGE::access_member_incomplete_type:
            return "cannot access structure member %s with arrow operator " + em("->")
                   + " on incomplete structure type %s";
        case ERROR_MESSAGE::incomplete_struct_type:
            return "incomplete structure type %s in expression";
        case ERROR_MESSAGE::return_value_in_void_function:
            return "found " + em("return") + " value in function %s returning type " + em("void");
        case ERROR_MESSAGE::no_return_value_in_function:
            return "found " + em("return") + " with no value in function %s returning type %s";
        case ERROR_MESSAGE::cannot_use_if_with_type:
            return "cannot use " + em("if") + " statement with expression type %s";
        case ERROR_MESSAGE::cannot_use_while_with_type:
            return "cannot use " + em("while") + " loop statement with expression type %s";
        case ERROR_MESSAGE::cannot_use_do_while_with_type:
            return "cannot use " + em("do while") + " loop statement with expression type %s";
        case ERROR_MESSAGE::cannot_use_for_with_type:
            return "cannot use " + em("for") + " loop statement with expression type %s";
        case ERROR_MESSAGE::non_char_array_from_string:
            return "non-character array type %s initialized from string literal";
        case ERROR_MESSAGE::wrong_string_literal_size:
            return "size %s string literal initialized with %s characters";
        case ERROR_MESSAGE::wrong_array_initializer_size:
            return "size %s array type %s initialized with %s initializers";
        case ERROR_MESSAGE::wrong_struct_members_number:
            return "structure type %s initialized with %s members instead of %s";
        case ERROR_MESSAGE::function_returns_array:
            return "function %s returns array type %s, instead of pointer type";
        case ERROR_MESSAGE::function_returns_incomplete:
            return "function %s returns incomplete structure type %s";
        case ERROR_MESSAGE::function_has_void_param:
            return "function %s declared with parameter %s type " + em("void");
        case ERROR_MESSAGE::function_has_incomplete_param:
            return "function %s defined with parameter %s incomplete structure type %s";
        case ERROR_MESSAGE::redeclaration_type_mismatch:
            return "function %s redeclared with function type %s, but previous declaration has function type %s";
        case ERROR_MESSAGE::redeclare_function_type:
            return "function %s already defined with function type %s";
        case ERROR_MESSAGE::redeclare_non_static_function:
            return "function %s with " + em("static") + " storage class already declared non-static";
        case ERROR_MESSAGE::pointer_type_from_constant:
            return "cannot statically initialize pointer type %s from constant %s, requires a constant integer";
        case ERROR_MESSAGE::pointer_type_from_non_null:
            return "cannot statically initialize pointer type %s from non-null value %s";
        case ERROR_MESSAGE::non_char_pointer_from_string:
            return "non-character pointer type %s statically initialized from string literal";
        case ERROR_MESSAGE::static_variable_non_constant:
            return "cannot statically initialize variable from non-constant type %s, requires a constant";
        case ERROR_MESSAGE::scalar_type_from_compound:
            return "cannot initialize scalar type %s with compound initializer";
        case ERROR_MESSAGE::variable_declared_void:
            return "variable %s declared with type " + em("void");
        case ERROR_MESSAGE::variable_incomplete_structure:
            return "variable %s declared with incomplete structure type %s";
        case ERROR_MESSAGE::redeclare_variable_mismatch:
            return "variable %s redeclared with conflicting type %s, but previously declared with type %s";
        case ERROR_MESSAGE::redeclare_variable_storage:
            return "variable %s redeclared with conflicting storage class";
        case ERROR_MESSAGE::initialized_extern_variable:
            return "illegal initializer, can only declare variable %s with " + em("extern") + " storage class";
        case ERROR_MESSAGE::structure_duplicate_member:
            return "structure type %s declared with duplicate member name %s";
        case ERROR_MESSAGE::structure_has_incomplete_member:
            return "structure type %s declared with member %s with incomplete type %s";
        case ERROR_MESSAGE::redeclare_structure_in_scope:
            return "structure type %s already declared in this scope";
        case ERROR_MESSAGE::break_outside_of_loop:
            return "found " + em("break") + " statement outside of loop";
        case ERROR_MESSAGE::continue_outside_of_loop:
            return "found " + em("continue") + " statement outside of loop";
        case ERROR_MESSAGE::goto_without_target_label:
            return "found " + em("goto") + " statement, but target label %s not defined in function %s";
        case ERROR_MESSAGE::structure_not_defined_in_scope:
            return "structure type %s not defined in this scope";
        case ERROR_MESSAGE::variable_not_declared_in_scope:
            return "variable %s not declared in this scope";
        case ERROR_MESSAGE::function_not_declared_in_scope:
            return "function %s not declared in this scope";
        case ERROR_MESSAGE::non_auto_variable_for_initial:
            return "variable %s declared with %s storage class in " + em("for") + " loop initial declaration";
        case ERROR_MESSAGE::redeclare_label_in_scope:
            return "label %s already defined in this scope";
        case ERROR_MESSAGE::redeclare_variable_in_scope:
            return "variable %s already declared in this scope";
        case ERROR_MESSAGE::function_defined_nested:
            return "function %s defined inside another function, but nested function definition are not "
                   "permitted";
        case ERROR_MESSAGE::static_function_declared_nested:
            return "cannot declare nested function %s in another function with " + em("static") + " storage class";
        case ERROR_MESSAGE::redeclare_function_in_scope:
            return "function %s already declared in this scope";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

std::string get_error_message(ERROR_MESSAGE message) {
    std::string code = std::to_string(static_cast<int>(message));
    std::string what = get_what_message(message);
    return "(no. " + code + ") " + what;
}
