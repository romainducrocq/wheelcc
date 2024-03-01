#include "backend/assembly/registers.hpp"

#include "util/error.hpp"
#include "ast/back_ast.hpp"

#include <memory>

// reg = AX | CX | DX | DI | SI | R8 | R9 | R10 | R11 | SP | XMM0 | XMM1 | XMM2 | XMM3 | XMM4 | XMM5 | XMM6 | XMM7
//     | XMM14 | XMM15
static std::unique_ptr<AsmReg> generate_reg(REGISTER_KIND register_kind) {
    switch(register_kind) {
        case REGISTER_KIND::Ax:
            return std::make_unique<AsmAx>();
        case REGISTER_KIND::Cx:
            return std::make_unique<AsmCx>();
        case REGISTER_KIND::Dx:
            return std::make_unique<AsmDx>();
        case REGISTER_KIND::Di:
            return std::make_unique<AsmDi>();
        case REGISTER_KIND::Si:
            return std::make_unique<AsmSi>();
        case REGISTER_KIND::R8:
            return std::make_unique<AsmR8>();
        case REGISTER_KIND::R9:
            return std::make_unique<AsmR9>();
        case REGISTER_KIND::R10:
            return std::make_unique<AsmR10>();
        case REGISTER_KIND::R11:
            return std::make_unique<AsmR11>();
        case REGISTER_KIND::Sp:
            return std::make_unique<AsmSp>();
        case REGISTER_KIND::Bp:
            return std::make_unique<AsmBp>();
        case REGISTER_KIND::Xmm0:
            return std::make_unique<AsmXMM0>();
        case REGISTER_KIND::Xmm1:
            return std::make_unique<AsmXMM1>();
        case REGISTER_KIND::Xmm2:
            return std::make_unique<AsmXMM2>();
        case REGISTER_KIND::Xmm3:
            return std::make_unique<AsmXMM3>();
        case REGISTER_KIND::Xmm4:
            return std::make_unique<AsmXMM4>();
        case REGISTER_KIND::Xmm5:
            return std::make_unique<AsmXMM5>();
        case REGISTER_KIND::Xmm6:
            return std::make_unique<AsmXMM6>();
        case REGISTER_KIND::Xmm7:
            return std::make_unique<AsmXMM7>();
        case REGISTER_KIND::Xmm14:
            return std::make_unique<AsmXMM14>();
        case REGISTER_KIND::Xmm15:
            return std::make_unique<AsmXMM15>();
        default:
            RAISE_INTERNAL_ERROR;
    }
}

std::shared_ptr<AsmRegister> generate_register(REGISTER_KIND register_kind) {
    std::unique_ptr<AsmReg> reg = generate_reg(register_kind);
    return std::make_shared<AsmRegister>(std::move(reg));
}

std::shared_ptr<AsmMemory> generate_memory(REGISTER_KIND register_kind, TInt value) {
    std::unique_ptr<AsmReg> reg = generate_reg(register_kind);
    return std::make_shared<AsmMemory>(std::move(value), std::move(reg));
}

std::shared_ptr<AsmIndexed> generate_indexed(REGISTER_KIND register_kind_base, REGISTER_KIND register_kind_index,
                                             TULong scale) {
    std::unique_ptr<AsmReg> reg_base = generate_reg(register_kind_base);
    std::unique_ptr<AsmReg> reg_index = generate_reg(register_kind_index);
    return std::make_shared<AsmIndexed>(std::move(scale), std::move(reg_base), std::move(reg_index));
}
