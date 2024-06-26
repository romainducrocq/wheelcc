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
        default:
            RAISE_INTERNAL_ERROR;
    }
}
