#ifndef _FRONTEND_PARSER_PARSER_HPP
#define _FRONTEND_PARSER_PARSER_HPP

#include "ast/front_ast.hpp"
#include "frontend/parser/lexer.hpp"

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

struct ParserContext {
    ParserContext(std::vector<Token>* p_tokens);

    std::vector<Token>* p_tokens;
    Token* next_token;
    Token* peek_token;
    size_t pop_index;
};

std::unique_ptr<CProgram> parsing(std::unique_ptr<std::vector<Token>> tokens);

#endif
