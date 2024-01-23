#ifndef _AST_C_AST_HPP
#define _AST_C_AST_HPP

#include "ast/ast.hpp"
#include "ast/symbol_table.hpp"

#include <memory>

// https://mkhan45.github.io/2021/05/10/Modeling-ASTs-in-Different-Languages.html
// https://github.com/agentcooper/cpp-ast-example/blob/main/ast_inheritance.cpp

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class CConst(AST):
    pass
*/
struct CConst : Ast {
    AST_T type() override;
};

/** TODO
cdef class CConstInt(CConst):
    cdef public TInt value
*/
struct CConstInt : CConst {
    AST_T type() override;
    CConstInt() = default;
    CConstInt(TInt value);

    TInt value;
};

/** TODO
cdef class CConstLong(CConst):
    cdef public TLong value
*/
struct CConstLong : CConst {
    AST_T type() override;
    CConstLong() = default;
    CConstLong(TLong value);

    TLong value;
};

/** TODO
cdef class CConstDouble(CConst):
    cdef public TDouble value
*/
struct CConstDouble : CConst {
    AST_T type() override;
    CConstDouble() = default;
    CConstDouble(TDouble value);

    TDouble value;
};

/** TODO
cdef class CConstUInt(CConst):
    cdef public TUInt value
*/
struct CConstUInt : CConst {
    AST_T type() override;
    CConstUInt() = default;
    CConstUInt(TUInt value);

    TUInt value;
};

/** TODO
cdef class CConstULong(CConst):
    cdef public TULong value
*/
struct CConstULong : CConst {
    AST_T type() override;
    CConstULong() = default;
    CConstULong(TULong value);

    TULong value;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class CUnaryOp(AST):
    pass
*/
struct CUnaryOp : Ast {
    AST_T type() override;
};

/** TODO
cdef class CComplement(CUnaryOp):
    pass
*/
struct CComplement : CUnaryOp {
    AST_T type() override;
};

/** TODO
cdef class CNegate(CUnaryOp):
    pass
*/
struct CNegate : CUnaryOp {
    AST_T type() override;
};

/** TODO
cdef class CNot(CUnaryOp):
    pass
*/
struct CNot : CUnaryOp {
    AST_T type() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class CBinaryOp(AST):
    pass
*/
struct CBinaryOp : Ast {
    AST_T type() override;
};

/** TODO
cdef class CAdd(CBinaryOp):
    pass
*/
struct CAdd : CBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class CSubtract(CBinaryOp):
    pass
*/
struct CSubtract : CBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class CMultiply(CBinaryOp):
    pass
*/
struct CMultiply : CBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class CDivide(CBinaryOp):
    pass
*/
struct CDivide : CBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class CRemainder(CBinaryOp):
    pass
*/
struct CRemainder : CBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class CBitAnd(CBinaryOp):
    pass
*/
struct CBitAnd : CBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class CBitOr(CBinaryOp):
    pass
*/
struct CBitOr : CBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class CBitXor(CBinaryOp):
    pass
*/
struct CBitXor : CBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class CBitShiftLeft(CBinaryOp):
    pass
*/
struct CBitShiftLeft : CBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class CBitShiftRight(CBinaryOp):
    pass
*/
struct CBitShiftRight : CBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class CAnd(CBinaryOp):
    pass
*/
struct CAnd : CBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class COr(CBinaryOp):
    pass
*/
struct COr : CBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class CEqual(CBinaryOp):
    pass
*/
struct CEqual : CBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class CNotEqual(CBinaryOp):
    pass
*/
struct CNotEqual : CBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class CLessThan(CBinaryOp):
    pass
*/
struct CLessThan : CBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class CLessOrEqual(CBinaryOp):
    pass
*/
struct CLessOrEqual : CBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class CGreaterThan(CBinaryOp):
    pass
*/
struct CGreaterThan : CBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class CGreaterOrEqual(CBinaryOp):
    pass
*/
struct CGreaterOrEqual : CBinaryOp {
    AST_T type() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class CExp(AST):
    cdef public Type exp_type
*/
struct CExp : Ast {
    AST_T type() override;
    CExp() = default;

    std::unique_ptr<Type> exp_type;
};

/** TODO
cdef class CConstant(CExp):
    cdef public CConst constant
*/
struct CConstant : CExp {
    AST_T type() override;
    CConstant() = default;
    CConstant(std::unique_ptr<CConst>&& constant);

