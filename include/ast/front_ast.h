#ifndef _AST_FRONT_AST_H
#define _AST_FRONT_AST_H

#include <memory>

#include "util/c_std.h"

#include "ast/ast.h"
#include "ast/front_symt.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Frontend abstract syntax tree

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// unary_operator = Complement
//                | Negate
//                | Not
//                | Prefix
//                | Postfix
struct CUnaryOp : Ast {
    AST_T type() override;
};

struct CComplement : CUnaryOp {
    AST_T type() override;
};

struct CNegate : CUnaryOp {
    AST_T type() override;
};

struct CNot : CUnaryOp {
    AST_T type() override;
};

struct CPrefix : CUnaryOp {
    AST_T type() override;
};

struct CPostfix : CUnaryOp {
    AST_T type() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// binary_operator = Add
//                 | Subtract
//                 | Multiply
//                 | Divide
//                 | Remainder
//                 | BitAnd
//                 | BitOr
//                 | BitXor
//                 | BitShiftLeft
//                 | BitShiftRight
//                 | BitShrArithmetic
//                 | And
//                 | Or
//                 | Equal
//                 | NotEqual
//                 | LessThan
//                 | LessOrEqual
//                 | GreaterThan
//                 | GreaterOrEqual
struct CBinaryOp : Ast {
    AST_T type() override;
};

struct CAdd : CBinaryOp {
    AST_T type() override;
};

struct CSubtract : CBinaryOp {
    AST_T type() override;
};

struct CMultiply : CBinaryOp {
    AST_T type() override;
};

struct CDivide : CBinaryOp {
    AST_T type() override;
};

struct CRemainder : CBinaryOp {
    AST_T type() override;
};

struct CBitAnd : CBinaryOp {
    AST_T type() override;
};

struct CBitOr : CBinaryOp {
    AST_T type() override;
};

struct CBitXor : CBinaryOp {
    AST_T type() override;
};

struct CBitShiftLeft : CBinaryOp {
    AST_T type() override;
};

struct CBitShiftRight : CBinaryOp {
    AST_T type() override;
};

struct CBitShrArithmetic : CBinaryOp {
    AST_T type() override;
};

struct CAnd : CBinaryOp {
    AST_T type() override;
};

struct COr : CBinaryOp {
    AST_T type() override;
};

struct CEqual : CBinaryOp {
    AST_T type() override;
};

struct CNotEqual : CBinaryOp {
    AST_T type() override;
};

struct CLessThan : CBinaryOp {
    AST_T type() override;
};

struct CLessOrEqual : CBinaryOp {
    AST_T type() override;
};

struct CGreaterThan : CBinaryOp {
    AST_T type() override;
};

struct CGreaterOrEqual : CBinaryOp {
    AST_T type() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// abstract_declarator = AbstractPointer(abstract_declarator)
//                     | AbstractArray(int, abstract_declarator)
//                     | AbstractBase
struct CAbstractDeclarator : Ast {
    AST_T type() override;
};

struct CAbstractPointer : CAbstractDeclarator {
    AST_T type() override;
    CAbstractPointer() = default;
    CAbstractPointer(std::unique_ptr<CAbstractDeclarator>&& abstract_decltor);

    std::unique_ptr<CAbstractDeclarator> abstract_decltor;
};

struct CAbstractArray : CAbstractDeclarator {
    AST_T type() override;
    CAbstractArray() = default;
    CAbstractArray(TLong size, std::unique_ptr<CAbstractDeclarator>&& abstract_decltor);

    TLong size;
    std::unique_ptr<CAbstractDeclarator> abstract_decltor;
};

struct CAbstractBase : CAbstractDeclarator {
    AST_T type() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// param_info = Param(type, declarator)
struct CDeclarator;
struct CParam : Ast {
    AST_T type() override;
    CParam() = default;
    CParam(std::unique_ptr<CDeclarator>&& decltor, std::shared_ptr<Type>&& param_type);

    std::unique_ptr<CDeclarator> decltor;
    std::shared_ptr<Type> param_type;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// declarator = Ident(identifier)
//            | PointerDeclarator(declarator)
//            | ArrayDeclarator(int, declarator)
//            | FunDeclarator(param_info*, declarator)
struct CDeclarator : Ast {
    AST_T type() override;
};

struct CIdent : CDeclarator {
    AST_T type() override;
    CIdent() = default;
    CIdent(TIdentifier name);

    TIdentifier name;
};

struct CPointerDeclarator : CDeclarator {
    AST_T type() override;
    CPointerDeclarator() = default;
    CPointerDeclarator(std::unique_ptr<CDeclarator>&& decltor);

