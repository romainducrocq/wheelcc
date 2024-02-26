#include "ast/front_ast.hpp"
#include "ast/ast.hpp"
#include "ast/front_symt.hpp"

#include <memory>
#include <vector>

AST_T CUnaryOp::type() { return AST_T::CUnaryOp_t; }
AST_T CComplement::type() { return AST_T::CComplement_t; }
AST_T CNegate::type() { return AST_T::CNegate_t; }
AST_T CNot::type() { return AST_T::CNot_t; }
AST_T CBinaryOp::type() { return AST_T::CBinaryOp_t; }
AST_T CAdd::type() { return AST_T::CAdd_t; }
AST_T CSubtract::type() { return AST_T::CSubtract_t; }
AST_T CMultiply::type() { return AST_T::CMultiply_t; }
AST_T CDivide::type() { return AST_T::CDivide_t; }
AST_T CRemainder::type() { return AST_T::CRemainder_t; }
AST_T CBitAnd::type() { return AST_T::CBitAnd_t; }
AST_T CBitOr::type() { return AST_T::CBitOr_t; }
AST_T CBitXor::type() { return AST_T::CBitXor_t; }
AST_T CBitShiftLeft::type() { return AST_T::CBitShiftLeft_t; }
AST_T CBitShiftRight::type() { return AST_T::CBitShiftRight_t; }
AST_T CAnd::type() { return AST_T::CAnd_t; }
AST_T COr::type() { return AST_T::COr_t; }
AST_T CEqual::type() { return AST_T::CEqual_t; }
AST_T CNotEqual::type() { return AST_T::CNotEqual_t; }
AST_T CLessThan::type() { return AST_T::CLessThan_t; }
AST_T CLessOrEqual::type() { return AST_T::CLessOrEqual_t; }
AST_T CGreaterThan::type() { return AST_T::CGreaterThan_t; }
AST_T CGreaterOrEqual::type() { return AST_T::CGreaterOrEqual_t; }
AST_T CAbstractDeclarator::type() { return AST_T::CAbstractDeclarator_t; }
AST_T CAbstractPointer::type() { return AST_T::CAbstractPointer_t; }
AST_T CAbstractArray::type() { return AST_T::CAbstractArray_t; }
AST_T CAbstractBase::type() { return AST_T::CAbstractBase_t; }
AST_T CParam::type() { return AST_T::CParam_t; }
AST_T CDeclarator::type() { return AST_T::CDeclarator_t; }
AST_T CIdent::type() { return AST_T::CIdent_t; }
AST_T CPointerDeclarator::type() { return AST_T::CPointerDeclarator_t; }
AST_T CArrayDeclarator::type() { return AST_T::CArrayDeclarator_t; }
AST_T CFunDeclarator::type() { return AST_T::CFunDeclarator_t; }
AST_T CExp::type() { return AST_T::CExp_t; }
AST_T CConstant::type() { return AST_T::CConstant_t; }
AST_T CVar::type() { return AST_T::CVar_t; }
AST_T CCast::type() { return AST_T::CCast_t; }
AST_T CUnary::type() { return AST_T::CUnary_t; }
AST_T CBinary::type() { return AST_T::CBinary_t; }
AST_T CAssignment::type() { return AST_T::CAssignment_t; }
AST_T CConditional::type() { return AST_T::CConditional_t; }
AST_T CFunctionCall::type() { return AST_T::CFunctionCall_t; }
AST_T CDereference::type() { return AST_T::CDereference_t; }
AST_T CAddrOf::type() { return AST_T::CAddrOf_t; }
AST_T CSubscript::type() { return AST_T::CSubscript_t; }
AST_T CStatement::type() { return AST_T::CStatement_t; }
AST_T CReturn::type() { return AST_T::CReturn_t; }
AST_T CExpression::type() { return AST_T::CExpression_t; }
AST_T CIf::type() { return AST_T::CIf_t; }
AST_T CGoto::type() { return AST_T::CGoto_t; }
AST_T CLabel::type() { return AST_T::CLabel_t; }
AST_T CCompound::type() { return AST_T::CCompound_t; }
AST_T CWhile::type() { return AST_T::CWhile_t; }
AST_T CDoWhile::type() { return AST_T::CDoWhile_t; }
AST_T CFor::type() { return AST_T::CFor_t; }
AST_T CBreak::type() { return AST_T::CBreak_t; }
AST_T CContinue::type() { return AST_T::CContinue_t; }
AST_T CNull::type() { return AST_T::CNull_t; }
AST_T CForInit::type() { return AST_T::CForInit_t; }
AST_T CInitDecl::type() { return AST_T::CInitDecl_t; }
AST_T CInitExp::type() { return AST_T::CInitExp_t; }
AST_T CBlock::type() { return AST_T::CBlock_t; }
AST_T CB::type() { return AST_T::CB_t; }
AST_T CBlockItem::type() { return AST_T::CBlockItem_t; }
AST_T CS::type() { return AST_T::CS_t; }
AST_T CD::type() { return AST_T::CD_t; }
AST_T CStorageClass::type() { return AST_T::CStorageClass_t; }
AST_T CStatic::type() { return AST_T::CStatic_t; }
AST_T CExtern::type() { return AST_T::CExtern_t; }
AST_T CInitializer::type() { return AST_T::CInitializer_t; }
AST_T CSingleInit::type() { return AST_T::CSingleInit_t; }
AST_T CCompoundInit::type() { return AST_T::CCompoundInit_t; }
AST_T CFunctionDeclaration::type() { return AST_T::CFunctionDeclaration_t; }
AST_T CVariableDeclaration::type() { return AST_T::CVariableDeclaration_t; }
AST_T CDeclaration::type() { return AST_T::CDeclaration_t; }
AST_T CFunDecl::type() { return AST_T::CFunDecl_t; }
AST_T CVarDecl::type() { return AST_T::CVarDecl_t; }
AST_T CProgram::type() { return AST_T::CProgram_t; }

