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
        case REGISTER_KIND::Xmm8:
            return std::make_unique<AsmXMM8>();
        case REGISTER_KIND::Xmm9:
            return std::make_unique<AsmXMM9>();
        case REGISTER_KIND::Xmm10:
            return std::make_unique<AsmXMM10>();
        case REGISTER_KIND::Xmm11:
            return std::make_unique<AsmXMM11>();
        case REGISTER_KIND::Xmm12:
            return std::make_unique<AsmXMM12>();
        case REGISTER_KIND::Xmm13:
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

REGISTER_KIND register_mask_kind(AsmRegister* node) {
    switch (node->reg->type()) {
        case AST_T::AsmAx_t:
            return REGISTER_KIND::Ax;
        case AST_T::AsmBx_t:
            return REGISTER_KIND::Bx;
        case AST_T::AsmCx_t:
            return REGISTER_KIND::Cx;
        case AST_T::AsmDx_t:
            return REGISTER_KIND::Dx;
        case AST_T::AsmDi_t:
            return REGISTER_KIND::Di;
        case AST_T::AsmSi_t:
            return REGISTER_KIND::Si;
        case AST_T::AsmR8_t:
            return REGISTER_KIND::R8;
        case AST_T::AsmR9_t:
            return REGISTER_KIND::R9;
        case AST_T::AsmR12_t:
            return REGISTER_KIND::R12;
        case AST_T::AsmR13_t:
            return REGISTER_KIND::R13;
        case AST_T::AsmR14_t:
            return REGISTER_KIND::R14;
        case AST_T::AsmR15_t:
            return REGISTER_KIND::R15;
        case AST_T::AsmXMM0_t:
            return REGISTER_KIND::Xmm0;
        case AST_T::AsmXMM1_t:
            return REGISTER_KIND::Xmm1;
        case AST_T::AsmXMM2_t:
            return REGISTER_KIND::Xmm2;
        case AST_T::AsmXMM3_t:
            return REGISTER_KIND::Xmm3;
        case AST_T::AsmXMM4_t:
            return REGISTER_KIND::Xmm4;
        case AST_T::AsmXMM5_t:
            return REGISTER_KIND::Xmm5;
        case AST_T::AsmXMM6_t:
            return REGISTER_KIND::Xmm6;
        case AST_T::AsmXMM7_t:
            return REGISTER_KIND::Xmm7;
        case AST_T::AsmXMM8_t:
            return REGISTER_KIND::Xmm8;
        case AST_T::AsmXMM9_t:
            return REGISTER_KIND::Xmm9;
        case AST_T::AsmXMM10_t:
            return REGISTER_KIND::Xmm10;
        case AST_T::AsmXMM11_t:
            return REGISTER_KIND::Xmm11;
        case AST_T::AsmXMM12_t:
            return REGISTER_KIND::Xmm12;
        case AST_T::AsmXMM13_t:
            return REGISTER_KIND::Xmm13;
        case AST_T::AsmR10_t:
        case AST_T::AsmR11_t:
        case AST_T::AsmSp_t:
        case AST_T::AsmBp_t:
        case AST_T::AsmXMM14_t:
        case AST_T::AsmXMM15_t:
            RAISE_INTERNAL_ERROR;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

size_t register_mask_bit(REGISTER_KIND register_kind) {
    switch (register_kind) {
        case REGISTER_KIND::Ax:
            return 0;
        case REGISTER_KIND::Bx:
            return 1;
        case REGISTER_KIND::Cx:
            return 2;
        case REGISTER_KIND::Dx:
            return 3;
        case REGISTER_KIND::Di:
            return 4;
        case REGISTER_KIND::Si:
            return 5;
        case REGISTER_KIND::R8:
            return 6;
        case REGISTER_KIND::R9:
            return 7;
        case REGISTER_KIND::R12:
            return 8;
        case REGISTER_KIND::R13:
            return 9;
        case REGISTER_KIND::R14:
            return 10;
        case REGISTER_KIND::R15:
            return 11;
        case REGISTER_KIND::Xmm0:
            return 12;
        case REGISTER_KIND::Xmm1:
            return 13;
        case REGISTER_KIND::Xmm2:
            return 14;
        case REGISTER_KIND::Xmm3:
            return 15;
        case REGISTER_KIND::Xmm4:
            return 16;
        case REGISTER_KIND::Xmm5:
            return 17;
        case REGISTER_KIND::Xmm6:
            return 18;
        case REGISTER_KIND::Xmm7:
            return 19;
        case REGISTER_KIND::Xmm8:
            return 20;
        case REGISTER_KIND::Xmm9:
            return 21;
        case REGISTER_KIND::Xmm10:
            return 22;
        case REGISTER_KIND::Xmm11:
            return 23;
        case REGISTER_KIND::Xmm12:
            return 24;
        case REGISTER_KIND::Xmm13:
            return 25;
        case REGISTER_KIND::R10:
        case REGISTER_KIND::R11:
        case REGISTER_KIND::Sp:
        case REGISTER_KIND::Bp:
        case REGISTER_KIND::Xmm14:
        case REGISTER_KIND::Xmm15:
            RAISE_INTERNAL_ERROR;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

bool register_mask_get(TULong register_mask, REGISTER_KIND register_kind) {
    return (register_mask & (static_cast<TULong>(1ul) << register_mask_bit(register_kind))) > 0;
}

void register_mask_set(TULong& register_mask, REGISTER_KIND register_kind, bool value) {
    if (value) {
        register_mask |= static_cast<TULong>(1ul) << register_mask_bit(register_kind);
    }
    else {
        register_mask &= ~(static_cast<TULong>(1ul) << register_mask_bit(register_kind));
    }
}
