#ifndef _INTERMEDIATE_TAC_GEN_HPP
#define _INTERMEDIATE_TAC_GEN_HPP

#include "ast/c_ast.hpp"
#include "ast/tac_ast.hpp"

#include <memory>

std::unique_ptr<TacProgram> three_address_code_representation(std::unique_ptr<CProgram> c_ast);

#endif
