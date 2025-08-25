#include <memory>

#include "util/c_std.h"

#include "ast/ast.h"
#include "ast/front_ast.h"
#include "ast/front_symt.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Frontend abstract syntax tree

AST_T CUnaryOp::type() { return AST_CUnaryOp_t; }
AST_T CComplement::type() { return AST_CComplement_t; }
AST_T CNegate::type() { return AST_CNegate_t; }
AST_T CNot::type() { return AST_CNot_t; }
AST_T CPrefix::type() { return AST_CPrefix_t; }
AST_T CPostfix::type() { return AST_CPostfix_t; }
AST_T CBinaryOp::type() { return AST_CBinaryOp_t; }
AST_T CAdd::type() { return AST_CAdd_t; }
AST_T CSubtract::type() { return AST_CSubtract_t; }
AST_T CMultiply::type() { return AST_CMultiply_t; }
AST_T CDivide::type() { return AST_CDivide_t; }
AST_T CRemainder::type() { return AST_CRemainder_t; }
AST_T CBitAnd::type() { return AST_CBitAnd_t; }
AST_T CBitOr::type() { return AST_CBitOr_t; }
AST_T CBitXor::type() { return AST_CBitXor_t; }
AST_T CBitShiftLeft::type() { return AST_CBitShiftLeft_t; }
AST_T CBitShiftRight::type() { return AST_CBitShiftRight_t; }
AST_T CBitShrArithmetic::type() { return AST_CBitShrArithmetic_t; }
AST_T CAnd::type() { return AST_CAnd_t; }
AST_T COr::type() { return AST_COr_t; }
AST_T CEqual::type() { return AST_CEqual_t; }
AST_T CNotEqual::type() { return AST_CNotEqual_t; }
AST_T CLessThan::type() { return AST_CLessThan_t; }
AST_T CLessOrEqual::type() { return AST_CLessOrEqual_t; }
AST_T CGreaterThan::type() { return AST_CGreaterThan_t; }
AST_T CGreaterOrEqual::type() { return AST_CGreaterOrEqual_t; }
AST_T CAbstractDeclarator::type() { return AST_CAbstractDeclarator_t; }
AST_T CAbstractPointer::type() { return AST_CAbstractPointer_t; }
AST_T CAbstractArray::type() { return AST_CAbstractArray_t; }
AST_T CAbstractBase::type() { return AST_CAbstractBase_t; }
AST_T CParam::type() { return AST_CParam_t; }
AST_T CDeclarator::type() { return AST_CDeclarator_t; }
AST_T CIdent::type() { return AST_CIdent_t; }
AST_T CPointerDeclarator::type() { return AST_CPointerDeclarator_t; }
AST_T CArrayDeclarator::type() { return AST_CArrayDeclarator_t; }
AST_T CFunDeclarator::type() { return AST_CFunDeclarator_t; }
AST_T CExp::type() { return AST_CExp_t; }
AST_T CConstant::type() { return AST_CConstant_t; }
AST_T CString::type() { return AST_CString_t; }
AST_T CVar::type() { return AST_CVar_t; }
AST_T CCast::type() { return AST_CCast_t; }
AST_T CUnary::type() { return AST_CUnary_t; }
AST_T CBinary::type() { return AST_CBinary_t; }
AST_T CAssignment::type() { return AST_CAssignment_t; }
AST_T CConditional::type() { return AST_CConditional_t; }
AST_T CFunctionCall::type() { return AST_CFunctionCall_t; }
AST_T CDereference::type() { return AST_CDereference_t; }
AST_T CAddrOf::type() { return AST_CAddrOf_t; }
AST_T CSubscript::type() { return AST_CSubscript_t; }
AST_T CSizeOf::type() { return AST_CSizeOf_t; }
AST_T CSizeOfT::type() { return AST_CSizeOfT_t; }
AST_T CDot::type() { return AST_CDot_t; }
AST_T CArrow::type() { return AST_CArrow_t; }
AST_T CStatement::type() { return AST_CStatement_t; }
AST_T CReturn::type() { return AST_CReturn_t; }
AST_T CExpression::type() { return AST_CExpression_t; }
AST_T CIf::type() { return AST_CIf_t; }
AST_T CGoto::type() { return AST_CGoto_t; }
AST_T CLabel::type() { return AST_CLabel_t; }
AST_T CCompound::type() { return AST_CCompound_t; }
AST_T CWhile::type() { return AST_CWhile_t; }
AST_T CDoWhile::type() { return AST_CDoWhile_t; }
AST_T CFor::type() { return AST_CFor_t; }
AST_T CSwitch::type() { return AST_CSwitch_t; }
AST_T CCase::type() { return AST_CCase_t; }
AST_T CDefault::type() { return AST_CDefault_t; }
AST_T CBreak::type() { return AST_CBreak_t; }
AST_T CContinue::type() { return AST_CContinue_t; }
AST_T CNull::type() { return AST_CNull_t; }
AST_T CForInit::type() { return AST_CForInit_t; }
AST_T CInitDecl::type() { return AST_CInitDecl_t; }
AST_T CInitExp::type() { return AST_CInitExp_t; }
AST_T CBlock::type() { return AST_CBlock_t; }
AST_T CB::type() { return AST_CB_t; }
AST_T CBlockItem::type() { return AST_CBlockItem_t; }
AST_T CS::type() { return AST_CS_t; }
AST_T CD::type() { return AST_CD_t; }
AST_T CStorageClass::type() { return AST_CStorageClass_t; }
AST_T CStatic::type() { return AST_CStatic_t; }
AST_T CExtern::type() { return AST_CExtern_t; }
AST_T CInitializer::type() { return AST_CInitializer_t; }
AST_T CSingleInit::type() { return AST_CSingleInit_t; }
AST_T CCompoundInit::type() { return AST_CCompoundInit_t; }
AST_T CMemberDeclaration::type() { return AST_CMemberDeclaration_t; }
AST_T CStructDeclaration::type() { return AST_CStructDeclaration_t; }
AST_T CFunctionDeclaration::type() { return AST_CFunctionDeclaration_t; }
AST_T CVariableDeclaration::type() { return AST_CVariableDeclaration_t; }
AST_T CDeclaration::type() { return AST_CDeclaration_t; }
AST_T CFunDecl::type() { return AST_CFunDecl_t; }
AST_T CVarDecl::type() { return AST_CVarDecl_t; }
AST_T CStructDecl::type() { return AST_CStructDecl_t; }
AST_T CProgram::type() { return AST_CProgram_t; }

