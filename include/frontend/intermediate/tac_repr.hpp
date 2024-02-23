#ifndef _FRONTEND_INTERMEDIATE_TAC_REPR_HPP
#define _FRONTEND_INTERMEDIATE_TAC_REPR_HPP

#include "ast/c_ast.hpp"
#include "ast/tac_ast.hpp"

#include <memory>

std::unique_ptr<TacProgram> three_address_code_representation(std::unique_ptr<CProgram> c_ast);

#endif
