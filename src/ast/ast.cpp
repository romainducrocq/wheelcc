#include "ast/ast.hpp"

Ast::~Ast() {};

bool is_instance(Ast_t t1, Ast_t t2) {
    return t1 == t2;
}
