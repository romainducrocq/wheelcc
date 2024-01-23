#ifndef _AST_AST_HPP
#define _AST_AST_HPP

#include <inttypes.h>
#include <string>

enum Ast_t {
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
    virtual Ast_t type() = 0;
};

using TIdentifier = std::string;
using int_t = int32_t;
using long_t = int64_t;
using uint_t = uint32_t;
using ulong_t = uint64_t;

bool is_instance(Ast_t t1, Ast_t t2);

#endif
