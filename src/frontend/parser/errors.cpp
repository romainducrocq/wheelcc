#include <string>

#include "util/throw.hpp"

#include "ast/ast.hpp"
#include "ast/front_ast.hpp"
#include "ast/front_symt.hpp"

#include "frontend/parser/errors.hpp"
#include "tokens.hpp" // frontend

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Errors

std::string get_token_kind_hr(TOKEN_KIND token_kind) {
    switch (token_kind) {
        case TOKEN_KIND::assignment_bitshiftleft:
            return "<<=";
        case TOKEN_KIND::assignment_bitshiftright:
            return ">>=";
        case TOKEN_KIND::unop_increment:
            return "++";
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
        case TOKEN_KIND::key_union:
            return "union";
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
        case TOKEN_KIND::key_switch:
            return "switch";
        case TOKEN_KIND::key_case:
            return "case";
        case TOKEN_KIND::key_default:
            return "default";
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
        case TOKEN_KIND::constant:
            return "const int";
        case TOKEN_KIND::long_constant:
            return "const long";
        case TOKEN_KIND::unsigned_constant:
            return "const unsigned int";
        case TOKEN_KIND::unsigned_long_constant:
            return "const unsigned long";
        case TOKEN_KIND::float_constant:
            return "const double";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

std::string get_name_hr(TIdentifier name) {
    return identifiers->hash_table[name].substr(0, identifiers->hash_table[name].find('.'));
}

std::string get_struct_name_hr(TIdentifier name, bool is_union) {
    std::string type_hr = is_union ? "union " : "struct ";
    type_hr += get_name_hr(name);
    return type_hr;
}

static std::string get_function_type_hr(FunType* fun_type) {
    std::string type_hr = "(";
    type_hr += get_type_hr(fun_type->ret_type.get());
    type_hr += ")(";
    for (const auto& param_type : fun_type->param_types) {
        type_hr += get_type_hr(param_type.get());
        type_hr += ", ";
    }
    if (!fun_type->param_types.empty()) {
        type_hr.pop_back();
        type_hr.pop_back();
    }
    type_hr += ")";
    return type_hr;
}

static std::string get_pointer_type_hr(Pointer* ptr_type) {
    std::string decl_type_hr = "*";
    while (ptr_type->ref_type->type() == Pointer_t) {
        ptr_type = static_cast<Pointer*>(ptr_type->ref_type.get());
        decl_type_hr += "*";
    }
    std::string type_hr = get_type_hr(ptr_type->ref_type.get());
    type_hr += decl_type_hr;
    return type_hr;
}

static std::string get_array_type_hr(Array* arr_type) {
    std::string decl_type_hr = "[";
    decl_type_hr += std::to_string(arr_type->size);
    decl_type_hr += "]";
    while (arr_type->elem_type->type() == Array_t) {
        arr_type = static_cast<Array*>(arr_type->elem_type.get());
        decl_type_hr += "[";
        decl_type_hr += std::to_string(arr_type->size);
        decl_type_hr += "]";
    }
    std::string type_hr = get_type_hr(arr_type->elem_type.get());
    type_hr += decl_type_hr;
    return type_hr;
}

static std::string get_structure_type_hr(Structure* struct_type) {
    return get_struct_name_hr(struct_type->tag, struct_type->is_union);
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

std::string get_assignment_hr(CBinaryOp* node, CUnaryOp* unary_op) {
    if (!node) {
        return "=";
    }
    else if (unary_op) {
        switch (unary_op->type()) {
            case CPrefix_t: {
                switch (node->type()) {
                    case CAdd_t:
                        return "prefix ++";
                    case CSubtract_t:
                        return "prefix --";
                    default:
                        RAISE_INTERNAL_ERROR;
                }
            }
            case CPostfix_t: {
                switch (node->type()) {
                    case CAdd_t:
                        return "postfix ++";
                    case CSubtract_t:
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

const char* get_what_message(ERROR_MESSAGE_ARGUMENT message) {
    switch (message) {
        case ERROR_MESSAGE_ARGUMENT::no_debug_code_in_argument:
            return "no debug code passed in first argument";
        case ERROR_MESSAGE_ARGUMENT::invalid_debug_code_in_argument:
            return "invalid debug code " EM_VARG " passed in first argument";
        case ERROR_MESSAGE_ARGUMENT::no_optim_1_mask_in_argument:
            return "no level 1 optimization mask passed in second argument";
        case ERROR_MESSAGE_ARGUMENT::invalid_optim_1_mask_in_argument:
            return "invalid level 1 optimization mask " EM_VARG " passed in second argument";
        case ERROR_MESSAGE_ARGUMENT::no_optim_2_code_in_argument:
            return "no level 2 optimization code passed in third argument";
        case ERROR_MESSAGE_ARGUMENT::invalid_optim_2_code_in_argument:
            return "invalid level 2 optimization code " EM_VARG " passed in third argument";
        case ERROR_MESSAGE_ARGUMENT::no_input_files_in_argument:
            return "no input file passed in fourth argument";
        case ERROR_MESSAGE_ARGUMENT::no_include_directories_in_argument:
            return "no include directories passed in fifth argument";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

const char* get_what_message(ERROR_MESSAGE_UTIL message) {
    switch (message) {
        case ERROR_MESSAGE_UTIL::failed_to_read_input_file:
            return "cannot read input file " EM_VARG;
        case ERROR_MESSAGE_UTIL::failed_to_write_to_output_file:
            return "cannot write output file " EM_VARG;
        case ERROR_MESSAGE_UTIL::failed_to_interpret_string_to_integer:
            return "cannot interpret string " EM_VARG " to an integer value";
        case ERROR_MESSAGE_UTIL::failed_to_interpret_string_to_unsigned_integer:
            return "cannot interpret string " EM_VARG " to an unsigned integer value";
        case ERROR_MESSAGE_UTIL::failed_to_interpret_string_to_float:
            return "cannot interpret string " EM_VARG " to a floating point value";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

const char* get_what_message(ERROR_MESSAGE_LEXER message) {
    switch (message) {
        case ERROR_MESSAGE_LEXER::invalid_token:
            return "found invalid token " EM_VARG;
        case ERROR_MESSAGE_LEXER::failed_to_include_header_file:
            return "cannot find " EM_VARG " header file in \033[1m‘include’\033[0m directive search";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

const char* get_what_message(ERROR_MESSAGE_PARSER message) {
    switch (message) {
        case ERROR_MESSAGE_PARSER::unexpected_next_token:
            return "found token " EM_VARG ", but expected " EM_VARG " next";
        case ERROR_MESSAGE_PARSER::reached_end_of_file:
            return "reached end of file, but expected declaration or statement next";
        case ERROR_MESSAGE_PARSER::number_too_large_for_long_constant:
            return "cannot represent " EM_VARG " as a 64 bits signed integer constant, very large number";
        case ERROR_MESSAGE_PARSER::number_too_large_for_unsigned_long_constant:
            return "cannot represent " EM_VARG " as a 64 bits unsigned integer constant, very large number";
        case ERROR_MESSAGE_PARSER::array_size_not_a_constant_integer:
            return "illegal array size " EM_VARG ", requires a constant integer";
        case ERROR_MESSAGE_PARSER::case_value_not_a_constant_integer:
            return "illegal \033[1m‘case’\033[0m value " EM_VARG ", requires a constant integer";
        case ERROR_MESSAGE_PARSER::unexpected_unary_operator:
            return "found token " EM_VARG ", but expected \033[1m‘~’\033[0m, \033[1m‘-’\033[0m or "
                   "\033[1m‘!’\033[0m next";
        case ERROR_MESSAGE_PARSER::unexpected_binary_operator:
            return "found token " EM_VARG ", but expected \033[1m‘+’\033[0m, \033[1m‘+=’\033[0m, "
                   "\033[1m‘++’\033[0m, \033[1m‘-’\033[0m, \033[1m‘-=’\033[0m, \033[1m‘--’\033[0m, \033[1m‘*’\033[0m, "
                   "\033[1m‘*=’\033[0m, \033[1m‘/’\033[0m, \033[1m‘/=’\033[0m, \033[1m‘%’\033[0m, \033[1m‘%=’\033[0m, "
                   "\033[1m‘&’\033[0m, \033[1m‘&=’\033[0m, \033[1m‘|’\033[0m, \033[1m‘|=’\033[0m, \033[1m‘^’\033[0m, "
                   "\033[1m‘^=’\033[0m, \033[1m‘<<’\033[0m, \033[1m‘<<=’\033[0m, \033[1m‘>>’\033[0m, "
                   "\033[1m‘>>=’\033[0m, \033[1m‘&&’\033[0m, \033[1m‘||’\033[0m, \033[1m‘==’\033[0m, "
                   "\033[1m‘!=’\033[0m, \033[1m‘<’\033[0m, \033[1m‘<=’\033[0m, \033[1m‘>’\033[0m or \033[1m‘>=’\033[0m "
                   "next";
        case ERROR_MESSAGE_PARSER::unexpected_abstract_declarator:
            return "found token " EM_VARG ", but expected \033[1m‘*’\033[0m, \033[1m‘(’\033[0m or "
                   "\033[1m‘[’\033[0m next";
        case ERROR_MESSAGE_PARSER::unexpected_pointer_unary_factor:
            return "found token " EM_VARG ", but expected \033[1m‘*’\033[0m or \033[1m‘&’\033[0m next";
        case ERROR_MESSAGE_PARSER::unexpected_primary_expression_factor:
            return "found token " EM_VARG ", but expected \033[1m‘const int’\033[0m, \033[1m‘const "
                   "long’\033[0m, \033[1m‘const char’\033[0m, \033[1m‘const double’\033[0m, \033[1m‘const unsigned "
                   "int’\033[0m, \033[1m‘const unsigned long’\033[0m, \033[1m‘identifier’\033[0m, "
                   "\033[1m‘identifier(’\033[0m, \033[1m‘string literal’\033[0m or \033[1m‘(’\033[0m next";
        case ERROR_MESSAGE_PARSER::unexpected_expression:
            return "found token " EM_VARG ", but expected \033[1m‘+’\033[0m, \033[1m‘-’\033[0m, "
                   "\033[1m‘*’\033[0m, \033[1m‘/’\033[0m, \033[1m‘%’\033[0m, \033[1m‘&’\033[0m, \033[1m‘|’\033[0m, "
                   "\033[1m‘^’\033[0m, \033[1m‘<<’\033[0m, \033[1m‘>>’\033[0m, \033[1m‘<’\033[0m, \033[1m‘<=’\033[0m, "
                   "\033[1m‘>’\033[0m, \033[1m‘>=’\033[0m, \033[1m‘==’\033[0m, \033[1m‘!=’\033[0m, \033[1m‘&&’\033[0m, "
                   "\033[1m‘||’\033[0m, \033[1m‘=’\033[0m, \033[1m‘+=’\033[0m, \033[1m‘-=’\033[0m, \033[1m‘*=’\033[0m, "
                   "\033[1m‘/=’\033[0m, \033[1m‘%=’\033[0m, \033[1m‘&=’\033[0m, \033[1m‘|=’\033[0m, "
                   "\033[1m‘^=’\033[0m, \033[1m‘<<=’\033[0m, \033[1m‘>>=’\033[0m or \033[1m‘?’\033[0m next";
        case ERROR_MESSAGE_PARSER::function_declared_in_for_initial:
            return "function " EM_VARG " declared in \033[1m‘for’\033[0m loop initial declaration";
        case ERROR_MESSAGE_PARSER::unexpected_type_specifier:
            return "found token " EM_VARG ", but expected \033[1m‘identifier’\033[0m, \033[1m‘)’\033[0m, "
                   "\033[1m‘char’\033[0m, \033[1m‘int’\033[0m, \033[1m‘long’\033[0m, \033[1m‘double’\033[0m, "
                   "\033[1m‘unsigned’\033[0m, \033[1m‘signed’\033[0m, \033[1m‘void’\033[0m, \033[1m‘struct’\033[0m, "
                   "\033[1m‘union’\033[0m, \033[1m‘static’\033[0m, \033[1m‘extern’\033[0m, \033[1m‘*’\033[0m, "
                   "\033[1m‘(’\033[0m or \033[1m‘[’\033[0m next";
        case ERROR_MESSAGE_PARSER::unexpected_type_specifier_list:
            return "found tokens " EM_VARG ", but expected valid list of unique type specifiers next";
        case ERROR_MESSAGE_PARSER::unexpected_storage_class:
            return "found token " EM_VARG ", but expected \033[1m‘static’\033[0m or \033[1m‘extern’\033[0m next";
        case ERROR_MESSAGE_PARSER::empty_compound_initializer:
            return "empty compound initializer requires at least one initializer";
        case ERROR_MESSAGE_PARSER::type_derivation_on_function_declaration:
            return "cannot apply further type derivation to function declaration";
        case ERROR_MESSAGE_PARSER::unexpected_simple_declarator:
            return "found token " EM_VARG ", but expected \033[1m‘identifier’\033[0m or \033[1m‘(’\033[0m next";
        case ERROR_MESSAGE_PARSER::unexpected_parameter_list:
            return "found token " EM_VARG ", but expected \033[1m‘void’\033[0m, \033[1m‘char’\033[0m, "
                   "\033[1m‘int’\033[0m, \033[1m‘long’\033[0m, \033[1m‘double’\033[0m, \033[1m‘unsigned’\033[0m, "
                   "\033[1m‘signed’\033[0m, \033[1m‘struct’\033[0m or \033[1m‘union’\033[0m next";
        case ERROR_MESSAGE_PARSER::member_declared_with_non_automatic_storage:
            return "data structure type declared with member " EM_VARG " with " EM_VARG " storage class";
        case ERROR_MESSAGE_PARSER::member_declared_as_function:
            return "data structure type declared with member " EM_VARG " as a function";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

const char* get_what_message(ERROR_MESSAGE_SEMANTIC message) {
    switch (message) {
        case ERROR_MESSAGE_SEMANTIC::array_of_incomplete_type:
            return "array type " EM_VARG " of incomplete type " EM_VARG ", requires a complete type";
        case ERROR_MESSAGE_SEMANTIC::joint_pointer_type_mismatch:
            return "pointer type mismatch " EM_VARG " and " EM_VARG " in operator";
        case ERROR_MESSAGE_SEMANTIC::function_used_as_variable:
            return "function " EM_VARG " used as a variable";
        case ERROR_MESSAGE_SEMANTIC::illegal_conversion_from_type_to:
            return "illegal cast, cannot convert expression from type " EM_VARG " to " EM_VARG;
        case ERROR_MESSAGE_SEMANTIC::unary_on_invalid_operand_type:
            return "cannot apply unary operator " EM_VARG " on operand type " EM_VARG;
        case ERROR_MESSAGE_SEMANTIC::binary_on_invalid_operand_type:
            return "cannot apply binary operator " EM_VARG " on operand type " EM_VARG;
        case ERROR_MESSAGE_SEMANTIC::binary_on_invalid_operand_types:
            return "cannot apply binary operator " EM_VARG " on operand types " EM_VARG " and " EM_VARG;
        case ERROR_MESSAGE_SEMANTIC::assignment_to_void_type:
            return "cannot assign \033[1m‘=’\033[0m to left operand type \033[1m‘void’\033[0m";
        case ERROR_MESSAGE_SEMANTIC::assignment_to_rvalue:
            return "assignment " EM_VARG " requires lvalue left operand, but got rvalue";
        case ERROR_MESSAGE_SEMANTIC::conditional_on_invalid_condition_type:
            return "cannot apply conditional \033[1m‘?’\033[0m on condition operand type " EM_VARG;
        case ERROR_MESSAGE_SEMANTIC::ternary_on_invalid_operand_types:
            return "cannot apply ternary operator \033[1m‘:’\033[0m on operand types " EM_VARG " and " EM_VARG;
        case ERROR_MESSAGE_SEMANTIC::variable_used_as_function:
            return "variable " EM_VARG " used as a function";
        case ERROR_MESSAGE_SEMANTIC::function_called_with_wrong_number_of_arguments:
            return "function " EM_VARG " called with " EM_VARG " arguments instead of " EM_VARG;
        case ERROR_MESSAGE_SEMANTIC::dereference_non_pointer:
            return "cannot apply dereference operator \033[1m‘*’\033[0m on non-pointer type " EM_VARG;
        case ERROR_MESSAGE_SEMANTIC::address_of_rvalue:
            return "addresssing \033[1m‘&’\033[0m requires lvalue operand, but got rvalue";
        case ERROR_MESSAGE_SEMANTIC::subscript_array_with_invalid_types:
            return "cannot subscript array with operand types " EM_VARG " and " EM_VARG
                   ", requires a complete pointer and an "
                   "integer types";
        case ERROR_MESSAGE_SEMANTIC::get_size_of_incomplete_type:
            return "cannot get size with \033[1m‘sizeof’\033[0m operator on incomplete type " EM_VARG;
        case ERROR_MESSAGE_SEMANTIC::dot_on_non_structure_type:
            return "cannot access data structure member " EM_VARG
                   " with dot operator \033[1m‘.’\033[0m on non-data structure type " EM_VARG;
        case ERROR_MESSAGE_SEMANTIC::member_not_in_structure_type:
            return "data structure type " EM_VARG " has no member named " EM_VARG;
        case ERROR_MESSAGE_SEMANTIC::arrow_on_non_pointer_to_structure_type:
            return "cannot access data structure member " EM_VARG
                   " with arrow operator \033[1m‘->’\033[0m on non-pointer-to-data structure type " EM_VARG;
        case ERROR_MESSAGE_SEMANTIC::arrow_on_incomplete_structure_type:
            return "cannot access data structure member " EM_VARG
                   " with arrow operator \033[1m‘->’\033[0m on incomplete data structure type " EM_VARG;
        case ERROR_MESSAGE_SEMANTIC::incomplete_structure_type_in_expression:
            return "incomplete data structure type " EM_VARG " in expression";
        case ERROR_MESSAGE_SEMANTIC::return_value_in_void_function:
            return "found \033[1m‘return’\033[0m value in function " EM_VARG " returning type \033[1m‘void’\033[0m";
        case ERROR_MESSAGE_SEMANTIC::no_return_value_in_non_void_function:
            return "found \033[1m‘return’\033[0m with no value in function " EM_VARG " returning type " EM_VARG;
        case ERROR_MESSAGE_SEMANTIC::if_used_with_condition_type:
            return "cannot use \033[1m‘if’\033[0m statement with condition expression type " EM_VARG;
        case ERROR_MESSAGE_SEMANTIC::while_used_with_condition_type:
            return "cannot use \033[1m‘while’\033[0m loop statement with condition expression type " EM_VARG;
        case ERROR_MESSAGE_SEMANTIC::do_while_used_with_condition_type:
            return "cannot use \033[1m‘do while’\033[0m loop statement with condition expression type " EM_VARG;
        case ERROR_MESSAGE_SEMANTIC::for_used_with_condition_type:
            return "cannot use \033[1m‘for’\033[0m loop statement with condition expression type " EM_VARG;
        case ERROR_MESSAGE_SEMANTIC::switch_used_with_match_type:
            return "cannot use \033[1m‘switch’\033[0m statement with match expression type " EM_VARG
                   ", requires an integer type";
        case ERROR_MESSAGE_SEMANTIC::duplicate_case_value_in_switch:
            return "found duplicate \033[1m‘case’\033[0m value " EM_VARG " in \033[1m‘switch’\033[0m statement";
        case ERROR_MESSAGE_SEMANTIC::non_char_array_initialized_from_string:
            return "non-character array type " EM_VARG " initialized from string literal";
        case ERROR_MESSAGE_SEMANTIC::string_initialized_with_too_many_characters:
            return "size " EM_VARG " string literal initialized with " EM_VARG " characters";
        case ERROR_MESSAGE_SEMANTIC::array_initialized_with_too_many_initializers:
            return "size " EM_VARG " array type " EM_VARG " initialized with " EM_VARG " initializers";
        case ERROR_MESSAGE_SEMANTIC::structure_initialized_with_too_many_members:
            return "data structure type " EM_VARG " initialized with " EM_VARG " members instead of " EM_VARG;
        case ERROR_MESSAGE_SEMANTIC::function_returns_array:
            return "function " EM_VARG " returns array type " EM_VARG ", instead of pointer type";
        case ERROR_MESSAGE_SEMANTIC::function_returns_incomplete_structure_type:
            return "function " EM_VARG " returns incomplete data structure type " EM_VARG;
        case ERROR_MESSAGE_SEMANTIC::parameter_with_type_void:
            return "function " EM_VARG " declared with parameter " EM_VARG " with type \033[1m‘void’\033[0m";
        case ERROR_MESSAGE_SEMANTIC::parameter_with_incomplete_structure_type:
            return "function " EM_VARG " defined with parameter " EM_VARG
                   " with incomplete data structure type " EM_VARG;
        case ERROR_MESSAGE_SEMANTIC::function_redeclared_with_conflicting_type:
            return "function " EM_VARG " redeclared with function type " EM_VARG
                   ", but previous declaration has function type " EM_VARG;
        case ERROR_MESSAGE_SEMANTIC::function_redefined:
            return "function " EM_VARG " already defined with function type " EM_VARG;
        case ERROR_MESSAGE_SEMANTIC::non_static_function_redeclared_static:
            return "function " EM_VARG " with \033[1m‘static’\033[0m storage class already declared non-static";
        case ERROR_MESSAGE_SEMANTIC::static_pointer_initialized_from_non_integer:
            return "cannot statically initialize pointer type " EM_VARG " from constant " EM_VARG
                   ", requires a constant integer";
        case ERROR_MESSAGE_SEMANTIC::static_pointer_initialized_from_non_null:
            return "cannot statically initialize pointer type " EM_VARG " from non-null value " EM_VARG;
        case ERROR_MESSAGE_SEMANTIC::aggregate_initialized_with_single_initializer:
            return "aggregate type " EM_VARG " statically initialized with single initializer";
        case ERROR_MESSAGE_SEMANTIC::static_non_char_pointer_initialized_from_string:
            return "non-character pointer type " EM_VARG " statically initialized from string literal";
        case ERROR_MESSAGE_SEMANTIC::static_initialized_with_non_constant:
            return "cannot statically initialize variable from non-constant type " EM_VARG ", requires a constant";
        case ERROR_MESSAGE_SEMANTIC::scalar_initialized_with_compound_initializer:
            return "cannot initialize scalar type " EM_VARG " with compound initializer";
        case ERROR_MESSAGE_SEMANTIC::variable_declared_with_type_void:
            return "variable " EM_VARG " declared with type \033[1m‘void’\033[0m";
        case ERROR_MESSAGE_SEMANTIC::variable_declared_with_incomplete_structure_type:
            return "variable " EM_VARG " declared with incomplete data structure type " EM_VARG;
        case ERROR_MESSAGE_SEMANTIC::variable_redeclared_with_conflicting_type:
            return "variable " EM_VARG " redeclared with conflicting type " EM_VARG
                   ", but previously declared with type " EM_VARG;
        case ERROR_MESSAGE_SEMANTIC::variable_redeclared_with_conflicting_storage:
            return "variable " EM_VARG " redeclared with conflicting storage class";
        case ERROR_MESSAGE_SEMANTIC::extern_variable_defined:
            return "illegal initializer, can only declare variable " EM_VARG
                   " with \033[1m‘extern’\033[0m storage class";
        case ERROR_MESSAGE_SEMANTIC::structure_declared_with_duplicate_member:
            return "data structure type " EM_VARG " declared with duplicate member name " EM_VARG;
        case ERROR_MESSAGE_SEMANTIC::member_declared_with_incomplete_type:
            return "data structure type " EM_VARG " declared with member " EM_VARG " with incomplete type " EM_VARG;
        case ERROR_MESSAGE_SEMANTIC::structure_redeclared_in_scope:
            return "data structure type " EM_VARG " already declared in this scope";
        case ERROR_MESSAGE_SEMANTIC::case_outside_of_switch:
            return "found \033[1m‘case’\033[0m statement outside of \033[1m‘switch’\033[0m";
        case ERROR_MESSAGE_SEMANTIC::default_outside_of_switch:
            return "found \033[1m‘default’\033[0m statement outside of \033[1m‘switch’\033[0m";
        case ERROR_MESSAGE_SEMANTIC::more_than_one_default_in_switch:
            return "found more than one \033[1m‘default’\033[0m statement in \033[1m‘switch’\033[0m";
        case ERROR_MESSAGE_SEMANTIC::break_outside_of_loop:
            return "found \033[1m‘break’\033[0m statement outside of loop";
        case ERROR_MESSAGE_SEMANTIC::continue_outside_of_loop:
            return "found \033[1m‘continue’\033[0m statement outside of loop";
        case ERROR_MESSAGE_SEMANTIC::goto_with_undefined_target_label:
            return "found \033[1m‘goto’\033[0m statement, but target label " EM_VARG
                   " not defined in function " EM_VARG;
        case ERROR_MESSAGE_SEMANTIC::structure_conflicts_with_previously_declared:
            return EM_VARG " conflicts with data structure type " EM_VARG
                           " previously declared or defined in this scope";
        case ERROR_MESSAGE_SEMANTIC::structure_not_defined_in_scope:
            return "data structure type " EM_VARG " not defined in this scope";
        case ERROR_MESSAGE_SEMANTIC::variable_not_declared_in_scope:
            return "variable " EM_VARG " not declared in this scope";
        case ERROR_MESSAGE_SEMANTIC::function_not_declared_in_scope:
            return "function " EM_VARG " not declared in this scope";
        case ERROR_MESSAGE_SEMANTIC::for_initial_declared_with_non_automatic_storage:
            return "variable " EM_VARG " declared with " EM_VARG
                   " storage class in \033[1m‘for’\033[0m loop initial declaration";
        case ERROR_MESSAGE_SEMANTIC::label_redefined_in_scope:
            return "label " EM_VARG " already defined in this scope";
        case ERROR_MESSAGE_SEMANTIC::variable_redeclared_in_scope:
            return "variable " EM_VARG " already declared in this scope";
        case ERROR_MESSAGE_SEMANTIC::nested_function_defined:
            return "function " EM_VARG " defined inside another function, but nested function definition are not "
                   "permitted";
        case ERROR_MESSAGE_SEMANTIC::nested_static_function_declared:
            return "cannot declare nested function " EM_VARG
                   " in another function with \033[1m‘static’\033[0m storage class";
        case ERROR_MESSAGE_SEMANTIC::function_redeclared_in_scope:
            return "function " EM_VARG " already declared in this scope";
        default:
            RAISE_INTERNAL_ERROR;
    }
}
