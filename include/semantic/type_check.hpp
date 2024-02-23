#ifndef _SEMANTIC_TYPE_CHECK_HPP
#define _SEMANTIC_TYPE_CHECK_HPP

#include "ast/symbol_table.hpp"
#include "ast/c_ast.hpp"

#include <inttypes.h>

bool is_same_type(Type* type_1, Type* type_2);
bool is_type_signed(Type* type_1);
int32_t get_type_size(Type* type_1);

void checktype_constant_expression(CConstant* node);
void checktype_var_expression(CVar* node);
void checktype_cast_expression(CCast* node);
void checktype_unary_expression(CUnary* node);
void checktype_binary_expression(CBinary* node);
void checktype_assignment_expression(CAssignment* node);
void checktype_conditional_expression(CConditional* node);
void checktype_function_call_expression(CFunctionCall* node);
void checktype_dereference_expression(CDereference* node);
void checktype_addrof_expression(CAddrOf* node);

void checktype_return_statement(CReturn* node);

void checktype_params(CFunctionDeclaration* node);
void checktype_function_declaration(CFunctionDeclaration* node);
void checktype_file_scope_variable_declaration(CVariableDeclaration* node);
void checktype_block_scope_variable_declaration(CVariableDeclaration* node);
void checktype_init_block_scope_variable_declaration(CVariableDeclaration* node);

#endif
