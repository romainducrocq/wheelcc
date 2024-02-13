#include "emitter/att_code.hpp"
#include "util/error.hpp"
#include "util/ctypes.hpp"
#include "ast/ast.hpp"
#include "ast/backend_st.hpp"
#include "ast/asm_ast.hpp"

#include <string>
#include <memory>
#include <vector>

/**
cdef str emit_identifier(TIdentifier node):
    # identifier -> $ identifier
    return node.str_t
*/
// identifier -> $ identifier
static const std::string& emit_identifier(const TIdentifier& identifier) {
    return identifier;
}

/**
cdef str emit_int(TInt node):
    # int -> $ int
    return str(node.int_t)
*/
// int -> $ int
static std::string emit_int(TInt value) {
    return std::to_string(value);
}

/**
cdef str emit_long(TLong node):
    # long -> $ long
    return str(node.long_t)
*/
// long -> $ long
static std::string emit_long(TLong value) {
    return std::to_string(value);
}

/**
cdef str emit_double(TDouble node):
    # double -> $ double
    return str(double_to_binary(node.double_t))
*/
// double -> $ double
static std::string emit_double(TDouble value) {
    return std::to_string(double_to_binary(value));
}

/**
cdef str emit_uint(TUInt node):
    # uint -> $ uint
    return str(node.uint_t)
*/
// uint -> $ uint
static std::string emit_uint(TUInt value) {
    return std::to_string(value);
}

/**
cdef str emit_ulong(TULong node):
    # ulong -> $ ulong
    return str(node.ulong_t)
*/
// ulong -> $ ulong
static std::string emit_ulong(TULong value) {
    return std::to_string(value);
}

/**
cdef str emit_register_rsp_sse(AsmReg node):
    # Reg(SP)    -> $ %rsp
    # Reg(XMM0)  -> $ %xmm0
    # Reg(XMM1)  -> $ %xmm1
    # Reg(XMM2)  -> $ %xmm2
    # Reg(XMM3)  -> $ %xmm3
    # Reg(XMM4)  -> $ %xmm4
    # Reg(XMM5)  -> $ %xmm5
    # Reg(XMM6)  -> $ %xmm6
    # Reg(XMM7)  -> $ %xmm7
    # Reg(XMM14) -> $ %xmm14
    # Reg(XMM15) -> $ %xmm15
    if isinstance(node, AsmSp):
        return "rsp"
    elif isinstance(node, AsmXMM0):
        return "xmm0"
    elif isinstance(node, AsmXMM1):
        return "xmm1"
    elif isinstance(node, AsmXMM2):
        return "xmm2"
    elif isinstance(node, AsmXMM3):
        return "xmm3"
    elif isinstance(node, AsmXMM4):
        return "xmm4"
    elif isinstance(node, AsmXMM5):
        return "xmm5"
    elif isinstance(node, AsmXMM6):
        return "xmm6"
    elif isinstance(node, AsmXMM7):
        return "xmm7"
    elif isinstance(node, AsmXMM14):
        return "xmm14"
    elif isinstance(node, AsmXMM15):
        return "xmm15"
    else:

        raise RuntimeError(
            "An error occurred in code emission, not all nodes were visited")
*/
// Reg(SP)    -> $ %rsp
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

/**
cdef str emit_register_1byte(AsmReg node):
    # Reg(AX)  -> $ %al
    # Reg(DX)  -> $ %dl
    # Reg(CX)  -> $ %cl
    # Reg(DI)  -> $ %dil
    # Reg(SI)  -> $ %sil
    # Reg(R8)  -> $ %r8b
    # Reg(R9)  -> $ %r9b
    # Reg(R10) -> $ %r10b
    # Reg(R11) -> $ %r11b
    if isinstance(node, AsmAx):
        return "al"
    elif isinstance(node, AsmDx):
        return "dl"
    elif isinstance(node, AsmCx):
        return "cl"
    elif isinstance(node, AsmDi):
        return "dil"
    elif isinstance(node, AsmSi):
        return "sil"
    elif isinstance(node, AsmR8):
        return "r8b"
    elif isinstance(node, AsmR9):
        return "r9b"
    elif isinstance(node, AsmR10):
        return "r10b"
    elif isinstance(node, AsmR11):
        return "r11b"
    else:
        return emit_register_rsp_sse(node)
*/
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

/**
cdef str emit_register_4byte(AsmReg node):
    # Reg(AX)  -> $ %eax
    # Reg(DX)  -> $ %edx
    # Reg(CX)  -> $ %ecx
    # Reg(DI)  -> $ %edi
    # Reg(SI)  -> $ %esi
    # Reg(R8)  -> $ %r8d
    # Reg(R9)  -> $ %r9d
    # Reg(R10) -> $ %r10d
    # Reg(R11) -> $ %r11d
    if isinstance(node, AsmAx):
        return "eax"
    elif isinstance(node, AsmDx):
        return "edx"
    elif isinstance(node, AsmCx):
        return "ecx"
    elif isinstance(node, AsmDi):
        return "edi"
    elif isinstance(node, AsmSi):
        return "esi"
    elif isinstance(node, AsmR8):
        return "r8d"
    elif isinstance(node, AsmR9):
        return "r9d"
    elif isinstance(node, AsmR10):
        return "r10d"
    elif isinstance(node, AsmR11):
        return "r11d"
    else:
        return emit_register_rsp_sse(node)
*/
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

