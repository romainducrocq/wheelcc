#ifndef _AST_FRONT_AST_HPP
#define _AST_FRONT_AST_HPP

#include "ast/ast.hpp"
#include "ast/front_symt.hpp"

#include <memory>
#include <vector>

// https://mkhan45.github.io/2021/05/10/Modeling-ASTs-in-Different-Languages.html
// https://github.com/agentcooper/cpp-ast-example/blob/main/ast_inheritance.cpp

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// const = ConstInt(int)
//       | ConstLong(long)
//       | ConstDouble(double)
//       | ConstUInt(uint)
//       | ConstULong(ulong)
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

struct CConstDouble : CConst {
    AST_T type() override;
    CConstDouble() = default;
    CConstDouble(TDouble value);

    TDouble value;
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// unary_operator = Complement
//                | Negate
//                | Not
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
    CAbstractPointer(std::unique_ptr<CAbstractDeclarator> abstract_declarator);

    std::unique_ptr<CAbstractDeclarator> abstract_declarator;
};

struct CAbstractArray : CAbstractDeclarator {
    AST_T type() override;
    CAbstractArray() = default;
    CAbstractArray(TInt size, std::unique_ptr<CAbstractDeclarator> abstract_declarator);

    TInt size;
    std::unique_ptr<CAbstractDeclarator> abstract_declarator;
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
    CParam(std::unique_ptr<CDeclarator> declarator, std::shared_ptr<Type> param_type);

    std::unique_ptr<CDeclarator> declarator;
    std::shared_ptr<Type> param_type;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// declarator = Ident(identifier)
//            | PointerDeclarator(declarator)
//            | ArrayDeclarator(int, declarator)
//            | FunDeclarator(param_info* params, declarator)
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
    CPointerDeclarator(std::unique_ptr<CDeclarator> declarator);

    std::unique_ptr<CDeclarator> declarator;
};

struct CArrayDeclarator : CDeclarator {
    AST_T type() override;
    CArrayDeclarator() = default;
    CArrayDeclarator(TInt size, std::unique_ptr<CDeclarator> declarator);

    TInt size;
    std::unique_ptr<CDeclarator> declarator;
};

struct CFunDeclarator : CDeclarator {
    AST_T type() override;
    CFunDeclarator() = default;
    CFunDeclarator(std::vector<std::unique_ptr<CParam>> param_list, std::unique_ptr<CDeclarator> declarator);

    std::vector<std::unique_ptr<CParam>> param_list;
    std::unique_ptr<CDeclarator> declarator;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// exp = Constant(const, type)
//     | Var(identifier, type)
//     | Cast(type, exp, type)
//     | Unary(unary_operator, exp, type)
//     | Binary(binary_operator, exp, exp, type)
//     | Assignment(exp, exp, type)
//     | Conditional(exp, exp, exp, type)
//     | FunctionCall(identifier, exp*, type)
//     | Dereference(exp, type)
//     | AddrOf(exp, type)
//     | Subscript(exp, exp)
struct CExp : Ast {
    AST_T type() override;
    CExp() = default;

    std::shared_ptr<Type> exp_type;
};

struct CConstant : CExp {
    AST_T type() override;
    CConstant() = default;
    CConstant(std::shared_ptr<CConst> constant);

    std::shared_ptr<CConst> constant;
    /*
    std::shared_ptr<Type> exp_type;
    */
};

struct CVar : CExp {
    AST_T type() override;
    CVar() = default;
    CVar(TIdentifier name);

    TIdentifier name;
    /*
    std::shared_ptr<Type> exp_type;
    */
};

struct CCast : CExp {
    AST_T type() override;
    CCast() = default;
    CCast(std::unique_ptr<CExp> exp, std::shared_ptr<Type> target_type);

    std::unique_ptr<CExp> exp;
    std::shared_ptr<Type> target_type;
    /*
    std::shared_ptr<Type> exp_type;
    */
};

struct CUnary : CExp {
    AST_T type() override;
    CUnary() = default;
    CUnary(std::unique_ptr<CUnaryOp> unary_op, std::unique_ptr<CExp> exp);

    std::unique_ptr<CUnaryOp> unary_op;
    std::unique_ptr<CExp> exp;
    /*
    std::shared_ptr<Type> exp_type;
    */
};

struct CBinary : CExp {
    AST_T type() override;
    CBinary() = default;
    CBinary(std::unique_ptr<CBinaryOp> binary_op, std::unique_ptr<CExp> exp_left,
            std::unique_ptr<CExp> exp_right);