CAbstractPointer::CAbstractPointer(std::unique_ptr<CAbstractDeclarator> abstract_declarator)
    : abstract_declarator(std::move(abstract_declarator)) {}

CAbstractArray::CAbstractArray(std::shared_ptr<CConst> size, std::unique_ptr<CAbstractDeclarator> abstract_declarator)
    : size(std::move(size)), abstract_declarator(std::move(abstract_declarator)) {}

CParam::CParam(std::unique_ptr<CDeclarator> declarator, std::shared_ptr<Type> param_type)
    : declarator(std::move(declarator)), param_type(std::move(param_type)) {}

CIdent::CIdent(TIdentifier name)
    : name(std::move(name)) {}

CPointerDeclarator::CPointerDeclarator(std::unique_ptr<CDeclarator> declarator)
    : declarator(std::move(declarator)) {}

CArrayDeclarator::CArrayDeclarator(std::shared_ptr<CConst> size, std::unique_ptr<CDeclarator> declarator)
    : size(std::move(size)), declarator(std::move(declarator)) {}

CFunDeclarator::CFunDeclarator(std::vector<std::unique_ptr<CParam>> param_list,
                               std::unique_ptr<CDeclarator> declarator)
    : param_list(std::move(param_list)), declarator(std::move(declarator)) {}

CConstant::CConstant(std::shared_ptr<CConst> constant)
    : constant(std::move(constant)) {}

CVar::CVar(TIdentifier name)
    : name(std::move(name)) {}

CCast::CCast(std::unique_ptr<CExp> exp, std::shared_ptr<Type> target_type)
    : exp(std::move(exp)), target_type(std::move(target_type)) {}

CUnary::CUnary(std::unique_ptr<CUnaryOp> unary_op, std::unique_ptr<CExp> exp)
    : unary_op(std::move(unary_op)), exp(std::move(exp)) {}

CBinary::CBinary(std::unique_ptr<CBinaryOp> binary_op, std::unique_ptr<CExp> exp_left,
                 std::unique_ptr<CExp> exp_right)
    : binary_op(std::move(binary_op)), exp_left(std::move(exp_left)),
      exp_right(std::move(exp_right)) {}

CAssignment::CAssignment(std::unique_ptr<CExp> exp_left, std::unique_ptr<CExp> exp_right)
    : exp_left(std::move(exp_left)), exp_right(std::move(exp_right)) {}

CConditional::CConditional(std::unique_ptr<CExp> condition, std::unique_ptr<CExp> exp_middle,
                           std::unique_ptr<CExp> exp_right)
    : condition(std::move(condition)), exp_middle(std::move(exp_middle)),
      exp_right(std::move(exp_right)) {}