    std::unique_ptr<CConst> constant;
};

/** TODO
cdef class CVar(CExp):
    cdef public TIdentifier name
*/
struct CVar : CExp {
    AST_T type() override;
    CVar() = default;
    CVar(TIdentifier&& name);

    TIdentifier name;
};

/** TODO
cdef class CCast(CExp):
    cdef public CExp exp
    cdef public Type target_type
*/
struct CCast : CExp {
    AST_T type() override;
    CCast() = default;
    CCast(std::unique_ptr<CExp>&& exp, std::unique_ptr<Type>&& target_type);

    std::unique_ptr<CExp> exp;
    std::unique_ptr<Type> target_type;
};

/** TODO
cdef class CUnary(CExp):
    cdef public CUnaryOp unary_op
    cdef public CExp exp
*/
struct CUnary : CExp {
    AST_T type() override;
    CUnary() = default;
    CUnary(std::unique_ptr<CUnaryOp>&& unary_op, std::unique_ptr<CExp>&& exp);

    std::unique_ptr<CUnaryOp> unary_op;
    std::unique_ptr<CExp> exp;
};

/** TODO
cdef class CBinary(CExp):
    cdef public CBinaryOp binary_op
    cdef public CExp exp_left
    cdef public CExp exp_right
*/
struct CBinary : CExp {
    AST_T type() override;
    CBinary() = default;
    CBinary(std::unique_ptr<CBinaryOp>&& binary_op, std::unique_ptr<CExp>&& exp_left,
            std::unique_ptr<CExp>&& exp_right);

    std::unique_ptr<CBinaryOp> binary_op;
    std::unique_ptr<CExp> exp_left;
    std::unique_ptr<CExp> exp_right;
};

/** TODO
cdef class CAssignment(CExp):
    cdef public CExp exp_left
    cdef public CExp exp_right
*/
struct CAssignment : CExp {
    AST_T type() override;
    CAssignment() = default;
    CAssignment(std::unique_ptr<CExp>&& exp_left, std::unique_ptr<CExp>&& exp_right);

    std::unique_ptr<CExp> exp_left;
    std::unique_ptr<CExp> exp_right;
};

/** TODO
cdef class CConditional(CExp):
    cdef public CExp condition
    cdef public CExp exp_middle
    cdef public CExp exp_right
*/
struct CConditional : CExp {
    AST_T type() override;
    CConditional() = default;
    CConditional(std::unique_ptr<CExp>&& condition, std::unique_ptr<CExp>&& exp_middle,
                 std::unique_ptr<CExp>&& exp_right);

    std::unique_ptr<CExp> condition;
    std::unique_ptr<CExp> exp_middle;
    std::unique_ptr<CExp> exp_right;
};

/** TODO
cdef class CAssignmentCompound(CExp):
    cdef public CBinaryOp binary_op
    cdef public CExp exp_left
    cdef public CExp exp_right
*/
struct CAssignmentCompound : CExp {
    AST_T type() override;
    CAssignmentCompound() = default;
    CAssignmentCompound(std::unique_ptr<CBinaryOp>&& binary_op, std::unique_ptr<CExp>&& exp_left,
                        std::unique_ptr<CExp>&& exp_right);

    std::unique_ptr<CBinaryOp> binary_op;
    std::unique_ptr<CExp> exp_left;
    std::unique_ptr<CExp> exp_right;
};

/** TODO
cdef class CFunctionCall(CExp):
    cdef public TIdentifier name
    cdef public list[CExp] args
 */
struct CFunctionCall : CExp {
    AST_T type() override;
    CFunctionCall() = default;
    CFunctionCall(TIdentifier&& name, std::vector<std::unique_ptr<CExp>>&& args);

    TIdentifier name;
    std::vector<std::unique_ptr<CExp>> args;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class CStatement(AST):
    pass
*/
struct CStatement : Ast {
    AST_T type() override;
};

/** TODO
cdef class CReturn(CStatement):
    cdef public CExp exp
*/
struct CReturn : CStatement {
    AST_T type() override;
    CReturn() = default;
    CReturn(std::unique_ptr<CExp>&& exp);

