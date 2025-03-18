#include <memory>

#include "util/throw.hpp"

#include "ast/back_ast.hpp"

#include "backend/assembly/registers.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Registers

// reg = AX | BX | CX | DX | DI | SI | R8 | R9 | R10 | R11 | R12 | R13 | R14 | R15 | SP | XMM0 | XMM1 | XMM2 | XMM3
//     | XMM4 | XMM5 | XMM6 | XMM7 | XMM8 | XMM9 | XMM10 | XMM11 | XMM12 | XMM13 | XMM14 | XMM15
static std::unique_ptr<AsmReg> generate_reg(REGISTER_KIND register_kind) {
    switch (register_kind) {
        case REGISTER_KIND::Ax:
            return std::make_unique<AsmAx>();
        case REGISTER_KIND::Bx:
            return std::make_unique<AsmBx>();
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
        case REGISTER_KIND::R12:
            return std::make_unique<AsmR12>();
        case REGISTER_KIND::R13:
            return std::make_unique<AsmR13>();
        case REGISTER_KIND::R14:
            return std::make_unique<AsmR14>();
        case REGISTER_KIND::R15:
            return std::make_unique<AsmR15>();
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
        case REGISTER_KIND::XMM8:
            return std::make_unique<AsmXMM8>();
        case REGISTER_KIND::XMM9:
            return std::make_unique<AsmXMM9>();
        case REGISTER_KIND::XMM10:
            return std::make_unique<AsmXMM10>();
        case REGISTER_KIND::XMM11:
            return std::make_unique<AsmXMM11>();
        case REGISTER_KIND::XMM12:
            return std::make_unique<AsmXMM12>();
        case REGISTER_KIND::XMM13:
            return std::make_unique<AsmXMM13>();
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

std::shared_ptr<AsmMemory> generate_memory(REGISTER_KIND register_kind, TLong value) {
    std::unique_ptr<AsmReg> reg = generate_reg(register_kind);
    return std::make_shared<AsmMemory>(std::move(value), std::move(reg));
}

std::shared_ptr<AsmIndexed> generate_indexed(
    REGISTER_KIND register_kind_base, REGISTER_KIND register_kind_index, TLong scale) {
    std::unique_ptr<AsmReg> reg_base = generate_reg(register_kind_base);
    std::unique_ptr<AsmReg> reg_index = generate_reg(register_kind_index);
    return std::make_shared<AsmIndexed>(std::move(scale), std::move(reg_base), std::move(reg_index));
}

bool register_mask_get(TULong register_mask, REGISTER_KIND register_kind) {
    return (register_mask & (static_cast<TULong>(1ul) << static_cast<size_t>(register_kind))) > 0;
}

void register_mask_set(TULong& register_mask, REGISTER_KIND register_kind, bool value) {
    if (value) {
        register_mask |= static_cast<TULong>(1ul) << static_cast<size_t>(register_kind);
    }
    else {
        register_mask &= ~(static_cast<TULong>(1ul) << static_cast<size_t>(register_kind));
    }
}