CFunctionCall::CFunctionCall(TIdentifier name, std::vector<std::unique_ptr<CExp>> args)
    : name(std::move(name)), args(std::move(args)) {}

CDereference::CDereference(std::unique_ptr<CExp> exp)
    : exp(std::move(exp)) {}

CAddrOf::CAddrOf(std::unique_ptr<CExp> exp)
    : exp(std::move(exp)) {}

CSubscript::CSubscript(std::unique_ptr<CExp> exp, std::unique_ptr<CExp> exp_index)
    : exp(std::move(exp)), exp_index(std::move(exp_index)) {}

CReturn::CReturn(std::unique_ptr<CExp> exp)
    : exp(std::move(exp)) {}

CExpression::CExpression(std::unique_ptr<CExp> exp)
    : exp(std::move(exp)) {}

CIf::CIf(std::unique_ptr<CExp> condition, std::unique_ptr<CStatement> then, std::unique_ptr<CStatement> else_fi)
    : condition(std::move(condition)), then(std::move(then)), else_fi(std::move(else_fi)) {}

CGoto::CGoto(TIdentifier target)
    : target(std::move(target)) {}

CLabel::CLabel(TIdentifier target, std::unique_ptr<CStatement> jump_to)
    : target(std::move(target)), jump_to(std::move(jump_to)) {}

CCompound::CCompound(std::unique_ptr<CBlock> block)
    : block(std::move(block)) {}

CWhile::CWhile(std::unique_ptr<CExp> condition, std::unique_ptr<CStatement> body)
    : condition(std::move(condition)), body(std::move(body)) {}

CDoWhile::CDoWhile(std::unique_ptr<CExp> condition, std::unique_ptr<CStatement> body)
    : condition(std::move(condition)), body(std::move(body)) {}

CFor::CFor(std::unique_ptr<CForInit> init, std::unique_ptr<CExp> condition, std::unique_ptr<CExp> post,
           std::unique_ptr<CStatement> body)
    : init(std::move(init)), condition(std::move(condition)), post(std::move(post)),
      body(std::move(body)) {}

CInitDecl::CInitDecl(std::unique_ptr<CVariableDeclaration> init)
    : init(std::move(init)) {}

CInitExp::CInitExp(std::unique_ptr<CExp> init)
    : init(std::move(init)) {}

CB::CB(std::vector<std::unique_ptr<CBlockItem>> block_items)
    : block_items(std::move(block_items)) {}

CS::CS(std::unique_ptr<CStatement> statement)
    : statement(std::move(statement)) {}

CD::CD(std::unique_ptr<CDeclaration> declaration)
    : declaration(std::move(declaration)) {}

CSingleInit::CSingleInit(std::unique_ptr<CExp> exp)
    : exp(std::move(exp)) {}

CCompoundInit::CCompoundInit(std::vector<std::unique_ptr<CInitializer>> initializers)
    : initializers(std::move(initializers)) {}

CFunctionDeclaration::CFunctionDeclaration(TIdentifier name, std::vector<TIdentifier> params,
                                           std::unique_ptr<CBlock> body, std::shared_ptr<Type> fun_type,
                                           std::unique_ptr<CStorageClass> storage_class)
    : name(std::move(name)), params(std::move(params)), body(std::move(body)), fun_type(std::move(fun_type)),
      storage_class(std::move(storage_class)) {}

CVariableDeclaration::CVariableDeclaration(TIdentifier name, std::unique_ptr<CExp> init,
                                           std::shared_ptr<Type> var_type,
                                           std::unique_ptr<CStorageClass> storage_class)
    :  name(std::move(name)), init(std::move(init)), var_type(std::move(var_type)),
       storage_class(std::move(storage_class)) {}

CFunDecl::CFunDecl(std::unique_ptr<CFunctionDeclaration> function_decl)
    : function_decl(std::move(function_decl)) {}

CVarDecl::CVarDecl(std::unique_ptr<CVariableDeclaration> variable_decl)
    : variable_decl(std::move(variable_decl)) {}

CProgram::CProgram(std::vector<std::unique_ptr<CDeclaration>> declarations)
    : declarations(std::move(declarations)) {}
