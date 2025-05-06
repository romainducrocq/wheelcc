#include <string>

#include "util/throw.hpp"

#include "ast/ast.hpp"
#include "ast/front_ast.hpp"
#include "ast/front_symt.hpp"

#include "frontend/parser/errors.hpp"
#include "parser/tokens.hpp" // frontend

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Errors

std::string get_tok_kind_fmt(TOKEN_KIND token_kind) {
    switch (token_kind) {
        case TOK_assign_shiftleft:
            return "<<=";
        case TOK_assign_shiftright:
            return ">>=";
        case TOK_unop_incr:
            return "++";
        case TOK_unop_decr:
            return "--";
        case TOK_binop_shiftleft:
            return "<<";
        case TOK_binop_shiftright:
            return ">>";
        case TOK_binop_and:
            return "&&";
        case TOK_binop_or:
            return "||";
        case TOK_binop_eq:
            return "==";
        case TOK_binop_ne:
            return "!=";
        case TOK_binop_le:
            return "<=";
        case TOK_binop_ge:
            return ">=";
        case TOK_assign_add:
            return "+=";
        case TOK_assign_subtract:
            return "-=";
        case TOK_assign_multiply:
            return "*=";
        case TOK_assign_divide:
            return "/=";
        case TOK_assign_remainder:
            return "%=";
        case TOK_assign_bitand:
            return "&=";
        case TOK_assign_bitor:
            return "|=";
        case TOK_assign_xor:
            return "^=";
        case TOK_structop_ptr:
            return "->";
        case TOK_open_paren:
            return "(";
        case TOK_close_paren:
            return ")";
        case TOK_open_brace:
            return "{";
        case TOK_close_brace:
            return "}";
        case TOK_open_bracket:
            return "[";
        case TOK_close_bracket:
            return "]";
        case TOK_semicolon:
            return ";";
        case TOK_unop_complement:
            return "~";
        case TOK_unop_neg:
            return "-";
        case TOK_unop_not:
            return "!";
        case TOK_binop_add:
            return "+";
        case TOK_binop_multiply:
            return "*";
        case TOK_binop_divide:
            return "/";
        case TOK_binop_remainder:
            return "%";
        case TOK_binop_bitand:
            return "&";
        case TOK_binop_bitor:
            return "|";
        case TOK_binop_xor:
            return "^";
        case TOK_binop_lt:
            return "<";
        case TOK_binop_gt:
            return ">";
        case TOK_assign:
            return "=";
        case TOK_ternary_if:
            return "?";
        case TOK_ternary_else:
            return ":";
        case TOK_comma_separator:
            return ",";
        case TOK_structop_member:
            return ".";
        case TOK_key_char:
            return "char";
        case TOK_key_int:
            return "int";
        case TOK_key_long:
            return "long";
        case TOK_key_double:
            return "double";
        case TOK_key_signed:
            return "signed";
        case TOK_key_unsigned:
            return "unsigned";
        case TOK_key_void:
            return "void";
        case TOK_key_struct:
            return "struct";
        case TOK_key_union:
            return "union";
        case TOK_key_sizeof:
            return "sizeof";
        case TOK_key_return:
            return "return";
        case TOK_key_if:
            return "if";
        case TOK_key_else:
            return "else";
        case TOK_key_goto:
            return "goto";
        case TOK_key_do:
            return "do";
        case TOK_key_while:
            return "while";
        case TOK_key_for:
            return "for";
        case TOK_key_switch:
            return "switch";
        case TOK_key_case:
            return "case";
        case TOK_key_default:
            return "default";
        case TOK_key_break:
            return "break";
        case TOK_key_continue:
            return "continue";
        case TOK_key_static:
            return "static";
        case TOK_key_extern:
            return "extern";
        case TOK_identifier:
            return "identifier";
        case TOK_string_literal:
            return "string literal";
        case TOK_char_const:
            return "const char";
        case TOK_int_const:
            return "const int";
        case TOK_long_const:
            return "const long";
        case TOK_uint_const:
            return "const unsigned int";
        case TOK_ulong_const:
            return "const unsigned long";
        case TOK_dbl_const:
            return "const double";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

std::string get_name_fmt(TIdentifier name) {
    return identifiers->hash_table[name].substr(0, identifiers->hash_table[name].find(UID_SEPARATOR[0]));
}

std::string get_struct_name_fmt(TIdentifier name, bool is_union) {
    std::string type_hr = is_union ? "union " : "struct ";
    type_hr += get_name_fmt(name);
    return type_hr;
}

static std::string get_fun_fmt(FunType* fun_type) {
    std::string type_hr = "(";
    type_hr += get_type_fmt(fun_type->ret_type.get());
    type_hr += ")(";
    for (const auto& param_type : fun_type->param_types) {
        type_hr += get_type_fmt(param_type.get());
        type_hr += ", ";
    }
    if (!fun_type->param_types.empty()) {
        type_hr.pop_back();
        type_hr.pop_back();
    }
    type_hr += ")";
    return type_hr;
}

static std::string get_ptr_fmt(Pointer* ptr_type) {
    std::string decl_type_hr = "*";
    while (ptr_type->ref_type->type() == AST_Pointer_t) {
        ptr_type = static_cast<Pointer*>(ptr_type->ref_type.get());
        decl_type_hr += "*";
    }
    std::string type_hr = get_type_fmt(ptr_type->ref_type.get());
    type_hr += decl_type_hr;
    return type_hr;
}

static std::string get_arr_fmt(Array* arr_type) {
    std::string decl_type_hr = "[";
    decl_type_hr += std::to_string(arr_type->size);
    decl_type_hr += "]";
    while (arr_type->elem_type->type() == AST_Array_t) {
        arr_type = static_cast<Array*>(arr_type->elem_type.get());
        decl_type_hr += "[";
        decl_type_hr += std::to_string(arr_type->size);
        decl_type_hr += "]";
    }
    std::string type_hr = get_type_fmt(arr_type->elem_type.get());
    type_hr += decl_type_hr;
    return type_hr;
}

static std::string get_struct_fmt(Structure* struct_type) {
    return get_struct_name_fmt(struct_type->tag, struct_type->is_union);
}

std::string get_type_fmt(Type* type) {
    switch (type->type()) {
        case AST_Char_t:
            return "char";
        case AST_SChar_t:
            return "signed char";
        case AST_UChar_t:
            return "unsigned char";
        case AST_Int_t:
            return "int";
        case AST_Long_t:
            return "long";
        case AST_UInt_t:
            return "unsigned int";
        case AST_ULong_t:
            return "unsigned long";
        case AST_Double_t:
            return "double";
        case AST_Void_t:
            return "void";
        case AST_FunType_t:
            return get_fun_fmt(static_cast<FunType*>(type));
        case AST_Pointer_t:
            return get_ptr_fmt(static_cast<Pointer*>(type));
        case AST_Array_t:
            return get_arr_fmt(static_cast<Array*>(type));
        case AST_Structure_t:
            return get_struct_fmt(static_cast<Structure*>(type));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

std::string get_const_fmt(CConst* node) {
    switch (node->type()) {
        case AST_CConstInt_t:
            return "int";
        case AST_CConstLong_t:
            return "long";
        case AST_CConstUInt_t:
            return "unsigned int";
        case AST_CConstULong_t:
            return "unsigned long";
        case AST_CConstDouble_t:
            return "double";
        case AST_CConstChar_t:
            return "char";
        case AST_CConstUChar_t:
            return "unsigned char";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

std::string get_storage_class_fmt(CStorageClass* node) {
    switch (node->type()) {
        case AST_CStatic_t:
            return "static";
        case AST_CExtern_t:
            return "extern";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

std::string get_unop_fmt(CUnaryOp* node) {
    switch (node->type()) {
        case AST_CComplement_t:
            return "~";
        case AST_CNegate_t:
            return "-";
        case AST_CNot_t:
            return "!";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

std::string get_binop_fmt(CBinaryOp* node) {
    switch (node->type()) {
        case AST_CAdd_t:
            return "+";
        case AST_CSubtract_t:
            return "-";
        case AST_CMultiply_t:
            return "*";
        case AST_CDivide_t:
            return "/";
        case AST_CRemainder_t:
            return "%";
        case AST_CBitAnd_t:
            return "&";
        case AST_CBitOr_t:
            return "|";
        case AST_CBitXor_t:
            return "^";
        case AST_CBitShiftLeft_t:
            return "<<";
        case AST_CBitShiftRight_t:
            return ">>";
        case AST_CBitShrArithmetic_t:
            return ">>";
        case AST_CAnd_t:
            return "&&";
        case AST_COr_t:
            return "||";
        case AST_CEqual_t:
            return "==";
        case AST_CNotEqual_t:
            return "!=";
        case AST_CLessThan_t:
            return "<";
        case AST_CLessOrEqual_t:
            return "<=";
        case AST_CGreaterThan_t:
            return ">";
        case AST_CGreaterOrEqual_t:
            return ">=";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

std::string get_assign_fmt(CBinaryOp* node, CUnaryOp* unary_op) {
    if (!node) {
        return "=";
    }
    else if (unary_op) {
        switch (unary_op->type()) {
            case AST_CPrefix_t: {
                switch (node->type()) {
                    case AST_CAdd_t:
                        return "prefix ++";
                    case AST_CSubtract_t:
                        return "prefix --";
                    default:
                        RAISE_INTERNAL_ERROR;
                }
            }
            case AST_CPostfix_t: {
                switch (node->type()) {
                    case AST_CAdd_t:
                        return "postfix ++";
                    case AST_CSubtract_t:
                        return "postfix --";
                    default:
                        RAISE_INTERNAL_ERROR;
                }
            }
            default:
                RAISE_INTERNAL_ERROR;
        }
    }
    else {
        switch (node->type()) {
            case AST_CAdd_t:
                return "+=";
            case AST_CSubtract_t:
                return "-=";
            case AST_CMultiply_t:
                return "*=";
            case AST_CDivide_t:
                return "/=";
            case AST_CRemainder_t:
                return "%=";
            case AST_CBitAnd_t:
                return "&=";
            case AST_CBitOr_t:
                return "|=";
            case AST_CBitXor_t:
                return "^=";
            case AST_CBitShiftLeft_t:
                return "<<=";
            case AST_CBitShiftRight_t:
                return ">>=";
            case AST_CBitShrArithmetic_t:
                return ">>=";
            default:
                RAISE_INTERNAL_ERROR;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define EM_VARG "\033[1m‘%s’\033[0m"
#define RETURN_ERRNO return "(no. %i) "

const char* get_fatal_msg(MESSAGE_FATAL message) {
    switch (message) {
        case MSG_unsupported_os:
            RETURN_ERRNO EM_VARG " operating system is not supported, requires \033[1m‘GNU/Linux’\033[0m (x86_64)";
        case MSG_unsupported_arch:
            RETURN_ERRNO EM_VARG " architecture is not supported, requires \033[1m‘x86_64’\033[0m";
        case MSG_unsupported_compiler:
            RETURN_ERRNO EM_VARG " compiler is not supported, requires \033[1m‘gcc’\033[0m >= 8.1.0";
        case MSG_unsupported_gcc_ver:
            RETURN_ERRNO "\033[1m‘gcc’\033[0m %i.%i.%i is not supported, requires \033[1m‘gcc’\033[0m >= 8.1.0";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

const char* get_arg_msg(MESSAGE_ARG message) {
    switch (message) {
        case MSG_no_debug_arg:
            RETURN_ERRNO "no debug code passed in first argument";
        case MSG_invalid_debug_arg:
            RETURN_ERRNO "invalid debug code " EM_VARG " passed in first argument";
        case MSG_no_optim_1_arg:
            RETURN_ERRNO "no level 1 optimization mask passed in second argument";
        case MSG_invalid_optim_1_arg:
            RETURN_ERRNO "invalid level 1 optimization mask " EM_VARG " passed in second argument";
        case MSG_no_optim_2_arg:
            RETURN_ERRNO "no level 2 optimization code passed in third argument";
        case MSG_invalid_optim_2_arg:
            RETURN_ERRNO "invalid level 2 optimization code " EM_VARG " passed in third argument";
        case MSG_no_input_files_arg:
            RETURN_ERRNO "no input file passed in fourth argument";
        case MSG_no_include_dir_arg:
            RETURN_ERRNO "no include directories passed in fifth argument";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

const char* get_util_msg(MESSAGE_UTIL message) {
    switch (message) {
        case MSG_failed_fread:
            RETURN_ERRNO "cannot read input file " EM_VARG;
        case MSG_failed_fwrite:
            RETURN_ERRNO "cannot write output file " EM_VARG;
        case MSG_failed_strtoi:
            RETURN_ERRNO "cannot interpret string " EM_VARG " to an integer value";
        case MSG_failed_strtou:
            RETURN_ERRNO "cannot interpret string " EM_VARG " to an unsigned integer value";
        case MSG_failed_strtod:
            RETURN_ERRNO "cannot interpret string " EM_VARG " to a floating point value";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

const char* get_lexer_msg(MESSAGE_LEXER message) {
    switch (message) {
        case MSG_invalid_tok:
            RETURN_ERRNO "found invalid token " EM_VARG;
        case MSG_failed_include:
            RETURN_ERRNO "cannot find " EM_VARG " header file in \033[1m‘include’\033[0m directive search";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

const char* get_parser_msg(MESSAGE_PARSER message) {
    switch (message) {
        case MSG_unexpected_next_tok:
            RETURN_ERRNO "found token " EM_VARG ", but expected " EM_VARG " next";
        case MSG_reached_eof:
            RETURN_ERRNO "reached end of file, but expected declaration or statement next";
        case MSG_overflow_long_const:
            RETURN_ERRNO "cannot represent " EM_VARG " as a 64 bits signed integer constant, very large number";
        case MSG_overflow_ulong_const:
            RETURN_ERRNO "cannot represent " EM_VARG " as a 64 bits unsigned integer constant, very large number";
        case MSG_arr_size_not_int_const:
            RETURN_ERRNO "illegal array size " EM_VARG ", requires a constant integer";
        case MSG_case_value_not_int_const:
            RETURN_ERRNO "illegal \033[1m‘case’\033[0m value " EM_VARG ", requires a constant integer";
        case MSG_expect_unop:
            RETURN_ERRNO "found token " EM_VARG
                         ", but expected \033[1m‘~’\033[0m, \033[1m‘-’\033[0m or \033[1m‘!’\033[0m next";
        case MSG_expect_binop:
            RETURN_ERRNO
            "found token " EM_VARG
            ", but expected \033[1m‘+’\033[0m, \033[1m‘+=’\033[0m, \033[1m‘++’\033[0m, \033[1m‘-’\033[0m, "
            "\033[1m‘-=’\033[0m, \033[1m‘--’\033[0m, \033[1m‘*’\033[0m, \033[1m‘*=’\033[0m, \033[1m‘/’\033[0m, "
            "\033[1m‘/=’\033[0m, \033[1m‘%’\033[0m, \033[1m‘%=’\033[0m, \033[1m‘&’\033[0m, \033[1m‘&=’\033[0m, "
            "\033[1m‘|’\033[0m, \033[1m‘|=’\033[0m, \033[1m‘^’\033[0m, \033[1m‘^=’\033[0m, \033[1m‘<<’\033[0m, "
            "\033[1m‘<<=’\033[0m, \033[1m‘>>’\033[0m, \033[1m‘>>=’\033[0m, \033[1m‘&&’\033[0m, \033[1m‘||’\033[0m, "
            "\033[1m‘==’\033[0m, \033[1m‘!=’\033[0m, \033[1m‘<’\033[0m, \033[1m‘<=’\033[0m, \033[1m‘>’\033[0m or "
            "\033[1m‘>=’\033[0m next";
        case MSG_expect_abstract_decltor:
            RETURN_ERRNO "found token " EM_VARG
                         ", but expected \033[1m‘*’\033[0m, \033[1m‘(’\033[0m or \033[1m‘[’\033[0m next";
        case MSG_expect_ptr_unary_factor:
            RETURN_ERRNO "found token " EM_VARG ", but expected \033[1m‘*’\033[0m or \033[1m‘&’\033[0m next";
        case MSG_expect_primary_exp_factor:
            RETURN_ERRNO "found token " EM_VARG
                         ", but expected \033[1m‘const int’\033[0m, \033[1m‘const long’\033[0m, \033[1m‘const "
                         "char’\033[0m, \033[1m‘const double’\033[0m, \033[1m‘const unsigned int’\033[0m, "
                         "\033[1m‘const unsigned long’\033[0m, \033[1m‘identifier’\033[0m, "
                         "\033[1m‘identifier(’\033[0m, \033[1m‘string literal’\033[0m or \033[1m‘(’\033[0m next";
        case MSG_expect_exp:
            RETURN_ERRNO
            "found token " EM_VARG
            ", but expected \033[1m‘+’\033[0m, \033[1m‘-’\033[0m, \033[1m‘*’\033[0m, \033[1m‘/’\033[0m, "
            "\033[1m‘%’\033[0m, \033[1m‘&’\033[0m, \033[1m‘|’\033[0m, \033[1m‘^’\033[0m, \033[1m‘<<’\033[0m, "
            "\033[1m‘>>’\033[0m, \033[1m‘<’\033[0m, \033[1m‘<=’\033[0m, \033[1m‘>’\033[0m, \033[1m‘>=’\033[0m, "
            "\033[1m‘==’\033[0m, \033[1m‘!=’\033[0m, \033[1m‘&&’\033[0m, \033[1m‘||’\033[0m, \033[1m‘=’\033[0m, "
            "\033[1m‘+=’\033[0m, \033[1m‘-=’\033[0m, \033[1m‘*=’\033[0m, \033[1m‘/=’\033[0m, \033[1m‘%=’\033[0m, "
            "\033[1m‘&=’\033[0m, \033[1m‘|=’\033[0m, \033[1m‘^=’\033[0m, \033[1m‘<<=’\033[0m, \033[1m‘>>=’\033[0m "
            "or \033[1m‘?’\033[0m next";
        case MSG_for_init_decl_as_fun:
            RETURN_ERRNO "function " EM_VARG " declared in \033[1m‘for’\033[0m loop initial declaration";
        case MSG_expect_specifier:
            RETURN_ERRNO "found token " EM_VARG
                         ", but expected \033[1m‘identifier’\033[0m, \033[1m‘)’\033[0m, \033[1m‘char’\033[0m, "
                         "\033[1m‘int’\033[0m, \033[1m‘long’\033[0m, \033[1m‘double’\033[0m, \033[1m‘unsigned’\033[0m, "
                         "\033[1m‘signed’\033[0m, \033[1m‘void’\033[0m, \033[1m‘struct’\033[0m, \033[1m‘union’\033[0m, "
                         "\033[1m‘static’\033[0m, \033[1m‘extern’\033[0m, \033[1m‘*’\033[0m, \033[1m‘(’\033[0m or "
                         "\033[1m‘[’\033[0m next";
        case MSG_expect_specifier_list:
            RETURN_ERRNO "found tokens " EM_VARG ", but expected valid list of unique type specifiers next";
        case MSG_expect_storage_class:
            RETURN_ERRNO "found token " EM_VARG ", but expected \033[1m‘static’\033[0m or \033[1m‘extern’\033[0m next";
        case MSG_empty_compound_init:
            RETURN_ERRNO "empty compound initializer requires at least one initializer";
        case MSG_derived_fun_decl:
            RETURN_ERRNO "cannot apply further type derivation to function declaration";
        case MSG_expect_simple_decltor:
            RETURN_ERRNO "found token " EM_VARG ", but expected \033[1m‘identifier’\033[0m or \033[1m‘(’\033[0m next";
        case MSG_expect_param_list:
            RETURN_ERRNO "found token " EM_VARG
                         ", but expected \033[1m‘void’\033[0m, \033[1m‘char’\033[0m, \033[1m‘int’\033[0m, "
                         "\033[1m‘long’\033[0m, \033[1m‘double’\033[0m, \033[1m‘unsigned’\033[0m, "
                         "\033[1m‘signed’\033[0m, \033[1m‘struct’\033[0m or \033[1m‘union’\033[0m next";
        case MSG_non_auto_member_decl:
            RETURN_ERRNO "data structure type declared with member " EM_VARG " with " EM_VARG " storage class";
        case MSG_member_decl_as_fun:
            RETURN_ERRNO "data structure type declared with member " EM_VARG " as a function";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

const char* get_semantic_msg(MESSAGE_SEMANTIC message) {
    switch (message) {
        case MSG_array_of_incomplete_type:
            RETURN_ERRNO "array type " EM_VARG " of incomplete type " EM_VARG ", requires a complete type";
        case MSG_joint_pointer_type_mismatch:
            RETURN_ERRNO "pointer type mismatch " EM_VARG " and " EM_VARG " in operator";
        case MSG_function_used_as_variable:
            RETURN_ERRNO "function " EM_VARG " used as a variable";
        case MSG_illegal_conversion_from_type_to:
            RETURN_ERRNO "illegal cast, cannot convert expression from type " EM_VARG " to " EM_VARG;
        case MSG_unary_on_invalid_operand_type:
            RETURN_ERRNO "cannot apply unary operator " EM_VARG " on operand type " EM_VARG;
        case MSG_binary_on_invalid_operand_type:
            RETURN_ERRNO "cannot apply binary operator " EM_VARG " on operand type " EM_VARG;
        case MSG_binary_on_invalid_operand_types:
            RETURN_ERRNO "cannot apply binary operator " EM_VARG " on operand types " EM_VARG " and " EM_VARG;
        case MSG_assignment_to_void_type:
            RETURN_ERRNO "cannot assign \033[1m‘=’\033[0m to left operand type \033[1m‘void’\033[0m";
        case MSG_assignment_to_rvalue:
            RETURN_ERRNO "assignment " EM_VARG " requires lvalue left operand, but got rvalue";
        case MSG_conditional_on_invalid_condition_type:
            RETURN_ERRNO "cannot apply conditional \033[1m‘?’\033[0m on condition operand type " EM_VARG;
        case MSG_ternary_on_invalid_operand_types:
            RETURN_ERRNO "cannot apply ternary operator \033[1m‘:’\033[0m on operand types " EM_VARG " and " EM_VARG;
        case MSG_variable_used_as_function:
            RETURN_ERRNO "variable " EM_VARG " used as a function";
        case MSG_function_called_with_wrong_number_of_arguments:
            RETURN_ERRNO "function " EM_VARG " called with " EM_VARG " arguments instead of " EM_VARG;
        case MSG_dereference_non_pointer:
            RETURN_ERRNO "cannot apply dereference operator \033[1m‘*’\033[0m on non-pointer type " EM_VARG;
        case MSG_address_of_rvalue:
            RETURN_ERRNO "addresssing \033[1m‘&’\033[0m requires lvalue operand, but got rvalue";
        case MSG_subscript_array_with_invalid_types:
            RETURN_ERRNO "cannot subscript array with operand types " EM_VARG " and " EM_VARG
                         ", requires a complete pointer and an integer types";
        case MSG_get_size_of_incomplete_type:
            RETURN_ERRNO "cannot get size with \033[1m‘sizeof’\033[0m operator on incomplete type " EM_VARG;
        case MSG_dot_on_non_structure_type:
            RETURN_ERRNO "cannot access data structure member " EM_VARG
                         " with dot operator \033[1m‘.’\033[0m on non-data structure type " EM_VARG;
        case MSG_member_not_in_structure_type:
            RETURN_ERRNO "data structure type " EM_VARG " has no member named " EM_VARG;
        case MSG_arrow_on_non_pointer_to_structure_type:
            RETURN_ERRNO "cannot access data structure member " EM_VARG
                         " with arrow operator \033[1m‘->’\033[0m on non-pointer-to-data structure type " EM_VARG;
        case MSG_arrow_on_incomplete_structure_type:
            RETURN_ERRNO "cannot access data structure member " EM_VARG
                         " with arrow operator \033[1m‘->’\033[0m on incomplete data structure type " EM_VARG;
        case MSG_incomplete_structure_type_in_expression:
            RETURN_ERRNO "incomplete data structure type " EM_VARG " in expression";
        case MSG_return_value_in_void_function:
            RETURN_ERRNO "found \033[1m‘return’\033[0m value in function " EM_VARG
                         " returning type \033[1m‘void’\033[0m";
        case MSG_no_return_value_in_non_void_function:
            RETURN_ERRNO "found \033[1m‘return’\033[0m with no value in function " EM_VARG " returning type " EM_VARG;
        case MSG_if_used_with_condition_type:
            RETURN_ERRNO "cannot use \033[1m‘if’\033[0m statement with condition expression type " EM_VARG;
        case MSG_while_used_with_condition_type:
            RETURN_ERRNO "cannot use \033[1m‘while’\033[0m loop statement with condition expression type " EM_VARG;
        case MSG_do_while_used_with_condition_type:
            RETURN_ERRNO "cannot use \033[1m‘do while’\033[0m loop statement with condition expression type " EM_VARG;
        case MSG_for_used_with_condition_type:
            RETURN_ERRNO "cannot use \033[1m‘for’\033[0m loop statement with condition expression type " EM_VARG;
        case MSG_switch_used_with_match_type:
            RETURN_ERRNO "cannot use \033[1m‘switch’\033[0m statement with match expression type " EM_VARG
                         ", requires an integer type";
        case MSG_duplicate_case_value_in_switch:
            RETURN_ERRNO "found duplicate \033[1m‘case’\033[0m value " EM_VARG " in \033[1m‘switch’\033[0m statement";
        case MSG_non_char_array_initialized_from_string:
            RETURN_ERRNO "non-character array type " EM_VARG " initialized from string literal";
        case MSG_string_initialized_with_too_many_characters:
            RETURN_ERRNO "size " EM_VARG " string literal initialized with " EM_VARG " characters";
        case MSG_array_initialized_with_too_many_initializers:
            RETURN_ERRNO "size " EM_VARG " array type " EM_VARG " initialized with " EM_VARG " initializers";
        case MSG_structure_initialized_with_too_many_members:
            RETURN_ERRNO "data structure type " EM_VARG " initialized with " EM_VARG " members instead of " EM_VARG;
        case MSG_function_returns_array:
            RETURN_ERRNO "function " EM_VARG " returns array type " EM_VARG ", instead of pointer type";
        case MSG_function_returns_incomplete_structure_type:
            RETURN_ERRNO "function " EM_VARG " returns incomplete data structure type " EM_VARG;
        case MSG_parameter_with_type_void:
            RETURN_ERRNO "function " EM_VARG " declared with parameter " EM_VARG " with type \033[1m‘void’\033[0m";
        case MSG_parameter_with_incomplete_structure_type:
            RETURN_ERRNO "function " EM_VARG " defined with parameter " EM_VARG
                         " with incomplete data structure type " EM_VARG;
        case MSG_function_redeclared_with_conflicting_type:
            RETURN_ERRNO "function " EM_VARG " redeclared with function type " EM_VARG
                         ", but previous declaration has function type " EM_VARG;
        case MSG_function_redefined:
            RETURN_ERRNO "function " EM_VARG " already defined with function type " EM_VARG;
        case MSG_non_static_function_redeclared_static:
            RETURN_ERRNO "function " EM_VARG " with \033[1m‘static’\033[0m storage class already declared non-static";
        case MSG_static_pointer_initialized_from_non_integer:
            RETURN_ERRNO "cannot statically initialize pointer type " EM_VARG " from constant " EM_VARG
                         ", requires a constant integer";
        case MSG_static_pointer_initialized_from_non_null:
            RETURN_ERRNO "cannot statically initialize pointer type " EM_VARG " from non-null value " EM_VARG;
        case MSG_aggregate_initialized_with_single_initializer:
            RETURN_ERRNO "aggregate type " EM_VARG " statically initialized with single initializer";
        case MSG_static_non_char_pointer_initialized_from_string:
            RETURN_ERRNO "non-character pointer type " EM_VARG " statically initialized from string literal";
        case MSG_static_initialized_with_non_constant:
            RETURN_ERRNO "cannot statically initialize variable from non-constant type " EM_VARG
                         ", requires a constant";
        case MSG_scalar_initialized_with_compound_initializer:
            RETURN_ERRNO "cannot initialize scalar type " EM_VARG " with compound initializer";
        case MSG_variable_declared_with_type_void:
            RETURN_ERRNO "variable " EM_VARG " declared with type \033[1m‘void’\033[0m";
        case MSG_variable_declared_with_incomplete_structure_type:
            RETURN_ERRNO "variable " EM_VARG " declared with incomplete data structure type " EM_VARG;
        case MSG_variable_redeclared_with_conflicting_type:
            RETURN_ERRNO "variable " EM_VARG " redeclared with conflicting type " EM_VARG
                         ", but previously declared with type " EM_VARG;
        case MSG_variable_redeclared_with_conflicting_storage:
            RETURN_ERRNO "variable " EM_VARG " redeclared with conflicting storage class";
        case MSG_extern_variable_defined:
            RETURN_ERRNO "illegal initializer, can only declare variable " EM_VARG
                         " with \033[1m‘extern’\033[0m storage class";
        case MSG_structure_declared_with_duplicate_member:
            RETURN_ERRNO "data structure type " EM_VARG " declared with duplicate member name " EM_VARG;
        case MSG_member_declared_with_incomplete_type:
            RETURN_ERRNO "data structure type " EM_VARG " declared with member " EM_VARG
                         " with incomplete type " EM_VARG;
        case MSG_structure_redeclared_in_scope:
            RETURN_ERRNO "data structure type " EM_VARG " already declared in this scope";
        case MSG_case_outside_of_switch:
            RETURN_ERRNO "found \033[1m‘case’\033[0m statement outside of \033[1m‘switch’\033[0m";
        case MSG_default_outside_of_switch:
            RETURN_ERRNO "found \033[1m‘default’\033[0m statement outside of \033[1m‘switch’\033[0m";
        case MSG_more_than_one_default_in_switch:
            RETURN_ERRNO "found more than one \033[1m‘default’\033[0m statement in \033[1m‘switch’\033[0m";
        case MSG_break_outside_of_loop:
            RETURN_ERRNO "found \033[1m‘break’\033[0m statement outside of loop";
        case MSG_continue_outside_of_loop:
            RETURN_ERRNO "found \033[1m‘continue’\033[0m statement outside of loop";
        case MSG_goto_with_undefined_target_label:
            RETURN_ERRNO "found \033[1m‘goto’\033[0m statement, but target label " EM_VARG
                         " not defined in function " EM_VARG;
        case MSG_structure_conflicts_with_previously_declared:
            RETURN_ERRNO EM_VARG " conflicts with data structure type " EM_VARG
                                 " previously declared or defined in this scope";
        case MSG_structure_not_defined_in_scope:
            RETURN_ERRNO "data structure type " EM_VARG " not defined in this scope";
        case MSG_variable_not_declared_in_scope:
            RETURN_ERRNO "variable " EM_VARG " not declared in this scope";
        case MSG_function_not_declared_in_scope:
            RETURN_ERRNO "function " EM_VARG " not declared in this scope";
        case MSG_for_initial_declared_with_non_automatic_storage:
            RETURN_ERRNO "variable " EM_VARG " declared with " EM_VARG
                         " storage class in \033[1m‘for’\033[0m loop initial declaration";
        case MSG_label_redefined_in_scope:
            RETURN_ERRNO "label " EM_VARG " already defined in this scope";
        case MSG_variable_redeclared_in_scope:
            RETURN_ERRNO "variable " EM_VARG " already declared in this scope";
        case MSG_nested_function_defined:
            RETURN_ERRNO "function " EM_VARG
                         " defined inside another function, but nested function definition are not permitted";
        case MSG_nested_static_function_declared:
            RETURN_ERRNO "cannot declare nested function " EM_VARG
                         " in another function with \033[1m‘static’\033[0m storage class";
        case MSG_function_redeclared_in_scope:
            RETURN_ERRNO "function " EM_VARG " already declared in this scope";
        default:
            RAISE_INTERNAL_ERROR;
    }
}
