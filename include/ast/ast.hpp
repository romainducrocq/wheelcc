#ifndef _AST_AST_HPP
#define _AST_AST_HPP

#include <inttypes.h>
#include <string>

enum AST_T {
    // Symbol table
    Type_t,
    Int_t,
    Long_t,
    Double_t,
    UInt_t,
    ULong_t,
    FunType_t,
    StaticInit_t,
    IntInit_t,
    LongInit_t,
    DoubleInit_t,
    UIntInit_t,
    ULongInit_t,
    InitialValue_t,
    Tentative_t,
    Initial_t,
    NoInitializer_t,
    IdentifierAttr_t,
    FunAttr_t,
    StaticAttr_t,
    LocalAttr_t,
    Symbol_t,

    // C ast
    CConst_t,
    CConstInt_t,
    CConstLong_t,
    CConstUInt_t,
    CConstULong_t,
    CConstDouble_t,
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
    CExp_t,
    CConstant_t,
    CVar_t,
    CCast_t,
    CUnary_t,
    CBinary_t,
    CAssignment_t,
    CConditional_t,
    CAssignmentCompound_t,
    CFunctionCall_t,
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
    CFunctionDeclaration_t,
    CVariableDeclaration_t,
    CDeclaration_t,
    CFunDecl_t,
    CVarDecl_t,
    CProgram_t
};

#define NODE_PTR(TYPE) TYPE*
#define MAKE_NODE(TYPE, NODE) static_cast<TYPE*>(NODE.get())
#define NODE_GET(TYPE, NODE, GET) MAKE_NODE(TYPE, NODE)->GET

struct Ast {
    virtual ~Ast() = 0;
    virtual AST_T type() = 0;
};

using TIdentifier = std::string;
using TInt = int32_t;
using TLong = int64_t;
using TDouble = double;
using TUInt = uint32_t;
using TULong = uint64_t;

bool is_instance(AST_T t1, AST_T t2);

#endif