/**
cdef str emit_register_8byte(AsmReg node):
    # Reg(AX)    -> $ %rax
    # Reg(DX)    -> $ %rdx
    # Reg(CX)    -> $ %rcx
    # Reg(DI)    -> $ %rdi
    # Reg(SI)    -> $ %rsi
    # Reg(R8)    -> $ %r8
    # Reg(R9)    -> $ %r9
    # Reg(R10)   -> $ %r10
    # Reg(R11)   -> $ %r11
    if isinstance(node, AsmAx):
        return "rax"
    elif isinstance(node, AsmDx):
        return "rdx"
    elif isinstance(node, AsmCx):
        return "rcx"
    elif isinstance(node, AsmDi):
        return "rdi"
    elif isinstance(node, AsmSi):
        return "rsi"
    elif isinstance(node, AsmR8):
        return "r8"
    elif isinstance(node, AsmR9):
        return "r9"
    elif isinstance(node, AsmR10):
        return "r10"
    elif isinstance(node, AsmR11):
        return "r11"
    else:
        return emit_register_rsp_sse(node)
*/
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

/**
cdef str emit_condition_code(AsmCondCode node):
    # E  -> $ e
    # NE -> $ ne
    # L  -> $ l
    # LE -> $ le
    # G  -> $ g
    # GE -> $ ge
    # B  -> $ b
    # BE -> $ be
    # A  -> $ a
    # AE -> $ ae
    # P  -> $ p
    if isinstance(node, AsmE):
        return "e"
    elif isinstance(node, AsmNE):
        return "ne"
    elif isinstance(node, AsmL):
        return "l"
    elif isinstance(node, AsmLE):
        return "le"
    elif isinstance(node, AsmG):
        return "g"
    elif isinstance(node, AsmGE):
        return "ge"
    elif isinstance(node, AsmB):
        return "b"
    elif isinstance(node, AsmBE):
        return "be"
    elif isinstance(node, AsmA):
        return "a"
    elif isinstance(node, AsmAE):
        return "ae"
    elif isinstance(node, AsmP):
        return "p"
    else:

        raise RuntimeError(
            "An error occurred in code emission, not all nodes were visited")
*/
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

/**
cdef int32 emit_type_alignment_bytes(AssemblyType node):
    # LongWord -> $ 4
    # QuadWord -> $ 8
    # Double   -> $ 8
    if isinstance(node, LongWord):
        return 4
    elif isinstance(node, (QuadWord, BackendDouble)):
        return 8
    else:

        raise RuntimeError(
            "An error occurred in code emission, not all nodes were visited")
*/
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

/**
cdef str emit_type_instruction_suffix(AssemblyType node):
    # LongWord -> $ l
    # QuadWord -> $ q
    # Double   -> $ sd
    if isinstance(node, LongWord):
        return "l"
    elif isinstance(node, QuadWord):
        return "q"
    elif isinstance(node, BackendDouble):
        return "sd"
    else:

        raise RuntimeError(
            "An error occurred in code emission, not all nodes were visited")
*/
// LongWord -> $ l
// QuadWord -> $ q
// Double   -> $ sd
static std::string emit_type_instruction_suffix(AssemblyType* node) {
    switch(node->type()) {
        case AST_T::LongWord_t:
            return "l";
        case AST_T::QuadWord_t:
            return "q";
        case AST_T::BackendDouble_t:
            return "sd";
        default:
            RAISE_INTERNAL_ERROR;
    }
}

