#include <string>

#include "util/fileio.hpp"

#include "frontend/parser/errors.hpp"
#include "frontend/parser/lexer.hpp"

std::unique_ptr<ErrorsContext> errors;

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
    std::string type_hr = "struct ";
    type_hr += get_name_hr(name);
    return type_hr;
}

std::string get_type_hr(Type* type);

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

std::string get_what_message(ERROR_MESSAGE_ARGUMENT message) {
    switch (message) {
        case ERROR_MESSAGE_ARGUMENT::no_debug_code_in_argument:
            return "no debug code passed in first argument";
        case ERROR_MESSAGE_ARGUMENT::invalid_debug_code_in_argument:
            return "invalid debug code %s passed in first argument";
        case ERROR_MESSAGE_ARGUMENT::no_input_files_in_argument:
            return "no input file passed in second argument";
        case ERROR_MESSAGE_ARGUMENT::no_include_directories_in_argument:
            return "no include directories passed in third argument";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

std::string get_what_message(ERROR_MESSAGE_UTIL message) {
    switch (message) {
        case ERROR_MESSAGE_UTIL::failed_to_read_input_file:
            return "cannot read input file %s";
        case ERROR_MESSAGE_UTIL::failed_to_write_to_output_file:
            return "cannot write output file %s";
        case ERROR_MESSAGE_UTIL::failed_to_interpret_string_to_integer:
            return "cannot interpret string %s to an integer value";
        case ERROR_MESSAGE_UTIL::failed_to_interpret_string_to_unsigned_integer:
            return "cannot interpret string %s to an unsigned integer value";
        case ERROR_MESSAGE_UTIL::failed_to_interpret_string_to_float:
            return "cannot interpret string %s to a floating point value";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

std::string get_what_message(ERROR_MESSAGE_LEXER message) {
    switch (message) {
        case ERROR_MESSAGE_LEXER::invalid_token:
            return "found invalid token %s";
        case ERROR_MESSAGE_LEXER::failed_to_include_header_file: {
            std::string what_message = "cannot find %s header file in ";
            what_message += em("include");
            what_message += " directive search";
            return what_message;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

std::string get_what_message(ERROR_MESSAGE_PARSER message) {
    switch (message) {
        case ERROR_MESSAGE_PARSER::unexpected_next_token:
            return "found token %s, but expected %s next";
        case ERROR_MESSAGE_PARSER::reached_end_of_file:
            return "reached end of file, but expected declaration or statement next";
        case ERROR_MESSAGE_PARSER::number_too_large_for_long_constant:
            return "cannot represent %s as a 64 bits signed integer constant, very large number";
        case ERROR_MESSAGE_PARSER::number_too_large_for_unsigned_long_constant:
            return "cannot represent %s as a 64 bits unsigned integer constant, very large number";
        case ERROR_MESSAGE_PARSER::array_size_not_a_constant_integer:
            return "illegal array size %s, requires a constant integer";
        case ERROR_MESSAGE_PARSER::unexpected_unary_operator: {
            std::string what_message = "found token %s, but expected ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::unop_complement));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::unop_negation));
            what_message += " or ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::unop_not));
            what_message += " next";
            return what_message;
        }
        case ERROR_MESSAGE_PARSER::unexpected_binary_operator: {
            std::string what_message = "found token %s, but expected ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_addition));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::assignment_plus));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::unop_negation));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::assignment_difference));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_multiplication));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::assignment_product));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_division));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::assignment_quotient));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_remainder));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::assignment_remainder));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_bitand));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::assignment_bitand));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_bitor));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::assignment_bitor));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_bitxor));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::assignment_bitxor));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_bitshiftleft));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::assignment_bitshiftleft));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_bitshiftright));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::assignment_bitshiftright));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_and));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_or));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_equalto));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_notequal));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_lessthan));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_lessthanorequal));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_greaterthan));
            what_message += " or ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_greaterthanorequal));
            what_message += " next";
            return what_message;
        }
        case ERROR_MESSAGE_PARSER::unexpected_abstract_declarator: {
            std::string what_message = "found token %s, but expected ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_multiplication));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::parenthesis_open));
            what_message += " or ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::brackets_open));
            what_message += " next";
            return what_message;
        }
        case ERROR_MESSAGE_PARSER::unexpected_pointer_unary_factor: {
            std::string what_message = "found token %s, but expected ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_multiplication));
            what_message += " or ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_bitand));
            what_message += " next";
            return what_message;
        }
        case ERROR_MESSAGE_PARSER::unexpected_primary_expression_factor: {
            std::string what_message = "found token %s, but expected ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::constant));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::long_constant));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::char_constant));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::float_constant));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::unsigned_constant));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::unsigned_long_constant));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::identifier));
            what_message += ", ";
            {
                std::string token_kind_hr = get_token_kind_hr(TOKEN_KIND::identifier);
                token_kind_hr += get_token_kind_hr(TOKEN_KIND::parenthesis_open);
                what_message += em(token_kind_hr);
            }
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::string_literal));
            what_message += " or ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::parenthesis_open));
            what_message += " next";
            return what_message;
        }
        case ERROR_MESSAGE_PARSER::unexpected_expression: {
            std::string what_message = "found token %s, but expected ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_addition));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::unop_negation));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_multiplication));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_division));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_remainder));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_bitand));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_bitor));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_bitxor));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_bitshiftleft));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_bitshiftright));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_lessthan));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_lessthanorequal));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_greaterthan));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_greaterthanorequal));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_equalto));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_notequal));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_and));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_or));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::assignment_simple));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::assignment_plus));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::assignment_difference));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::assignment_product));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::assignment_quotient));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::assignment_remainder));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::assignment_bitand));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::assignment_bitor));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::assignment_bitxor));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::assignment_bitshiftleft));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::assignment_bitshiftright));
            what_message += " or ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::ternary_if));
            what_message += " next";
            return what_message;
        }
        case ERROR_MESSAGE_PARSER::function_declared_in_for_initial: {
            std::string what_message = "function %s declared in ";
            what_message += em("for");
            what_message += " loop initial declaration";
            return what_message;
        }
        case ERROR_MESSAGE_PARSER::unexpected_type_specifier: {
            std::string what_message = "found token %s, but expected ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::identifier));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::parenthesis_close));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::key_char));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::key_int));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::key_long));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::key_double));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::key_unsigned));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::key_signed));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::key_void));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::key_struct));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::key_static));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::key_extern));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::binop_multiplication));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::parenthesis_open));
            what_message += " or ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::brackets_open));
            what_message += " next";
            return what_message;
        }
        case ERROR_MESSAGE_PARSER::unexpected_type_specifier_list:
            return "found tokens %s, but expected valid list of unique type specifiers next";
        case ERROR_MESSAGE_PARSER::unexpected_storage_class: {
            std::string what_message = "found token %s, but expected ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::key_static));
            what_message += " or ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::key_extern));
            what_message += " next";
            return what_message;
        }
        case ERROR_MESSAGE_PARSER::empty_compound_initializer:
            return "empty compound initializer requires at least one initializer";
        case ERROR_MESSAGE_PARSER::type_derivation_on_function_declaration:
            return "cannot apply further type derivation to function declaration";
        case ERROR_MESSAGE_PARSER::unexpected_simple_declarator: {
            std::string what_message = "found token %s, but expected ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::identifier));
            what_message += " or ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::parenthesis_open));
            what_message += " next";
            return what_message;
        }
        case ERROR_MESSAGE_PARSER::unexpected_parameter_list: {
            std::string what_message = "found token %s, but expected ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::key_void));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::key_char));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::key_int));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::key_long));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::key_double));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::key_unsigned));
            what_message += ", ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::key_signed));
            what_message += " or ";
            what_message += em(get_token_kind_hr(TOKEN_KIND::key_struct));
            what_message += " next";
            return what_message;
        }
        case ERROR_MESSAGE_PARSER::member_declared_with_non_automatic_storage:
            return "structure type declared with member %s with %s storage class";
        case ERROR_MESSAGE_PARSER::member_declared_as_function:
            return "structure type declared with member %s as a function";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

std::string get_what_message(ERROR_MESSAGE_SEMANTIC message) {
    switch (message) {
        case ERROR_MESSAGE_SEMANTIC::array_of_incomplete_type:
            return "array type %s of incomplete type %s, requires a complete type";
        case ERROR_MESSAGE_SEMANTIC::joint_pointer_type_mismatch:
            return "pointer type mismatch %s and %s in operator";
        case ERROR_MESSAGE_SEMANTIC::function_used_as_variable:
            return "function %s used as a variable";
        case ERROR_MESSAGE_SEMANTIC::illegal_conversion_from_type_to:
            return "illegal cast, cannot convert expression from type %s to %s";
        case ERROR_MESSAGE_SEMANTIC::unary_on_invalid_operand_type:
            return "cannot apply unary operator %s on operand type %s";
        case ERROR_MESSAGE_SEMANTIC::binary_on_invalid_operand_type:
            return "cannot apply binary operator %s on operand type %s";
        case ERROR_MESSAGE_SEMANTIC::binary_on_invalid_operand_types:
            return "cannot apply binary operator %s on operand types %s and %s";
        case ERROR_MESSAGE_SEMANTIC::assignment_to_void_type: {
            std::string what_message = "cannot assign ";
            what_message += em("=");
            what_message += " to left operand type ";
            what_message += em("void");
            return what_message;
        }
        case ERROR_MESSAGE_SEMANTIC::assignment_to_rvalue:
            return "assignment %s requires lvalue left operand, but got rvalue";
        case ERROR_MESSAGE_SEMANTIC::conditional_on_invalid_condition_type: {
            std::string what_message = "cannot apply conditional ";
            what_message += em("?");
            what_message += " on condition operand type %s";
            return what_message;
        }
        case ERROR_MESSAGE_SEMANTIC::ternary_on_invalid_operand_types: {
            std::string what_message = "cannot apply ternary operator ";
            what_message += em(":");
            what_message += " on operand types %s and %s";
            return what_message;
        }
        case ERROR_MESSAGE_SEMANTIC::variable_used_as_function:
            return "variable %s used as a function";
        case ERROR_MESSAGE_SEMANTIC::function_called_with_wrong_number_of_arguments:
            return "function %s called with %s arguments instead of %s";
        case ERROR_MESSAGE_SEMANTIC::dereference_non_pointer: {
            std::string what_message = "cannot apply dereference operator ";
            what_message += em("*");
            what_message += " on non-pointer type %s";
            return what_message;
        }
        case ERROR_MESSAGE_SEMANTIC::address_of_rvalue: {
            std::string what_message = "addresssing ";
            what_message += em("&");
            what_message += " requires lvalue operand, but got rvalue";
            return what_message;
        }
        case ERROR_MESSAGE_SEMANTIC::subscript_array_with_invalid_types:
            return "cannot subscript array with operand types %s and %s, requires a complete pointer and an "
                   "integer types";
        case ERROR_MESSAGE_SEMANTIC::get_size_of_incomplete_type: {
            std::string what_message = "cannot get size with ";
            what_message += em("sizeof");
            what_message += " operator on incomplete type %s";
            return what_message;
        }
        case ERROR_MESSAGE_SEMANTIC::dot_on_non_structure_type: {
            std::string what_message = "cannot access structure member %s with dot operator ";
            what_message += em(".");
            what_message += " on non-structure type %s";
            return what_message;
        }
        case ERROR_MESSAGE_SEMANTIC::member_not_in_structure_type:
            return "structure type %s has no member named %s";
        case ERROR_MESSAGE_SEMANTIC::arrow_on_non_pointer_to_structure_type: {
            std::string what_message = "cannot access structure member %s with arrow operator ";
            what_message += em("->");
            what_message += " on non-pointer-to-structure type %s";
            return what_message;
        }
        case ERROR_MESSAGE_SEMANTIC::arrow_on_incomplete_structure_type: {
            std::string what_message = "cannot access structure member %s with arrow operator ";
            what_message += em("->");
            what_message += " on incomplete structure type %s";
            return what_message;
        }
        case ERROR_MESSAGE_SEMANTIC::incomplete_structure_type_in_expression:
            return "incomplete structure type %s in expression";
        case ERROR_MESSAGE_SEMANTIC::return_value_in_void_function: {
            std::string what_message = "found ";
            what_message += em("return");
            what_message += " value in function %s returning type ";
            what_message += em("void");
            return what_message;
        }
        case ERROR_MESSAGE_SEMANTIC::no_return_value_in_non_void_function: {
            std::string what_message = "found ";
            what_message += em("return");
            what_message += " with no value in function %s returning type %s";
            return what_message;
        }
        case ERROR_MESSAGE_SEMANTIC::if_used_with_condition_type: {
            std::string what_message = "cannot use ";
            what_message += em("if");
            what_message += " statement with condition expression type %s";
            return what_message;
        }
        case ERROR_MESSAGE_SEMANTIC::while_used_with_condition_type: {
            std::string what_message = "cannot use ";
            what_message += em("while");
            what_message += " loop statement with condition expression type %s";
            return what_message;
        }
        case ERROR_MESSAGE_SEMANTIC::do_while_used_with_condition_type: {
            std::string what_message = "cannot use ";
            what_message += em("do while");
            what_message += " loop statement with condition expression type %s";
            return what_message;
        }
        case ERROR_MESSAGE_SEMANTIC::for_used_with_condition_type: {
            std::string what_message = "cannot use ";
            what_message += em("for");
            what_message += " loop statement with condition expression type %s";
            return what_message;
        }
        case ERROR_MESSAGE_SEMANTIC::non_char_array_initialized_from_string:
            return "non-character array type %s initialized from string literal";
        case ERROR_MESSAGE_SEMANTIC::string_initialized_with_too_many_characters:
            return "size %s string literal initialized with %s characters";
        case ERROR_MESSAGE_SEMANTIC::array_initialized_with_too_many_initializers:
            return "size %s array type %s initialized with %s initializers";
        case ERROR_MESSAGE_SEMANTIC::structure_initialized_with_too_many_members:
            return "structure type %s initialized with %s members instead of %s";
        case ERROR_MESSAGE_SEMANTIC::function_returns_array:
            return "function %s returns array type %s, instead of pointer type";
        case ERROR_MESSAGE_SEMANTIC::function_returns_incomplete_structure_type:
            return "function %s returns incomplete structure type %s";
        case ERROR_MESSAGE_SEMANTIC::parameter_with_type_void: {
            std::string what_message = "function %s declared with parameter %s with type ";
            what_message += em("void");
            return what_message;
        }
        case ERROR_MESSAGE_SEMANTIC::parameter_with_incomplete_structure_type:
            return "function %s defined with parameter %s with incomplete structure type %s";
        case ERROR_MESSAGE_SEMANTIC::function_redeclared_with_conflicting_type:
            return "function %s redeclared with function type %s, but previous declaration has function type %s";
        case ERROR_MESSAGE_SEMANTIC::function_redefined:
            return "function %s already defined with function type %s";
        case ERROR_MESSAGE_SEMANTIC::non_static_function_redeclared_static: {
            std::string what_message = "function %s with ";
            what_message += em("static");
            what_message += " storage class already declared non-static";
            return what_message;
        }
        case ERROR_MESSAGE_SEMANTIC::static_pointer_initialized_from_non_integer:
            return "cannot statically initialize pointer type %s from constant %s, requires a constant integer";
        case ERROR_MESSAGE_SEMANTIC::static_pointer_initialized_from_non_null:
            return "cannot statically initialize pointer type %s from non-null value %s";
        case ERROR_MESSAGE_SEMANTIC::aggregate_initialized_with_single_initializer:
            return "aggregate type %s statically initialized with single initializer";
        case ERROR_MESSAGE_SEMANTIC::static_non_char_pointer_initialized_from_string:
            return "non-character pointer type %s statically initialized from string literal";
        case ERROR_MESSAGE_SEMANTIC::static_initialized_with_non_constant:
            return "cannot statically initialize variable from non-constant type %s, requires a constant";
        case ERROR_MESSAGE_SEMANTIC::scalar_initialized_with_compound_initializer:
            return "cannot initialize scalar type %s with compound initializer";
        case ERROR_MESSAGE_SEMANTIC::variable_declared_with_type_void: {
            std::string what_message = "variable %s declared with type ";
            what_message += em("void");
            return what_message;
        }
        case ERROR_MESSAGE_SEMANTIC::variable_declared_with_incomplete_structure_type:
            return "variable %s declared with incomplete structure type %s";
        case ERROR_MESSAGE_SEMANTIC::variable_redeclared_with_conflicting_type:
            return "variable %s redeclared with conflicting type %s, but previously declared with type %s";
        case ERROR_MESSAGE_SEMANTIC::variable_redeclared_with_conflicting_storage:
            return "variable %s redeclared with conflicting storage class";
        case ERROR_MESSAGE_SEMANTIC::extern_variable_defined: {
            std::string what_message = "illegal initializer, can only declare variable %s with ";
            what_message += em("extern");
            what_message += " storage class";
            return what_message;
        }
        case ERROR_MESSAGE_SEMANTIC::structure_declared_with_duplicate_member:
            return "structure type %s declared with duplicate member name %s";
        case ERROR_MESSAGE_SEMANTIC::member_declared_with_incomplete_type:
            return "structure type %s declared with member %s with incomplete type %s";
        case ERROR_MESSAGE_SEMANTIC::structure_redeclared_in_scope:
            return "structure type %s already declared in this scope";
        case ERROR_MESSAGE_SEMANTIC::break_outside_of_loop: {
            std::string what_message = "found ";
            what_message += em("break");
            what_message += " statement outside of loop";
            return what_message;
        }
        case ERROR_MESSAGE_SEMANTIC::continue_outside_of_loop: {
            std::string what_message = "found ";
            what_message += em("continue");
            what_message += " statement outside of loop";
            return what_message;
        }
        case ERROR_MESSAGE_SEMANTIC::goto_with_undefined_target_label: {
            std::string what_message = "found ";
            what_message += em("goto");
            what_message += " statement, but target label %s not defined in function %s";
            return what_message;
        }
        case ERROR_MESSAGE_SEMANTIC::structure_not_defined_in_scope:
            return "structure type %s not defined in this scope";
        case ERROR_MESSAGE_SEMANTIC::variable_not_declared_in_scope:
            return "variable %s not declared in this scope";
        case ERROR_MESSAGE_SEMANTIC::function_not_declared_in_scope:
            return "function %s not declared in this scope";
        case ERROR_MESSAGE_SEMANTIC::for_initial_declared_with_non_automatic_storage: {
            std::string what_message = "variable %s declared with %s storage class in ";
            what_message += em("for");
            what_message += " loop initial declaration";
            return what_message;
        }
        case ERROR_MESSAGE_SEMANTIC::label_redefined_in_scope:
            return "label %s already defined in this scope";
        case ERROR_MESSAGE_SEMANTIC::variable_redeclared_in_scope:
            return "variable %s already declared in this scope";
        case ERROR_MESSAGE_SEMANTIC::nested_function_defined:
            return "function %s defined inside another function, but nested function definition are not "
                   "permitted";
        case ERROR_MESSAGE_SEMANTIC::nested_static_function_declared: {
            std::string what_message = "cannot declare nested function %s in another function with ";
            what_message += em("static");
            what_message += " storage class";
            return what_message;
        }
        case ERROR_MESSAGE_SEMANTIC::function_redeclared_in_scope:
            return "function %s already declared in this scope";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

size_t handle_error_at_line(size_t total_line_number) {
    for (size_t i = 0; i < errors->file_open_lines.size() - 1; ++i) {
        if (total_line_number < errors->file_open_lines[i + 1].total_line_number) {
            set_filename(errors->file_open_lines[i].filename);
            return total_line_number - errors->file_open_lines[i].total_line_number
                   + errors->file_open_lines[i].line_number;
        }
    }
    set_filename(errors->file_open_lines.back().filename);
    return total_line_number - errors->file_open_lines.back().total_line_number
           + errors->file_open_lines.back().line_number;
}
