#ifndef _FRONTEND_PARSER_ERRORS_HPP
#define _FRONTEND_PARSER_ERRORS_HPP

#include <cstdio>
#include <string>

#include "ast_t.hpp" // ast

#include "parser/messages.hpp" // frontend
#include "parser/tokens.hpp"   // frontend

#include "intermediate/messages.hpp" // frontend

struct Type;
struct CConst;
struct CUnaryOp;
struct CBinaryOp;
struct CStorageClass;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Errors

enum MESSAGE_FATAL {
    unhandled_fatal_error = 0,
    operating_system_not_supported,
    architecture_not_supported,
    compiler_not_supported,
    gcc_version_not_supported
};

enum MESSAGE_ARGUMENT {
    unhandled_argument_error = 100,
    no_debug_code_in_argument,
    invalid_debug_code_in_argument,
    no_optim_1_mask_in_argument,
    invalid_optim_1_mask_in_argument,
    no_optim_2_code_in_argument,
    invalid_optim_2_code_in_argument,
    no_input_files_in_argument,
    no_include_directories_in_argument
};

enum MESSAGE_UTIL {
    unhandled_util_error = 200,
    failed_to_read_input_file,
    failed_to_write_to_output_file,
    failed_to_interpret_string_to_integer,
    failed_to_interpret_string_to_unsigned_integer,
    failed_to_interpret_string_to_float
};

std::string get_tok_fmt(TOKEN_KIND token_kind);
std::string get_name_fmt(TIdentifier name);
std::string get_struct_name_fmt(TIdentifier name, bool is_union);
std::string get_type_fmt(Type* type);
std::string get_const_fmt(CConst* node);
std::string get_storage_class_fmt(CStorageClass* node);
std::string get_unop_fmt(CUnaryOp* node);
std::string get_binop_fmt(CBinaryOp* node);
std::string get_assign_fmt(CBinaryOp* node, CUnaryOp* unary_op);
#define get_token_kind_hr_c_str(X) get_tok_fmt(X).c_str()
#define get_name_hr_c_str(X) get_name_fmt(X).c_str()
#define get_struct_name_hr_c_str(X, Y) get_struct_name_fmt(X, Y).c_str()
#define get_type_hr_c_str(X) get_type_fmt(X).c_str()
#define get_const_hr_c_str(X) get_const_fmt(X).c_str()
#define get_storage_class_hr_c_str(X) get_storage_class_fmt(X).c_str()
#define get_unary_op_hr_c_str(X) get_unop_fmt(X).c_str()
#define get_binary_op_hr_c_str(X) get_binop_fmt(X).c_str()
#define get_assignment_hr_c_str(X, Y) get_assign_fmt(X, Y).c_str()

const char* get_fatal_msg(MESSAGE_FATAL message);
const char* get_arg_msg(MESSAGE_ARGUMENT message);
const char* get_util_msg(MESSAGE_UTIL message);
const char* get_lexer_msg(MESSAGE_LEXER message);
const char* get_parser_msg(MESSAGE_PARSER message);
const char* get_semantic_msg(MESSAGE_SEMANTIC message);
#define GET_FATAL_MESSAGE(X, ...) GET_ERROR_MESSAGE(get_fatal_msg(X), (int)X, __VA_ARGS__)
#define GET_ARGUMENT_MESSAGE(X, ...) GET_ERROR_MESSAGE(get_arg_msg(X), (int)X, __VA_ARGS__)
#define GET_UTIL_MESSAGE(X, ...) GET_ERROR_MESSAGE(get_util_msg(X), (int)X, __VA_ARGS__)
#define GET_LEXER_MESSAGE(X, ...) GET_ERROR_MESSAGE(get_lexer_msg(X), (int)X, __VA_ARGS__)
#define GET_PARSER_MESSAGE(X, ...) GET_ERROR_MESSAGE(get_parser_msg(X), (int)X, __VA_ARGS__)
#define GET_SEMANTIC_MESSAGE(X, ...) GET_ERROR_MESSAGE(get_semantic_msg(X), (int)X, __VA_ARGS__)
#define GET_ARGUMENT_MESSAGE_0(X) GET_ERROR_MESSAGE(get_arg_msg(X), (int)X)
#define GET_PARSER_MESSAGE_0(X) GET_ERROR_MESSAGE(get_parser_msg(X), (int)X)
#define GET_SEMANTIC_MESSAGE_0(X) GET_ERROR_MESSAGE(get_semantic_msg(X), (int)X)
#define GCC_VERSION __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__

#endif