    std::unique_ptr<CDeclarator> decltor;
};

struct CArrayDeclarator : CDeclarator {
    AST_T type() override;
    CArrayDeclarator() = default;
    CArrayDeclarator(TLong size, std::unique_ptr<CDeclarator>&& decltor);

    TLong size;
    std::unique_ptr<CDeclarator> decltor;
};

struct CFunDeclarator : CDeclarator {
    AST_T type() override;
    CFunDeclarator();
    CFunDeclarator(vector_t(std::unique_ptr<CParam>) * param_list, std::unique_ptr<CDeclarator>&& decltor);
    ~CFunDeclarator();

    vector_t(std::unique_ptr<CParam>) param_list;
    std::unique_ptr<CDeclarator> decltor;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// exp = Constant(const, type)
//     | String(string, type)
//     | Var(identifier, type)
//     | Cast(type, exp, type)
//     | Unary(unary_operator, exp, type)
//     | Binary(binary_operator, exp, exp, type)
//     | Assignment(unary_operator, exp, exp, type)
//     | Conditional(exp, exp, exp, type)
//     | FunctionCall(identifier, exp*, type)
//     | Dereference(exp, type)
//     | AddrOf(exp, type)
//     | Subscript(exp, exp, type)
//     | SizeOf(exp, type)
//     | SizeOfT(type, type)
//     | Dot(exp, identifier, type)
//     | Arrow(exp, identifier, type)
struct CExp : Ast {
    AST_T type() override;
    CExp() = default;
    CExp(size_t line);

    std::shared_ptr<Type> exp_type;
    size_t line;
};

struct CConstant : CExp {
    AST_T type() override;
    CConstant() = default;
    CConstant(std::shared_ptr<CConst>&& constant, size_t line);

    std::shared_ptr<CConst> constant;
    /*
    std::shared_ptr<Type> exp_type;
    */
};

struct CString : CExp {
    AST_T type() override;
    CString() = default;
    CString(std::shared_ptr<CStringLiteral>&& literal, size_t line);

    std::shared_ptr<CStringLiteral> literal;
    /*
    std::shared_ptr<Type> exp_type;
    */
};

struct CVar : CExp {
    AST_T type() override;
    CVar() = default;
    CVar(TIdentifier name, size_t line);

    TIdentifier name;
    /*
    std::shared_ptr<Type> exp_type;
    */
};

struct CCast : CExp {
    AST_T type() override;
    CCast() = default;
    CCast(std::unique_ptr<CExp>&& exp, std::shared_ptr<Type>&& target_type, size_t line);

    std::unique_ptr<CExp> exp;
    std::shared_ptr<Type> target_type;
    /*
    std::shared_ptr<Type> exp_type;
    */
};

struct CUnary : CExp {
    AST_T type() override;
    CUnary() = default;
    CUnary(std::unique_ptr<CUnaryOp>&& unop, std::unique_ptr<CExp>&& exp, size_t line);

    std::unique_ptr<CUnaryOp> unop;
    std::unique_ptr<CExp> exp;
    /*
    std::shared_ptr<Type> exp_type;
    */
};

struct CBinary : CExp {
    AST_T type() override;
    CBinary() = default;
    CBinary(std::unique_ptr<CBinaryOp>&& binop, std::unique_ptr<CExp>&& exp_left, std::unique_ptr<CExp>&& exp_right,
        size_t line);

    std::unique_ptr<CBinaryOp> binop;
    std::unique_ptr<CExp> exp_left;
    std::unique_ptr<CExp> exp_right;
    /*
    std::shared_ptr<Type> exp_type;
    */
};

struct CAssignment : CExp {
    AST_T type() override;
    CAssignment() = default;
    CAssignment(std::unique_ptr<CUnaryOp>&& unop, std::unique_ptr<CExp>&& exp_left, std::unique_ptr<CExp>&& exp_right,
        size_t line);

    // Optional
    std::unique_ptr<CUnaryOp> unop;
    // Optional
    std::unique_ptr<CExp> exp_left;
    std::unique_ptr<CExp> exp_right;
    /*
    std::shared_ptr<Type> exp_type;
    */
};

struct CConditional : CExp {
    AST_T type() override;
    CConditional() = default;
    CConditional(std::unique_ptr<CExp>&& condition, std::unique_ptr<CExp>&& exp_middle,
        std::unique_ptr<CExp>&& exp_right, size_t line);

