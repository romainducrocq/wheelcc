#ifndef _PARSER_C_AST_HPP
#define _PARSER_C_AST_HPP

#include <inttypes.h>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class CConst(AST):
    pass
*/

/** TODO
cdef class CConstInt(CConst):
    cdef public TInt value
*/

/** TODO
cdef class CConstLong(CConst):
    cdef public TLong value
*/

/** TODO
cdef class CConstUInt(CConst):
    cdef public TUInt value
*/

/** TODO
cdef class CConstULong(CConst):
    cdef public TULong value
*/

/** TODO
cdef class CConstDouble(CConst):
    cdef public TDouble value
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class CUnaryOp(AST):
    pass
*/

/** TODO
cdef class CComplement(CUnaryOp):
    pass
*/

/** TODO
cdef class CNegate(CUnaryOp):
    pass
*/

/** TODO
cdef class CNot(CUnaryOp):
    pass
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class CBinaryOp(AST):
    pass
*/

/** TODO
cdef class CAdd(CBinaryOp):
    pass
*/

/** TODO
cdef class CSubtract(CBinaryOp):
    pass
*/

/** TODO
cdef class CMultiply(CBinaryOp):
    pass
*/

/** TODO
cdef class CDivide(CBinaryOp):
    pass
*/

/** TODO
cdef class CRemainder(CBinaryOp):
    pass
*/

/** TODO
cdef class CBitAnd(CBinaryOp):
    pass
*/

/** TODO
cdef class CBitOr(CBinaryOp):
    pass
*/

/** TODO
cdef class CBitXor(CBinaryOp):
    pass
*/

/** TODO
cdef class CBitShiftLeft(CBinaryOp):
    pass
*/

/** TODO
cdef class CBitShiftRight(CBinaryOp):
    pass
*/

/** TODO
cdef class CAnd(CBinaryOp):
    pass
*/

/** TODO
cdef class COr(CBinaryOp):
    pass
*/

/** TODO
cdef class CEqual(CBinaryOp):
    pass
*/

/** TODO
cdef class CNotEqual(CBinaryOp):
    pass
*/

/** TODO
cdef class CLessThan(CBinaryOp):
    pass
*/

/** TODO
cdef class CLessOrEqual(CBinaryOp):
    pass
*/

/** TODO
cdef class CGreaterThan(CBinaryOp):
    pass
*/

/** TODO
cdef class CGreaterOrEqual(CBinaryOp):
    pass
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class CExp(AST):
    cdef public Type exp_type
*/

/** TODO
cdef class CConstant(CExp):
    cdef public CConst constant
*/

/** TODO
cdef class CVar(CExp):
    cdef public TIdentifier name
*/

/** TODO
cdef class CCast(CExp):
    cdef public CExp exp
    cdef public Type target_type
*/

/** TODO
cdef class CUnary(CExp):
    cdef public CUnaryOp unary_op
    cdef public CExp exp
*/

/** TODO
cdef class CBinary(CExp):
    cdef public CBinaryOp binary_op
    cdef public CExp exp_left
    cdef public CExp exp_right
*/

/** TODO
cdef class CAssignment(CExp):
    cdef public CExp exp_left
    cdef public CExp exp_right
*/

/** TODO
cdef class CConditional(CExp):
    cdef public CExp condition
    cdef public CExp exp_middle
    cdef public CExp exp_right
*/

/** TODO
cdef class CAssignmentCompound(CExp):
    cdef public CBinaryOp binary_op
    cdef public CExp exp_left
    cdef public CExp exp_right
*/

/** TODO
cdef class CFunctionCall(CExp):
    cdef public TIdentifier name
    cdef public list[CExp] args
 */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class CStatement(AST):
    pass
*/

/** TODO
cdef class CReturn(CStatement):
    cdef public CExp exp
*/

/** TODO
cdef class CExpression(CStatement):
    cdef public CExp exp
*/

/** TODO
cdef class CIf(CStatement):
    cdef public CExp condition
    cdef public CStatement then
    # Optional
    cdef public CStatement else_fi
*/

/** TODO
cdef class CGoto(CStatement):
    cdef public TIdentifier target
*/

/** TODO
cdef class CLabel(CStatement):
    cdef public TIdentifier target
    cdef public CStatement jump_to
*/

/** TODO
cdef class CCompound(CStatement):
    cdef public CBlock block
*/

/** TODO
cdef class CWhile(CStatement):
    cdef public CExp condition
    cdef public CStatement body
    cdef public TIdentifier target
*/

/** TODO
cdef class CDoWhile(CStatement):
    cdef public CExp condition
    cdef public CStatement body
    cdef public TIdentifier target
*/

/** TODO
cdef class CFor(CStatement):
    cdef public CForInit init
    # Optional
    cdef public CExp condition
    # Optional
    cdef public CExp post
    cdef public CStatement body
    cdef public TIdentifier target
*/

/** TODO
cdef class CBreak(CStatement):
    cdef public TIdentifier target
*/

/** TODO
cdef class CContinue(CStatement):
    cdef public TIdentifier target
*/

/** TODO
cdef class CNull(CStatement):
    pass
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class CForInit(AST):
    pass
*/

/** TODO
cdef class CInitDecl(CForInit):
    cdef public CVariableDeclaration init
*/

/** TODO
cdef class CInitExp(CForInit):
    # Optional
    cdef public CExp init
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class CBlock(AST):
    pass
*/

/** TODO
cdef class CB(CBlock):
    cdef public list[CBlockItem] block_items
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class CBlockItem(AST):
    pass
*/

/** TODO
cdef class CS(CBlockItem):
    cdef public CStatement statement
*/

/** TODO
cdef class CD(CBlockItem):
    cdef public CDeclaration declaration
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class CStorageClass(AST):
    pass
*/

/** TODO
cdef class CStatic(CStorageClass):
    pass
*/

/** TODO
cdef class CExtern(CStorageClass):
    pass
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class CFunctionDeclaration(AST):
    cdef public TIdentifier name
    cdef public list[TIdentifier] params
    # Optional
    cdef public CBlock body
    cdef public Type fun_type
    # Optional
    cdef public CStorageClass storage_class
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class CVariableDeclaration(AST):
    cdef public TIdentifier name
    # Optional
    cdef public CExp init
    cdef public Type var_type
    # Optional
    cdef public CStorageClass storage_class
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class CDeclaration(AST):
    pass
*/

/** TODO

cdef class CFunDecl(CDeclaration):
    cdef public CFunctionDeclaration function_decl
*/

/** TODO
cdef class CVarDecl(CDeclaration):
    cdef public CVariableDeclaration variable_decl
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class CProgram(AST):
    cdef public list[CDeclaration] declarations
*/

#endif