CAbstractPointer::CAbstractPointer(std::unique_ptr<CAbstractDeclarator>&& abstract_decltor) :
    abstract_decltor(std::move(abstract_decltor)) {}

CAbstractArray::CAbstractArray(TLong size, std::unique_ptr<CAbstractDeclarator>&& abstract_decltor) :
    size(size), abstract_decltor(std::move(abstract_decltor)) {}

CParam::CParam(std::unique_ptr<CDeclarator>&& decltor, std::shared_ptr<Type>&& param_type) :
    decltor(std::move(decltor)), param_type(std::move(param_type)) {}

CIdent::CIdent(TIdentifier name) : name(name) {}

CPointerDeclarator::CPointerDeclarator(std::unique_ptr<CDeclarator>&& decltor) : decltor(std::move(decltor)) {}

CArrayDeclarator::CArrayDeclarator(TLong size, std::unique_ptr<CDeclarator>&& decltor) :
    size(size), decltor(std::move(decltor)) {}

CFunDeclarator::CFunDeclarator() : param_list(vec_new()) {}
CFunDeclarator::CFunDeclarator(vector_t(std::unique_ptr<CParam>) * param_list, std::unique_ptr<CDeclarator>&& decltor) :
    param_list(vec_new()), decltor(std::move(decltor)) {
    vec_move(param_list, &this->param_list);
}
CFunDeclarator::~CFunDeclarator() {
    for (size_t i = 0; i < vec_size(this->param_list); ++i) {
        this->param_list[i].reset();
    }
    vec_delete(this->param_list);
}

CExp::CExp(size_t line) : line(line) {}

CConstant::CConstant(std::shared_ptr<CConst>&& constant, size_t line) : CExp(line), constant(std::move(constant)) {}

CString::CString(std::shared_ptr<CStringLiteral>&& literal, size_t line) : CExp(line), literal(std::move(literal)) {}

CVar::CVar(TIdentifier name, size_t line) : CExp(line), name(name) {}

CCast::CCast(std::unique_ptr<CExp>&& exp, std::shared_ptr<Type>&& target_type, size_t line) :
    CExp(line), exp(std::move(exp)), target_type(std::move(target_type)) {}

CUnary::CUnary(std::unique_ptr<CUnaryOp>&& unop, std::unique_ptr<CExp>&& exp, size_t line) :
    CExp(line), unop(std::move(unop)), exp(std::move(exp)) {}

