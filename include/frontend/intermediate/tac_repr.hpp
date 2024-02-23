#ifndef _FRONTEND_INTERMEDIATE_TAC_REPR_HPP
#define _FRONTEND_INTERMEDIATE_TAC_REPR_HPP

#include "ast/front_ast.hpp"
#include "ast/interm_ast.hpp"

#include <memory>

std::unique_ptr<TacProgram> three_address_code_representation(std::unique_ptr<CProgram> c_ast);

#endif
