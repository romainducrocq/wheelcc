#ifndef _PARSER_PARSER_HPP
#define _PARSER_PARSER_HPP

#include "ast/c_ast.hpp"
#include "parser/lexer.hpp"

#include <memory>
#include <vector>

struct AbstractDeclarator {
    std::shared_ptr<Type> derived_type;
};

struct Declarator {
    TIdentifier name;
    std::shared_ptr<Type> derived_type;
    std::vector<TIdentifier> params;
};

std::unique_ptr<CProgram> parsing(std::vector<Token>&& tokens);

#endif