    std::unique_ptr<CExp> exp;
};

/** TODO
cdef class CExpression(CStatement):
    cdef public CExp exp
*/
struct CExpression : CStatement {
    AST_T type() override;
    CExpression() = default;
    CExpression(std::unique_ptr<CExp>&& exp);

    std::unique_ptr<CExp> exp;
};

/** TODO
cdef class CIf(CStatement):
    cdef public CExp condition
    cdef public CStatement then
    # Optional
    cdef public CStatement else_fi
*/
struct CIf : CStatement {
    AST_T type() override;
    CIf() = default;
    CIf(std::unique_ptr<CExp>&& condition, std::unique_ptr<CStatement>&& then,
        std::unique_ptr<CStatement>&& else_fi);

    std::unique_ptr<CExp> condition;
    std::unique_ptr<CStatement> then;
    // Optional
    std::unique_ptr<CStatement> else_fi;
};

/** TODO
cdef class CGoto(CStatement):
    cdef public TIdentifier target
*/
struct CGoto : CStatement {
    AST_T type() override;
    CGoto() = default;
    CGoto(TIdentifier&& target);

    TIdentifier target;
};

/** TODO
cdef class CLabel(CStatement):
    cdef public TIdentifier target
    cdef public CStatement jump_to
*/
struct CLabel : CStatement {
    AST_T type() override;
    CLabel() = default;
    CLabel(TIdentifier&& target, std::unique_ptr<CStatement>&& jump_to);

    TIdentifier target;
    std::unique_ptr<CStatement> jump_to;
};

/** TODO
cdef class CCompound(CStatement):
    cdef public CBlock block
*/
struct CBlock;
struct CCompound : CStatement {
    AST_T type() override;
    CCompound() = default;
    CCompound(std::unique_ptr<CBlock>&& block);

    std::unique_ptr<CBlock> block;
};

/** TODO
cdef class CWhile(CStatement):
    cdef public CExp condition
    cdef public CStatement body
    cdef public TIdentifier target
*/
struct CWhile : CStatement {
    AST_T type() override;
    CWhile() = default;
    CWhile(std::unique_ptr<CExp>&& condition, std::unique_ptr<CStatement>&& body);

    std::unique_ptr<CExp> condition;
    std::unique_ptr<CStatement> body;
    TIdentifier target;
};

/** TODO
cdef class CDoWhile(CStatement):
    cdef public CExp condition
    cdef public CStatement body
    cdef public TIdentifier target
*/
struct CDoWhile : CStatement {
    AST_T type() override;
    CDoWhile() = default;
    CDoWhile(std::unique_ptr<CExp>&& condition, std::unique_ptr<CStatement>&& body);

    std::unique_ptr<CExp> condition;
    std::unique_ptr<CStatement> body;
    TIdentifier target;
};

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
struct CForInit;
struct CFor : CStatement {
    AST_T type() override;
    CFor() = default;
    CFor(std::unique_ptr<CForInit>&& init, std::unique_ptr<CExp>&& condition, std::unique_ptr<CExp>&& post,
         std::unique_ptr<CStatement>&& body);

    std::unique_ptr<CForInit> init;
    std::unique_ptr<CExp> condition;
    std::unique_ptr<CExp> post;
    std::unique_ptr<CStatement> body;
    TIdentifier target;
};

/** TODO
cdef class CBreak(CStatement):
    cdef public TIdentifier target
*/
struct CBreak : CStatement {
    AST_T type() override;
    CBreak() = default;

    TIdentifier target;
};

/** TODO
cdef class CContinue(CStatement):
    cdef public TIdentifier target
*/
struct CContinue : CStatement {
    AST_T type() override;
    CContinue() = default;

    TIdentifier target;
};

/** TODO
cdef class CNull(CStatement):
    pass
*/
struct CNull : CStatement {
    AST_T type() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class CForInit(AST):
    pass
*/
struct CForInit : Ast {
    AST_T type() override;
};

/** TODO
cdef class CInitDecl(CForInit):
    cdef public CVariableDeclaration init
*/
struct CVariableDeclaration;
struct CInitDecl : CForInit {
    AST_T type() override;
    CInitDecl() = default;
    CInitDecl(std::unique_ptr<CVariableDeclaration>&& init);

