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
            return "const char8";
        case TOKEN_KIND::float_constant:
            return "const float64";
        case TOKEN_KIND::unsigned_long_constant:
            return "const uint64";
        case TOKEN_KIND::unsigned_constant:
            return "const uint32";
        case TOKEN_KIND::long_constant:
            return "const int64";
        case TOKEN_KIND::constant:
            return "const int32";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

std::string get_name_hr(const TIdentifier& name) { return name.substr(0, name.find('.')); }

std::string get_type_hr(Type* type);

static std::string get_pointer_type_hr(Pointer* ptr_type) {
    std::string type_hr = get_type_hr(ptr_type->ref_type.get());
    return type_hr + "*";
}

static std::string get_array_type_hr(Array* arr_type) {
    std::string type_hr = get_type_hr(arr_type->elem_type.get());
    return type_hr + "[" + std::to_string(arr_type->size) + "]";
}

static std::string get_structure_type_hr(Structure* struct_type) {
    std::string type_hr = get_name_hr(struct_type->tag);
    return "struct " + type_hr;
}

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
            return "function";
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

std::string get_error_message(ERROR_MESSAGE message) {
    switch (message) {
        case ERROR_MESSAGE::no_option_code:
            return "no option code";
        case ERROR_MESSAGE::invalid_option_code:
            return "invalid option code %s";
        case ERROR_MESSAGE::no_input_files:
            return "no input files";
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
        case ERROR_MESSAGE::invalid_token:
            return "invalid token %s";
        case ERROR_MESSAGE::invalid_next_token:
            return "expected %s but found %s instead";
        case ERROR_MESSAGE::reach_end_of_input:
            return "expected declaration or statement at end of input";
        case ERROR_MESSAGE::out_of_bound_constant:
            return "constant %s too large to be represented as a long";
        case ERROR_MESSAGE::out_of_bound_unsigned:
            return "constant %s too large to be represented as an unsigned long";
        case ERROR_MESSAGE::invalid_arr_size_type:
            return "size of array must have constant positive integer type";
        case ERROR_MESSAGE::invalid_unary_op:
            return "expected " + em(get_token_kind_hr(TOKEN_KIND::unop_complement)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::unop_negation)) + " or "
                   + em(get_token_kind_hr(TOKEN_KIND::unop_not)) + " but found %s instead";
        case ERROR_MESSAGE::invalid_binary_op:
            return "expected " + em(get_token_kind_hr(TOKEN_KIND::binop_addition)) + ", "
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
                   + em(get_token_kind_hr(TOKEN_KIND::binop_greaterthanorequal)) + " but found %s instead";
        case ERROR_MESSAGE::invalid_abstract_declarator:
            return "expected " + em(get_token_kind_hr(TOKEN_KIND::binop_multiplication)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::parenthesis_open)) + " or "
                   + em(get_token_kind_hr(TOKEN_KIND::brackets_open)) + " but found %s instead";
        case ERROR_MESSAGE::invalid_pointer_unary_factor:
            return "expected " + em(get_token_kind_hr(TOKEN_KIND::binop_multiplication)) + " or "
                   + em(get_token_kind_hr(TOKEN_KIND::binop_bitand)) + " but found %s instead";
        case ERROR_MESSAGE::invalid_primary_exp_factor:
            return "expected " + em(get_token_kind_hr(TOKEN_KIND::constant)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::long_constant)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::char_constant)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::float_constant)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::unsigned_constant)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::unsigned_long_constant)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::identifier)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::identifier) + get_token_kind_hr(TOKEN_KIND::parenthesis_open))
                   + ", " + em(get_token_kind_hr(TOKEN_KIND::string_literal)) + " or "
                   + em(get_token_kind_hr(TOKEN_KIND::parenthesis_open)) + " but found %s instead";
        case ERROR_MESSAGE::invalid_exp:
            return "expected " + em(get_token_kind_hr(TOKEN_KIND::binop_addition)) + ", "
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
                   + em(get_token_kind_hr(TOKEN_KIND::ternary_if)) + " but found %s instead";
        case ERROR_MESSAGE::invalid_for_loop_decl_type:
            return "declaration of non-variable %s in for loop initial declaration";
        case ERROR_MESSAGE::invalid_type_specifier:
            return "expected " + em(get_token_kind_hr(TOKEN_KIND::identifier)) + ", "
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
                   + em(get_token_kind_hr(TOKEN_KIND::brackets_open)) + " but found %s instead";
        case ERROR_MESSAGE::invalid_type_specifier_list:
            return "expected valid list of unique type specifiers but found %s instead";
        case ERROR_MESSAGE::invalid_storage_class:
            return "expected " + em(get_token_kind_hr(TOKEN_KIND::key_static)) + " or "
                   + em(get_token_kind_hr(TOKEN_KIND::key_extern)) + " but found %s instead";
        case ERROR_MESSAGE::empty_compound_initializer:
            return "compound initializer must have at least one element";
        case ERROR_MESSAGE::many_fun_type_derivation:
            return "too many type derivations for function type";
        case ERROR_MESSAGE::fun_ptr_param_derivation:
            return "function pointer type parameter %s in type derivations";
        case ERROR_MESSAGE::invalid_simple_declarator:
            return "expected " + em(get_token_kind_hr(TOKEN_KIND::identifier)) + " or "
                   + em(get_token_kind_hr(TOKEN_KIND::parenthesis_open)) + " but found %s instead";
        case ERROR_MESSAGE::invalid_param_list:
            return "expected " + em(get_token_kind_hr(TOKEN_KIND::key_void)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::key_char)) + ", " + em(get_token_kind_hr(TOKEN_KIND::key_int))
                   + ", " + em(get_token_kind_hr(TOKEN_KIND::key_long)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::key_double)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::key_unsigned)) + ", "
                   + em(get_token_kind_hr(TOKEN_KIND::key_signed)) + " or "
                   + em(get_token_kind_hr(TOKEN_KIND::key_struct)) + " but found %s instead";
        case ERROR_MESSAGE::invalid_member_decl_storage:
            return "field %s declared with non-automatic storage";
        case ERROR_MESSAGE::invalid_member_decl_fun_type:
            return "field %s declared as a function";
        case ERROR_MESSAGE::joint_pointer_type_mismatch:
            return "###4 pointer type mismatch %s and %s in expression";
        case ERROR_MESSAGE::function_used_as_variable:
            return "###5 function %s used as a variable";
        case ERROR_MESSAGE::cannot_convert_from_to:
            return "###6 cannot convert from type %s to %s";
        case ERROR_MESSAGE::cannot_apply_unop_on_type:
            return "###11 cannot apply unary %s on type %s";
        case ERROR_MESSAGE::cannot_apply_binop_on_type:
            return "###13 cannot apply binary %s on type %s";
        case ERROR_MESSAGE::cannot_apply_binop_on_types:
            return "###12 cannot apply binary %s on types %s and %s";
        case ERROR_MESSAGE::wrong_lhs_assignment_type:
            return "###28 cannot assign to lhs-operand type %s";
        case ERROR_MESSAGE::invalid_lvalue_lhs_assignment:
            return "###29 assignment requires lvalue lhs-operand";
        case ERROR_MESSAGE::wrong_cond_type_conditional:
            return "###32 cannot have type %s condition in conditional";
        case ERROR_MESSAGE::conditional_type_mismatch:
            return "###33 type mismatch %s and %s in conditional";
        case ERROR_MESSAGE::variable_used_as_function:
            return "###36 variable %s used as a function";
        case ERROR_MESSAGE::wrong_number_of_arguments:
            return "###37 function %s called with %s arguments instead of %s";
        case ERROR_MESSAGE::cannot_dereference_non_pointer:
            return "###38 cannot dereference non-pointer type %s";
        case ERROR_MESSAGE::invalid_lvalue_address_of:
            return "###39 address-of requires lvalue operand";
        case ERROR_MESSAGE::invalid_array_subscript_types:
            return "###40 cannot subscript array with types %s and %s";
        case ERROR_MESSAGE::size_of_incomplete_type:
            return "###41 cannot get size of incomplete type %s";
        case ERROR_MESSAGE::access_member_non_struct:
            return "###43 cannot access member %s on non-structure type %s";
        case ERROR_MESSAGE::struct_has_no_member_named:
            return "###44 structure type %s has no member named %s";
        case ERROR_MESSAGE::access_member_non_pointer:
            return "###45 cannot access member %s on non-pointer-to-structure type %s";
        case ERROR_MESSAGE::access_member_incomplete_type:
            return "###47 cannot access member on incomplete structure type %s";
        case ERROR_MESSAGE::incomplete_struct_type:
            return "###49 incomplete structure type %s in expression";
        case ERROR_MESSAGE::return_value_in_void_function:
            return "###50 " + em("return") + " value in function %s returning type " + em("void");
        case ERROR_MESSAGE::no_return_value_in_function:
            return "###51 " + em("return") + " with no value in function %s returning type %s";
        case ERROR_MESSAGE::cannot_use_if_with_type:
            return "###52 cannot use " + em("if") + " statement with type %s";
        case ERROR_MESSAGE::cannot_use_while_with_type:
            return "###53 cannot use " + em("while") + " loop statement with type %s";
        case ERROR_MESSAGE::cannot_use_do_while_with_type:
            return "###54 cannot use " + em("do while") + " loop statement with type %s";
        case ERROR_MESSAGE::cannot_use_for_with_type:
            return "###55 cannot use " + em("for") + " loop statement with type %s";
        case ERROR_MESSAGE::non_char_array_from_string:
            return "###56 array type %s initialized from string literal";
        case ERROR_MESSAGE::wrong_string_literal_size:
            return "###57 size %s string literal initialized with %s characters";
        case ERROR_MESSAGE::wrong_array_initializer_size:
            return "###58 size %s array type %s initialized with %s initializers";
        case ERROR_MESSAGE::wrong_struct_members_number:
            return "###59 structure type %s initialized with %s members instead of %s";
        default:
            RAISE_INTERNAL_ERROR;
    }
}
