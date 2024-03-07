#ifndef _AST_AST_HPP
#define _AST_AST_HPP

#include <inttypes.h>
#include <string>

enum AST_T {
    // Symbol table
    Type_t,
    Char_t,
    SChar_t,
    UChar_t,
    Int_t,
    Long_t,
    UInt_t,
    ULong_t,
    Double_t,
    FunType_t,
    Pointer_t,
    Array_t,
    StaticInit_t,
    IntInit_t,
    LongInit_t,
    UIntInit_t,
    ULongInit_t,
    DoubleInit_t,
    ZeroInit_t,
    InitialValue_t,
    Tentative_t,
    Initial_t,
    NoInitializer_t,
    IdentifierAttr_t,
    FunAttr_t,
    StaticAttr_t,
    LocalAttr_t,
    Symbol_t,

    // Backend st
    AssemblyType_t,
    LongWord_t,
    QuadWord_t,
    BackendDouble_t,
    ByteArray_t,
    BackendSymbol_t,
    BackendObj_t,
    BackendFun_t,

    // C ast
    CConst_t,
    CConstInt_t,
    CConstLong_t,
    CConstUInt_t,
    CConstULong_t,
    CConstDouble_t,
    CConstChar_t,
    CConstUChar_t,
    CUnaryOp_t,
    CComplement_t,
    CNegate_t,
    CNot_t,
    CBinaryOp_t,
    CAdd_t,
    CSubtract_t,
    CMultiply_t,
    CDivide_t,
    CRemainder_t,
    CBitAnd_t,
    CBitOr_t,
    CBitXor_t,
    CBitShiftLeft_t,
    CBitShiftRight_t,
    CAnd_t,
    COr_t,
    CEqual_t,
    CNotEqual_t,
    CLessThan_t,
    CLessOrEqual_t,
    CGreaterThan_t,
    CGreaterOrEqual_t,
    CAbstractDeclarator_t,
    CAbstractPointer_t,
    CAbstractArray_t,
    CAbstractBase_t,
    CParam_t,
    CDeclarator_t,
    CIdent_t,
    CPointerDeclarator_t,
    CArrayDeclarator_t,
    CFunDeclarator_t,
    CExp_t,
    CConstant_t,
    CString_t,
    CVar_t,
    CCast_t,
    CUnary_t,
    CBinary_t,
    CAssignment_t,
    CConditional_t,
    CFunctionCall_t,
    CDereference_t,
    CAddrOf_t,
    CSubscript_t,
    CStatement_t,
    CReturn_t,
    CExpression_t,
    CIf_t,
    CGoto_t,
    CLabel_t,
    CCompound_t,
    CWhile_t,
    CDoWhile_t,
    CFor_t,
    CBreak_t,
    CContinue_t,
    CNull_t,
    CForInit_t,
    CInitDecl_t,
    CInitExp_t,
    CBlock_t,
    CB_t,
    CBlockItem_t,
    CS_t,
    CD_t,
    CStorageClass_t,
    CStatic_t,
    CExtern_t,
    CInitializer_t,
    CSingleInit_t,
    CCompoundInit_t,
    CFunctionDeclaration_t,
    CVariableDeclaration_t,
    CDeclaration_t,
    CFunDecl_t,
    CVarDecl_t,
    CProgram_t,

    // TAC ast
    TacUnaryOp_t,
    TacComplement_t,
    TacNegate_t,
    TacNot_t,
    TacBinaryOp_t,
    TacAdd_t,
    TacSubtract_t,
    TacMultiply_t,
    TacDivide_t,
    TacRemainder_t,
    TacBitAnd_t,
    TacBitOr_t,
    TacBitXor_t,
    TacBitShiftLeft_t,
    TacBitShiftRight_t,
    TacEqual_t,
    TacNotEqual_t,
    TacLessThan_t,
    TacLessOrEqual_t,
    TacGreaterThan_t,
    TacGreaterOrEqual_t,
    TacValue_t,
    TacConstant_t,
    TacVariable_t,
    TacExpResult_t,
    TacPlainOperand_t,
    TacDereferencedPointer_t,
    TacInstruction_t,
    TacReturn_t,
    TacSignExtend_t,
    TacTruncate_t,
    TacZeroExtend_t,
    TacDoubleToInt_t,
    TacDoubleToUInt_t,
    TacIntToDouble_t,
    TacUIntToDouble_t,
    TacFunCall_t,
    TacUnary_t,
    TacBinary_t,
    TacCopy_t,
    TacGetAddress_t,
    TacLoad_t,
    TacStore_t,
    TacAddPtr_t,
    TacCopyToOffset_t,
    TacJump_t,
    TacJumpIfZero_t,
    TacJumpIfNotZero_t,
    TacLabel_t,
    TacTopLevel_t,
    TacFunction_t,
    TacStaticVariable_t,
    TacProgram_t,

