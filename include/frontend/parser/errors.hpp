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
    MSG_unsupported_os,
    MSG_unsupported_arch,
    MSG_unsupported_compiler,
    MSG_unsupported_gcc_ver
};

enum MESSAGE_ARG {
    MSG_unhandled_arg_error = 100,
    MSG_no_debug_arg,
    MSG_invalid_debug_arg,
    MSG_no_optim_1_arg,
    MSG_invalid_optim_1_arg,
    MSG_no_optim_2_arg,
    MSG_invalid_optim_2_arg,
    MSG_no_input_files_arg,
    MSG_no_include_dir_arg
};

enum MESSAGE_UTIL {
    MSG_unhandled_util_error = 200,
    MSG_failed_fread,
    MSG_failed_fwrite,
    MSG_failed_strtoi,
    MSG_failed_strtou,
    MSG_failed_strtod
};

std::string get_tok_kind_fmt(TOKEN_KIND token_kind);
std::string get_name_fmt(TIdentifier name);
std::string get_struct_name_fmt(TIdentifier name, bool is_union);
std::string get_type_fmt(Type* type);
std::string get_const_fmt(CConst* node);
std::string get_storage_class_fmt(CStorageClass* node);
std::string get_unop_fmt(CUnaryOp* node);
std::string get_binop_fmt(CBinaryOp* node);
std::string get_assign_fmt(CBinaryOp* node, CUnaryOp* unary_op);
#define fmt_tok_kind_c_str(X) get_tok_kind_fmt(X).c_str()
#define fmt_name_c_str(X) get_name_fmt(X).c_str()
#define fmt_struct_name_c_str(X, Y) get_struct_name_fmt(X, Y).c_str()
#define fmt_type_c_str(X) get_type_fmt(X).c_str()
#define fmt_const_c_str(X) get_const_fmt(X).c_str()
#define fmt_storage_class_c_str(X) get_storage_class_fmt(X).c_str()
#define fmt_unop_c_str(X) get_unop_fmt(X).c_str()
#define fmt_binop_c_str(X) get_binop_fmt(X).c_str()
#define fmt_assign_c_str(X, Y) get_assign_fmt(X, Y).c_str()

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