/**
cdef str emit_operand(AsmOperand node, int32 byte):
    # Imm(int)         -> $ $<int>
    # Register(reg)    -> $ %reg
    # Stack(int)       -> $ <int>(%rbp)
    # Data(identifier) -> $ <identifier>(%rip)
    cdef str operand

    if isinstance(node, AsmImm):
        operand = emit_identifier(node.value)
        return f"${operand}"
    elif isinstance(node, AsmRegister):
        if byte == 1:
            operand = emit_register_1byte(node.reg)
        elif byte == 4:
            operand = emit_register_4byte(node.reg)
        elif byte == 8:
            operand = emit_register_8byte(node.reg)
        else:

            raise RuntimeError(
                "An error occurred in code emission, unmanaged register byte size")

        return f"%{operand}"
    elif isinstance(node, AsmStack):
        operand = emit_int(node.value)
        return F"{operand}(%rbp)"
    elif isinstance(node, AsmData):
        operand = emit_identifier(node.name)
        if operand in backend_symbol_table and \
           isinstance(backend_symbol_table[operand], BackendObj) and \
           backend_symbol_table[operand].is_constant:
            operand = f".L{operand}"
        return f"{operand}(%rip)"
    else:

        raise RuntimeError(
            "An error occurred in code emission, not all nodes were visited")
*/
// Imm(int)         -> $ $<int>
// Register(reg)    -> $ %reg
// Stack(int)       -> $ <int>(%rbp)
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
        case AST_T::AsmStack_t: {
            operand = emit_int(static_cast<AsmStack*>(node)->value);
            return operand + "(%rbp)";
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

/**
cdef str emit_binary_op(AsmBinaryOp node):
    # Add           -> $ add
    # Sub           -> $ sub
    # Mult<i>       -> $ imul
    # Mult<d>       -> $ mul
    # DivDouble     -> $ div
    # BitAnd        -> $ and
    # BitOr         -> $ or
    # BitXor        -> $ xor
    # BitShiftLeft  -> $ shl
    # BitShiftRight -> $ shr
    if isinstance(node, AsmAdd):
        return "add"
    elif isinstance(node, AsmSub):
        return "sub"
    elif isinstance(node, AsmMult):
        return "mul"
    elif isinstance(node, AsmDivDouble):
        return "div"
    elif isinstance(node, AsmBitAnd):
        return "and"
    elif isinstance(node, AsmBitOr):
        return "or"
    elif isinstance(node, AsmBitXor):
        return "xor"
    elif isinstance(node, AsmBitShiftLeft):
        return "shl"
    elif isinstance(node, AsmBitShiftRight):
        return "shr"
    else:

        raise RuntimeError(
            "An error occurred in code emission, not all nodes were visited")
*/
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
static std::string emit_binary_op(AsmBinaryOp* node) {
    switch(node->type()) {
        case AST_T::AsmAdd_t:
            return "add";
        case AST_T::AsmSub_t:
            return "sub";
        case AST_T::AsmMult_t:
            return "mul";
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

/**
cdef str emit_unary_op(AsmUnaryOp node):
    # Neg -> $ neg
    # Not -> $ not
    # Shr -> $ shr
    if isinstance(node, AsmNeg):
        return "neg"
    elif isinstance(node, AsmNot):
        return "not"
    elif isinstance(node, AsmShr):
        return "shr"
    else:

        raise RuntimeError(
            "An error occurred in code emission, not all nodes were visited")
*/
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

/**
cdef void emit(str line, int32 indent):
    line = " " * 4 * indent + line

    write_line(line)
*/
// TODO
#include <iostream>
static void emit(std::string&& line, size_t t) {
    for(size_t i = 0; i < t; i++) {
        line = "    " + line;
    }

    std::cout << line << std::endl;
    // write_line(line);
}

/**
cdef void emit_ret_instructions(AsmRet node):
    emit("movq %rbp, %rsp", 1)
    emit("popq %rbp", 1)
    emit("ret", 1)
*/
static void emit_ret_instructions(AsmRet* /*node*/) {
    emit("movq %rbp, %rsp", 1);
    emit("popq %rbp", 1);
    emit("ret", 1);
}

/**
cdef void emit_mov_instructions(AsmMov node):
    cdef int32 byte = emit_type_alignment_bytes(node.assembly_type)
    cdef str t = emit_type_instruction_suffix(node.assembly_type)
    cdef str src = emit_operand(node.src, byte)
    cdef str dst = emit_operand(node.dst, byte)
    emit(f"mov{t} {src}, {dst}", 1)
*/
static void emit_mov_instructions(AsmMov* node) {
    TInt byte = emit_type_alignment_bytes(node->assembly_type.get());
    std::string t = emit_type_instruction_suffix(node->assembly_type.get());
    std::string src = emit_operand(node->src.get(), byte);
    std::string dst = emit_operand(node->dst.get(), byte);
    emit("mov" + t + " " + src + ", " + dst, 1);
}

/**
cdef void emit_mov_sx_instructions(AsmMovSx node):
    cdef str src = emit_operand(node.src, 4)
    cdef str dst = emit_operand(node.dst, 8)
    emit(f"movslq {src}, {dst}", 1)
*/
static void emit_mov_sx_instructions(AsmMovSx* node) {
    std::string src = emit_operand(node->src.get(), 4);
    std::string dst = emit_operand(node->dst.get(), 8);
    emit("movslq " + src + ", " + dst, 1);
}

/**
cdef void emit_cvttsd2si_instructions(AsmCvttsd2si node):
    cdef int32 byte = emit_type_alignment_bytes(node.assembly_type)
    cdef str t = emit_type_instruction_suffix(node.assembly_type)
    cdef str src = emit_operand(node.src, byte)
    cdef str dst = emit_operand(node.dst, byte)
    emit(f"cvttsd2si{t} {src}, {dst}", 1)
*/
static void emit_cvttsd2si_instructions(AsmCvttsd2si* node) {
    TInt byte = emit_type_alignment_bytes(node->assembly_type.get());
    std::string t = emit_type_instruction_suffix(node->assembly_type.get());
    std::string src = emit_operand(node->src.get(), byte);
    std::string dst = emit_operand(node->dst.get(), byte);
    emit("cvttsd2si" + t + " " + src + ", " + dst, 1);
}

/**
cdef void emit_cvtsi2sd_instructions(AsmCvtsi2sd node):
    cdef int32 byte = emit_type_alignment_bytes(node.assembly_type)
    cdef str t = emit_type_instruction_suffix(node.assembly_type)
    cdef str src = emit_operand(node.src, byte)
    cdef str dst = emit_operand(node.dst, byte)
    emit(f"cvtsi2sd{t} {src}, {dst}", 1)
*/
static void emit_cvtsi2sd_instructions(AsmCvtsi2sd* node) {
    TInt byte = emit_type_alignment_bytes(node->assembly_type.get());
    std::string t = emit_type_instruction_suffix(node->assembly_type.get());
    std::string src = emit_operand(node->src.get(), byte);
    std::string dst = emit_operand(node->dst.get(), byte);
    emit("cvtsi2sd" + t + " " + src + ", " + dst, 1);
}

/**
cdef void emit_push_instructions(AsmPush node):
    cdef str src = emit_operand(node.src, 8)
    emit(f"pushq {src}", 1)
*/
static void emit_push_instructions(AsmPush* node) {
    std::string src = emit_operand(node->src.get(), 8);
    emit("pushq " + src, 1);
}

/**
cdef void emit_call_instructions(AsmCall node):
    cdef str label = emit_identifier(node.name)
    emit(f"call {label}@PLT", 1)
*/
static void emit_call_instructions(AsmCall* node) {
    std::string label = emit_identifier(node->name);
    emit("call " + label + "@PLT", 1);
}

/**
cdef void emit_label_instructions(AsmLabel node):
    cdef str label = emit_identifier(node.name)
    emit(f".L{label}:", 0)
*/
static void emit_label_instructions(AsmLabel* node) {
    std::string label = emit_identifier(node->name);
    emit(".L" + label + ":", 0);
}

/**
cdef void emit_cmp_instructions(AsmCmp node):
    cdef int32 byte = emit_type_alignment_bytes(node.assembly_type)
    cdef str t = emit_type_instruction_suffix(node.assembly_type)
    cdef str src = emit_operand(node.src, byte)
    cdef str dst = emit_operand(node.dst, byte)
    if isinstance(node.assembly_type, BackendDouble):
        emit(f"comi{t} {src}, {dst}", 1)
    else:
        emit(f"cmp{t} {src}, {dst}", 1)
*/
static void emit_cmp_instructions(AsmCmp* node) {
    TInt byte = emit_type_alignment_bytes(node->assembly_type.get());
    std::string t = emit_type_instruction_suffix(node->assembly_type.get());
    std::string src = emit_operand(node->src.get(), byte);
    std::string dst = emit_operand(node->dst.get(), byte);
    if(node->assembly_type->type() == AST_T::BackendDouble_t) {
        emit("comi" + t + " " + src + ", " + dst, 1);
    }
    else {
        emit("cmp" + t + " " + src + ", " + dst, 1);
    }
}

/**
cdef void emit_jmp_instructions(AsmJmp node):
    cdef str label = emit_identifier(node.target)
    emit(f"jmp .L{label}", 1)
*/
static void emit_jmp_instructions(AsmJmp* node) {
    std::string label = emit_identifier(node->target);
    emit("jmp .L" + label, 1);
}

/**
cdef void emit_jmp_cc_instructions(AsmJmpCC node):
    cdef str cond_code = emit_condition_code(node.cond_code)
    cdef str label = emit_identifier(node.target)
    emit(f"j{cond_code} .L{label}", 1)
*/
static void emit_jmp_cc_instructions(AsmJmpCC* node) {
    std::string cond_code = emit_condition_code(node->cond_code.get());
    std::string label = emit_identifier(node->target);
    emit("j" + cond_code + " .L" + label, 1);
}

/**
cdef void emit_set_cc_instructions(AsmSetCC node):
    cdef str cond_code = emit_condition_code(node.cond_code)
    cdef str dst = emit_operand(node.dst, 1)
    emit(f"set{cond_code} {dst}", 1)
*/
static void emit_set_cc_instructions(AsmSetCC* node) {
    std::string cond_code = emit_condition_code(node->cond_code.get());
    std::string dst = emit_operand(node->dst.get(), 1);
    emit("set" + cond_code + " " + dst, 1);
}

/**
cdef void emit_unary_instructions(AsmUnary node):
    cdef int32 byte = emit_type_alignment_bytes(node.assembly_type)
    cdef str t = emit_type_instruction_suffix(node.assembly_type)
    cdef str unary_op = emit_unary_op(node.unary_op)
    cdef str dst = emit_operand(node.dst, byte)
    emit(f"{unary_op}{t} {dst}", 1)
*/
static void emit_unary_instructions(AsmUnary* node) {
    TInt byte = emit_type_alignment_bytes(node->assembly_type.get());
    std::string t = emit_type_instruction_suffix(node->assembly_type.get());
    std::string unary_op = emit_unary_op(node->unary_op.get());
    std::string dst = emit_operand(node->dst.get(), byte);
    emit(unary_op + t + " " + dst, 1);
}

/**
cdef void emit_binary_instructions(AsmBinary node):
    cdef int32 byte = emit_type_alignment_bytes(node.assembly_type)
    cdef str t
    if isinstance(node.binary_op, AsmBitXor) and \
       isinstance(node.assembly_type, BackendDouble):
        t = "pd"
    else:
        t = emit_type_instruction_suffix(node.assembly_type)
    cdef str binary_op = emit_binary_op(node.binary_op)
    if isinstance(node.binary_op, AsmMult) and \
       not isinstance(node.assembly_type, BackendDouble):
        binary_op = f"i{binary_op}"
    cdef str src = emit_operand(node.src, byte)
    cdef str dst = emit_operand(node.dst, byte)
    emit(f"{binary_op}{t} {src}, {dst}", 1)
*/
static void emit_binary_instructions(AsmBinary* node) {
    TInt byte = emit_type_alignment_bytes(node->assembly_type.get());
    std::string t;
    if(node->binary_op->type() == AST_T::AsmBitXor_t &&
       node->assembly_type->type() == AST_T::BackendDouble_t) {
        t = "pd";
    }
    else {
        t = emit_type_instruction_suffix(node->assembly_type.get());
    }
    std::string binary_op = emit_binary_op(node->binary_op.get());
    if(node->binary_op->type() == AST_T::AsmMult_t &&
       node->assembly_type->type() != AST_T::BackendDouble_t) {
        binary_op = "i" + binary_op;
    }
    std::string src = emit_operand(node->src.get(), byte);
    std::string dst = emit_operand(node->dst.get(), byte);
    emit(binary_op + t + " " + src + ", " + dst, 1);
}

/**
cdef void emit_idiv_instructions(AsmIdiv node):
    cdef int32 byte = emit_type_alignment_bytes(node.assembly_type)
    cdef str t = emit_type_instruction_suffix(node.assembly_type)
    cdef str src = emit_operand(node.src, byte)
    emit(f"idiv{t} {src}", 1)
*/
static void emit_idiv_instructions(AsmIdiv* node) {
    TInt byte = emit_type_alignment_bytes(node->assembly_type.get());
    std::string t = emit_type_instruction_suffix(node->assembly_type.get());
    std::string src = emit_operand(node->src.get(), byte);
    emit("idiv" + t + " " + src, 1);
}

/**
cdef void emit_div_instructions(AsmDiv node):
    cdef int32 byte = emit_type_alignment_bytes(node.assembly_type)
    cdef str t = emit_type_instruction_suffix(node.assembly_type)
    cdef str src = emit_operand(node.src, byte)
    emit(f"div{t} {src}", 1)
*/
static void emit_div_instructions(AsmDiv* node) {
    TInt byte = emit_type_alignment_bytes(node->assembly_type.get());
    std::string t = emit_type_instruction_suffix(node->assembly_type.get());
    std::string src = emit_operand(node->src.get(), byte);
    emit("div" + t + " " + src, 1);
}

/**
cdef void emit_cdq_instructions(AsmCdq node):
    if isinstance(node.assembly_type, LongWord):
        emit("cdq", 1)
    elif isinstance(node.assembly_type, QuadWord):
        emit("cqo", 1)
    else:

        raise RuntimeError(
            "An error occurred in code emission, not all nodes were visited")
*/
static void emit_cdq_instructions(AsmCdq* node) {
    switch(node->assembly_type->type()) {
        case AST_T::LongWord_t:
            emit("cdq", 1);
            break;
        case AST_T::QuadWord_t:
            emit("cqo", 1);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

/**
cdef void emit_instructions(AsmInstruction node):
    # Ret                                  -> $ movq %rbp, %rsp
    #                                         $ popq %rbp
    #                                         $ ret
    # Mov(t, src, dst)                     -> $ mov<t> <src>, <dst>
    # MovSx(src, dst)                      -> $ movslq <src>, <dst>
    # Cvttsd2si(t, src, dst)               -> $ cvttsd2si<t> <src>, <dst>
    # Cvtsi2sd(t, src, dst)                -> $ cvtsi2sd<t> <src>, <dst>
    # Push(operand)                        -> $ pushq <operand>
    # Call(label)                          -> $ call <label>@PLT
    # Label(label)                         -> $ .L<label>:
    # Cmp(t, operand, operand)<i>          -> $ cmp<t> <operand>, <operand>
    # Cmp(t, operand, operand)<d>          -> $ comi<t> <operand>, <operand>
    # Jmp(label)                           -> $ jmp .L<label>
    # JmpCC(cond_code, label)              -> $ j<cond_code> .L<label>
    # SetCC(cond_code, operand)            -> $ set<cond_code> <operand>
    # Unary(unary_operator, t, operand)    -> $ <unary_operator><t> <operand>
    # Binary(binary_operator, t, src, dst) -> $ <binary_operator><t> <src>, <dst>
    # Idiv(t, operand)                     -> $ idiv<t> <operand>
    # Div(t, operand)                      -> $ div<t> <operand>
    # Cdq<l>                               -> $ cdq
    # Cdq<q>                               -> $ cqo
    if isinstance(node, AsmRet):
        emit_ret_instructions(node)
    elif isinstance(node, AsmMov):
        emit_mov_instructions(node)
    elif isinstance(node, AsmMovSx):
        emit_mov_sx_instructions(node)
    elif isinstance(node, AsmCvttsd2si):
        emit_cvttsd2si_instructions(node)
    elif isinstance(node, AsmCvtsi2sd):
        emit_cvtsi2sd_instructions(node)
    elif isinstance(node, AsmPush):
        emit_push_instructions(node)
    elif isinstance(node, AsmCall):
        emit_call_instructions(node)
    elif isinstance(node, AsmLabel):
        emit_label_instructions(node)
    elif isinstance(node, AsmCmp):
        emit_cmp_instructions(node)
    elif isinstance(node, AsmJmp):
        emit_jmp_instructions(node)
    elif isinstance(node, AsmJmpCC):
        emit_jmp_cc_instructions(node)
    elif isinstance(node, AsmSetCC):
        emit_set_cc_instructions(node)
    elif isinstance(node, AsmUnary):
        emit_unary_instructions(node)
    elif isinstance(node, AsmBinary):
        emit_binary_instructions(node)
    elif isinstance(node, AsmIdiv):
        emit_idiv_instructions(node)
    elif isinstance(node, AsmDiv):
        emit_div_instructions(node)
    elif isinstance(node, AsmCdq):
        emit_cdq_instructions(node)
    else:

        raise RuntimeError(
            "An error occurred in code emission, not all nodes were visited")
*/
// Ret                                  -> $ movq %rbp, %rsp
//                                         $ popq %rbp
//                                         $ ret
// Mov(t, src, dst)                     -> $ mov<t> <src>, <dst>
// MovSx(src, dst)                      -> $ movslq <src>, <dst>
// Cvttsd2si(t, src, dst)               -> $ cvttsd2si<t> <src>, <dst>
// Cvtsi2sd(t, src, dst)                -> $ cvtsi2sd<t> <src>, <dst>
// Push(operand)                        -> $ pushq <operand>
// Call(label)                          -> $ call <label>@PLT
// Label(label)                         -> $ .L<label>:
// Cmp(t, operand, operand)<i>          -> $ cmp<t> <operand>, <operand>
// Cmp(t, operand, operand)<d>          -> $ comi<t> <operand>, <operand>
// Jmp(label)                           -> $ jmp .L<label>
// JmpCC(cond_code, label)              -> $ j<cond_code> .L<label>
// SetCC(cond_code, operand)            -> $ set<cond_code> <operand>
// Unary(unary_operator, t, operand)    -> $ <unary_operator><t> <operand>
// Binary(binary_operator, t, src, dst) -> $ <binary_operator><t> <src>, <dst>
// Idiv(t, operand)                     -> $ idiv<t> <operand>
// Div(t, operand)                      -> $ div<t> <operand>
// Cdq<l>                               -> $ cdq
// Cdq<q>                               -> $ cqo
static void emit_instructions(AsmInstruction* node) {
    switch(node->type()) {
        case AST_T::AsmRet_t:
            emit_ret_instructions(static_cast<AsmRet*>(node));
            break;
        case AST_T::AsmMov_t:
            emit_mov_instructions(static_cast<AsmMov*>(node));
            break;
        case AST_T::AsmMovSx_t:
            emit_mov_sx_instructions(static_cast<AsmMovSx*>(node));
            break;
        case AST_T::AsmCvttsd2si_t:
            emit_cvttsd2si_instructions(static_cast<AsmCvttsd2si*>(node));
            break;
        case AST_T::AsmCvtsi2sd_t:
            emit_cvtsi2sd_instructions(static_cast<AsmCvtsi2sd*>(node));
            break;
        case AST_T::AsmPush_t:
            emit_push_instructions(static_cast<AsmPush*>(node));
            break;
        case AST_T::AsmCall_t:
            emit_call_instructions(static_cast<AsmCall*>(node));
            break;
        case AST_T::AsmLabel_t:
            emit_label_instructions(static_cast<AsmLabel*>(node));
            break;
        case AST_T::AsmCmp_t:
            emit_cmp_instructions(static_cast<AsmCmp*>(node));
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
        case AST_T::AsmUnary_t:
            emit_unary_instructions(static_cast<AsmUnary*>(node));
            break;
        case AST_T::AsmBinary_t:
            emit_binary_instructions(static_cast<AsmBinary*>(node));
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
        default:
            RAISE_INTERNAL_ERROR;
    }
}

/**
cdef void emit_list_instructions(list[AsmInstruction] list_node):
    cdef Py_ssize_t instruction
    for instruction in range(len(list_node)):
        emit_instructions(list_node[instruction])
*/
static void emit_list_instructions(std::vector<std::unique_ptr<AsmInstruction>>& list_node) {
    for(size_t instruction = 0; instruction < list_node.size(); instruction++) {
        emit_instructions(list_node[instruction].get());
    }
}

/**
cdef void emit_alignment_directive_top_level(TInt alignment):
    # $ .align <alignment>
    cdef str align = emit_int(alignment)
    emit(f".align {align}", 1)
*/
// $ .align <alignment>
static void emit_alignment_directive_top_level(TInt alignment) {
    std::string align = emit_int(alignment);
    emit(".align " + align, 1);
}

/**
cdef void emit_global_directive_top_level(bint is_global, str name):
    # if is_global: -> $ .globl <identifier>
    if is_global:
        emit(f".globl {name}", 1)
*/
// -> if is_global $ .globl <identifier>
static void emit_global_directive_top_level(const std::string& name, bool is_global) {
    if(is_global) {
        emit(".globl " + name, 1);
    }
}

/**
cdef void emit_function_top_level(AsmFunction node):
    # Function(name, global, instructions) -> $     <global-directive>
    #                                         $     .text
    #                                         $ <name>:
    #                                         $     pushq %rbp
    #                                         $     movq %rsp, %rbp
    #                                         $     <instructions>
    cdef str name = emit_identifier(node.name)
    emit_global_directive_top_level(node.is_global, name)
    emit(".text", 1)
    emit(f"{name}:", 0)
    emit("pushq %rbp", 1)
    emit("movq %rsp, %rbp", 1)
    emit_list_instructions(node.instructions)
*/
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

/**
cdef void emit_data_static_variable_top_level(AsmStaticVariable node, str static_init):
    # StaticVariable(name, global, init) initialized to non-zero value -> $     <global-directive>
    #                                                                     $     .data
    #                                                                     $     <alignment-directive>
    #                                                                     $ <name>:
    #                                                       if init<i>(i) $     .long <i>
    #                                                     elif init<i>(i) $     .quad <i>
    #                                                     elif init<d>(d) $     .quad <d>
    cdef str name = emit_identifier(node.name)
    emit_global_directive_top_level(node.is_global, name)
    emit(".data", 1)
    emit_alignment_directive_top_level(node.alignment)
    emit(f"{name}:", 0)
    emit(static_init, 1)
*/
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
    emit(std::move(static_init), 1);
}

/**
cdef void emit_bss_static_variable_top_level(AsmStaticVariable node, str static_init):
    # StaticVariable(name, global, init) initialized to zero -> $     <global-directive>
    #                                                           $     .bss
    #                                                           $     <alignment-directive>
    #                                                           $ <name>:
    #                                            if int-init(0) $     .zero 4
    #                                         elif long-init(0) $     .zero 8
    cdef str name = emit_identifier(node.name)
    emit_global_directive_top_level(node.is_global, name)
    emit(".bss", 1)
    emit_alignment_directive_top_level(node.alignment)
    emit(f"{name}:", 0)
    emit(static_init, 1)
*/
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
    emit(std::move(static_init), 1);
}

/**
cdef void emit_static_variable_top_level(AsmStaticVariable node):
    # StaticVariable(name, global, align, init)<i> initialized to non-zero value -> $ <data-static-variable-directives>
    # StaticVariable(name, global, align, init)<d>                               -> $ <data-static-variable-directives>
    # StaticVariable(name, global, align, init)<i> initialized to zero           -> $ <bss-static-variable-directives>
    cdef str static_init
    if isinstance(node.initial_value, IntInit):
        if node.initial_value.value.int_t:
            static_init = emit_int(node.initial_value.value)
            static_init = f".long {static_init}"
            emit_data_static_variable_top_level(node, static_init)
        else:
            static_init = ".zero 4"
            emit_bss_static_variable_top_level(node, static_init)
    elif isinstance(node.initial_value, LongInit):
        if node.initial_value.value.long_t:
            static_init = emit_long(node.initial_value.value)
            static_init = f".quad {static_init}"
            emit_data_static_variable_top_level(node, static_init)
        else:
            static_init = ".zero 8"
            emit_bss_static_variable_top_level(node, static_init)
    elif isinstance(node.initial_value, DoubleInit):
        static_init = emit_double(node.initial_value.value)
        static_init = f".quad {static_init}"
        emit_data_static_variable_top_level(node, static_init)
    elif isinstance(node.initial_value, UIntInit):
        if node.initial_value.value.uint_t:
            static_init = emit_uint(node.initial_value.value)
            static_init = f".long {static_init}"
            emit_data_static_variable_top_level(node, static_init)
        else:
            static_init = ".zero 4"
            emit_bss_static_variable_top_level(node, static_init)
    elif isinstance(node.initial_value, ULongInit):
        if node.initial_value.value.ulong_t:
            static_init = emit_ulong(node.initial_value.value)
            static_init = f".quad {static_init}"
            emit_data_static_variable_top_level(node, static_init)
        else:
            static_init = ".zero 8"
            emit_bss_static_variable_top_level(node, static_init)
    else:

        raise RuntimeError(
            "An error occurred in code emission, not all nodes were visited")
*/
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
            static_init = emit_double(static_cast<DoubleInit*>(node->initial_value.get())->value);
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

/**
cdef void emit_double_static_constant_top_level(AsmStaticConstant node):
    # StaticConstant(name, align, init) -> $     .section .rodata
    #                                      $     <alignment-directive>
    #                                      $ .L<name>:
    #                                      $     .quad <d>
    cdef str name = emit_identifier(node.name)
    cdef str static_init = emit_double(node.initial_value.value)
    emit(".section .rodata", 1)
    emit_alignment_directive_top_level(node.alignment)
    emit(f".L{name}:", 0)
    emit(f".quad {static_init}", 1)
*/
// StaticConstant(name, align, init) -> $     .section .rodata
//                                      $     <alignment-directive>
//                                      $ .L<name>:
//                                      $     .quad <d>
static void emit_double_static_constant_top_level(AsmStaticConstant* node) {
    std::string name = emit_identifier(node->name);
    std::string static_init = emit_double(static_cast<DoubleInit*>(node->initial_value.get())->value);
    emit(".section .rodata", 1);
    emit_alignment_directive_top_level(node->alignment);
    emit(".L" + name + ":", 0);
    emit(".quad " + static_init, 1);
}

/**
cdef void emit_static_constant_top_level(AsmStaticConstant node):
    # StaticConstant(name, align, init)<d> -> $ <double-static-constant-directives>
    if isinstance(node.initial_value, DoubleInit):
        emit_double_static_constant_top_level(node)
    else:

        raise RuntimeError(
            "An error occurred in code emission, not all nodes were visited")
*/
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

/**
cdef void emit_top_level(AsmTopLevel node):
    # Function(name, global, instructions)      -> $ <function-top-level-directives>
    # StaticVariable(name, global, align, init) -> $ <static-variable-top-level-directives>
    # StaticConstant(name, align, init)         -> $ <static-constant-top-level-directives>
    if isinstance(node, AsmFunction):
        emit_function_top_level(node)
    elif isinstance(node, AsmStaticVariable):
        emit_static_variable_top_level(node)
    elif isinstance(node, AsmStaticConstant):
        emit_static_constant_top_level(node)
    else:

        raise RuntimeError(
            "An error occurred in code emission, not all nodes were visited")
*/
// Function(name, global, instructions)      -> $ <function-top-level-directives>
// StaticVariable(name, global, align, init) -> $ <static-variable-top-level-directives>
// StaticConstant(name, align, init)         -> $ <static-constant-top-level-directives>
static void emit_top_level(AsmTopLevel* node) {
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

/**
cdef void emit_program(AsmProgram node):
    # Program(top_level*) -> $ [<top_level>]
    #                        $     .section .note.GNU-stack,"",@progbits
    cdef Py_ssize_t top_level
    for top_level in range(len(node.static_constant_top_levels)):
        emit_top_level(node.static_constant_top_levels[top_level])
    for top_level in range(len(node.top_levels)):
        emit_top_level(node.top_levels[top_level])
    emit(".section .note.GNU-stack,\"\",@progbits", 1)
*/
// Program(top_level*) -> $ [<top_level>]
//                        $     .section .note.GNU-stack,"",@progbits
static void emit_program(AsmProgram* node) {
    for(size_t top_level = 0; top_level < node->static_constant_top_levels.size(); top_level++) {
        emit_top_level(node->static_constant_top_levels[top_level].get());
    }
    for(size_t top_level = 0; top_level < node->top_levels.size(); top_level++) {
        emit_top_level(node->top_levels[top_level].get());
    }
    emit(".section .note.GNU-stack,\"\",@progbits", 1);
}

/**
cdef void code_emission(AsmProgram asm_ast, str filename):

    file_open_write(filename)

    emit_program(asm_ast)

    file_close_write()
*/
// TODO
void code_emission(std::unique_ptr<AsmProgram> asm_ast, std::string&& /*filename*/) {
    // file_open_write(filename);
    emit_program(asm_ast.get());
    asm_ast.reset();
    // file_close_write();
}
