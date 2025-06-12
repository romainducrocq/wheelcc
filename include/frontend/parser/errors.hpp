#ifndef _FRONT_PARSER_ERRORS_H
#define _FRONT_PARSER_ERRORS_H

#include <cstdio>
#include <string>

#include "ast_t.hpp" // ast

#include "parser/messages.hpp" // frontend
#include "parser/tokens.hpp"   // frontend

#include "intermediate/messages.hpp" // frontend

struct Token;
struct Type;
struct CConst;
struct CUnaryOp;
struct CBinaryOp;
struct CStorageClass;
struct IdentifierContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Errors

enum MESSAGE_FATAL {
    MSG_unhandled_fatal_error = 0,
    MSG_unsupported_os,
    MSG_unsupported_arch,
    MSG_unsupported_compiler,
    MSG_unsupported_cc_ver
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

const char* get_tok_kind_fmt(TOKEN_KIND tok_kind);
const char* get_tok_fmt(IdentifierContext* ctx, Token* token);
const char* get_const_fmt(CConst* node);
const char* get_storage_class_fmt(CStorageClass* node);
const char* get_unop_fmt(CUnaryOp* node);
const char* get_binop_fmt(CBinaryOp* node);
const char* get_assign_fmt(CBinaryOp* node, CUnaryOp* unop);
std::string get_name_fmt(IdentifierContext* ctx, TIdentifier name);
std::string get_struct_name_fmt(IdentifierContext* ctx, TIdentifier name, bool is_union);
std::string get_type_fmt(IdentifierContext* ctx, Type* type);
#define fmt_name_c_str(X) get_name_fmt(ctx->identifiers, X).c_str()
#define fmt_struct_name_c_str(X, Y) get_struct_name_fmt(ctx->identifiers, X, Y).c_str()
#define fmt_type_c_str(X) get_type_fmt(ctx->identifiers, X).c_str()

const char* get_fatal_msg(MESSAGE_FATAL msg);
const char* get_arg_msg(MESSAGE_ARG msg);
const char* get_util_msg(MESSAGE_UTIL msg);
const char* get_lexer_msg(MESSAGE_LEXER msg);
const char* get_parser_msg(MESSAGE_PARSER msg);
const char* get_semantic_msg(MESSAGE_SEMANTIC msg);
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
#define CLANG_VERSION __clang_major__, __clang_minor__, __clang_patchlevel__

#endif
