#ifndef _FRONTEND_INTERMEDIATE_TAC_REPR_HPP
#define _FRONTEND_INTERMEDIATE_TAC_REPR_HPP

#include "ast/front_ast.hpp"
#include "ast/interm_ast.hpp"

#include <memory>
#include <vector>

struct TacReprContext {
    std::vector<std::unique_ptr<TacInstruction>>* p_instructions;
    std::vector<std::unique_ptr<TacTopLevel>>* p_top_levels;
    std::vector<std::unique_ptr<TacTopLevel>>* p_static_constant_top_levels;
};

std::unique_ptr<TacProgram> three_address_code_representation(std::unique_ptr<CProgram> c_ast);

#endif