    std::unique_ptr<CBinaryOp> binary_op;
    std::unique_ptr<CExp> exp_left;
    std::unique_ptr<CExp> exp_right;
    /*
    std::shared_ptr<Type> exp_type;
    */
};

struct CAssignment : CExp {
    AST_T type() override;
    CAssignment() = default;
    CAssignment(std::unique_ptr<CExp> exp_left, std::unique_ptr<CExp> exp_right);

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
    CConditional(std::unique_ptr<CExp> condition, std::unique_ptr<CExp> exp_middle,
                 std::unique_ptr<CExp> exp_right);

    std::unique_ptr<CExp> condition;
    std::unique_ptr<CExp> exp_middle;
    std::unique_ptr<CExp> exp_right;
    /*
    std::shared_ptr<Type> exp_type;
    */
};

struct CFunctionCall : CExp {
    AST_T type() override;
    CFunctionCall() = default;
    CFunctionCall(TIdentifier name, std::vector<std::unique_ptr<CExp>> args);

    TIdentifier name;
    std::vector<std::unique_ptr<CExp>> args;
    /*
    std::shared_ptr<Type> exp_type;
    */
};

struct CDereference : CExp {
    AST_T type() override;
    CDereference() = default;
    CDereference(std::unique_ptr<CExp> exp);

    std::unique_ptr<CExp> exp;
    /*
    std::shared_ptr<Type> exp_type;
    */
};

struct CAddrOf : CExp {
    AST_T type() override;
    CAddrOf() = default;
    CAddrOf(std::unique_ptr<CExp> exp);

    std::unique_ptr<CExp> exp;
    /*
    std::shared_ptr<Type> exp_type;
    */
};

struct CSubscript : CExp {
    AST_T type() override;
    CSubscript() = default;
    CSubscript(std::unique_ptr<CExp> exp, std::unique_ptr<CExp> exp_index);

    std::unique_ptr<CExp> exp;
    std::unique_ptr<CExp> exp_index;
    /*
    std::shared_ptr<Type> exp_type;
    */
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// statement = Return(exp)
//           | Expression(exp)
//           | If(exp, statement, statement?)
//           | Goto(identifier)
//           | Label(identifier, target)
//           | Compound(block)
//           | While(exp, statement, identifier)
//           | DoWhile(statement, exp, identifier)
//           | For(for_init, exp?, exp?, statement, identifier)
//           | Break(identifier)
//           | Continue(identifier)
//           | Null
struct CStatement : Ast {
    AST_T type() override;
};

struct CReturn : CStatement {
    AST_T type() override;
    CReturn() = default;
    CReturn(std::unique_ptr<CExp> exp);

    std::unique_ptr<CExp> exp;
};

struct CExpression : CStatement {
    AST_T type() override;
    CExpression() = default;
    CExpression(std::unique_ptr<CExp> exp);

    std::unique_ptr<CExp> exp;
};

struct CIf : CStatement {
    AST_T type() override;
    CIf() = default;
    CIf(std::unique_ptr<CExp> condition, std::unique_ptr<CStatement> then,
        std::unique_ptr<CStatement> else_fi);

    std::unique_ptr<CExp> condition;
    std::unique_ptr<CStatement> then;
    // Optional
    std::unique_ptr<CStatement> else_fi;
};

struct CGoto : CStatement {
    AST_T type() override;
    CGoto() = default;
    CGoto(TIdentifier target);

    TIdentifier target;
};

struct CLabel : CStatement {
    AST_T type() override;
    CLabel() = default;
    CLabel(TIdentifier target, std::unique_ptr<CStatement> jump_to);

    TIdentifier target;
    std::unique_ptr<CStatement> jump_to;
};

struct CBlock;
struct CCompound : CStatement {
    AST_T type() override;
    CCompound() = default;
    CCompound(std::unique_ptr<CBlock> block);

    std::unique_ptr<CBlock> block;
};

struct CWhile : CStatement {
    AST_T type() override;
    CWhile() = default;
    CWhile(std::unique_ptr<CExp> condition, std::unique_ptr<CStatement> body);

    TIdentifier target;
    std::unique_ptr<CExp> condition;
    std::unique_ptr<CStatement> body;
};

struct CDoWhile : CStatement {
    AST_T type() override;
    CDoWhile() = default;
    CDoWhile(std::unique_ptr<CExp> condition, std::unique_ptr<CStatement> body);

