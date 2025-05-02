#ifndef _FRONTEND_INTERMEDIATE_TAC_REPR_HPP
#define _FRONTEND_INTERMEDIATE_TAC_REPR_HPP

#include <memory>

struct CProgram;
struct TacProgram;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Three address code representation

std::unique_ptr<TacProgram> tac_representation(std::unique_ptr<CProgram> c_ast);

#endif