    std::unique_ptr<CExp> condition;
    std::unique_ptr<CExp> exp_middle;
    std::unique_ptr<CExp> exp_right;
    /*
    std::shared_ptr<Type> exp_type;
    */
};

struct CFunctionCall : CExp {
    AST_T type() override;
    CFunctionCall();
    CFunctionCall(TIdentifier name, vector_t(std::unique_ptr<CExp>) * args, size_t line);
    ~CFunctionCall();

    TIdentifier name;
    vector_t(std::unique_ptr<CExp>) args;
    /*
    std::shared_ptr<Type> exp_type;
    */
};

struct CDereference : CExp {
    AST_T type() override;
    CDereference() = default;
    CDereference(std::unique_ptr<CExp>&& exp, size_t line);

    std::unique_ptr<CExp> exp;
    /*
    std::shared_ptr<Type> exp_type;
    */
};

struct CAddrOf : CExp {
    AST_T type() override;
    CAddrOf() = default;
    CAddrOf(std::unique_ptr<CExp>&& exp, size_t line);

    std::unique_ptr<CExp> exp;
    /*
    std::shared_ptr<Type> exp_type;
    */
};

struct CSubscript : CExp {
    AST_T type() override;
    CSubscript() = default;
    CSubscript(std::unique_ptr<CExp>&& primary_exp, std::unique_ptr<CExp>&& subscript_exp, size_t line);

    std::unique_ptr<CExp> primary_exp;
    std::unique_ptr<CExp> subscript_exp;
    /*
    std::shared_ptr<Type> exp_type;
    */
};

struct CSizeOf : CExp {
    AST_T type() override;
    CSizeOf() = default;
    CSizeOf(std::unique_ptr<CExp>&& exp, size_t line);

    std::unique_ptr<CExp> exp;
    /*
    std::shared_ptr<Type> exp_type;
    */
};

struct CSizeOfT : CExp {
    AST_T type() override;
    CSizeOfT() = default;
    CSizeOfT(std::shared_ptr<Type>&& target_type, size_t line);

    std::shared_ptr<Type> target_type;
    /*
    std::shared_ptr<Type> exp_type;
    */
};

struct CDot : CExp {
    AST_T type() override;
    CDot() = default;
    CDot(TIdentifier member, std::unique_ptr<CExp>&& structure, size_t line);

    TIdentifier member;
    std::unique_ptr<CExp> structure;
    /*
    std::shared_ptr<Type> exp_type;
    */
};

struct CArrow : CExp {
    AST_T type() override;
    CArrow() = default;
    CArrow(TIdentifier member, std::unique_ptr<CExp>&& pointer, size_t line);

    TIdentifier member;
    std::unique_ptr<CExp> pointer;
    /*
    std::shared_ptr<Type> exp_type;
    */
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// statement = Return(exp?)
//           | Expression(exp)
//           | If(exp, statement, statement?)
//           | Goto(identifier)
//           | Label(identifier, target)
//           | Compound(block)
//           | While(exp, statement, identifier)
//           | DoWhile(statement, exp, identifier)
//           | For(for_init, exp?, exp?, statement, identifier)
//           | Switch(identifier, bool, exp, statement, exp*)
//           | Case(identifier, exp, statement)
//           | Default(identifier, statement)
//           | Break(identifier)
//           | Continue(identifier)
//           | Null
struct CStatement : Ast {
    AST_T type() override;
};

struct CReturn : CStatement {
    AST_T type() override;
    CReturn() = default;
    CReturn(std::unique_ptr<CExp>&& exp, size_t line);

    // Optional
    std::unique_ptr<CExp> exp;
    size_t line;
};

struct CExpression : CStatement {
    AST_T type() override;
    CExpression() = default;
    CExpression(std::unique_ptr<CExp>&& exp);

    std::unique_ptr<CExp> exp;
};

struct CIf : CStatement {
    AST_T type() override;
    CIf() = default;
    CIf(std::unique_ptr<CExp>&& condition, std::unique_ptr<CStatement>&& then, std::unique_ptr<CStatement>&& else_fi);

    std::unique_ptr<CExp> condition;
    std::unique_ptr<CStatement> then;
    // Optional
    std::unique_ptr<CStatement> else_fi;
};

struct CGoto : CStatement {
    AST_T type() override;
    CGoto() = default;
    CGoto(TIdentifier target, size_t line);

    TIdentifier target;
    size_t line;
};

struct CLabel : CStatement {
    AST_T type() override;
    CLabel() = default;
    CLabel(TIdentifier target, std::unique_ptr<CStatement>&& jump_to, size_t line);