    // ASM AST
    AsmReg_t,
    AsmAx_t,
    AsmCx_t,
    AsmDx_t,
    AsmDi_t,
    AsmSi_t,
    AsmR8_t,
    AsmR9_t,
    AsmR10_t,
    AsmR11_t,
    AsmSp_t,
    AsmBp_t,
    AsmXMM0_t,
    AsmXMM1_t,
    AsmXMM2_t,
    AsmXMM3_t,
    AsmXMM4_t,
    AsmXMM5_t,
    AsmXMM6_t,
    AsmXMM7_t,
    AsmXMM14_t,
    AsmXMM15_t,
    AsmCondCode_t,
    AsmE_t,
    AsmNE_t,
    AsmG_t,
    AsmGE_t,
    AsmL_t,
    AsmLE_t,
    AsmA_t,
    AsmAE_t,
    AsmB_t,
    AsmBE_t,
    AsmP_t,
    AsmOperand_t,
    AsmImm_t,
    AsmRegister_t,
    AsmPseudo_t,
    AsmMemory_t,
    AsmData_t,
    AsmPseudoMem_t,
    AsmIndexed_t,
    AsmBinaryOp_t,
    AsmAdd_t,
    AsmSub_t,
    AsmMult_t,
    AsmDivDouble_t,
    AsmBitAnd_t,
    AsmBitOr_t,
    AsmBitXor_t,
    AsmBitShiftLeft_t,
    AsmBitShiftRight_t,
    AsmUnaryOp_t,
    AsmNot_t,
    AsmNeg_t,
    AsmShr_t,
    AsmInstruction_t,
    AsmMov_t,
    AsmMovSx_t,
    AsmMovZeroExtend_t,
    AsmLea_t,
    AsmCvttsd2si_t,
    AsmCvtsi2sd_t,
    AsmUnary_t,
    AsmBinary_t,
    AsmCmp_t,
    AsmIdiv_t,
    AsmDiv_t,
    AsmCdq_t,
    AsmJmp_t,
    AsmJmpCC_t,
    AsmSetCC_t,
    AsmLabel_t,
    AsmPush_t,
    AsmCall_t,
    AsmRet_t,
    AsmTopLevel_t,
    AsmFunction_t,
    AsmStaticVariable_t,
    AsmStaticConstant_t,
    AsmProgram_t
};

struct Ast {
    virtual ~Ast() = 0;
    virtual AST_T type() = 0;
};

using TIdentifier = std::string;
using TString = std::string;
using TChar = int8_t;
using TInt = int32_t;
using TLong = int64_t;
using TDouble = double;
using TUChar = uint8_t;
using TUInt = uint32_t;
using TULong = uint64_t;

// https://mkhan45.github.io/2021/05/10/Modeling-ASTs-in-Different-Languages.html
// https://github.com/agentcooper/cpp-ast-example/blob/main/ast_inheritance.cpp

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// const = ConstInt(int)
//       | ConstLong(long)
//       | ConstUInt(uint)
//       | ConstULong(ulong)
//       | ConstDouble(double)
struct CConst : Ast {
    AST_T type() override;
};

struct CConstInt : CConst {
    AST_T type() override;
    CConstInt() = default;
    CConstInt(TInt value);

    TInt value;
};

struct CConstLong : CConst {
    AST_T type() override;
    CConstLong() = default;
    CConstLong(TLong value);

    TLong value;
};

struct CConstUInt : CConst {
    AST_T type() override;
    CConstUInt() = default;
    CConstUInt(TUInt value);

    TUInt value;
};

struct CConstULong : CConst {
    AST_T type() override;
    CConstULong() = default;
    CConstULong(TULong value);

    TULong value;
};

struct CConstDouble : CConst {
    AST_T type() override;
    CConstDouble() = default;
    CConstDouble(TDouble value);

    TDouble value;
};

struct CConstChar : CConst {
    AST_T type() override;
    CConstChar() = default;
    CConstChar(TChar value);

    TChar value;
};

struct CConstUChar : CConst {
    AST_T type() override;
    CConstUChar() = default;
    CConstUChar(TUChar value);

    TUChar value;
};

/*
struct Dummy : Ast {
};
*/

#endif
