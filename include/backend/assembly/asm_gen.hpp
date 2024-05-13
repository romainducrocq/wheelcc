#ifndef _BACKEND_ASSEMBLY_ASM_GEN_HPP
#define _BACKEND_ASSEMBLY_ASM_GEN_HPP

#include "ast/interm_ast.hpp"
#include "ast/back_ast.hpp"
#include "backend/assembly/registers.hpp"

#include <memory>
#include <array>
#include <vector>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Assembly generation

enum STRUCT_8B_CLS {
    INTEGER,
    SSE,
    MEMORY
};

struct AsmGenContext {
    AsmGenContext();

    std::array<REGISTER_KIND, 6> ARG_REGISTERS;
    std::array<REGISTER_KIND, 8> ARG_SSE_REGISTERS;
    std::unordered_map<TIdentifier, std::vector<STRUCT_8B_CLS>> struct_8b_cls_map;
    std::vector<std::unique_ptr<AsmInstruction>>* p_instructions;
    std::vector<std::unique_ptr<AsmTopLevel>>* p_static_constant_top_levels;
};

std::unique_ptr<AsmProgram> assembly_generation(std::unique_ptr<TacProgram> tac_ast);

#endif