    TIdentifier target;
    std::unique_ptr<CStatement> jump_to;
    size_t line;
};

struct CBlock;
struct CCompound : CStatement {
    AST_T type() override;
    CCompound() = default;
    CCompound(std::unique_ptr<CBlock>&& block);

    std::unique_ptr<CBlock> block;
};

struct CWhile : CStatement {
    AST_T type() override;
    CWhile() = default;
    CWhile(std::unique_ptr<CExp>&& condition, std::unique_ptr<CStatement>&& body);

    TIdentifier target;
    std::unique_ptr<CExp> condition;
    std::unique_ptr<CStatement> body;
};

struct CDoWhile : CStatement {
    AST_T type() override;
    CDoWhile() = default;
    CDoWhile(std::unique_ptr<CExp>&& condition, std::unique_ptr<CStatement>&& body);

    TIdentifier target;
    std::unique_ptr<CExp> condition;
    std::unique_ptr<CStatement> body;
};

struct CForInit;
struct CFor : CStatement {
    AST_T type() override;
    CFor() = default;
    CFor(std::unique_ptr<CForInit>&& init, std::unique_ptr<CExp>&& condition, std::unique_ptr<CExp>&& post,
        std::unique_ptr<CStatement>&& body);

    TIdentifier target;
    std::unique_ptr<CForInit> init;
    std::unique_ptr<CExp> condition;
    std::unique_ptr<CExp> post;
    std::unique_ptr<CStatement> body;
};

struct CSwitch : CStatement {
    AST_T type() override;
    CSwitch();
    CSwitch(std::unique_ptr<CExp>&& match, std::unique_ptr<CStatement>&& body);
    ~CSwitch();

    TIdentifier target;
    bool is_default;
    std::unique_ptr<CExp> match;
    std::unique_ptr<CStatement> body;
    vector_t(std::unique_ptr<CExp>) cases;
};

struct CCase : CStatement {
    AST_T type() override;
    CCase() = default;
    CCase(std::unique_ptr<CExp>&& value, std::unique_ptr<CStatement>&& jump_to);

    TIdentifier target;
    std::unique_ptr<CExp> value;
    std::unique_ptr<CStatement> jump_to;
};

struct CDefault : CStatement {
    AST_T type() override;
    CDefault() = default;
    CDefault(std::unique_ptr<CStatement>&& jump_to, size_t line);

    TIdentifier target;
    std::unique_ptr<CStatement> jump_to;
    size_t line;
};

struct CBreak : CStatement {
    AST_T type() override;
    CBreak() = default;
    CBreak(size_t line);

    TIdentifier target;
    size_t line;
};

struct CContinue : CStatement {
    AST_T type() override;
    CContinue() = default;
    CContinue(size_t line);

    TIdentifier target;
    size_t line;
};

struct CNull : CStatement {
    AST_T type() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// for_init = InitDecl(variable_declaration)
//          | InitExp(exp?)
struct CForInit : Ast {
    AST_T type() override;
};

struct CVariableDeclaration;
struct CInitDecl : CForInit {
    AST_T type() override;
    CInitDecl() = default;
    CInitDecl(std::unique_ptr<CVariableDeclaration>&& init);

    std::unique_ptr<CVariableDeclaration> init;
};

struct CInitExp : CForInit {
    AST_T type() override;
    CInitExp() = default;
    CInitExp(std::unique_ptr<CExp>&& init);

    // Optional
    std::unique_ptr<CExp> init;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// block = B(block_item*)
struct CBlock : Ast {
    AST_T type() override;
};

struct CBlockItem;
struct CB : CBlock {
    AST_T type() override;
    CB();
    CB(vector_t(std::unique_ptr<CBlockItem>) * block_items);
    ~CB();

    vector_t(std::unique_ptr<CBlockItem>) block_items;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// block_item = S(statement)
//            | D(declaration)
struct CBlockItem : Ast {
    AST_T type() override;
};

struct CS : CBlockItem {
    AST_T type() override;
    CS() = default;
    CS(std::unique_ptr<CStatement>&& statement);

    std::unique_ptr<CStatement> statement;
};

struct CDeclaration;
struct CD : CBlockItem {
    AST_T type() override;
    CD() = default;
    CD(std::unique_ptr<CDeclaration>&& declaration);