CBinary::CBinary(std::unique_ptr<CBinaryOp>&& binop, std::unique_ptr<CExp>&& exp_left,
    std::unique_ptr<CExp>&& exp_right, size_t line) :
    CExp(line),
    binop(std::move(binop)), exp_left(std::move(exp_left)), exp_right(std::move(exp_right)) {}

CAssignment::CAssignment(std::unique_ptr<CUnaryOp>&& unop, std::unique_ptr<CExp>&& exp_left,
    std::unique_ptr<CExp>&& exp_right, size_t line) :
    CExp(line),
    unop(std::move(unop)), exp_left(std::move(exp_left)), exp_right(std::move(exp_right)) {}

CConditional::CConditional(std::unique_ptr<CExp>&& condition, std::unique_ptr<CExp>&& exp_middle,
    std::unique_ptr<CExp>&& exp_right, size_t line) :
    CExp(line),
    condition(std::move(condition)), exp_middle(std::move(exp_middle)), exp_right(std::move(exp_right)) {}

CFunctionCall::CFunctionCall() : args(vec_new()) {}
CFunctionCall::CFunctionCall(TIdentifier name, vector_t(std::unique_ptr<CExp>) * args, size_t line) :
    CExp(line), name(name), args(vec_new()) {
    vec_move(args, &this->args);
}
CFunctionCall::~CFunctionCall() {
    for (size_t i = 0; i < vec_size(this->args); ++i) {
        this->args[i].reset();
    }
    vec_delete(this->args);
}

CDereference::CDereference(std::unique_ptr<CExp>&& exp, size_t line) : CExp(line), exp(std::move(exp)) {}

CAddrOf::CAddrOf(std::unique_ptr<CExp>&& exp, size_t line) : CExp(line), exp(std::move(exp)) {}

CSubscript::CSubscript(std::unique_ptr<CExp>&& primary_exp, std::unique_ptr<CExp>&& subscript_exp, size_t line) :
    CExp(line), primary_exp(std::move(primary_exp)), subscript_exp(std::move(subscript_exp)) {}

CSizeOf::CSizeOf(std::unique_ptr<CExp>&& exp, size_t line) : CExp(line), exp(std::move(exp)) {}

CSizeOfT::CSizeOfT(std::shared_ptr<Type>&& target_type, size_t line) :
    CExp(line), target_type(std::move(target_type)) {}

CDot::CDot(TIdentifier member, std::unique_ptr<CExp>&& structure, size_t line) :
    CExp(line), member(member), structure(std::move(structure)) {}

CArrow::CArrow(TIdentifier member, std::unique_ptr<CExp>&& pointer, size_t line) :
    CExp(line), member(member), pointer(std::move(pointer)) {}

CReturn::CReturn(std::unique_ptr<CExp>&& exp, size_t line) : exp(std::move(exp)), line(line) {}

CExpression::CExpression(std::unique_ptr<CExp>&& exp) : exp(std::move(exp)) {}

CIf::CIf(std::unique_ptr<CExp>&& condition, std::unique_ptr<CStatement>&& then, std::unique_ptr<CStatement>&& else_fi) :
    condition(std::move(condition)), then(std::move(then)), else_fi(std::move(else_fi)) {}

CGoto::CGoto(TIdentifier target, size_t line) : target(target), line(line) {}

CLabel::CLabel(TIdentifier target, std::unique_ptr<CStatement>&& jump_to, size_t line) :
    target(target), jump_to(std::move(jump_to)), line(line) {}

CCompound::CCompound(std::unique_ptr<CBlock>&& block) : block(std::move(block)) {}

CWhile::CWhile(std::unique_ptr<CExp>&& condition, std::unique_ptr<CStatement>&& body) :
    condition(std::move(condition)), body(std::move(body)) {}

CDoWhile::CDoWhile(std::unique_ptr<CExp>&& condition, std::unique_ptr<CStatement>&& body) :
    condition(std::move(condition)), body(std::move(body)) {}

CFor::CFor(std::unique_ptr<CForInit>&& init, std::unique_ptr<CExp>&& condition, std::unique_ptr<CExp>&& post,
    std::unique_ptr<CStatement>&& body) :
    init(std::move(init)),
    condition(std::move(condition)), post(std::move(post)), body(std::move(body)) {}

CSwitch::CSwitch() : cases(vec_new()) {}
CSwitch::CSwitch(std::unique_ptr<CExp>&& match, std::unique_ptr<CStatement>&& body) :
    match(std::move(match)), body(std::move(body)), cases(vec_new()) {}
CSwitch::~CSwitch() {
    for (size_t i = 0; i < vec_size(this->cases); ++i) {
        this->cases[i].reset();
    }
    vec_delete(this->cases);
}

