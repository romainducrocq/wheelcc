#include <memory>
#include <string>
#include <vector>

#include "util/fileio.hpp"
#include "util/throw.hpp"

#include "ast/ast.hpp"
#include "ast/back_ast.hpp"
#include "ast/back_symt.hpp"

#include "backend/emitter/gas_code.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Gnu assembler code emission

// identifier -> $ identifier
static const std::string& emit_identifier(TIdentifier identifier) { return identifiers->hash_table[identifier]; }

// string -> $ string
static const std::string& emit_string(TIdentifier string_const) { return identifiers->hash_table[string_const]; }

// char -> $ char
static std::string emit_char(TChar value) { return std::to_string(value); }

// int -> $ int
static std::string emit_int(TInt value) { return std::to_string(value); }

// long -> $ long
static std::string emit_long(TLong value) { return std::to_string(value); }

// double -> $ double
static std::string emit_dbl(TIdentifier dbl_const) { return identifiers->hash_table[dbl_const]; }

// uchar -> $ uchar
static std::string emit_uchar(TUChar value) { return std::to_string(value); }

// uint -> $ uint
static std::string emit_uint(TUInt value) { return std::to_string(value); }

// ulong -> $ ulong
static std::string emit_ulong(TULong value) { return std::to_string(value); }

// Reg(SP)    -> $ %rsp
// Reg(BP)    -> $ %rbp
// Reg(XMM0)  -> $ %xmm0
// Reg(XMM1)  -> $ %xmm1
// Reg(XMM2)  -> $ %xmm2
// Reg(XMM3)  -> $ %xmm3
// Reg(XMM4)  -> $ %xmm4
// Reg(XMM5)  -> $ %xmm5
// Reg(XMM6)  -> $ %xmm6
// Reg(XMM7)  -> $ %xmm7
// Reg(XMM8)  -> $ %xmm8
// Reg(XMM9)  -> $ %xmm9
// Reg(XMM10) -> $ %xmm10
// Reg(XMM11) -> $ %xmm11
// Reg(XMM12) -> $ %xmm12
// Reg(XMM13) -> $ %xmm13
// Reg(XMM14) -> $ %xmm14
// Reg(XMM15) -> $ %xmm15
static std::string reg_rsp_sse(AsmReg* node) {
    switch (node->type()) {
        case AST_AsmSp_t:
            return "rsp";
        case AST_AsmBp_t:
            return "rbp";
        case AST_AsmXMM0_t:
            return "xmm0";
        case AST_AsmXMM1_t:
            return "xmm1";
        case AST_AsmXMM2_t:
            return "xmm2";
        case AST_AsmXMM3_t:
            return "xmm3";
        case AST_AsmXMM4_t:
            return "xmm4";
        case AST_AsmXMM5_t:
            return "xmm5";
        case AST_AsmXMM6_t:
            return "xmm6";
        case AST_AsmXMM7_t:
            return "xmm7";
        case AST_AsmXMM8_t:
            return "xmm8";
        case AST_AsmXMM9_t:
            return "xmm9";
        case AST_AsmXMM10_t:
            return "xmm10";
        case AST_AsmXMM11_t:
            return "xmm11";
        case AST_AsmXMM12_t:
            return "xmm12";
        case AST_AsmXMM13_t:
            return "xmm13";
        case AST_AsmXMM14_t:
            return "xmm14";
        case AST_AsmXMM15_t:
            return "xmm15";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

// Reg(AX)  -> $ %al
// Reg(BX)  -> $ %bl
// Reg(DX)  -> $ %dl
// Reg(CX)  -> $ %cl
// Reg(DI)  -> $ %dil
// Reg(SI)  -> $ %sil
// Reg(R8)  -> $ %r8b
// Reg(R9)  -> $ %r9b
// Reg(R10) -> $ %r10b
// Reg(R11) -> $ %r11b
// Reg(R12) -> $ %r12b
// Reg(R13) -> $ %r13b
// Reg(R14) -> $ %r14b
// Reg(R15) -> $ %r15b
static std::string emit_reg_1b(AsmReg* node) {
    switch (node->type()) {
        case AST_AsmAx_t:
            return "al";
        case AST_AsmBx_t:
            return "bl";
        case AST_AsmDx_t:
            return "dl";
        case AST_AsmCx_t:
            return "cl";
        case AST_AsmDi_t:
            return "dil";
        case AST_AsmSi_t:
            return "sil";
        case AST_AsmR8_t:
            return "r8b";
        case AST_AsmR9_t:
            return "r9b";
        case AST_AsmR10_t:
            return "r10b";
        case AST_AsmR11_t:
            return "r11b";
        case AST_AsmR12_t:
            return "r12b";
        case AST_AsmR13_t:
            return "r13b";
        case AST_AsmR14_t:
            return "r14b";
        case AST_AsmR15_t:
            return "r15b";
        default:
            return reg_rsp_sse(node);
    }
}

// Reg(AX)  -> $ %eax
// Reg(BX)  -> $ %ebx
// Reg(DX)  -> $ %edx
// Reg(CX)  -> $ %ecx
// Reg(DI)  -> $ %edi
// Reg(SI)  -> $ %esi
// Reg(R8)  -> $ %r8d
// Reg(R9)  -> $ %r9d
// Reg(R10) -> $ %r10d
// Reg(R11) -> $ %r11d
// Reg(R12) -> $ %r12d
// Reg(R13) -> $ %r13d
// Reg(R14) -> $ %r14d
// Reg(R15) -> $ %r15d
static std::string emit_reg_4b(AsmReg* node) {
    switch (node->type()) {
        case AST_AsmAx_t:
            return "eax";
        case AST_AsmBx_t:
            return "ebx";
        case AST_AsmDx_t:
            return "edx";
        case AST_AsmCx_t:
            return "ecx";
        case AST_AsmDi_t:
            return "edi";
        case AST_AsmSi_t:
            return "esi";
        case AST_AsmR8_t:
            return "r8d";
        case AST_AsmR9_t:
            return "r9d";
        case AST_AsmR10_t:
            return "r10d";
        case AST_AsmR11_t:
            return "r11d";
        case AST_AsmR12_t:
            return "r12d";
        case AST_AsmR13_t:
            return "r13d";
        case AST_AsmR14_t:
            return "r14d";
        case AST_AsmR15_t:
            return "r15d";
        default:
            return reg_rsp_sse(node);
    }
}

// Reg(AX)  -> $ %rax
// Reg(BX)  -> $ %rbx
// Reg(DX)  -> $ %rdx
// Reg(CX)  -> $ %rcx
// Reg(DI)  -> $ %rdi
// Reg(SI)  -> $ %rsi
// Reg(R8)  -> $ %r8
// Reg(R9)  -> $ %r9
// Reg(R10) -> $ %r10
// Reg(R11) -> $ %r11
// Reg(R12) -> $ %r12
// Reg(R13) -> $ %r13
// Reg(R14) -> $ %r14
// Reg(R15) -> $ %r15
static std::string emit_reg_8b(AsmReg* node) {
    switch (node->type()) {
        case AST_AsmAx_t:
            return "rax";
        case AST_AsmBx_t:
            return "rbx";
        case AST_AsmDx_t:
            return "rdx";
        case AST_AsmCx_t:
            return "rcx";
        case AST_AsmDi_t:
            return "rdi";
        case AST_AsmSi_t:
            return "rsi";
        case AST_AsmR8_t:
            return "r8";
        case AST_AsmR9_t:
            return "r9";
        case AST_AsmR10_t:
            return "r10";
        case AST_AsmR11_t:
            return "r11";
        case AST_AsmR12_t:
            return "r12";
        case AST_AsmR13_t:
            return "r13";
        case AST_AsmR14_t:
            return "r14";
        case AST_AsmR15_t:
            return "r15";
        default:
            return reg_rsp_sse(node);
    }
}

// E  -> $ e
// NE -> $ ne
// L  -> $ l
// LE -> $ le
// G  -> $ g
// GE -> $ ge
// B  -> $ b
// BE -> $ be
// A  -> $ a
// AE -> $ ae
// P  -> $ p
static std::string emit_cond_code(AsmCondCode* node) {
    switch (node->type()) {
        case AST_AsmE_t:
            return "e";
        case AST_AsmNE_t:
            return "ne";
        case AST_AsmL_t:
            return "l";
        case AST_AsmLE_t:
            return "le";
        case AST_AsmG_t:
            return "g";
        case AST_AsmGE_t:
            return "ge";
        case AST_AsmB_t:
            return "b";
        case AST_AsmBE_t:
            return "be";
        case AST_AsmA_t:
            return "a";
        case AST_AsmAE_t:
            return "ae";
        case AST_AsmP_t:
            return "p";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

// Byte      -> $ 1
// LongWord  -> $ 4
// QuadWord  -> $ 8
// Double    -> $ 8
// ByteArray -> $ alignment
static TInt type_align_bytes(AssemblyType* node) {
    switch (node->type()) {
        case AST_Byte_t:
            return 1;
        case AST_LongWord_t:
            return 4;
        case AST_QuadWord_t:
        case AST_BackendDouble_t:
            return 8;
        case AST_ByteArray_t:
            return static_cast<ByteArray*>(node)->alignment;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

// Byte             -> $ b
// LongWord         -> $ l
// QuadWord         -> $ q
// Double if packed -> $ pd
//             else -> $ sd
static std::string emit_type_suffix(AssemblyType* node, bool is_packed) {
    switch (node->type()) {
        case AST_Byte_t:
            return "b";
        case AST_LongWord_t:
            return "l";
        case AST_QuadWord_t:
            return "q";
        case AST_BackendDouble_t:
            return is_packed ? "pd" : "sd";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::string imm_op(AsmImm* node) {
    std::string operand = "$";
    if (node->is_neg) {
        operand += emit_long(static_cast<TLong>(node->value));
    }
    else {
        operand += emit_ulong(node->value);
    }
    return operand;
}

static std::string reg_op(AsmRegister* node, TInt byte) {
    std::string operand = "%";
    switch (byte) {
        case 1:
            operand += emit_reg_1b(node->reg.get());
            break;
        case 4:
            operand += emit_reg_4b(node->reg.get());
            break;
        case 8:
            operand += emit_reg_8b(node->reg.get());
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
    return operand;
}

static std::string memory_op(AsmMemory* node) {
    std::string operand = node->value != 0l ? emit_long(node->value) : "";
    operand += "(%";
    operand += emit_reg_8b(node->reg.get());
    operand += ")";
    return operand;
}

static std::string data_op(AsmData* node) {
    std::string operand = backend->symbol_table.find(node->name) != backend->symbol_table.end()
                                  && backend->symbol_table[node->name]->type() == AST_BackendObj_t
                                  && static_cast<BackendObj*>(backend->symbol_table[node->name].get())->is_const ?
                              ".L" :
                              "";
    operand += emit_identifier(node->name);
    if (node->offset != 0l) {
        operand += "+";
        operand += emit_long(node->offset);
    }
    operand += "(%rip)";
    return operand;
}

static std::string indexed_op(AsmIndexed* node) {
    std::string operand = "(%";
    operand += emit_reg_8b(node->reg_base.get());
    operand += ", %";
    operand += emit_reg_8b(node->reg_index.get());
    operand += ", ";
    operand += emit_long(node->scale);
    operand += ")";
    return operand;
}

// Imm(int)                 -> $ $<int>
// Register(reg)            -> $ %reg
// Memory(int, reg)         -> $ <int>(<reg>)
// Data(identifier, int)    -> $ <identifier>+<int>(%rip)
// Indexed(reg1, reg2, int) -> $ (<reg1>, <reg2>, <int>)
static std::string emit_op(AsmOperand* node, TInt byte) {
    switch (node->type()) {
        case AST_AsmImm_t:
            return imm_op(static_cast<AsmImm*>(node));
        case AST_AsmRegister_t:
            return reg_op(static_cast<AsmRegister*>(node), byte);
        case AST_AsmMemory_t:
            return memory_op(static_cast<AsmMemory*>(node));
        case AST_AsmData_t:
            return data_op(static_cast<AsmData*>(node));
        case AST_AsmIndexed_t:
            return indexed_op(static_cast<AsmIndexed*>(node));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

// Neg -> $ neg
// Not -> $ not
// Shr -> $ shr
static std::string emit_unop(AsmUnaryOp* node) {
    switch (node->type()) {
        case AST_AsmNeg_t:
            return "neg";
        case AST_AsmNot_t:
            return "not";
        case AST_AsmShr_t:
            return "shr";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

// Add              -> $ add
// Sub              -> $ sub
// Mult<i>          -> $ imul
// Mult<d>          -> $ mul
// DivDouble        -> $ div
// BitAnd           -> $ and
// BitOr            -> $ or
// BitXor           -> $ xor
// BitShiftLeft     -> $ shl
// BitShiftRight    -> $ shr
// BitShrArithmetic -> $ sar
static std::string emit_binop(AsmBinaryOp* node, bool is_dbl) {
    switch (node->type()) {
        case AST_AsmAdd_t:
            return "add";
        case AST_AsmSub_t:
            return "sub";
        case AST_AsmMult_t:
            return is_dbl ? "mul" : "imul";
        case AST_AsmDivDouble_t:
            return "div";
        case AST_AsmBitAnd_t:
            return "and";
        case AST_AsmBitOr_t:
            return "or";
        case AST_AsmBitXor_t:
            return "xor";
        case AST_AsmBitShiftLeft_t:
            return "shl";
        case AST_AsmBitShiftRight_t:
            return "shr";
        case AST_AsmBitShrArithmetic_t:
            return "sar";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void emit(std::string&& line, size_t t) {
    if (t > 0) {
        std::string indent_line = "    ";
        while (t > 1) {
            indent_line += "    ";
            t--;
        }
        indent_line += line;
        emit(std::move(indent_line), 0);
    }
    else {
        write_line(std::move(line));
    }
}

static void mov_instr(AsmMov* node) {
    std::string instr = "mov";
    instr += emit_type_suffix(node->asm_type.get(), false);
    instr += " ";
    {
        TInt byte = type_align_bytes(node->asm_type.get());
        instr += emit_op(node->src.get(), byte);
        instr += ", ";
        instr += emit_op(node->dst.get(), byte);
    }
    emit(std::move(instr), 2);
}

static void mov_sx_instr(AsmMovSx* node) {
    std::string instr = "movs";
    instr += emit_type_suffix(node->asm_type_src.get(), false);
    instr += emit_type_suffix(node->asm_type_dst.get(), false);
    instr += " ";
    {
        TInt byte = type_align_bytes(node->asm_type_src.get());
        instr += emit_op(node->src.get(), byte);
    }
    instr += ", ";
    {
        TInt byte = type_align_bytes(node->asm_type_dst.get());
        instr += emit_op(node->dst.get(), byte);
    }
    emit(std::move(instr), 2);
}

static void zero_extend_instr(AsmMovZeroExtend* node) {
    std::string instr = "movzb";
    instr += emit_type_suffix(node->asm_type_dst.get(), false);
    instr += " ";
    instr += emit_op(node->src.get(), 1);
    instr += ", ";
    {
        TInt byte = type_align_bytes(node->asm_type_dst.get());
        instr += emit_op(node->dst.get(), byte);
    }
    emit(std::move(instr), 2);
}

static void lea_instr(AsmLea* node) {
    std::string instr = "leaq ";
    instr += emit_op(node->src.get(), 8);
    instr += ", ";
    instr += emit_op(node->dst.get(), 8);
    emit(std::move(instr), 2);
}

static void cvttsd2si_instr(AsmCvttsd2si* node) {
    std::string instr = "cvttsd2si";
    instr += emit_type_suffix(node->asm_type.get(), false);
    instr += " ";
    {
        TInt byte = type_align_bytes(node->asm_type.get());
        instr += emit_op(node->src.get(), byte);
        instr += ", ";
        instr += emit_op(node->dst.get(), byte);
    }
    emit(std::move(instr), 2);
}

static void cvtsi2sd_instr(AsmCvtsi2sd* node) {
    std::string instr = "cvtsi2sd";
    instr += emit_type_suffix(node->asm_type.get(), false);
    instr += " ";
    {
        TInt byte = type_align_bytes(node->asm_type.get());
        instr += emit_op(node->src.get(), byte);
        instr += ", ";
        instr += emit_op(node->dst.get(), byte);
    }
    emit(std::move(instr), 2);
}

static void unary_instr(AsmUnary* node) {
    std::string instr = emit_unop(node->unop.get());
    instr += emit_type_suffix(node->asm_type.get(), false);
    instr += " ";
    {
        TInt byte = type_align_bytes(node->asm_type.get());
        instr += emit_op(node->dst.get(), byte);
    }
    emit(std::move(instr), 2);
}

static void binary_instr(AsmBinary* node) {
    bool is_dbl = node->asm_type->type() == AST_BackendDouble_t;
    std::string instr = emit_binop(node->binop.get(), is_dbl);
    {
        bool is_packed = node->binop->type() == AST_AsmBitXor_t && is_dbl;
        instr += emit_type_suffix(node->asm_type.get(), is_packed);
    }
    instr += " ";
    {
        TInt byte = type_align_bytes(node->asm_type.get());
        instr += emit_op(node->src.get(), byte);
        instr += ", ";
        instr += emit_op(node->dst.get(), byte);
    }
    emit(std::move(instr), 2);
}

static void cmp_instr(AsmCmp* node) {
    std::string instr = node->asm_type->type() == AST_BackendDouble_t ? "comi" : "cmp";
    instr += emit_type_suffix(node->asm_type.get(), false);
    instr += " ";
    {
        TInt byte = type_align_bytes(node->asm_type.get());
        instr += emit_op(node->src.get(), byte);
        instr += ", ";
        instr += emit_op(node->dst.get(), byte);
    }
    emit(std::move(instr), 2);
}

static void idiv_instr(AsmIdiv* node) {
    std::string instr = "idiv";
    instr += emit_type_suffix(node->asm_type.get(), false);
    instr += " ";
    {
        TInt byte = type_align_bytes(node->asm_type.get());
        instr += emit_op(node->src.get(), byte);
    }
    emit(std::move(instr), 2);
}

static void div_instr(AsmDiv* node) {
    std::string instr = "div";
    instr += emit_type_suffix(node->asm_type.get(), false);
    instr += " ";
    {
        TInt byte = type_align_bytes(node->asm_type.get());
        instr += emit_op(node->src.get(), byte);
    }
    emit(std::move(instr), 2);
}

static void cdq_instr(AsmCdq* node) {
    switch (node->asm_type->type()) {
        case AST_LongWord_t:
            emit("cdq", 2);
            break;
        case AST_QuadWord_t:
            emit("cqo", 2);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void jmp_instr(AsmJmp* node) {
    std::string instr = "jmp .L";
    instr += emit_identifier(node->target);
    emit(std::move(instr), 2);
}

static void jmp_cc_instr(AsmJmpCC* node) {
    std::string instr = "j";
    instr += emit_cond_code(node->cond_code.get());
    instr += " .L";
    instr += emit_identifier(node->target);
    emit(std::move(instr), 2);
}

static void set_cc_instr(AsmSetCC* node) {
    std::string instr = "set";
    instr += emit_cond_code(node->cond_code.get());
    instr += " ";
    instr += emit_op(node->dst.get(), 1);
    emit(std::move(instr), 2);
}

static void label_instr(AsmLabel* node) {
    std::string instr = ".L";
    instr += emit_identifier(node->name);
    instr += ":";
    emit(std::move(instr), 1);
}

static void push_instr(AsmPush* node) {
    std::string instr = "pushq ";
    instr += emit_op(node->src.get(), 8);
    emit(std::move(instr), 2);
}

static void pop_instr(AsmPop* node) {
    std::string instr = "popq %";
    instr += emit_reg_8b(node->reg.get());
    emit(std::move(instr), 2);
}

static void call_instr(AsmCall* node) {
    std::string instr = "call ";
    instr += emit_identifier(node->name);
    if (backend->symbol_table[node->name]->type() != AST_BackendFun_t) {
        RAISE_INTERNAL_ERROR;
    }
    else if (!static_cast<BackendFun*>(backend->symbol_table[node->name].get())->is_def) {
        instr += "@PLT";
    }
    emit(std::move(instr), 2);
}

static void ret_instr() {
    emit("movq %rbp, %rsp", 1);
    emit("popq %rbp", 1);
    emit("ret", 1);
}

// Mov(t, src, dst)                      -> $ mov<t> <src>, <dst>
// Movsx(src_t, dst_t, src, dst)         -> $ movs<src_t><dst_t> <src>, <dst>
// MovZeroExtend(src_t, dst_t, src, dst) -> $ movz<src_t><dst_t> <src>, <dst>
// Lea(src, dst)                         -> $ leaq <src>, <dst>
// Cvttsd2si(t, src, dst)                -> $ cvttsd2si<t> <src>, <dst>
// Cvtsi2sd(t, src, dst)                 -> $ cvtsi2sd<t> <src>, <dst>
// Unary(unary_operator, t, operand)     -> $ <unary_operator><t> <operand>
// Binary(binary_operator, t, src, dst)  -> $ <binary_operator><t> <src>, <dst>
// Cmp(t, operand, operand)<i>           -> $ cmp<t> <operand>, <operand>
// Cmp(operand, operand)<d>              -> $ comisd <operand>, <operand>
// Idiv(t, operand)                      -> $ idiv<t> <operand>
// Div(t, operand)                       -> $ div<t> <operand>
// Cdq<l>                                -> $ cdq
// Cdq<q>                                -> $ cqo
// Jmp(label)                            -> $ jmp .L<label>
// JmpCC(cond_code, label)               -> $ j<cond_code> .L<label>
// SetCC(cond_code, operand)             -> $ set<cond_code> <operand>
// Label(label)                          -> $ .L<label>:
// Push(operand)                         -> $ pushq <operand>
// Pop(reg)                              -> $ popq <reg>
// Call(label)                           -> $ call <label>@PLT
// Ret                                   -> $ movq %rbp, %rsp
//                                          $ popq %rbp
//                                          $ ret
static void emit_instr(AsmInstruction* node) {
    switch (node->type()) {
        case AST_AsmMov_t:
            mov_instr(static_cast<AsmMov*>(node));
            break;
        case AST_AsmMovSx_t:
            mov_sx_instr(static_cast<AsmMovSx*>(node));
            break;
        case AST_AsmMovZeroExtend_t:
            zero_extend_instr(static_cast<AsmMovZeroExtend*>(node));
            break;
        case AST_AsmLea_t:
            lea_instr(static_cast<AsmLea*>(node));
            break;
        case AST_AsmCvttsd2si_t:
            cvttsd2si_instr(static_cast<AsmCvttsd2si*>(node));
            break;
        case AST_AsmCvtsi2sd_t:
            cvtsi2sd_instr(static_cast<AsmCvtsi2sd*>(node));
            break;
        case AST_AsmUnary_t:
            unary_instr(static_cast<AsmUnary*>(node));
            break;
        case AST_AsmBinary_t:
            binary_instr(static_cast<AsmBinary*>(node));
            break;
        case AST_AsmCmp_t:
            cmp_instr(static_cast<AsmCmp*>(node));
            break;
        case AST_AsmIdiv_t:
            idiv_instr(static_cast<AsmIdiv*>(node));
            break;
        case AST_AsmDiv_t:
            div_instr(static_cast<AsmDiv*>(node));
            break;
        case AST_AsmCdq_t:
            cdq_instr(static_cast<AsmCdq*>(node));
            break;
        case AST_AsmJmp_t:
            jmp_instr(static_cast<AsmJmp*>(node));
            break;
        case AST_AsmJmpCC_t:
            jmp_cc_instr(static_cast<AsmJmpCC*>(node));
            break;
        case AST_AsmSetCC_t:
            set_cc_instr(static_cast<AsmSetCC*>(node));
            break;
        case AST_AsmLabel_t:
            label_instr(static_cast<AsmLabel*>(node));
            break;
        case AST_AsmPush_t:
            push_instr(static_cast<AsmPush*>(node));
            break;
        case AST_AsmPop_t:
            pop_instr(static_cast<AsmPop*>(node));
            break;
        case AST_AsmCall_t:
            call_instr(static_cast<AsmCall*>(node));
            break;
        case AST_AsmRet_t:
            ret_instr();
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void emit_instr_list(const std::vector<std::unique_ptr<AsmInstruction>>& node_list) {
    for (size_t i = node_list[0] ? 0 : 1; i < node_list.size(); ++i) {
        emit_instr(node_list[i].get());
    }
}

// $ .balign <alignment>
static void align_directive_toplvl(TInt alignment) {
    if (alignment > 1) {
        std::string directive = ".balign ";
        directive += emit_int(alignment);
        emit(std::move(directive), 1);
    }
}

// -> if is_glob $ .globl <identifier>
static void glob_directive_toplvl(const std::string& name, bool is_glob) {
    if (is_glob) {
        std::string directive = ".globl ";
        directive += name;
        emit(std::move(directive), 1);
    }
}

// Function(name, global, return_memory, instructions) -> $     <global-directive>
//                                                        $     .text
//                                                        $ <name>:
//                                                        $     pushq %rbp
//                                                        $     movq %rsp, %rbp
//                                                        $     <instructions>
static void emit_fun_toplvl(AsmFunction* node) {
    const std::string& name = emit_identifier(node->name);
    glob_directive_toplvl(name, node->is_glob);
    emit(".text", 1);
    {
        std::string directive = name;
        directive += ":";
        emit(std::move(directive), 0);
    }
    emit("pushq %rbp", 1);
    emit("movq %rsp, %rbp", 1);
    emit_instr_list(node->instructions);
}

// CharInit(i)                         -> .byte <i>
// IntInit(i)                          -> .long <i>
// LongInit(i)                         -> .quad <i>
// UCharInit(i)                        -> .byte <i>
// UIntInit(i)                         -> .long <i>
// ULongInit(i)                        -> .quad <i>
// ZeroInit(n)                         -> .zero <n>
// StringInit(s, b) if null terminated -> .asciz "<s>"
//                                else -> .ascii "<s>"
// PointerInit(label)                  -> .quad .L<label>
static void static_init_toplvl(StaticInit* node) {
    std::string directive;
    switch (node->type()) {
        case AST_CharInit_t: {
            directive = ".byte ";
            directive += emit_char(static_cast<CharInit*>(node)->value);
            break;
        }
        case AST_IntInit_t: {
            directive = ".long ";
            directive += emit_int(static_cast<IntInit*>(node)->value);
            break;
        }
        case AST_LongInit_t: {
            directive = ".quad ";
            directive += emit_long(static_cast<LongInit*>(node)->value);
            break;
        }
        case AST_DoubleInit_t: {
            directive = ".quad ";
            directive += emit_dbl(static_cast<DoubleInit*>(node)->dbl_const);
            break;
        }
        case AST_UCharInit_t: {
            directive = ".byte ";
            directive += emit_uchar(static_cast<UCharInit*>(node)->value);
            break;
        }
        case AST_UIntInit_t: {
            directive = ".long ";
            directive += emit_uint(static_cast<UIntInit*>(node)->value);
            break;
        }
        case AST_ULongInit_t: {
            directive = ".quad ";
            directive += emit_ulong(static_cast<ULongInit*>(node)->value);
            break;
        }
        case AST_ZeroInit_t: {
            directive = ".zero ";
            directive += emit_long(static_cast<ZeroInit*>(node)->byte);
            break;
        }
        case AST_StringInit_t: {
            directive = ".asci";
            {
                StringInit* p_node = static_cast<StringInit*>(node);
                directive += p_node->is_null_term ? "z" : "i";
                directive += " \"";
                directive += emit_string(p_node->string_const);
            }
            directive += "\"";
            break;
        }
        case AST_PointerInit_t: {
            directive = ".quad .L";
            directive += emit_identifier(static_cast<PointerInit*>(node)->name);
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    emit(std::move(directive), 2);
}

// -> if zero initialized $ .bss
// ->                else $ .data
static void static_section_toplvl(const std::vector<std::shared_ptr<StaticInit>>& node_list) {
    if (node_list.size() == 1 && node_list[0]->type() == AST_ZeroInit_t) {
        emit(".bss", 1);
    }
    else {
        emit(".data", 1);
    }
}

// StaticVariable(name, global, init*) -> $     <global-directive>
//                                     -> $     <data-section>
//                                     -> $     <alignment-directive>
//                                     -> $ <name>:
//                                     -> $     <init_list>
static void emit_static_var_toplvl(AsmStaticVariable* node) {
    const std::string& name = emit_identifier(node->name);
    glob_directive_toplvl(name, node->is_glob);
    static_section_toplvl(node->static_inits);
    align_directive_toplvl(node->alignment);
    {
        std::string directive = name;
        directive += ":";
        emit(std::move(directive), 0);
    }
    for (const auto& static_init : node->static_inits) {
        static_init_toplvl(static_init.get());
    }
}

// StaticConstant(name, align, init) -> $     .section .rodata
//                                      $     <alignment-directive>
//                                      $ .L<name>:
//                                      $     <init>
static void emit_static_const_toplvl(AsmStaticConstant* node) {
    emit(".section .rodata", 1);
    align_directive_toplvl(node->alignment);
    {
        std::string directive = ".L";
        directive += emit_identifier(node->name);
        directive += ":";
        emit(std::move(directive), 0);
    }
    static_init_toplvl(node->static_init.get());
}

// Function(name, global, return_memory, instructions) -> $ <function-top-level-directives>
// StaticVariable(name, global, align, init*)          -> $ <static-variable-top-level-directives>
// StaticConstant(name, align, init)                   -> $ <static-constant-top-level-directives>
static void emit_toplvl(AsmTopLevel* node) {
    emit("", 0);
    switch (node->type()) {
        case AST_AsmFunction_t:
            emit_fun_toplvl(static_cast<AsmFunction*>(node));
            break;
        case AST_AsmStaticVariable_t:
            emit_static_var_toplvl(static_cast<AsmStaticVariable*>(node));
            break;
        case AST_AsmStaticConstant_t:
            emit_static_const_toplvl(static_cast<AsmStaticConstant*>(node));
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

// Program(top_level*) -> $ [<top_level>]
//                        $     .section .note.GNU-stack,"",@progbits
static void emit_program(AsmProgram* node) {
    for (const auto& top_level : node->static_const_toplvls) {
        emit_toplvl(top_level.get());
    }
    for (const auto& top_level : node->top_levels) {
        emit_toplvl(top_level.get());
    }
    emit(".section .note.GNU-stack,\"\",@progbits", 2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void emit_gas_code(std::unique_ptr<AsmProgram> asm_ast, std::string&& filename) {
    open_fwrite(filename);
    emit_program(asm_ast.get());
    asm_ast.reset();
    close_fwrite();
}