    std::unique_ptr<CDeclaration> declaration;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// storage_class = Static
//               | Extern
struct CStorageClass : Ast {
    AST_T type() override;
};

struct CStatic : CStorageClass {
    AST_T type() override;
};

struct CExtern : CStorageClass {
    AST_T type() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// initializer = SingleInit(exp)
//             | CompoundInit(initializer*)
struct CInitializer : Ast {
    AST_T type() override;
    CInitializer() = default;

    std::shared_ptr<Type> init_type;
};

struct CSingleInit : CInitializer {
    AST_T type() override;
    CSingleInit() = default;
    CSingleInit(std::unique_ptr<CExp>&& exp);

    std::unique_ptr<CExp> exp;
    /*
    std::shared_ptr<Type> init_type;
    */
};

struct CCompoundInit : CInitializer {
    AST_T type() override;
    CCompoundInit();
    CCompoundInit(vector_t(std::unique_ptr<CInitializer>) * initializers);
    ~CCompoundInit();

    vector_t(std::unique_ptr<CInitializer>) initializers;
    /*
    std::shared_ptr<Type> init_type;
    */
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// member_declaration = CMemberDeclaration(identifier, type)
struct CMemberDeclaration : Ast {
    AST_T type() override;
    CMemberDeclaration() = default;
    CMemberDeclaration(TIdentifier member_name, std::shared_ptr<Type>&& member_type, size_t line);

    TIdentifier member_name;
    std::shared_ptr<Type> member_type;
    size_t line;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// struct_declaration = StructDeclaration(identifier, bool, member_declaration*)
struct CStructDeclaration : Ast {
    AST_T type() override;
    CStructDeclaration();
    CStructDeclaration(
        TIdentifier tag, bool is_union, vector_t(std::unique_ptr<CMemberDeclaration>) * members, size_t line);
    ~CStructDeclaration();

    TIdentifier tag;
    bool is_union;
    vector_t(std::unique_ptr<CMemberDeclaration>) members;
    size_t line;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// function_declaration = FunctionDeclaration(identifier, identifier*, block?, type, storage_class?)
struct CFunctionDeclaration : Ast {
    AST_T type() override;
    CFunctionDeclaration();
    CFunctionDeclaration(TIdentifier name, vector_t(TIdentifier) * params, std::unique_ptr<CBlock>&& body,
        std::shared_ptr<Type>&& fun_type, std::unique_ptr<CStorageClass>&& storage_class, size_t line);
    ~CFunctionDeclaration();

    TIdentifier name;
    vector_t(TIdentifier) params;
    // Optional
    std::unique_ptr<CBlock> body;
    std::shared_ptr<Type> fun_type;
    // Optional
    std::unique_ptr<CStorageClass> storage_class;
    size_t line;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// variable_declaration = VariableDeclaration(identifier, initializer?, type, storage_class?)
struct CVariableDeclaration : Ast {
    AST_T type() override;
    CVariableDeclaration() = default;
    CVariableDeclaration(TIdentifier name, std::unique_ptr<CInitializer>&& init, std::shared_ptr<Type>&& var_type,
        std::unique_ptr<CStorageClass>&& storage_class, size_t line);

    TIdentifier name;
    // Optional
    std::unique_ptr<CInitializer> init;
    std::shared_ptr<Type> var_type;
    std::unique_ptr<CStorageClass> storage_class;
    size_t line;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// declaration = FunDecl(function_declaration)
//             | VarDecl(variable_declaration)
//             | StructDecl(struct_declaration)
struct CDeclaration : Ast {
    AST_T type() override;
};

struct CFunDecl : CDeclaration {
    AST_T type() override;
    CFunDecl() = default;
    CFunDecl(std::unique_ptr<CFunctionDeclaration>&& fun_decl);

    std::unique_ptr<CFunctionDeclaration> fun_decl;
};

struct CVarDecl : CDeclaration {
    AST_T type() override;
    CVarDecl() = default;
    CVarDecl(std::unique_ptr<CVariableDeclaration>&& var_decl);

    std::unique_ptr<CVariableDeclaration> var_decl;
};

struct CStructDecl : CDeclaration {
    AST_T type() override;
    CStructDecl() = default;
    CStructDecl(std::unique_ptr<CStructDeclaration>&& struct_decl);

    std::unique_ptr<CStructDeclaration> struct_decl;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// AST = Program(declaration*)
struct CProgram : Ast {
    AST_T type() override;
    CProgram();
    CProgram(vector_t(std::unique_ptr<CDeclaration>) * declarations);
    ~CProgram();

    vector_t(std::unique_ptr<CDeclaration>) declarations;
};

/*
struct Dummy : Ast {
};
*/

#endif
