#ifndef _FRONTEND_PARSER_ERRORS_HPP
#define _FRONTEND_PARSER_ERRORS_HPP

#include <cstdio>
#include <string>

#include "util/throw.hpp"

#include "ast/ast.hpp"
#include "ast/front_ast.hpp"
#include "ast/front_symt.hpp"

#include "frontend/parser/lexer.hpp"
#include "messages.hpp" // frontend

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Errors

size_t handle_error_at_line(size_t line_number);
std::string get_token_kind_hr(TOKEN_KIND token_kind);
std::string get_name_hr(const TIdentifier& name);
std::string get_struct_name_hr(const TIdentifier& name);
std::string get_type_hr(Type* type);
std::string get_const_hr(CConst* node);
std::string get_storage_class_hr(CStorageClass* node);
std::string get_unary_op_hr(CUnaryOp* node);
std::string get_binary_op_hr(CBinaryOp* node);
std::string get_assignment_hr(CBinaryOp* node);
std::string get_error_message(ERROR_MESSAGE message);
template <typename... TArgs> inline std::string get_error_message(ERROR_MESSAGE message, TArgs&&... args) {
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), get_error_message(message).c_str(), em(std::forward<TArgs>(args)).c_str()...);
    return std::string(buffer);
}
#define GET_ERROR_MESSAGE(...) get_error_message(__VA_ARGS__)
#define RAISE_RUNTIME_ERROR_AT_LINE(X, Y) raise_runtime_error_at_line((X), handle_error_at_line(Y))

#endif
