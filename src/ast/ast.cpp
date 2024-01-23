#include "ast/ast.hpp"

Ast::~Ast() {};

bool is_instance(AST_T t1, AST_T t2) {
    return t1 == t2;
}