    TIdentifier target;
    std::unique_ptr<CExp> condition;
    std::unique_ptr<CStatement> body;
};

struct CForInit;
struct CFor : CStatement {
    AST_T type() override;
    CFor() = default;
    CFor(std::unique_ptr<CForInit> init, std::unique_ptr<CExp> condition, std::unique_ptr<CExp> post,
         std::unique_ptr<CStatement> body);

    TIdentifier target;
    std::unique_ptr<CForInit> init;
    std::unique_ptr<CExp> condition;
    std::unique_ptr<CExp> post;
    std::unique_ptr<CStatement> body;
};

struct CBreak : CStatement {
    AST_T type() override;
    CBreak() = default;

    TIdentifier target;
};

struct CContinue : CStatement {
    AST_T type() override;
    CContinue() = default;

    TIdentifier target;
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
    CInitDecl(std::unique_ptr<CVariableDeclaration> init);

    std::unique_ptr<CVariableDeclaration> init;
};

struct CInitExp : CForInit {
    AST_T type() override;
    CInitExp() = default;
    CInitExp(std::unique_ptr<CExp> init);

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
    CB() = default;
    CB(std::vector<std::unique_ptr<CBlockItem>> block_items);

    std::vector<std::unique_ptr<CBlockItem>> block_items;
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
    CS(std::unique_ptr<CStatement> statement);

    std::unique_ptr<CStatement> statement;
};

struct CDeclaration;
struct CD : CBlockItem {
    AST_T type() override;
    CD() = default;
    CD(std::unique_ptr<CDeclaration> declaration);

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
    CSingleInit(std::unique_ptr<CExp> exp);

    std::unique_ptr<CExp> exp;
    /*
    std::shared_ptr<Type> init_type;
    */
};

struct CompoundInit : CInitializer {
    AST_T type() override;
    CompoundInit() = default;
    CompoundInit(std::vector<std::unique_ptr<CInitializer>> initializers);

    std::vector<std::unique_ptr<CInitializer>> initializers;
    /*
    std::shared_ptr<Type> init_type;
    */
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// function_declaration = FunctionDeclaration(identifier name, identifier* params, block? body, type fun_type,
//                                            storage_class?)
struct CFunctionDeclaration : Ast {
    AST_T type() override;
    CFunctionDeclaration() = default;
    CFunctionDeclaration(TIdentifier name, std::vector<TIdentifier> params,
                         std::unique_ptr<CBlock> body, std::shared_ptr<Type> fun_type,
                         std::unique_ptr<CStorageClass> storage_class);

    TIdentifier name;
    std::vector<TIdentifier> params;
    // Optional
    std::unique_ptr<CBlock> body;
    std::shared_ptr<Type> fun_type;
    // Optional
    std::unique_ptr<CStorageClass> storage_class;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// variable_declaration = VariableDeclaration(identifier name, exp? init, type var_type, storage_class?)
struct CVariableDeclaration : Ast {
    AST_T type() override;
    CVariableDeclaration() = default;
    CVariableDeclaration(TIdentifier name, std::unique_ptr<CExp> init, std::shared_ptr<Type> var_type,
                         std::unique_ptr<CStorageClass> storage_class);

    TIdentifier name;
    std::unique_ptr<CExp> init;
    std::shared_ptr<Type> var_type;
    std::unique_ptr<CStorageClass> storage_class;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// declaration = FunDecl(function_declaration)
//             | VarDecl(variable_declaration)
struct CDeclaration : Ast {
    AST_T type() override;
};

struct CFunDecl : CDeclaration {
    AST_T type() override;
    CFunDecl() = default;
    CFunDecl(std::unique_ptr<CFunctionDeclaration> function_decl);

    std::unique_ptr<CFunctionDeclaration> function_decl;
};

struct CVarDecl : CDeclaration {
    AST_T type() override;
    CVarDecl() = default;
    CVarDecl(std::unique_ptr<CVariableDeclaration> variable_decl);

    std::unique_ptr<CVariableDeclaration> variable_decl;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// AST = Program(declaration*)
struct CProgram : Ast {
    AST_T type() override;
    CProgram() = default;
    CProgram(std::vector<std::unique_ptr<CDeclaration>> declarations);

    std::vector<std::unique_ptr<CDeclaration>> declarations;
};

/*
struct Dummy : Ast {
};
*/

#endif