    std::unique_ptr<CVariableDeclaration> init;
};

/** TODO
cdef class CInitExp(CForInit):
    # Optional
    cdef public CExp init
*/
struct CInitExp : CForInit {
    AST_T type() override;
    CInitExp() = default;
    CInitExp(std::unique_ptr<CExp>&& init);

    // Optional
    std::unique_ptr<CExp> init;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class CBlock(AST):
    pass
*/
struct CBlock : Ast {
    AST_T type() override;
};

/** TODO
cdef class CB(CBlock):
    cdef public list[CBlockItem] block_items
*/
struct CBlockItem;
struct CB : CBlock {
    AST_T type() override;
    CB() = default;
    CB(std::vector<std::unique_ptr<CBlockItem>>&& block_items);

    std::vector<std::unique_ptr<CBlockItem>> block_items;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class CBlockItem(AST):
    pass
*/
struct CBlockItem : Ast {
    AST_T type() override;
};

/** TODO
cdef class CS(CBlockItem):
    cdef public CStatement statement
*/
struct CS : CBlockItem {
    AST_T type() override;
    CS() = default;
    CS(std::unique_ptr<CStatement>&& statement);

    std::unique_ptr<CStatement> statement;
};

/** TODO
cdef class CD(CBlockItem):
    cdef public CDeclaration declaration
*/
struct CDeclaration;
struct CD : CBlockItem {
    AST_T type() override;
    CD() = default;
    CD(std::unique_ptr<CDeclaration>&& declaration);

    std::unique_ptr<CDeclaration> declaration;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class CStorageClass(AST):
    pass
*/
struct CStorageClass : Ast {
    AST_T type() override;
};

/** TODO
cdef class CStatic(CStorageClass):
    pass
*/
struct CStatic : CStorageClass {
    AST_T type() override;
};

/** TODO
cdef class CExtern(CStorageClass):
    pass
*/
struct CExtern : CStorageClass {
    AST_T type() override;
};

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
struct CFunctionDeclaration : Ast {
    AST_T type() override;
    CFunctionDeclaration() = default;
    CFunctionDeclaration(TIdentifier&& target, std::vector<std::unique_ptr<TIdentifier>>&& params,
                         std::unique_ptr<CBlock>&& body, std::unique_ptr<Type>&& fun_type,
                         std::unique_ptr<CStorageClass>&& storage_class);

    TIdentifier target;
    std::vector<std::unique_ptr<TIdentifier>> params;
    // Optional
    std::unique_ptr<CBlock> body;
    std::unique_ptr<Type> fun_type;
    // Optional
    std::unique_ptr<CStorageClass> storage_class;
};

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
struct CVariableDeclaration : Ast {
    AST_T type() override;
    CVariableDeclaration() = default;
    CVariableDeclaration(TIdentifier&& target, std::unique_ptr<CExp>&& init, std::unique_ptr<Type>&& var_type,
                         std::unique_ptr<CStorageClass>&& storage_class);

    TIdentifier target;
    std::unique_ptr<CExp> init;
    std::unique_ptr<Type> var_type;
    std::unique_ptr<CStorageClass> storage_class;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class CDeclaration(AST):
    pass
*/
struct CDeclaration : Ast {
    AST_T type() override;
};

/** TODO
cdef class CFunDecl(CDeclaration):
    cdef public CFunctionDeclaration function_decl
*/
struct CFunDecl : CDeclaration {
    AST_T type() override;
    CFunDecl() = default;
    CFunDecl(std::unique_ptr<CFunctionDeclaration>&& function_decl);

    std::unique_ptr<CFunctionDeclaration> function_decl;
};

/** TODO
cdef class CVarDecl(CDeclaration):
    cdef public CVariableDeclaration variable_decl
*/
struct CVarDecl : CDeclaration {
    AST_T type() override;
    CVarDecl() = default;
    CVarDecl(std::unique_ptr<CVariableDeclaration>&& variable_decl);

    std::unique_ptr<CVariableDeclaration> variable_decl;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class CProgram(AST):
    cdef public list[CDeclaration] declarations
*/
struct CProgram : Ast {
    AST_T type() override;
    CProgram() = default;
    CProgram(std::vector<std::unique_ptr<CDeclaration>>&& declarations);

    std::vector<std::unique_ptr<CDeclaration>> declarations;
};

#endif
