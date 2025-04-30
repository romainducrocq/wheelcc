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
static const std::string& emit_string(TIdentifier string_constant) { return identifiers->hash_table[string_constant]; }

// char -> $ char
static std::string emit_char(TChar value) { return std::to_string(value); }

// int -> $ int
static std::string emit_int(TInt value) { return std::to_string(value); }

// long -> $ long
static std::string emit_long(TLong value) { return std::to_string(value); }

// double -> $ double
static std::string emit_dbl(TIdentifier double_constant) { return identifiers->hash_table[double_constant]; }

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
        case AST_T::AsmSp_t:
            return "rsp";
        case AST_T::AsmBp_t:
            return "rbp";
        case AST_T::AsmXMM0_t:
            return "xmm0";
        case AST_T::AsmXMM1_t:
            return "xmm1";
        case AST_T::AsmXMM2_t:
            return "xmm2";
        case AST_T::AsmXMM3_t:
            return "xmm3";
        case AST_T::AsmXMM4_t:
            return "xmm4";
        case AST_T::AsmXMM5_t:
            return "xmm5";
        case AST_T::AsmXMM6_t:
            return "xmm6";
        case AST_T::AsmXMM7_t:
            return "xmm7";
        case AST_T::AsmXMM8_t:
            return "xmm8";
        case AST_T::AsmXMM9_t:
            return "xmm9";
        case AST_T::AsmXMM10_t:
            return "xmm10";
        case AST_T::AsmXMM11_t:
            return "xmm11";
        case AST_T::AsmXMM12_t:
            return "xmm12";
        case AST_T::AsmXMM13_t:
            return "xmm13";
        case AST_T::AsmXMM14_t:
            return "xmm14";
        case AST_T::AsmXMM15_t:
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
        case AST_T::AsmAx_t:
            return "al";
        case AST_T::AsmBx_t:
            return "bl";
        case AST_T::AsmDx_t:
            return "dl";
        case AST_T::AsmCx_t:
            return "cl";
        case AST_T::AsmDi_t:
            return "dil";
        case AST_T::AsmSi_t:
            return "sil";
        case AST_T::AsmR8_t:
            return "r8b";
        case AST_T::AsmR9_t:
            return "r9b";
        case AST_T::AsmR10_t:
            return "r10b";
        case AST_T::AsmR11_t:
            return "r11b";
        case AST_T::AsmR12_t:
            return "r12b";
        case AST_T::AsmR13_t:
            return "r13b";
        case AST_T::AsmR14_t:
            return "r14b";
        case AST_T::AsmR15_t:
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
        case AST_T::AsmAx_t:
            return "eax";
        case AST_T::AsmBx_t:
            return "ebx";
        case AST_T::AsmDx_t:
            return "edx";
        case AST_T::AsmCx_t:
            return "ecx";
        case AST_T::AsmDi_t:
            return "edi";
        case AST_T::AsmSi_t:
            return "esi";
        case AST_T::AsmR8_t:
            return "r8d";
        case AST_T::AsmR9_t:
            return "r9d";
        case AST_T::AsmR10_t:
            return "r10d";
        case AST_T::AsmR11_t:
            return "r11d";
        case AST_T::AsmR12_t:
            return "r12d";
        case AST_T::AsmR13_t:
            return "r13d";
        case AST_T::AsmR14_t:
            return "r14d";
        case AST_T::AsmR15_t:
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
        case AST_T::AsmAx_t:
            return "rax";
        case AST_T::AsmBx_t:
            return "rbx";
        case AST_T::AsmDx_t:
            return "rdx";
        case AST_T::AsmCx_t:
            return "rcx";
        case AST_T::AsmDi_t:
            return "rdi";
        case AST_T::AsmSi_t:
            return "rsi";
        case AST_T::AsmR8_t:
            return "r8";
        case AST_T::AsmR9_t:
            return "r9";
        case AST_T::AsmR10_t:
            return "r10";
        case AST_T::AsmR11_t:
            return "r11";
        case AST_T::AsmR12_t:
            return "r12";
        case AST_T::AsmR13_t:
            return "r13";
        case AST_T::AsmR14_t:
            return "r14";
        case AST_T::AsmR15_t:
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
        case AST_T::AsmE_t:
            return "e";
        case AST_T::AsmNE_t:
            return "ne";
        case AST_T::AsmL_t:
            return "l";
        case AST_T::AsmLE_t:
            return "le";
        case AST_T::AsmG_t:
            return "g";
        case AST_T::AsmGE_t:
            return "ge";
        case AST_T::AsmB_t:
            return "b";
        case AST_T::AsmBE_t:
            return "be";
        case AST_T::AsmA_t:
            return "a";
        case AST_T::AsmAE_t:
            return "ae";
        case AST_T::AsmP_t:
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
        case AST_T::Byte_t:
            return 1;
        case AST_T::LongWord_t:
            return 4;
        case AST_T::QuadWord_t:
        case AST_T::BackendDouble_t:
            return 8;
        case AST_T::ByteArray_t:
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
        case AST_T::Byte_t:
            return "b";
        case AST_T::LongWord_t:
            return "l";
        case AST_T::QuadWord_t:
            return "q";
        case AST_T::BackendDouble_t:
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
    std::string operand =
        backend->backend_symbol_table.find(node->name) != backend->backend_symbol_table.end()
                && backend->backend_symbol_table[node->name]->type() == AST_T::BackendObj_t
                && static_cast<BackendObj*>(backend->backend_symbol_table[node->name].get())->is_constant ?
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
        case AST_T::AsmImm_t:
            return imm_op(static_cast<AsmImm*>(node));
        case AST_T::AsmRegister_t:
            return reg_op(static_cast<AsmRegister*>(node), byte);
        case AST_T::AsmMemory_t:
            return memory_op(static_cast<AsmMemory*>(node));
        case AST_T::AsmData_t:
            return data_op(static_cast<AsmData*>(node));
        case AST_T::AsmIndexed_t:
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
        case AST_T::AsmNeg_t:
            return "neg";
        case AST_T::AsmNot_t:
            return "not";
        case AST_T::AsmShr_t:
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
static std::string emit_binop(AsmBinaryOp* node, bool is_double) {
    switch (node->type()) {
        case AST_T::AsmAdd_t:
            return "add";
        case AST_T::AsmSub_t:
            return "sub";
        case AST_T::AsmMult_t:
            return is_double ? "mul" : "imul";
        case AST_T::AsmDivDouble_t:
            return "div";
        case AST_T::AsmBitAnd_t:
            return "and";
        case AST_T::AsmBitOr_t:
            return "or";
        case AST_T::AsmBitXor_t:
            return "xor";
        case AST_T::AsmBitShiftLeft_t:
            return "shl";
        case AST_T::AsmBitShiftRight_t:
            return "shr";
        case AST_T::AsmBitShrArithmetic_t:
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
    std::string instruction = "mov";
    instruction += emit_type_suffix(node->assembly_type.get(), false);
    instruction += " ";
    {
        TInt byte = type_align_bytes(node->assembly_type.get());
        instruction += emit_op(node->src.get(), byte);
        instruction += ", ";
        instruction += emit_op(node->dst.get(), byte);
    }
    emit(std::move(instruction), 2);
}

static void mov_sx_instr(AsmMovSx* node) {
    std::string instruction = "movs";
    instruction += emit_type_suffix(node->assembly_type_src.get(), false);
    instruction += emit_type_suffix(node->assembly_type_dst.get(), false);
    instruction += " ";
    {
        TInt byte = type_align_bytes(node->assembly_type_src.get());
        instruction += emit_op(node->src.get(), byte);
    }
    instruction += ", ";
    {
        TInt byte = type_align_bytes(node->assembly_type_dst.get());
        instruction += emit_op(node->dst.get(), byte);
    }
    emit(std::move(instruction), 2);
}

static void zero_extend_instr(AsmMovZeroExtend* node) {
    std::string instruction = "movzb";
    instruction += emit_type_suffix(node->assembly_type_dst.get(), false);
    instruction += " ";
    instruction += emit_op(node->src.get(), 1);
    instruction += ", ";
    {
        TInt byte = type_align_bytes(node->assembly_type_dst.get());
        instruction += emit_op(node->dst.get(), byte);
    }
    emit(std::move(instruction), 2);
}

static void lea_instr(AsmLea* node) {
    std::string instruction = "leaq ";
    instruction += emit_op(node->src.get(), 8);
    instruction += ", ";
    instruction += emit_op(node->dst.get(), 8);
    emit(std::move(instruction), 2);
}

static void cvttsd2si_instr(AsmCvttsd2si* node) {
    std::string instruction = "cvttsd2si";
    instruction += emit_type_suffix(node->assembly_type.get(), false);
    instruction += " ";
    {
        TInt byte = type_align_bytes(node->assembly_type.get());
        instruction += emit_op(node->src.get(), byte);
        instruction += ", ";
        instruction += emit_op(node->dst.get(), byte);
    }
    emit(std::move(instruction), 2);
}

static void cvtsi2sd_instr(AsmCvtsi2sd* node) {
    std::string instruction = "cvtsi2sd";
    instruction += emit_type_suffix(node->assembly_type.get(), false);
    instruction += " ";
    {
        TInt byte = type_align_bytes(node->assembly_type.get());
        instruction += emit_op(node->src.get(), byte);
        instruction += ", ";
        instruction += emit_op(node->dst.get(), byte);
    }
    emit(std::move(instruction), 2);
}

static void unary_instr(AsmUnary* node) {
    std::string instruction = emit_unop(node->unary_op.get());
    instruction += emit_type_suffix(node->assembly_type.get(), false);
    instruction += " ";
    {
        TInt byte = type_align_bytes(node->assembly_type.get());
        instruction += emit_op(node->dst.get(), byte);
    }
    emit(std::move(instruction), 2);
}

static void binary_instr(AsmBinary* node) {
    bool is_double = node->assembly_type->type() == AST_T::BackendDouble_t;
    std::string instruction = emit_binop(node->binary_op.get(), is_double);
    {
        bool is_packed = node->binary_op->type() == AST_T::AsmBitXor_t && is_double;
        instruction += emit_type_suffix(node->assembly_type.get(), is_packed);
    }
    instruction += " ";
    {
        TInt byte = type_align_bytes(node->assembly_type.get());
        instruction += emit_op(node->src.get(), byte);
        instruction += ", ";
        instruction += emit_op(node->dst.get(), byte);
    }
    emit(std::move(instruction), 2);
}

static void cmp_instr(AsmCmp* node) {
    std::string instruction = node->assembly_type->type() == AST_T::BackendDouble_t ? "comi" : "cmp";
    instruction += emit_type_suffix(node->assembly_type.get(), false);
    instruction += " ";
    {
        TInt byte = type_align_bytes(node->assembly_type.get());
        instruction += emit_op(node->src.get(), byte);
        instruction += ", ";
        instruction += emit_op(node->dst.get(), byte);
    }
    emit(std::move(instruction), 2);
}

static void idiv_instr(AsmIdiv* node) {
    std::string instruction = "idiv";
    instruction += emit_type_suffix(node->assembly_type.get(), false);
    instruction += " ";
    {
        TInt byte = type_align_bytes(node->assembly_type.get());
        instruction += emit_op(node->src.get(), byte);
    }
    emit(std::move(instruction), 2);
}

static void div_instr(AsmDiv* node) {
    std::string instruction = "div";
    instruction += emit_type_suffix(node->assembly_type.get(), false);
    instruction += " ";
    {
        TInt byte = type_align_bytes(node->assembly_type.get());
        instruction += emit_op(node->src.get(), byte);
    }
    emit(std::move(instruction), 2);
}

static void cdq_instr(AsmCdq* node) {
    switch (node->assembly_type->type()) {
        case AST_T::LongWord_t:
            emit("cdq", 2);
            break;
        case AST_T::QuadWord_t:
            emit("cqo", 2);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void jmp_instr(AsmJmp* node) {
    std::string instruction = "jmp .L";
    instruction += emit_identifier(node->target);
    emit(std::move(instruction), 2);
}

static void jmp_cc_instr(AsmJmpCC* node) {
    std::string instruction = "j";
    instruction += emit_cond_code(node->cond_code.get());
    instruction += " .L";
    instruction += emit_identifier(node->target);
    emit(std::move(instruction), 2);
}

static void set_cc_instr(AsmSetCC* node) {
    std::string instruction = "set";
    instruction += emit_cond_code(node->cond_code.get());
    instruction += " ";
    instruction += emit_op(node->dst.get(), 1);
    emit(std::move(instruction), 2);
}

static void label_instr(AsmLabel* node) {
    std::string instruction = ".L";
    instruction += emit_identifier(node->name);
    instruction += ":";
    emit(std::move(instruction), 1);
}

static void push_instr(AsmPush* node) {
    std::string instruction = "pushq ";
    instruction += emit_op(node->src.get(), 8);
    emit(std::move(instruction), 2);
}

static void pop_instr(AsmPop* node) {
    std::string instruction = "popq %";
    instruction += emit_reg_8b(node->reg.get());
    emit(std::move(instruction), 2);
}

static void call_instr(AsmCall* node) {
    std::string instruction = "call ";
    instruction += emit_identifier(node->name);
    if (backend->backend_symbol_table[node->name]->type() != AST_T::BackendFun_t) {
        RAISE_INTERNAL_ERROR;
    }
    else if (!static_cast<BackendFun*>(backend->backend_symbol_table[node->name].get())->is_defined) {
        instruction += "@PLT";
    }
    emit(std::move(instruction), 2);
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
        case AST_T::AsmMov_t:
            mov_instr(static_cast<AsmMov*>(node));
            break;
        case AST_T::AsmMovSx_t:
            mov_sx_instr(static_cast<AsmMovSx*>(node));
            break;
        case AST_T::AsmMovZeroExtend_t:
            zero_extend_instr(static_cast<AsmMovZeroExtend*>(node));
            break;
        case AST_T::AsmLea_t:
            lea_instr(static_cast<AsmLea*>(node));
            break;
        case AST_T::AsmCvttsd2si_t:
            cvttsd2si_instr(static_cast<AsmCvttsd2si*>(node));
            break;
        case AST_T::AsmCvtsi2sd_t:
            cvtsi2sd_instr(static_cast<AsmCvtsi2sd*>(node));
            break;
        case AST_T::AsmUnary_t:
            unary_instr(static_cast<AsmUnary*>(node));
            break;
        case AST_T::AsmBinary_t:
            binary_instr(static_cast<AsmBinary*>(node));
            break;
        case AST_T::AsmCmp_t:
            cmp_instr(static_cast<AsmCmp*>(node));
            break;
        case AST_T::AsmIdiv_t:
            idiv_instr(static_cast<AsmIdiv*>(node));
            break;
        case AST_T::AsmDiv_t:
            div_instr(static_cast<AsmDiv*>(node));
            break;
        case AST_T::AsmCdq_t:
            cdq_instr(static_cast<AsmCdq*>(node));
            break;
        case AST_T::AsmJmp_t:
            jmp_instr(static_cast<AsmJmp*>(node));
            break;
        case AST_T::AsmJmpCC_t:
            jmp_cc_instr(static_cast<AsmJmpCC*>(node));
            break;
        case AST_T::AsmSetCC_t:
            set_cc_instr(static_cast<AsmSetCC*>(node));
            break;
        case AST_T::AsmLabel_t:
            label_instr(static_cast<AsmLabel*>(node));
            break;
        case AST_T::AsmPush_t:
            push_instr(static_cast<AsmPush*>(node));
            break;
        case AST_T::AsmPop_t:
            pop_instr(static_cast<AsmPop*>(node));
            break;
        case AST_T::AsmCall_t:
            call_instr(static_cast<AsmCall*>(node));
            break;
        case AST_T::AsmRet_t:
            ret_instr();
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void emit_instr_list(const std::vector<std::unique_ptr<AsmInstruction>>& list_node) {
    for (size_t i = list_node[0] ? 0 : 1; i < list_node.size(); ++i) {
        emit_instr(list_node[i].get());
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

// -> if is_global $ .globl <identifier>
static void glob_directive_toplvl(const std::string& name, bool is_global) {
    if (is_global) {
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
static void fun_toplvl(AsmFunction* node) {
    const std::string& name = emit_identifier(node->name);
    glob_directive_toplvl(name, node->is_global);
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
        case AST_T::CharInit_t: {
            directive = ".byte ";
            directive += emit_char(static_cast<CharInit*>(node)->value);
            break;
        }
        case AST_T::IntInit_t: {
            directive = ".long ";
            directive += emit_int(static_cast<IntInit*>(node)->value);
            break;
        }
        case AST_T::LongInit_t: {
            directive = ".quad ";
            directive += emit_long(static_cast<LongInit*>(node)->value);
            break;
        }
        case AST_T::DoubleInit_t: {
            directive = ".quad ";
            directive += emit_dbl(static_cast<DoubleInit*>(node)->double_constant);
            break;
        }
        case AST_T::UCharInit_t: {
            directive = ".byte ";
            directive += emit_uchar(static_cast<UCharInit*>(node)->value);
            break;
        }
        case AST_T::UIntInit_t: {
            directive = ".long ";
            directive += emit_uint(static_cast<UIntInit*>(node)->value);
            break;
        }
        case AST_T::ULongInit_t: {
            directive = ".quad ";
            directive += emit_ulong(static_cast<ULongInit*>(node)->value);
            break;
        }
        case AST_T::ZeroInit_t: {
            directive = ".zero ";
            directive += emit_long(static_cast<ZeroInit*>(node)->byte);
            break;
        }
        case AST_T::StringInit_t: {
            directive = ".asci";
            {
                StringInit* p_node = static_cast<StringInit*>(node);
                directive += p_node->is_null_terminated ? "z" : "i";
                directive += " \"";
                directive += emit_string(p_node->string_constant);
            }
            directive += "\"";
            break;
        }
        case AST_T::PointerInit_t: {
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
static void section_static_toplvl(const std::vector<std::shared_ptr<StaticInit>>& list_node) {
    if (list_node.size() == 1 && list_node[0]->type() == AST_T::ZeroInit_t) {
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
static void static_var_toplvl(AsmStaticVariable* node) {
    const std::string& name = emit_identifier(node->name);
    glob_directive_toplvl(name, node->is_global);
    section_static_toplvl(node->static_inits);
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
static void static_const_toplvl(AsmStaticConstant* node) {
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
        case AST_T::AsmFunction_t:
            fun_toplvl(static_cast<AsmFunction*>(node));
            break;
        case AST_T::AsmStaticVariable_t:
            static_var_toplvl(static_cast<AsmStaticVariable*>(node));
            break;
        case AST_T::AsmStaticConstant_t:
            static_const_toplvl(static_cast<AsmStaticConstant*>(node));
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

// Program(top_level*) -> $ [<top_level>]
//                        $     .section .note.GNU-stack,"",@progbits
static void emit_program(AsmProgram* node) {
    for (const auto& top_level : node->static_constant_top_levels) {
        emit_toplvl(top_level.get());
    }
    for (const auto& top_level : node->top_levels) {
        emit_toplvl(top_level.get());
    }
    emit(".section .note.GNU-stack,\"\",@progbits", 2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void gas_code_emission(std::unique_ptr<AsmProgram> asm_ast, std::string&& filename) {
    file_open_write(filename);
    emit_program(asm_ast.get());
    asm_ast.reset();
    file_close_write();
}
