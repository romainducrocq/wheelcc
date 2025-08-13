#include <memory>
#include <vector>

#include "util/c_std.hpp"
#include "util/fileio.hpp"
#include "util/throw.hpp"

#include "ast/ast.hpp"
#include "ast/back_ast.hpp"
#include "ast/back_symt.hpp"

#include "backend/emitter/gas_code.hpp"

struct GasCodeContext {
    BackEndContext* backend;
    FileIoContext* fileio;
    IdentifierContext* identifiers;
    // Gnu assembler code emission
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Gnu assembler code emission

typedef GasCodeContext* Ctx;

#define LF "\n"
#define TAB "    "
#ifdef __APPLE__
#define LBL "L"
#else
#define LBL ".L"
#endif

static void emit(Ctx ctx, const char* code) { write_buffer(ctx->fileio, code); }

// identifier -> $ identifier
static void emit_identifier(Ctx ctx, TIdentifier identifier) {
#ifdef __APPLE__
    emit(ctx, "_");
#endif
    string_t value = ctx->identifiers->hash_table[identifier];
    emit(ctx, value);
}

// string -> $ string
static void emit_string(Ctx ctx, TIdentifier string_const) {
    string_t value = ctx->identifiers->hash_table[string_const];
    emit(ctx, value);
}

// char -> $ char
static void emit_char(Ctx ctx, TChar value) {
    string_t strto_value = str_to_string(value);
    emit(ctx, strto_value);
    str_delete(strto_value);
}

// int -> $ int
static void emit_int(Ctx ctx, TInt value) {
    string_t strto_value = str_to_string(value);
    emit(ctx, strto_value);
    str_delete(strto_value);
}

// long -> $ long
static void emit_long(Ctx ctx, TLong value) {
    string_t strto_value = str_to_string(value);
    emit(ctx, strto_value);
    str_delete(strto_value);
}

// double -> $ double
static void emit_dbl(Ctx ctx, TIdentifier dbl_const) {
    string_t value = ctx->identifiers->hash_table[dbl_const];
    emit(ctx, value);
}

// uchar -> $ uchar
static void emit_uchar(Ctx ctx, TUChar value) {
    string_t strto_value = str_to_string(value);
    emit(ctx, strto_value);
    str_delete(strto_value);
}

// uint -> $ uint
static void emit_uint(Ctx ctx, TUInt value) {
    string_t strto_value = str_to_string(value);
    emit(ctx, strto_value);
    str_delete(strto_value);
}

// ulong -> $ ulong
static void emit_ulong(Ctx ctx, TULong value) {
    string_t strto_value = str_to_string(value);
    emit(ctx, strto_value);
    str_delete(strto_value);
}

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
static const char* get_reg_rsp_sse(AsmReg* node) {
    switch (node->type()) {
        case AST_AsmSp_t:
            return "%rsp";
        case AST_AsmBp_t:
            return "%rbp";
        case AST_AsmXMM0_t:
            return "%xmm0";
        case AST_AsmXMM1_t:
            return "%xmm1";
        case AST_AsmXMM2_t:
            return "%xmm2";
        case AST_AsmXMM3_t:
            return "%xmm3";
        case AST_AsmXMM4_t:
            return "%xmm4";
        case AST_AsmXMM5_t:
            return "%xmm5";
        case AST_AsmXMM6_t:
            return "%xmm6";
        case AST_AsmXMM7_t:
            return "%xmm7";
        case AST_AsmXMM8_t:
            return "%xmm8";
        case AST_AsmXMM9_t:
            return "%xmm9";
        case AST_AsmXMM10_t:
            return "%xmm10";
        case AST_AsmXMM11_t:
            return "%xmm11";
        case AST_AsmXMM12_t:
            return "%xmm12";
        case AST_AsmXMM13_t:
            return "%xmm13";
        case AST_AsmXMM14_t:
            return "%xmm14";
        case AST_AsmXMM15_t:
            return "%xmm15";
        default:
            THROW_ABORT;
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
static const char* get_reg_1b(AsmReg* node) {
    switch (node->type()) {
        case AST_AsmAx_t:
            return "%al";
        case AST_AsmBx_t:
            return "%bl";
        case AST_AsmDx_t:
            return "%dl";
        case AST_AsmCx_t:
            return "%cl";
        case AST_AsmDi_t:
            return "%dil";
        case AST_AsmSi_t:
            return "%sil";
        case AST_AsmR8_t:
            return "%r8b";
        case AST_AsmR9_t:
            return "%r9b";
        case AST_AsmR10_t:
            return "%r10b";
        case AST_AsmR11_t:
            return "%r11b";
        case AST_AsmR12_t:
            return "%r12b";
        case AST_AsmR13_t:
            return "%r13b";
        case AST_AsmR14_t:
            return "%r14b";
        case AST_AsmR15_t:
            return "%r15b";
        default:
            return get_reg_rsp_sse(node);
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
static const char* get_reg_4b(AsmReg* node) {
    switch (node->type()) {
        case AST_AsmAx_t:
            return "%eax";
        case AST_AsmBx_t:
            return "%ebx";
        case AST_AsmDx_t:
            return "%edx";
        case AST_AsmCx_t:
            return "%ecx";
        case AST_AsmDi_t:
            return "%edi";
        case AST_AsmSi_t:
            return "%esi";
        case AST_AsmR8_t:
            return "%r8d";
        case AST_AsmR9_t:
            return "%r9d";
        case AST_AsmR10_t:
            return "%r10d";
        case AST_AsmR11_t:
            return "%r11d";
        case AST_AsmR12_t:
            return "%r12d";
        case AST_AsmR13_t:
            return "%r13d";
        case AST_AsmR14_t:
            return "%r14d";
        case AST_AsmR15_t:
            return "%r15d";
        default:
            return get_reg_rsp_sse(node);
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
static const char* get_reg_8b(AsmReg* node) {
    switch (node->type()) {
        case AST_AsmAx_t:
            return "%rax";
        case AST_AsmBx_t:
            return "%rbx";
        case AST_AsmDx_t:
            return "%rdx";
        case AST_AsmCx_t:
            return "%rcx";
        case AST_AsmDi_t:
            return "%rdi";
        case AST_AsmSi_t:
            return "%rsi";
        case AST_AsmR8_t:
            return "%r8";
        case AST_AsmR9_t:
            return "%r9";
        case AST_AsmR10_t:
            return "%r10";
        case AST_AsmR11_t:
            return "%r11";
        case AST_AsmR12_t:
            return "%r12";
        case AST_AsmR13_t:
            return "%r13";
        case AST_AsmR14_t:
            return "%r14";
        case AST_AsmR15_t:
            return "%r15";
        default:
            return get_reg_rsp_sse(node);
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
static const char* get_cond_code(AsmCondCode* node) {
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
            THROW_ABORT;
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
            THROW_ABORT;
    }
}

// Byte             -> $ b
// LongWord         -> $ l
// QuadWord         -> $ q
// Double if packed -> $ pd
//             else -> $ sd
static const char* get_type_suffix(AssemblyType* node, bool is_packed) {
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
            THROW_ABORT;
    }
}

static void imm_op(Ctx ctx, AsmImm* node) {
    emit(ctx, "$");
    if (node->is_neg) {
        emit_long(ctx, (TLong)node->value);
    }
    else {
        emit_ulong(ctx, node->value);
    }
}

static void reg_op(Ctx ctx, AsmRegister* node, TInt byte) {
    switch (byte) {
        case 1:
            emit(ctx, get_reg_1b(node->reg.get()));
            break;
        case 4:
            emit(ctx, get_reg_4b(node->reg.get()));
            break;
        case 8:
            emit(ctx, get_reg_8b(node->reg.get()));
            break;
        default:
            THROW_ABORT;
    }
}

static void memory_op(Ctx ctx, AsmMemory* node) {
    if (node->value != 0l) {
        emit_long(ctx, node->value);
    }
    emit(ctx, "(");
    emit(ctx, get_reg_8b(node->reg.get()));
    emit(ctx, ")");
}

static void data_op(Ctx ctx, AsmData* node) {
    if (ctx->backend->symbol_table.find(node->name) != ctx->backend->symbol_table.end()
        && ctx->backend->symbol_table[node->name]->type() == AST_BackendObj_t
        && static_cast<BackendObj*>(ctx->backend->symbol_table[node->name].get())->is_const) {
        emit(ctx, LBL);
    }
    emit_identifier(ctx, node->name);
    if (node->offset != 0l) {
        emit(ctx, "+");
        emit_long(ctx, node->offset);
    }
    emit(ctx, "(%rip)");
}

static void indexed_op(Ctx ctx, AsmIndexed* node) {
    emit(ctx, "(");
    emit(ctx, get_reg_8b(node->reg_base.get()));
    emit(ctx, ", ");
    emit(ctx, get_reg_8b(node->reg_index.get()));
    emit(ctx, ", ");
    emit_long(ctx, node->scale);
    emit(ctx, ")");
}

// Imm(int)                 -> $ $<int>
// Register(reg)            -> $ %reg
// Memory(int, reg)         -> $ <int>(<reg>)
// Data(identifier, int)    -> $ <identifier>+<int>(%rip)
// Indexed(reg1, reg2, int) -> $ (<reg1>, <reg2>, <int>)
static void emit_op(Ctx ctx, AsmOperand* node, TInt byte) {
    switch (node->type()) {
        case AST_AsmImm_t:
            imm_op(ctx, static_cast<AsmImm*>(node));
            break;
        case AST_AsmRegister_t:
            reg_op(ctx, static_cast<AsmRegister*>(node), byte);
            break;
        case AST_AsmMemory_t:
            memory_op(ctx, static_cast<AsmMemory*>(node));
            break;
        case AST_AsmData_t:
            data_op(ctx, static_cast<AsmData*>(node));
            break;
        case AST_AsmIndexed_t:
            indexed_op(ctx, static_cast<AsmIndexed*>(node));
            break;
        default:
            THROW_ABORT;
    }
}

// Neg -> $ neg
// Not -> $ not
// Shr -> $ shr
static const char* get_unop(AsmUnaryOp* node) {
    switch (node->type()) {
        case AST_AsmNeg_t:
            return "neg";
        case AST_AsmNot_t:
            return "not";
        case AST_AsmShr_t:
            return "shr";
        default:
            THROW_ABORT;
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
static const char* get_binop(AsmBinaryOp* node, bool is_dbl) {
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
            THROW_ABORT;
    }
}

static void mov_instr(Ctx ctx, AsmMov* node) {
    emit(ctx, TAB TAB "mov");
    emit(ctx, get_type_suffix(node->asm_type.get(), false));
    emit(ctx, " ");
    {
        TInt byte = type_align_bytes(node->asm_type.get());
        emit_op(ctx, node->src.get(), byte);
        emit(ctx, ", ");
        emit_op(ctx, node->dst.get(), byte);
    }
    emit(ctx, LF);
}

static void mov_sx_instr(Ctx ctx, AsmMovSx* node) {
    emit(ctx, TAB TAB "movs");
    emit(ctx, get_type_suffix(node->asm_type_src.get(), false));
    emit(ctx, get_type_suffix(node->asm_type_dst.get(), false));
    emit(ctx, " ");
    {
        TInt byte = type_align_bytes(node->asm_type_src.get());
        emit_op(ctx, node->src.get(), byte);
    }
    emit(ctx, ", ");
    {
        TInt byte = type_align_bytes(node->asm_type_dst.get());
        emit_op(ctx, node->dst.get(), byte);
    }
    emit(ctx, LF);
}

static void zero_extend_instr(Ctx ctx, AsmMovZeroExtend* node) {
    emit(ctx, TAB TAB "movzb");
    emit(ctx, get_type_suffix(node->asm_type_dst.get(), false));
    emit(ctx, " ");
    emit_op(ctx, node->src.get(), 1);
    emit(ctx, ", ");
    {
        TInt byte = type_align_bytes(node->asm_type_dst.get());
        emit_op(ctx, node->dst.get(), byte);
    }
    emit(ctx, LF);
}

static void lea_instr(Ctx ctx, AsmLea* node) {
    emit(ctx, TAB TAB "leaq ");
    emit_op(ctx, node->src.get(), 8);
    emit(ctx, ", ");
    emit_op(ctx, node->dst.get(), 8);
    emit(ctx, LF);
}

static void cvttsd2si_instr(Ctx ctx, AsmCvttsd2si* node) {
    emit(ctx, TAB TAB "cvttsd2si");
    emit(ctx, get_type_suffix(node->asm_type.get(), false));
    emit(ctx, " ");
    {
        TInt byte = type_align_bytes(node->asm_type.get());
        emit_op(ctx, node->src.get(), byte);
        emit(ctx, ", ");
        emit_op(ctx, node->dst.get(), byte);
    }
    emit(ctx, LF);
}

static void cvtsi2sd_instr(Ctx ctx, AsmCvtsi2sd* node) {
    emit(ctx, TAB TAB "cvtsi2sd");
    emit(ctx, get_type_suffix(node->asm_type.get(), false));
    emit(ctx, " ");
    {
        TInt byte = type_align_bytes(node->asm_type.get());
        emit_op(ctx, node->src.get(), byte);
        emit(ctx, ", ");
        emit_op(ctx, node->dst.get(), byte);
    }
    emit(ctx, LF);
}

static void unary_instr(Ctx ctx, AsmUnary* node) {
    emit(ctx, TAB TAB);
    emit(ctx, get_unop(node->unop.get()));
    emit(ctx, get_type_suffix(node->asm_type.get(), false));
    emit(ctx, " ");
    {
        TInt byte = type_align_bytes(node->asm_type.get());
        emit_op(ctx, node->dst.get(), byte);
    }
    emit(ctx, LF);
}

static void binary_instr(Ctx ctx, AsmBinary* node) {
    emit(ctx, TAB TAB);
    {
        bool is_dbl = node->asm_type->type() == AST_BackendDouble_t;
        emit(ctx, get_binop(node->binop.get(), is_dbl));
        bool is_packed = node->binop->type() == AST_AsmBitXor_t && is_dbl;
        emit(ctx, get_type_suffix(node->asm_type.get(), is_packed));
    }
    emit(ctx, " ");
    {
        TInt byte = type_align_bytes(node->asm_type.get());
        switch (node->binop->type()) {
            case AST_AsmBitShiftLeft_t:
            case AST_AsmBitShiftRight_t:
            case AST_AsmBitShrArithmetic_t:
                emit_op(ctx, node->src.get(), 1);
                break;
            default:
                emit_op(ctx, node->src.get(), byte);
                break;
        }
        emit(ctx, ", ");
        emit_op(ctx, node->dst.get(), byte);
    }
    emit(ctx, LF);
}

static void cmp_instr(Ctx ctx, AsmCmp* node) {
    if (node->asm_type->type() == AST_BackendDouble_t) {
        emit(ctx, TAB TAB "comi");
    }
    else {
        emit(ctx, TAB TAB "cmp");
    }
    emit(ctx, get_type_suffix(node->asm_type.get(), false));
    emit(ctx, " ");
    {
        TInt byte = type_align_bytes(node->asm_type.get());
        emit_op(ctx, node->src.get(), byte);
        emit(ctx, ", ");
        emit_op(ctx, node->dst.get(), byte);
    }
    emit(ctx, LF);
}

static void idiv_instr(Ctx ctx, AsmIdiv* node) {
    emit(ctx, TAB TAB "idiv");
    emit(ctx, get_type_suffix(node->asm_type.get(), false));
    emit(ctx, " ");
    {
        TInt byte = type_align_bytes(node->asm_type.get());
        emit_op(ctx, node->src.get(), byte);
    }
    emit(ctx, LF);
}

static void div_instr(Ctx ctx, AsmDiv* node) {
    emit(ctx, TAB TAB "div");
    emit(ctx, get_type_suffix(node->asm_type.get(), false));
    emit(ctx, " ");
    {
        TInt byte = type_align_bytes(node->asm_type.get());
        emit_op(ctx, node->src.get(), byte);
    }
    emit(ctx, LF);
}

static void cdq_instr(Ctx ctx, AsmCdq* node) {
    switch (node->asm_type->type()) {
        case AST_LongWord_t:
            emit(ctx, TAB TAB "cdq" LF);
            break;
        case AST_QuadWord_t:
            emit(ctx, TAB TAB "cqo" LF);
            break;
        default:
            THROW_ABORT;
    }
}

static void jmp_instr(Ctx ctx, AsmJmp* node) {
    emit(ctx, TAB TAB "jmp " LBL);
    emit_identifier(ctx, node->target);
    emit(ctx, LF);
}

static void jmp_cc_instr(Ctx ctx, AsmJmpCC* node) {
    emit(ctx, TAB TAB "j");
    emit(ctx, get_cond_code(node->cond_code.get()));
    emit(ctx, " " LBL);
    emit_identifier(ctx, node->target);
    emit(ctx, LF);
}

static void set_cc_instr(Ctx ctx, AsmSetCC* node) {
    emit(ctx, TAB TAB "set");
    emit(ctx, get_cond_code(node->cond_code.get()));
    emit(ctx, " ");
    emit_op(ctx, node->dst.get(), 1);
    emit(ctx, LF);
}

static void label_instr(Ctx ctx, AsmLabel* node) {
    emit(ctx, TAB LBL);
    emit_identifier(ctx, node->name);
    emit(ctx, ":" LF);
}

static void push_instr(Ctx ctx, AsmPush* node) {
    emit(ctx, TAB TAB "pushq ");
    emit_op(ctx, node->src.get(), 8);
    emit(ctx, LF);
}

static void pop_instr(Ctx ctx, AsmPop* node) {
    emit(ctx, TAB TAB "popq ");
    emit(ctx, get_reg_8b(node->reg.get()));
    emit(ctx, LF);
}

static void call_instr(Ctx ctx, AsmCall* node) {
    emit(ctx, TAB TAB "call ");
    emit_identifier(ctx, node->name);
#ifndef __APPLE__
    THROW_ABORT_IF(ctx->backend->symbol_table[node->name]->type() != AST_BackendFun_t);
    if (!static_cast<BackendFun*>(ctx->backend->symbol_table[node->name].get())->is_def) {
        emit(ctx, "@PLT");
    }
#endif
    emit(ctx, LF);
}

static void ret_instr(Ctx ctx) { emit(ctx, TAB "movq %rbp, %rsp" LF TAB "popq %rbp" LF TAB "ret" LF); }

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
static void emit_instr(Ctx ctx, AsmInstruction* node) {
    switch (node->type()) {
        case AST_AsmMov_t:
            mov_instr(ctx, static_cast<AsmMov*>(node));
            break;
        case AST_AsmMovSx_t:
            mov_sx_instr(ctx, static_cast<AsmMovSx*>(node));
            break;
        case AST_AsmMovZeroExtend_t:
            zero_extend_instr(ctx, static_cast<AsmMovZeroExtend*>(node));
            break;
        case AST_AsmLea_t:
            lea_instr(ctx, static_cast<AsmLea*>(node));
            break;
        case AST_AsmCvttsd2si_t:
            cvttsd2si_instr(ctx, static_cast<AsmCvttsd2si*>(node));
            break;
        case AST_AsmCvtsi2sd_t:
            cvtsi2sd_instr(ctx, static_cast<AsmCvtsi2sd*>(node));
            break;
        case AST_AsmUnary_t:
            unary_instr(ctx, static_cast<AsmUnary*>(node));
            break;
        case AST_AsmBinary_t:
            binary_instr(ctx, static_cast<AsmBinary*>(node));
            break;
        case AST_AsmCmp_t:
            cmp_instr(ctx, static_cast<AsmCmp*>(node));
            break;
        case AST_AsmIdiv_t:
            idiv_instr(ctx, static_cast<AsmIdiv*>(node));
            break;
        case AST_AsmDiv_t:
            div_instr(ctx, static_cast<AsmDiv*>(node));
            break;
        case AST_AsmCdq_t:
            cdq_instr(ctx, static_cast<AsmCdq*>(node));
            break;
        case AST_AsmJmp_t:
            jmp_instr(ctx, static_cast<AsmJmp*>(node));
            break;
        case AST_AsmJmpCC_t:
            jmp_cc_instr(ctx, static_cast<AsmJmpCC*>(node));
            break;
        case AST_AsmSetCC_t:
            set_cc_instr(ctx, static_cast<AsmSetCC*>(node));
            break;
        case AST_AsmLabel_t:
            label_instr(ctx, static_cast<AsmLabel*>(node));
            break;
        case AST_AsmPush_t:
            push_instr(ctx, static_cast<AsmPush*>(node));
            break;
        case AST_AsmPop_t:
            pop_instr(ctx, static_cast<AsmPop*>(node));
            break;
        case AST_AsmCall_t:
            call_instr(ctx, static_cast<AsmCall*>(node));
            break;
        case AST_AsmRet_t:
            ret_instr(ctx);
            break;
        default:
            THROW_ABORT;
    }
}

static void emit_instr_list(Ctx ctx, const std::vector<std::unique_ptr<AsmInstruction>>& node_list) {
    for (size_t i = node_list[0] ? 0 : 1; i < node_list.size(); ++i) {
        emit_instr(ctx, node_list[i].get());
    }
}

// -> if is_global $ .globl <identifier>
static void glob_directive_toplvl(Ctx ctx, TIdentifier name, bool is_glob) {
    if (is_glob) {
        emit(ctx, TAB ".globl ");
        emit_identifier(ctx, name);
        emit(ctx, LF);
    }
}

// Function(name, global, return_memory, instructions) -> $     <global-directive>
//                                                        $     .text
//                                                        $ <name>:
//                                                        $     pushq %rbp
//                                                        $     movq %rsp, %rbp
//                                                        $     <instructions>
static void emit_fun_toplvl(Ctx ctx, AsmFunction* node) {
    glob_directive_toplvl(ctx, node->name, node->is_glob);
    emit(ctx, TAB ".text" LF);
    emit_identifier(ctx, node->name);
    emit(ctx, ":" LF TAB "pushq %rbp" LF TAB "movq %rsp, %rbp" LF);
    emit_instr_list(ctx, node->instructions);
}

// -> if zero initialized $ .bss
// ->                else $ .data
static void static_section_toplvl(Ctx ctx, const vector_t(std::shared_ptr<StaticInit>) node_list) {
    if (vec_size(node_list) == 1 && node_list[0]->type() == AST_ZeroInit_t) {
        emit(ctx, TAB ".bss" LF);
    }
    else {
        emit(ctx, TAB ".data" LF);
    }
}

// $ .balign <alignment>
static void align_directive_toplvl(Ctx ctx, TInt alignment) {
    if (alignment > 1) {
        emit(ctx, TAB ".balign ");
        emit_int(ctx, alignment);
        emit(ctx, LF);
    }
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
static void static_init_toplvl(Ctx ctx, StaticInit* node) {
    switch (node->type()) {
        case AST_CharInit_t:
            emit(ctx, TAB TAB ".byte ");
            emit_char(ctx, static_cast<CharInit*>(node)->value);
            emit(ctx, LF);
            break;
        case AST_IntInit_t:
            emit(ctx, TAB TAB ".long ");
            emit_int(ctx, static_cast<IntInit*>(node)->value);
            emit(ctx, LF);
            break;
        case AST_LongInit_t:
            emit(ctx, TAB TAB ".quad ");
            emit_long(ctx, static_cast<LongInit*>(node)->value);
            emit(ctx, LF);
            break;
        case AST_DoubleInit_t:
            emit(ctx, TAB TAB ".quad ");
            emit_dbl(ctx, static_cast<DoubleInit*>(node)->dbl_const);
            emit(ctx, LF);
            break;
        case AST_UCharInit_t:
            emit(ctx, TAB TAB ".byte ");
            emit_uchar(ctx, static_cast<UCharInit*>(node)->value);
            emit(ctx, LF);
            break;
        case AST_UIntInit_t:
            emit(ctx, TAB TAB ".long ");
            emit_uint(ctx, static_cast<UIntInit*>(node)->value);
            emit(ctx, LF);
            break;
        case AST_ULongInit_t:
            emit(ctx, TAB TAB ".quad ");
            emit_ulong(ctx, static_cast<ULongInit*>(node)->value);
            emit(ctx, LF);
            break;
        case AST_ZeroInit_t:
            emit(ctx, TAB TAB ".zero ");
            emit_long(ctx, static_cast<ZeroInit*>(node)->byte);
            emit(ctx, LF);
            break;
        case AST_StringInit_t:
            emit(ctx, TAB TAB ".asci");
            {
                StringInit* p_node = static_cast<StringInit*>(node);
                emit(ctx, p_node->is_null_term ? "z" : "i");
                emit(ctx, " \"");
                emit_string(ctx, p_node->string_const);
            }
            emit(ctx, "\"" LF);
            break;
        case AST_PointerInit_t:
            emit(ctx, TAB TAB ".quad " LBL);
            emit_identifier(ctx, static_cast<PointerInit*>(node)->name);
            emit(ctx, LF);
            break;
        default:
            THROW_ABORT;
    }
}

// StaticVariable(name, global, init*) -> $     <global-directive>
//                                        $     <data-section>
//                                        $     <alignment-directive>
//                                        $ <name>:
//                                        $     <init_list>
static void emit_static_var_toplvl(Ctx ctx, AsmStaticVariable* node) {
    glob_directive_toplvl(ctx, node->name, node->is_glob);
    static_section_toplvl(ctx, node->static_inits);
    align_directive_toplvl(ctx, node->alignment);
    emit_identifier(ctx, node->name);
    emit(ctx, ":" LF);
    for (size_t i = 0; i < vec_size(node->static_inits); ++i) {
        static_init_toplvl(ctx, node->static_inits[i].get());
    }
}

// StaticConstant(name, align, init) -> $     .section .rodata
//                                      $     <alignment-directive>
//                                      $ .L<name>:
//                                      $     <init>
static void emit_static_const_toplvl(Ctx ctx, AsmStaticConstant* node) {
#ifdef __APPLE__
    switch (node->static_init->type()) {
        case AST_DoubleInit_t:
            switch (node->alignment) {
                case 8:
                    emit(ctx, TAB ".literal8" LF);
                    emit(ctx, TAB ".balign 8" LF);
                    break;
                case 16:
                    emit(ctx, TAB ".literal16" LF);
                    emit(ctx, TAB ".balign 16" LF);
                    break;
                default:
                    THROW_ABORT;
            }
            break;
        case AST_StringInit_t:
            emit(ctx, TAB ".cstring" LF);
            break;
        default:
            THROW_ABORT;
    }
#else
    emit(ctx, TAB ".section .rodata" LF);
    align_directive_toplvl(ctx, node->alignment);
#endif
    emit(ctx, LBL);
    emit_identifier(ctx, node->name);
    emit(ctx, ":" LF);
    static_init_toplvl(ctx, node->static_init.get());
#ifdef __APPLE__
    if (node->static_init->type() == AST_DoubleInit_t && node->alignment == 16) {
        emit(ctx, TAB TAB ".quad 0" LF);
    }
#endif
}

// Function(name, global, return_memory, instructions) -> $ <function-top-level-directives>
// StaticVariable(name, global, align, init*)          -> $ <static-variable-top-level-directives>
// StaticConstant(name, align, init)                   -> $ <static-constant-top-level-directives>
static void emit_toplvl(Ctx ctx, AsmTopLevel* node) {
    emit(ctx, LF);
    switch (node->type()) {
        case AST_AsmFunction_t:
            emit_fun_toplvl(ctx, static_cast<AsmFunction*>(node));
            break;
        case AST_AsmStaticVariable_t:
            emit_static_var_toplvl(ctx, static_cast<AsmStaticVariable*>(node));
            break;
        case AST_AsmStaticConstant_t:
            emit_static_const_toplvl(ctx, static_cast<AsmStaticConstant*>(node));
            break;
        default:
            THROW_ABORT;
    }
}

// Program(top_level*) -> $ [<top_level>]
//                        $     .section .note.GNU-stack,"",@progbits
static void emit_program(Ctx ctx, AsmProgram* node) {
    for (size_t i = 0; i < vec_size(node->static_const_toplvls); ++i) {
        emit_toplvl(ctx, node->static_const_toplvls[i].get());
    }
    for (size_t i = 0; i < vec_size(node->top_levels); ++i) {
        emit_toplvl(ctx, node->top_levels[i].get());
    }
#ifndef __APPLE__
    emit(ctx, TAB TAB ".section .note.GNU-stack,\"\",@progbits" LF);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void emit_gas_code(std::unique_ptr<AsmProgram>&& asm_ast, BackEndContext* backend, FileIoContext* fileio,
    IdentifierContext* identifiers) {
    GasCodeContext ctx;
    {
        ctx.backend = backend;
        ctx.fileio = fileio;
        ctx.identifiers = identifiers;
    }
    emit_program(&ctx, asm_ast.get());
    asm_ast.reset();
}
