#ifndef _FRONT_PARSER_ERRORS_H
#define _FRONT_PARSER_ERRORS_H

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
    MSG_unhandled_fatal_error = 0,
    MSG_operating_system_not_supported,
    MSG_architecture_not_supported,
    MSG_compiler_not_supported,
    MSG_gcc_version_not_supported
};

enum MESSAGE_ARG {
    MSG_unhandled_argument_error = 100,
    MSG_no_debug_code_in_argument,
    MSG_invalid_debug_code_in_argument,
    MSG_no_optim_1_mask_in_argument,
    MSG_invalid_optim_1_mask_in_argument,
    MSG_no_optim_2_code_in_argument,
    MSG_invalid_optim_2_code_in_argument,
    MSG_no_input_files_in_argument,
    MSG_no_include_directories_in_argument
};

enum MESSAGE_UTIL {
    MSG_unhandled_util_error = 200,
    MSG_failed_to_read_input_file,
    MSG_failed_to_write_to_output_file,
    MSG_failed_to_interpret_string_to_integer,
    MSG_failed_to_interpret_string_to_unsigned_integer,
    MSG_failed_to_interpret_string_to_float
};

std::string fmt_tok_kind(TOKEN_KIND token_kind);
std::string fmt_name(TIdentifier name);
std::string fmt_struct_name(TIdentifier name, bool is_union);
std::string fmt_type(Type* type);
std::string fmt_const(CConst* node);
std::string fmt_storage_class(CStorageClass* node);
std::string fmt_unop(CUnaryOp* node);
std::string fmt_binop(CBinaryOp* node);
std::string fmt_assign(CBinaryOp* node, CUnaryOp* unary_op);
#define fmt_tok_kind_c_str(X) fmt_tok_kind(X).c_str()
#define fmt_name_c_str(X) fmt_name(X).c_str()
#define fmt_struct_name_c_str(X, Y) fmt_struct_name(X, Y).c_str()
#define fmt_type_c_str(X) fmt_type(X).c_str()
#define fmt_const_c_str(X) fmt_const(X).c_str()
#define fmt_storage_class_c_str(X) fmt_storage_class(X).c_str()
#define fmt_unop_c_str(X) fmt_unop(X).c_str()
#define fmt_binop_c_str(X) fmt_binop(X).c_str()
#define fmt_assign_c_str(X, Y) fmt_assign(X, Y).c_str()

const char* get_fatal_msg(MESSAGE_FATAL message);
const char* get_arg_msg(MESSAGE_ARG message);
const char* get_util_msg(MESSAGE_UTIL message);
const char* get_lexer_msg(MESSAGE_LEXER message);
const char* get_parser_msg(MESSAGE_PARSER message);
const char* get_semantic_msg(MESSAGE_SEMANTIC message);
#define GET_FATAL_MSG(X, ...) GET_ERROR_MSG(get_fatal_msg(X), (int)X, __VA_ARGS__)
#define GET_ARG_MSG(X, ...) GET_ERROR_MSG(get_arg_msg(X), (int)X, __VA_ARGS__)
#define GET_UTIL_MSG(X, ...) GET_ERROR_MSG(get_util_msg(X), (int)X, __VA_ARGS__)
#define GET_LEXER_MSG(X, ...) GET_ERROR_MSG(get_lexer_msg(X), (int)X, __VA_ARGS__)
#define GET_PARSER_MSG(X, ...) GET_ERROR_MSG(get_parser_msg(X), (int)X, __VA_ARGS__)
#define GET_SEMANTIC_MSG(X, ...) GET_ERROR_MSG(get_semantic_msg(X), (int)X, __VA_ARGS__)
#define GET_ARG_MSG_0(X) GET_ERROR_MSG(get_arg_msg(X), (int)X)
#define GET_PARSER_MSG_0(X) GET_ERROR_MSG(get_parser_msg(X), (int)X)
#define GET_SEMANTIC_MSG_0(X) GET_ERROR_MSG(get_semantic_msg(X), (int)X)
#define GCC_VERSION __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__

#endif