CCase::CCase(std::unique_ptr<CExp>&& value, std::unique_ptr<CStatement>&& jump_to) :
    value(std::move(value)), jump_to(std::move(jump_to)) {}

CDefault::CDefault(std::unique_ptr<CStatement>&& jump_to, size_t line) : jump_to(std::move(jump_to)), line(line) {}

CBreak::CBreak(size_t line) : line(line) {}

CContinue::CContinue(size_t line) : line(line) {}

CInitDecl::CInitDecl(std::unique_ptr<CVariableDeclaration>&& init) : init(std::move(init)) {}

CInitExp::CInitExp(std::unique_ptr<CExp>&& init) : init(std::move(init)) {}

CB::CB() : block_items(vec_new()) {}
CB::CB(vector_t(std::unique_ptr<CBlockItem>) * block_items) : block_items(vec_new()) {
    vec_move(block_items, &this->block_items);
}
CB::~CB() {
    for (size_t i = 0; i < vec_size(this->block_items); ++i) {
        this->block_items[i].reset();
    }
    vec_delete(this->block_items);
}

CS::CS(std::unique_ptr<CStatement>&& statement) : statement(std::move(statement)) {}

CD::CD(std::unique_ptr<CDeclaration>&& declaration) : declaration(std::move(declaration)) {}

CSingleInit::CSingleInit(std::unique_ptr<CExp>&& exp) : exp(std::move(exp)) {}

CCompoundInit::CCompoundInit() : initializers(vec_new()) {}
CCompoundInit::CCompoundInit(vector_t(std::unique_ptr<CInitializer>) * initializers) : initializers(vec_new()) {
    vec_move(initializers, &this->initializers);
}
CCompoundInit::~CCompoundInit() {
    for (size_t i = 0; i < vec_size(this->initializers); ++i) {
        this->initializers[i].reset();
    }
    vec_delete(this->initializers);
}

CMemberDeclaration::CMemberDeclaration(TIdentifier member_name, std::shared_ptr<Type>&& member_type, size_t line) :
    member_name(member_name), member_type(std::move(member_type)), line(line) {}

CStructDeclaration::CStructDeclaration() : members(vec_new()) {}
CStructDeclaration::CStructDeclaration(
    TIdentifier tag, bool is_union, vector_t(std::unique_ptr<CMemberDeclaration>) * members, size_t line) :
    tag(tag),
    is_union(is_union), members(vec_new()), line(line) {
    vec_move(members, &this->members);
}
CStructDeclaration::~CStructDeclaration() {
    for (size_t i = 0; i < vec_size(this->members); ++i) {
        this->members[i].reset();
    }
    vec_delete(this->members);
}

CFunctionDeclaration::CFunctionDeclaration() : params(vec_new()) {}
CFunctionDeclaration::CFunctionDeclaration(TIdentifier name, vector_t(TIdentifier) * params,
    std::unique_ptr<CBlock>&& body, std::shared_ptr<Type>&& fun_type, std::unique_ptr<CStorageClass>&& storage_class,
    size_t line) :
    name(name),
    params(vec_new()), body(std::move(body)), fun_type(std::move(fun_type)), storage_class(std::move(storage_class)),
    line(line) {
    vec_move(params, &this->params);
}
CFunctionDeclaration::~CFunctionDeclaration() { vec_delete(this->params); }

CVariableDeclaration::CVariableDeclaration(TIdentifier name, std::unique_ptr<CInitializer>&& init,
    std::shared_ptr<Type>&& var_type, std::unique_ptr<CStorageClass>&& storage_class, size_t line) :
    name(name),
    init(std::move(init)), var_type(std::move(var_type)), storage_class(std::move(storage_class)), line(line) {}

CFunDecl::CFunDecl(std::unique_ptr<CFunctionDeclaration>&& fun_decl) : fun_decl(std::move(fun_decl)) {}

CVarDecl::CVarDecl(std::unique_ptr<CVariableDeclaration>&& var_decl) : var_decl(std::move(var_decl)) {}

CStructDecl::CStructDecl(std::unique_ptr<CStructDeclaration>&& struct_decl) : struct_decl(std::move(struct_decl)) {}

CProgram::CProgram() : declarations(vec_new()) {}
CProgram::CProgram(vector_t(std::unique_ptr<CDeclaration>) * declarations) : declarations(vec_new()) {
    vec_move(declarations, &this->declarations);
}
CProgram::~CProgram() {
    for (size_t i = 0; i < vec_size(this->declarations); ++i) {
        this->declarations[i].reset();
    }
    vec_delete(this->declarations);
}
