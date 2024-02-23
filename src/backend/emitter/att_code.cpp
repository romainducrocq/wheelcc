#include "backend/emitter/att_code.hpp"
#include "util/error.hpp"
#include "util/fopen.hpp"
#include "ast/ast.hpp"
#include "ast/back_symt.hpp"
#include "ast/back_ast.hpp"

#include <string>
#include <memory>
#include <vector>

// identifier -> $ identifier
static const std::string& emit_identifier(const TIdentifier& identifier) {
    return identifier;
}

// int -> $ int
static std::string emit_int(TInt value) {
    return std::to_string(value);
}

// long -> $ long
static std::string emit_long(TLong value) {
    return std::to_string(value);
}

// double -> $ double
static std::string emit_double(TULong binary) {
    return std::to_string(binary);
}

// uint -> $ uint
static std::string emit_uint(TUInt value) {
    return std::to_string(value);
}

// ulong -> $ ulong
static std::string emit_ulong(TULong value) {
    return std::to_string(value);
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
// Reg(XMM14) -> $ %xmm14
// Reg(XMM15) -> $ %xmm15
static std::string emit_register_rsp_sse(AsmReg* node) {
    switch(node->type()) {
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
        case AST_T::AsmXMM14_t:
            return "xmm14";
        case AST_T::AsmXMM15_t:
            return "xmm15";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

// Reg(AX)  -> $ %al
// Reg(DX)  -> $ %dl
// Reg(CX)  -> $ %cl
// Reg(DI)  -> $ %dil
// Reg(SI)  -> $ %sil
// Reg(R8)  -> $ %r8b
// Reg(R9)  -> $ %r9b
// Reg(R10) -> $ %r10b
// Reg(R11) -> $ %r11b
static std::string emit_register_1byte(AsmReg* node) {
    switch(node->type()) {
        case AST_T::AsmAx_t:
            return "al";
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
        default:
            return emit_register_rsp_sse(node);
    }
}

// Reg(AX)  -> $ %eax
// Reg(DX)  -> $ %edx
// Reg(CX)  -> $ %ecx
// Reg(DI)  -> $ %edi
// Reg(SI)  -> $ %esi
// Reg(R8)  -> $ %r8d
// Reg(R9)  -> $ %r9d
// Reg(R10) -> $ %r10d
// Reg(R11) -> $ %r11d
static std::string emit_register_4byte(AsmReg* node) {
    switch(node->type()) {
        case AST_T::AsmAx_t:
            return "eax";
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
        default:
            return emit_register_rsp_sse(node);
    }
}

// Reg(AX)    -> $ %rax
// Reg(DX)    -> $ %rdx
// Reg(CX)    -> $ %rcx
// Reg(DI)    -> $ %rdi
// Reg(SI)    -> $ %rsi
// Reg(R8)    -> $ %r8
// Reg(R9)    -> $ %r9
// Reg(R10)   -> $ %r10
// Reg(R11)   -> $ %r11
static std::string emit_register_8byte(AsmReg* node) {
    switch(node->type()) {
        case AST_T::AsmAx_t:
            return "rax";
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
        default:
            return emit_register_rsp_sse(node);
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
static std::string emit_condition_code(AsmCondCode* node) {
    switch(node->type()) {
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

// LongWord -> $ 4
// QuadWord -> $ 8
// Double   -> $ 8
static TInt emit_type_alignment_bytes(AssemblyType* node) {
    switch(node->type()) {
        case AST_T::LongWord_t:
            return 4;
        case AST_T::QuadWord_t:
        case AST_T::BackendDouble_t:
            return 8;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

// LongWord -> $ l
// QuadWord -> $ q
// Double   -> $ sd
static std::string emit_type_instruction_suffix(AssemblyType* node, bool c) {
    switch(node->type()) {
        case AST_T::LongWord_t:
            return "l";
        case AST_T::QuadWord_t:
            return "q";
        case AST_T::BackendDouble_t:
            return c ? "pd" : "sd";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::string emit_type_instruction_suffix(AssemblyType* node) {
    return emit_type_instruction_suffix(node, false);
}

// Imm(int)         -> $ $<int>
// Register(reg)    -> $ %reg
// Memory(int, reg) -> $ <int>(<reg>)
// Data(identifier) -> $ <identifier>(%rip)
static std::string emit_operand(AsmOperand* node, TInt byte) {
    std::string operand;
    switch(node->type()) {
        case AST_T::AsmImm_t: {
            operand = emit_identifier(static_cast<AsmImm*>(node)->value);
            return "$" + operand;
        }
        case AST_T::AsmRegister_t: {
            switch(byte) {
                case 1:
                    operand = emit_register_1byte(static_cast<AsmRegister*>(node)->reg.get());
                    break;
                case 4:
                    operand = emit_register_4byte(static_cast<AsmRegister*>(node)->reg.get());
                    break;
                case 8:
                    operand = emit_register_8byte(static_cast<AsmRegister*>(node)->reg.get());
                    break;
                default:
                    RAISE_INTERNAL_ERROR;
            }
            return "%" + operand;
        }
        case AST_T::AsmMemory_t: {
            AsmMemory* p_node = static_cast<AsmMemory*>(node);
            std::string value = "";
            if(p_node->value != 0) {
                value = emit_int(p_node->value);
            }
            operand = emit_register_8byte(p_node->reg.get());
            return value + "(%" + operand + ")";
        }
        case AST_T::AsmData_t: {
            operand = emit_identifier(static_cast<AsmData*>(node)->name);
            if(backend_symbol_table.find(operand) != backend_symbol_table.end() &&
               backend_symbol_table[operand]->type() == AST_T::BackendObj_t &&
               static_cast<BackendObj*>(backend_symbol_table[operand].get())->is_constant) {
                operand = ".L" + operand;
            }
            return operand + "(%rip)";
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

// Neg -> $ neg
// Not -> $ not
// Shr -> $ shr
static std::string emit_unary_op(AsmUnaryOp* node) {
    switch(node->type()) {
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

// Add           -> $ add
// Sub           -> $ sub
// Mult<i>       -> $ imul
// Mult<d>       -> $ mul
// DivDouble     -> $ div
// BitAnd        -> $ and
// BitOr         -> $ or
// BitXor        -> $ xor
// BitShiftLeft  -> $ shl
// BitShiftRight -> $ shr
static std::string emit_binary_op(AsmBinaryOp* node, bool c) {
    switch(node->type()) {
        case AST_T::AsmAdd_t:
            return "add";
        case AST_T::AsmSub_t:
            return "sub";
        case AST_T::AsmMult_t:
            return c ? "mul" : "imul";
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
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void emit(std::string&& line, size_t t) {
    for(size_t i = 0; i < t; i++) {
        line = "    " + line;
    }

    write_line(std::move(line));
}

static void emit_mov_instructions(AsmMov* node) {
    TInt byte = emit_type_alignment_bytes(node->assembly_type.get());
    std::string t = emit_type_instruction_suffix(node->assembly_type.get());
    std::string src = emit_operand(node->src.get(), byte);
    std::string dst = emit_operand(node->dst.get(), byte);
    emit("mov" + t + " " + src + ", " + dst, 2);
}

static void emit_mov_sx_instructions(AsmMovSx* node) {
    std::string src = emit_operand(node->src.get(), 4);
    std::string dst = emit_operand(node->dst.get(), 8);
    emit("movslq " + src + ", " + dst, 2);
}

static void emit_lea_instructions(AsmLea* node) {
    std::string src = emit_operand(node->src.get(), 8);
    std::string dst = emit_operand(node->dst.get(), 8);
    emit("leaq " + src + ", " + dst, 2);
}

static void emit_cvttsd2si_instructions(AsmCvttsd2si* node) {
    TInt byte = emit_type_alignment_bytes(node->assembly_type.get());
    std::string t = emit_type_instruction_suffix(node->assembly_type.get());
    std::string src = emit_operand(node->src.get(), byte);
    std::string dst = emit_operand(node->dst.get(), byte);
    emit("cvttsd2si" + t + " " + src + ", " + dst, 2);
}

static void emit_cvtsi2sd_instructions(AsmCvtsi2sd* node) {
    TInt byte = emit_type_alignment_bytes(node->assembly_type.get());
    std::string t = emit_type_instruction_suffix(node->assembly_type.get());
    std::string src = emit_operand(node->src.get(), byte);
    std::string dst = emit_operand(node->dst.get(), byte);
    emit("cvtsi2sd" + t + " " + src + ", " + dst, 2);
}

static void emit_unary_instructions(AsmUnary* node) {
    TInt byte = emit_type_alignment_bytes(node->assembly_type.get());
    std::string t = emit_type_instruction_suffix(node->assembly_type.get());
    std::string unary_op = emit_unary_op(node->unary_op.get());
    std::string dst = emit_operand(node->dst.get(), byte);
    emit(unary_op + t + " " + dst, 2);
}

static void emit_binary_instructions(AsmBinary* node) {
    TInt byte = emit_type_alignment_bytes(node->assembly_type.get());
    std::string t = emit_type_instruction_suffix(node->assembly_type.get(),
                                                 node->binary_op->type() == AST_T::AsmBitXor_t &&
                                                    node->assembly_type->type() == AST_T::BackendDouble_t);
    std::string binary_op = emit_binary_op(node->binary_op.get(),
                                           node->assembly_type->type() == AST_T::BackendDouble_t);
    std::string src = emit_operand(node->src.get(), byte);
    std::string dst = emit_operand(node->dst.get(), byte);
    emit(binary_op + t + " " + src + ", " + dst, 2);
}

static void emit_cmp_instructions(AsmCmp* node) {
    TInt byte = emit_type_alignment_bytes(node->assembly_type.get());
    std::string t = emit_type_instruction_suffix(node->assembly_type.get());
    std::string src = emit_operand(node->src.get(), byte);
    std::string dst = emit_operand(node->dst.get(), byte);
    if(node->assembly_type->type() == AST_T::BackendDouble_t) {
        emit("comi" + t + " " + src + ", " + dst, 2);
    }
    else {
        emit("cmp" + t + " " + src + ", " + dst, 2);
    }
}

static void emit_idiv_instructions(AsmIdiv* node) {
    TInt byte = emit_type_alignment_bytes(node->assembly_type.get());
    std::string t = emit_type_instruction_suffix(node->assembly_type.get());
    std::string src = emit_operand(node->src.get(), byte);
    emit("idiv" + t + " " + src, 2);
}

static void emit_div_instructions(AsmDiv* node) {
    TInt byte = emit_type_alignment_bytes(node->assembly_type.get());
    std::string t = emit_type_instruction_suffix(node->assembly_type.get());
    std::string src = emit_operand(node->src.get(), byte);
    emit("div" + t + " " + src, 2);
}

static void emit_cdq_instructions(AsmCdq* node) {
    switch(node->assembly_type->type()) {
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

static void emit_jmp_instructions(AsmJmp* node) {
    std::string label = emit_identifier(node->target);
    emit("jmp .L" + label, 2);
}

static void emit_jmp_cc_instructions(AsmJmpCC* node) {
    std::string cond_code = emit_condition_code(node->cond_code.get());
    std::string label = emit_identifier(node->target);
    emit("j" + cond_code + " .L" + label, 2);
}

static void emit_set_cc_instructions(AsmSetCC* node) {
    std::string cond_code = emit_condition_code(node->cond_code.get());
    std::string dst = emit_operand(node->dst.get(), 1);
    emit("set" + cond_code + " " + dst, 2);
}

static void emit_label_instructions(AsmLabel* node) {
    std::string label = emit_identifier(node->name);
    emit(".L" + label + ":", 1);
}

static void emit_push_instructions(AsmPush* node) {
    std::string src = emit_operand(node->src.get(), 8);
    emit("pushq " + src, 2);
}

static void emit_call_instructions(AsmCall* node) {
    std::string label = emit_identifier(node->name);
    emit("call " + label + "@PLT", 2);
}

static void emit_ret_instructions() {
    emit("movq %rbp, %rsp", 1);
    emit("popq %rbp", 1);
    emit("ret", 1);
}

// Mov(t, src, dst)                     -> $ mov<t> <src>, <dst>
// MovSx(src, dst)                      -> $ movslq <src>, <dst>
// Lea(src, dst)                        -> $ leaq <src>, <dst>
// Cvttsd2si(t, src, dst)               -> $ cvttsd2si<t> <src>, <dst>
// Cvtsi2sd(t, src, dst)                -> $ cvtsi2sd<t> <src>, <dst>
// Unary(unary_operator, t, operand)    -> $ <unary_operator><t> <operand>
// Binary(binary_operator, t, src, dst) -> $ <binary_operator><t> <src>, <dst>
// Cmp(t, operand, operand)<i>          -> $ cmp<t> <operand>, <operand>
// Cmp(operand, operand)<d>             -> $ comisd <operand>, <operand>
// Idiv(t, operand)                     -> $ idiv<t> <operand>
// Div(t, operand)                      -> $ div<t> <operand>
// Cdq<l>                               -> $ cdq
// Cdq<q>                               -> $ cqo
// Jmp(label)                           -> $ jmp .L<label>
// JmpCC(cond_code, label)              -> $ j<cond_code> .L<label>
// SetCC(cond_code, operand)            -> $ set<cond_code> <operand>
// Label(label)                         -> $ .L<label>:
// Push(operand)                        -> $ pushq <operand>
// Call(label)                          -> $ call <label>@PLT
// Ret                                  -> $ movq %rbp, %rsp
//                                         $ popq %rbp
//                                         $ ret
static void emit_instructions(AsmInstruction* node) {
    switch(node->type()) {
        case AST_T::AsmMov_t:
            emit_mov_instructions(static_cast<AsmMov*>(node));
            break;
        case AST_T::AsmMovSx_t:
            emit_mov_sx_instructions(static_cast<AsmMovSx*>(node));
            break;
        case AST_T::AsmLea_t:
            emit_lea_instructions(static_cast<AsmLea*>(node));
            break;
        case AST_T::AsmCvttsd2si_t:
            emit_cvttsd2si_instructions(static_cast<AsmCvttsd2si*>(node));
            break;
        case AST_T::AsmCvtsi2sd_t:
            emit_cvtsi2sd_instructions(static_cast<AsmCvtsi2sd*>(node));
            break;
        case AST_T::AsmUnary_t:
            emit_unary_instructions(static_cast<AsmUnary*>(node));
            break;
        case AST_T::AsmBinary_t:
            emit_binary_instructions(static_cast<AsmBinary*>(node));
            break;
        case AST_T::AsmCmp_t:
            emit_cmp_instructions(static_cast<AsmCmp*>(node));
            break;
        case AST_T::AsmIdiv_t:
            emit_idiv_instructions(static_cast<AsmIdiv*>(node));
            break;
        case AST_T::AsmDiv_t:
            emit_div_instructions(static_cast<AsmDiv*>(node));
            break;
        case AST_T::AsmCdq_t:
            emit_cdq_instructions(static_cast<AsmCdq*>(node));
            break;
        case AST_T::AsmJmp_t:
            emit_jmp_instructions(static_cast<AsmJmp*>(node));
            break;
        case AST_T::AsmJmpCC_t:
            emit_jmp_cc_instructions(static_cast<AsmJmpCC*>(node));
            break;
        case AST_T::AsmSetCC_t:
            emit_set_cc_instructions(static_cast<AsmSetCC*>(node));
            break;
        case AST_T::AsmLabel_t:
            emit_label_instructions(static_cast<AsmLabel*>(node));
            break;
        case AST_T::AsmPush_t:
            emit_push_instructions(static_cast<AsmPush*>(node));
            break;
        case AST_T::AsmCall_t:
            emit_call_instructions(static_cast<AsmCall*>(node));
            break;
        case AST_T::AsmRet_t:
            emit_ret_instructions();
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void emit_list_instructions(std::vector<std::unique_ptr<AsmInstruction>>& list_node) {
    for(size_t instruction = list_node[0] ? 0 : 1; instruction < list_node.size(); instruction++) {
        emit_instructions(list_node[instruction].get());
    }
}

// $ .align <alignment>
static void emit_alignment_directive_top_level(TInt alignment) {
    std::string align = emit_int(alignment);
    emit(".balign " + align, 1);
}

// -> if is_global $ .globl <identifier>
static void emit_global_directive_top_level(const std::string& name, bool is_global) {
    if(is_global) {
        emit(".globl " + name, 1);
    }
}

// Function(name, global, instructions) -> $     <global-directive>
//                                         $     .text
//                                         $ <name>:
//                                         $     pushq %rbp
//                                         $     movq %rsp, %rbp
//                                         $     <instructions>
static void emit_function_top_level(AsmFunction* node) {
    std::string name = emit_identifier(node->name);
    emit_global_directive_top_level(name, node->is_global);
    emit(".text", 1);
    emit(name + ":", 0);
    emit("pushq %rbp", 1);
    emit("movq %rsp, %rbp", 1);
    emit_list_instructions(node->instructions);
}

// StaticVariable(name, global, init) initialized to non-zero value -> $     <global-directive>
//                                                                     $     .data
//                                                                     $     <alignment-directive>
//                                                                     $ <name>:
//                                                       if init<i>(i) $     .long <i>
//                                                     elif init<i>(i) $     .quad <i>
//                                                     elif init<d>(d) $     .quad <d>
static void emit_data_static_variable_top_level(AsmStaticVariable* node, std::string&& static_init) {
    std::string name = emit_identifier(node->name);
    emit_global_directive_top_level(name, node->is_global);
    emit(".data", 1);
    emit_alignment_directive_top_level(node->alignment);
    emit(name + ":", 0);
    emit(std::move(static_init), 2);
}

// StaticVariable(name, global, init) initialized to zero -> $     <global-directive>
//                                                           $     .bss
//                                                           $     <alignment-directive>
//                                                           $ <name>:
//                                            if int-init(0) $     .zero 4
//                                         elif long-init(0) $     .zero 8
static void emit_bss_static_variable_top_level(AsmStaticVariable* node, std::string&& static_init) {
    std::string name = emit_identifier(node->name);
    emit_global_directive_top_level(name, node->is_global);
    emit(".bss", 1);
    emit_alignment_directive_top_level(node->alignment);
    emit(name + ":", 0);
    emit(std::move(static_init), 2);
}

// StaticVariable(name, global, align, init)<i> initialized to non-zero value -> $ <data-static-variable-directives>
// StaticVariable(name, global, align, init)<d>                               -> $ <data-static-variable-directives>
// StaticVariable(name, global, align, init)<i> initialized to zero           -> $ <bss-static-variable-directives>
static void emit_static_variable_top_level(AsmStaticVariable* node) {
    std::string static_init;
    switch(node->initial_value->type()) {
        case AST_T::IntInit_t: {
            IntInit* initial_value = static_cast<IntInit*>(node->initial_value.get());
            if(initial_value->value != 0) {
                static_init = emit_int(initial_value->value);
                static_init = ".long " + static_init;
                emit_data_static_variable_top_level(node, std::move(static_init));
            }
            else {
                static_init = ".zero 4";
                emit_bss_static_variable_top_level(node, std::move(static_init));
            }
            break;
        }
        case AST_T::LongInit_t: {
            LongInit* initial_value = static_cast<LongInit*>(node->initial_value.get());
            if(initial_value->value != 0) {
                static_init = emit_long(initial_value->value);
                static_init = ".quad " + static_init;
                emit_data_static_variable_top_level(node, std::move(static_init));
            }
            else {
                static_init = ".zero 8";
                emit_bss_static_variable_top_level(node, std::move(static_init));
            }
            break;
        }
        case AST_T::DoubleInit_t: {
            static_init = emit_double(static_cast<DoubleInit*>(node->initial_value.get())->binary);
            static_init = ".quad " + static_init;
            emit_data_static_variable_top_level(node, std::move(static_init));
            break;
        }
        case AST_T::UIntInit_t: {
            UIntInit* initial_value = static_cast<UIntInit*>(node->initial_value.get());
            if(initial_value->value != 0) {
                static_init = emit_uint(initial_value->value);
                static_init = ".long " + static_init;
                emit_data_static_variable_top_level(node, std::move(static_init));
            }
            else {
                static_init = ".zero 4";
                emit_bss_static_variable_top_level(node, std::move(static_init));
            }
            break;
        }
        case AST_T::ULongInit_t: {
            ULongInit* initial_value = static_cast<ULongInit*>(node->initial_value.get());
            if(initial_value->value != 0) {
                static_init = emit_ulong(initial_value->value);
                static_init = ".quad " + static_init;
                emit_data_static_variable_top_level(node, std::move(static_init));
            }
            else {
                static_init = ".zero 8";
                emit_bss_static_variable_top_level(node, std::move(static_init));
            }
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

// StaticConstant(name, align, init) -> $     .section .rodata
//                                      $     <alignment-directive>
//                                      $ .L<name>:
//                                      $     .quad <d>
static void emit_double_static_constant_top_level(AsmStaticConstant* node) {
    std::string name = emit_identifier(node->name);
    std::string static_init = emit_double(static_cast<DoubleInit*>(node->initial_value.get())->binary);
    emit(".section .rodata", 1);
    emit_alignment_directive_top_level(node->alignment);
    emit(".L" + name + ":", 0);
    emit(".quad " + static_init, 2);
}

// StaticConstant(name, align, init)<d> -> $ <double-static-constant-directives>
static void emit_static_constant_top_level(AsmStaticConstant* node) {
    switch(node->initial_value->type()) {
        case AST_T::DoubleInit_t:
            emit_double_static_constant_top_level(node);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

// Function(name, global, instructions)      -> $ <function-top-level-directives>
// StaticVariable(name, global, align, init) -> $ <static-variable-top-level-directives>
// StaticConstant(name, align, init)         -> $ <static-constant-top-level-directives>
static void emit_top_level(AsmTopLevel* node) {
    emit("", 0);
    switch(node->type()) {
        case AST_T::AsmFunction_t:
            emit_function_top_level(static_cast<AsmFunction*>(node));
            break;
        case AST_T::AsmStaticVariable_t:
            emit_static_variable_top_level(static_cast<AsmStaticVariable*>(node));
            break;
        case AST_T::AsmStaticConstant_t:
            emit_static_constant_top_level(static_cast<AsmStaticConstant*>(node));
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

// Program(top_level*) -> $ [<top_level>]
//                        $     .section .note.GNU-stack,"",@progbits
static void emit_program(AsmProgram* node) {
    for(size_t top_level = 0; top_level < node->static_constant_top_levels.size(); top_level++) {
        emit_top_level(node->static_constant_top_levels[top_level].get());
    }
    for(size_t top_level = 0; top_level < node->top_levels.size(); top_level++) {
        emit_top_level(node->top_levels[top_level].get());
    }
    emit(".section .note.GNU-stack,\"\",@progbits", 2);
}

void code_emission(std::unique_ptr<AsmProgram> asm_ast, std::string&& filename) {
    file_open_write(filename);
    emit_program(asm_ast.get());
    asm_ast.reset();
    file_close_write();
}
