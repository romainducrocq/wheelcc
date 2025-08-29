#ifndef _AST_FRONT_AST_H
#define _AST_FRONT_AST_H

#include "util/c_std.h"

#include "ast/ast.h"
#include "ast/front_symt.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Frontend abstract syntax tree

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct CUnaryOp CUnaryOp;
typedef struct CBinaryOp CBinaryOp;
typedef struct CAbstractDeclarator CAbstractDeclarator;
typedef struct CParam CParam;
typedef struct CDeclarator CDeclarator;
typedef struct CExp CExp;
typedef struct CStatement CStatement;
typedef struct CForInit CForInit;
typedef struct CBlock CBlock;
typedef struct CBlockItem CBlockItem;
typedef struct CStorageClass CStorageClass;
typedef struct CInitializer CInitializer;
typedef struct CMemberDeclaration CMemberDeclaration;
typedef struct CStructDeclaration CStructDeclaration;
typedef struct CFunctionDeclaration CFunctionDeclaration;
typedef struct CVariableDeclaration CVariableDeclaration;
typedef struct CDeclaration CDeclaration;
typedef struct CProgram CProgram;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// unary_operator = Complement
//                | Negate
//                | Not
//                | Prefix
//                | Postfix

typedef struct CComplement {
    int8_t _empty;
} CComplement;

typedef struct CNegate {
    int8_t _empty;
} CNegate;

typedef struct CNot {
    int8_t _empty;
} CNot;

typedef struct CPrefix {
    int8_t _empty;
} CPrefix;

typedef struct CPostfix {
    int8_t _empty;
} CPostfix;

typedef struct CUnaryOp {
    unique_ptr_impl(AST_T);

    union {
        CComplement _CComplement;
        CNegate _CNegate;
        CNot _CNot;
        CPrefix _CPrefix;
        CPostfix _CPostfix;
    } get;
} CUnaryOp;

unique_ptr_t(CUnaryOp) make_CUnaryOp(void);
unique_ptr_t(CUnaryOp) make_CComplement(void);
unique_ptr_t(CUnaryOp) make_CNegate(void);
unique_ptr_t(CUnaryOp) make_CNot(void);
unique_ptr_t(CUnaryOp) make_CPrefix(void);
unique_ptr_t(CUnaryOp) make_CPostfix(void);
void free_CUnaryOp(unique_ptr_t(CUnaryOp) * self);
void move_CUnaryOp(unique_ptr_t(CUnaryOp) * self, unique_ptr_t(CUnaryOp) * other);

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

typedef struct CAdd {
    int8_t _empty;
} CAdd;

typedef struct CSubtract {
    int8_t _empty;
} CSubtract;

typedef struct CMultiply {
    int8_t _empty;
} CMultiply;

typedef struct CDivide {
    int8_t _empty;
} CDivide;

typedef struct CRemainder {
    int8_t _empty;
} CRemainder;

typedef struct CBitAnd {
    int8_t _empty;
} CBitAnd;

typedef struct CBitOr {
    int8_t _empty;
} CBitOr;

typedef struct CBitXor {
    int8_t _empty;
} CBitXor;

typedef struct CBitShiftLeft {
    int8_t _empty;
} CBitShiftLeft;

typedef struct CBitShiftRight {
    int8_t _empty;
} CBitShiftRight;

typedef struct CBitShrArithmetic {
    int8_t _empty;
} CBitShrArithmetic;

typedef struct CAnd {
    int8_t _empty;
} CAnd;

typedef struct COr {
    int8_t _empty;
} COr;

typedef struct CEqual {
    int8_t _empty;
} CEqual;

typedef struct CNotEqual {
    int8_t _empty;
} CNotEqual;

typedef struct CLessThan {
    int8_t _empty;
} CLessThan;

typedef struct CLessOrEqual {
    int8_t _empty;
} CLessOrEqual;

typedef struct CGreaterThan {
    int8_t _empty;
} CGreaterThan;

typedef struct CGreaterOrEqual {
    int8_t _empty;
} CGreaterOrEqual;

typedef struct CBinaryOp {
    unique_ptr_impl(AST_T);

    union {
        CAdd _CAdd;
        CSubtract _CSubtract;
        CMultiply _CMultiply;
        CDivide _CDivide;
        CRemainder _CRemainder;
        CBitAnd _CBitAnd;
        CBitOr _CBitOr;
        CBitXor _CBitXor;
        CBitShiftLeft _CBitShiftLeft;
        CBitShiftRight _CBitShiftRight;
        CBitShrArithmetic _CBitShrArithmetic;
        CAnd _CAnd;
        COr _COr;
        CEqual _CEqual;
        CNotEqual _CNotEqual;
        CLessThan _CLessThan;
        CLessOrEqual _CLessOrEqual;
        CGreaterThan _CGreaterThan;
        CGreaterOrEqual _CGreaterOrEqual;
    } get;
} CBinaryOp;

unique_ptr_t(CBinaryOp) make_CBinaryOp(void);
unique_ptr_t(CBinaryOp) make_CAdd(void);
unique_ptr_t(CBinaryOp) make_CSubtract(void);
unique_ptr_t(CBinaryOp) make_CMultiply(void);
unique_ptr_t(CBinaryOp) make_CDivide(void);
unique_ptr_t(CBinaryOp) make_CRemainder(void);
unique_ptr_t(CBinaryOp) make_CBitAnd(void);
unique_ptr_t(CBinaryOp) make_CBitOr(void);
unique_ptr_t(CBinaryOp) make_CBitXor(void);
unique_ptr_t(CBinaryOp) make_CBitShiftLeft(void);
unique_ptr_t(CBinaryOp) make_CBitShiftRight(void);
unique_ptr_t(CBinaryOp) make_CBitShrArithmetic(void);
unique_ptr_t(CBinaryOp) make_CAnd(void);
unique_ptr_t(CBinaryOp) make_COr(void);
unique_ptr_t(CBinaryOp) make_CEqual(void);
unique_ptr_t(CBinaryOp) make_CNotEqual(void);
unique_ptr_t(CBinaryOp) make_CLessThan(void);
unique_ptr_t(CBinaryOp) make_CLessOrEqual(void);
unique_ptr_t(CBinaryOp) make_CGreaterThan(void);
unique_ptr_t(CBinaryOp) make_CGreaterOrEqual(void);
void free_CBinaryOp(unique_ptr_t(CBinaryOp) * self);
void move_CBinaryOp(unique_ptr_t(CBinaryOp) * self, unique_ptr_t(CBinaryOp) * other);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// abstract_declarator = AbstractPointer(abstract_declarator)
//                     | AbstractArray(int, abstract_declarator)
//                     | AbstractBase

typedef struct CAbstractPointer {
    unique_ptr_t(CAbstractDeclarator) abstract_decltor;
} CAbstractPointer;

typedef struct CAbstractArray {
    TLong size;
    unique_ptr_t(CAbstractDeclarator) abstract_decltor;
} CAbstractArray;

typedef struct CAbstractBase {
    int8_t _empty;
} CAbstractBase;

typedef struct CAbstractDeclarator {
    unique_ptr_impl(AST_T);

    union {
        CAbstractPointer _CAbstractPointer;
        CAbstractArray _CAbstractArray;
        CAbstractBase _CAbstractBase;
    } get;
} CAbstractDeclarator;

unique_ptr_t(CAbstractDeclarator) make_CAbstractDeclarator(void);
unique_ptr_t(CAbstractDeclarator) make_CAbstractPointer(unique_ptr_t(CAbstractDeclarator) * abstract_decltor);
unique_ptr_t(CAbstractDeclarator) make_CAbstractArray(TLong size, unique_ptr_t(CAbstractDeclarator) * abstract_decltor);
unique_ptr_t(CAbstractDeclarator) make_CAbstractBase(void);
void free_CAbstractDeclarator(unique_ptr_t(CAbstractDeclarator) * self);
void move_CAbstractDeclarator(unique_ptr_t(CAbstractDeclarator) * self, unique_ptr_t(CAbstractDeclarator) * other);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// param_info = Param(type, declarator)

typedef struct CParam {
    unique_ptr_impl(AST_T);
    unique_ptr_t(CDeclarator) decltor;
    shared_ptr_t(Type) param_type;

} CParam;

unique_ptr_t(CParam) make_CParam(unique_ptr_t(CDeclarator) * decltor, shared_ptr_t(Type) * param_type);
void free_CParam(unique_ptr_t(CParam) * self);
void move_CParam(unique_ptr_t(CParam) * self, unique_ptr_t(CParam) * other);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// declarator = Ident(identifier)
//            | PointerDeclarator(declarator)
//            | ArrayDeclarator(int, declarator)
//            | FunDeclarator(param_info*, declarator)

typedef struct CIdent {
    TIdentifier name;
} CIdent;

typedef struct CPointerDeclarator {
    unique_ptr_t(CDeclarator) decltor;
} CPointerDeclarator;

typedef struct CArrayDeclarator {
    TLong size;
    unique_ptr_t(CDeclarator) decltor;
} CArrayDeclarator;

typedef struct CFunDeclarator {
    vector_t(unique_ptr_t(CParam)) param_list;
    unique_ptr_t(CDeclarator) decltor;
} CFunDeclarator;

typedef struct CDeclarator {
    unique_ptr_impl(AST_T);

    union {
        CIdent _CIdent;
        CPointerDeclarator _CPointerDeclarator;
        CArrayDeclarator _CArrayDeclarator;
        CFunDeclarator _CFunDeclarator;
    } get;
} CDeclarator;

unique_ptr_t(CDeclarator) make_CDeclarator(void);
unique_ptr_t(CDeclarator) make_CIdent(TIdentifier name);
unique_ptr_t(CDeclarator) make_CPointerDeclarator(unique_ptr_t(CDeclarator) * decltor);
unique_ptr_t(CDeclarator) make_CArrayDeclarator(TLong size, unique_ptr_t(CDeclarator) * decltor);
unique_ptr_t(CDeclarator)
    make_CFunDeclarator(vector_t(unique_ptr_t(CParam)) * param_list, unique_ptr_t(CDeclarator) * decltor);
void free_CDeclarator(unique_ptr_t(CDeclarator) * self);
void move_CDeclarator(unique_ptr_t(CDeclarator) * self, unique_ptr_t(CDeclarator) * other);

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

typedef struct CConstant {
    shared_ptr_t(CConst) constant;
} CConstant;

typedef struct CString {
    shared_ptr_t(CStringLiteral) literal;
} CString;

typedef struct CVar {
    TIdentifier name;
} CVar;

typedef struct CCast {
    unique_ptr_t(CExp) exp;
    shared_ptr_t(Type) target_type;
} CCast;

typedef struct CUnary {
    unique_ptr_t(CUnaryOp) unop;
    unique_ptr_t(CExp) exp;
} CUnary;

typedef struct CBinary {
    unique_ptr_t(CBinaryOp) binop;
    unique_ptr_t(CExp) exp_left;
    unique_ptr_t(CExp) exp_right;
} CBinary;

typedef struct CAssignment {
    unique_ptr_t(CUnaryOp) unop;
    unique_ptr_t(CExp) exp_left;
    unique_ptr_t(CExp) exp_right;
} CAssignment;

typedef struct CConditional {
    unique_ptr_t(CExp) condition;
    unique_ptr_t(CExp) exp_middle;
    unique_ptr_t(CExp) exp_right;
} CConditional;

typedef struct CFunctionCall {
    TIdentifier name;
    vector_t(unique_ptr_t(CExp)) args;
} CFunctionCall;

typedef struct CDereference {
    unique_ptr_t(CExp) exp;
} CDereference;

typedef struct CAddrOf {
    unique_ptr_t(CExp) exp;
} CAddrOf;

typedef struct CSubscript {
    unique_ptr_t(CExp) primary_exp;
    unique_ptr_t(CExp) subscript_exp;
} CSubscript;

typedef struct CSizeOf {
    unique_ptr_t(CExp) exp;
} CSizeOf;

typedef struct CSizeOfT {
    shared_ptr_t(Type) target_type;
} CSizeOfT;

typedef struct CDot {
    TIdentifier member;
    unique_ptr_t(CExp) structure;
} CDot;

typedef struct CArrow {
    TIdentifier member;
    unique_ptr_t(CExp) pointer;
} CArrow;

typedef struct CExp {
    unique_ptr_impl(AST_T);
    shared_ptr_t(Type) exp_type;
    size_t line;

    union {
        CConstant _CConstant;
        CString _CString;
        CVar _CVar;
        CCast _CCast;
        CUnary _CUnary;
        CBinary _CBinary;
        CAssignment _CAssignment;
        CConditional _CConditional;
        CFunctionCall _CFunctionCall;
        CDereference _CDereference;
        CAddrOf _CAddrOf;
        CSubscript _CSubscript;
        CSizeOf _CSizeOf;
        CSizeOfT _CSizeOfT;
        CDot _CDot;
        CArrow _CArrow;
    } get;
} CExp;

unique_ptr_t(CExp) make_CExp(size_t line);
unique_ptr_t(CExp) make_CConstant(shared_ptr_t(CConst) * constant, size_t line);
unique_ptr_t(CExp) make_CString(shared_ptr_t(CStringLiteral) * literal, size_t line);
unique_ptr_t(CExp) make_CVar(TIdentifier name, size_t line);
unique_ptr_t(CExp) make_CCast(unique_ptr_t(CExp) * exp, shared_ptr_t(Type) * target_type, size_t line);
unique_ptr_t(CExp) make_CUnary(unique_ptr_t(CUnaryOp) * unop, unique_ptr_t(CExp) * exp, size_t line);
unique_ptr_t(CExp) make_CBinary(
    unique_ptr_t(CBinaryOp) * binop, unique_ptr_t(CExp) * exp_left, unique_ptr_t(CExp) * exp_right, size_t line);
unique_ptr_t(CExp) make_CAssignment(
    unique_ptr_t(CUnaryOp) * unop, unique_ptr_t(CExp) * exp_left, unique_ptr_t(CExp) * exp_right, size_t line);
unique_ptr_t(CExp) make_CConditional(
    unique_ptr_t(CExp) * condition, unique_ptr_t(CExp) * exp_middle, unique_ptr_t(CExp) * exp_right, size_t line);
unique_ptr_t(CExp) make_CFunctionCall(TIdentifier name, vector_t(unique_ptr_t(CExp)) * args, size_t line);
unique_ptr_t(CExp) make_CDereference(unique_ptr_t(CExp) * exp, size_t line);
unique_ptr_t(CExp) make_CAddrOf(unique_ptr_t(CExp) * exp, size_t line);
unique_ptr_t(CExp) make_CSubscript(unique_ptr_t(CExp) * primary_exp, unique_ptr_t(CExp) * subscript_exp, size_t line);
unique_ptr_t(CExp) make_CSizeOf(unique_ptr_t(CExp) * exp, size_t line);
unique_ptr_t(CExp) make_CSizeOfT(shared_ptr_t(Type) * target_type, size_t line);
unique_ptr_t(CExp) make_CDot(TIdentifier member, unique_ptr_t(CExp) * structure, size_t line);
unique_ptr_t(CExp) make_CArrow(TIdentifier member, unique_ptr_t(CExp) * pointer, size_t line);
void free_CExp(unique_ptr_t(CExp) * self);
void move_CExp(unique_ptr_t(CExp) * self, unique_ptr_t(CExp) * other);

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

typedef struct CReturn {
    unique_ptr_t(CExp) exp;
    size_t line;
} CReturn;

typedef struct CExpression {
    unique_ptr_t(CExp) exp;
} CExpression;

typedef struct CIf {
    unique_ptr_t(CExp) condition;
    unique_ptr_t(CStatement) then;
    unique_ptr_t(CStatement) else_fi;
} CIf;

typedef struct CGoto {
    TIdentifier target;
    size_t line;
} CGoto;

typedef struct CLabel {
    TIdentifier target;
    unique_ptr_t(CStatement) jump_to;
    size_t line;
} CLabel;

typedef struct CCompound {
    unique_ptr_t(CBlock) block;
} CCompound;

typedef struct CWhile {
    TIdentifier target;
    unique_ptr_t(CExp) condition;
    unique_ptr_t(CStatement) body;
} CWhile;

typedef struct CDoWhile {
    TIdentifier target;
    unique_ptr_t(CExp) condition;
    unique_ptr_t(CStatement) body;
} CDoWhile;

typedef struct CFor {
    TIdentifier target;
    unique_ptr_t(CForInit) init;
    unique_ptr_t(CExp) condition;
    unique_ptr_t(CExp) post;
    unique_ptr_t(CStatement) body;
} CFor;

typedef struct CSwitch {
    TIdentifier target;
    bool is_default;
    unique_ptr_t(CExp) match;
    unique_ptr_t(CStatement) body;
    vector_t(unique_ptr_t(CExp)) cases;
} CSwitch;

typedef struct CCase {
    TIdentifier target;
    unique_ptr_t(CExp) value;
    unique_ptr_t(CStatement) jump_to;
} CCase;

typedef struct CDefault {
    TIdentifier target;
    unique_ptr_t(CStatement) jump_to;
    size_t line;
} CDefault;

typedef struct CBreak {
    TIdentifier target;
    size_t line;
} CBreak;

typedef struct CContinue {
    TIdentifier target;
    size_t line;
} CContinue;

typedef struct CNull {
    int8_t _empty;
} CNull;

typedef struct CStatement {
    unique_ptr_impl(AST_T);

    union {
        CReturn _CReturn;
        CExpression _CExpression;
        CIf _CIf;
        CGoto _CGoto;
        CLabel _CLabel;
        CCompound _CCompound;
        CWhile _CWhile;
        CDoWhile _CDoWhile;
        CFor _CFor;
        CSwitch _CSwitch;
        CCase _CCase;
        CDefault _CDefault;
        CBreak _CBreak;
        CContinue _CContinue;
        CNull _CNull;
    } get;
} CStatement;

unique_ptr_t(CStatement) make_CStatement(void);
unique_ptr_t(CStatement) make_CReturn(unique_ptr_t(CExp) * exp, size_t line);
unique_ptr_t(CStatement) make_CExpression(unique_ptr_t(CExp) * exp);
unique_ptr_t(CStatement)
    make_CIf(unique_ptr_t(CExp) * condition, unique_ptr_t(CStatement) * then, unique_ptr_t(CStatement) * else_fi);
unique_ptr_t(CStatement) make_CGoto(TIdentifier target, size_t line);
unique_ptr_t(CStatement) make_CLabel(TIdentifier target, unique_ptr_t(CStatement) * jump_to, size_t line);
unique_ptr_t(CStatement) make_CCompound(unique_ptr_t(CBlock) * block);
unique_ptr_t(CStatement) make_CWhile(unique_ptr_t(CExp) * condition, unique_ptr_t(CStatement) * body);
unique_ptr_t(CStatement) make_CDoWhile(unique_ptr_t(CExp) * condition, unique_ptr_t(CStatement) * body);
unique_ptr_t(CStatement) make_CFor(unique_ptr_t(CForInit) * init, unique_ptr_t(CExp) * condition,
    unique_ptr_t(CExp) * post, unique_ptr_t(CStatement) * body);
unique_ptr_t(CStatement) make_CSwitch(unique_ptr_t(CExp) * match, unique_ptr_t(CStatement) * body);
unique_ptr_t(CStatement) make_CCase(unique_ptr_t(CExp) * value, unique_ptr_t(CStatement) * jump_to);
unique_ptr_t(CStatement) make_CDefault(unique_ptr_t(CStatement) * jump_to, size_t line);
unique_ptr_t(CStatement) make_CBreak(size_t line);
unique_ptr_t(CStatement) make_CContinue(size_t line);
unique_ptr_t(CStatement) make_CNull(void);
void free_CStatement(unique_ptr_t(CStatement) * self);
void move_CStatement(unique_ptr_t(CStatement) * self, unique_ptr_t(CStatement) * other);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// for_init = InitDecl(variable_declaration)
//          | InitExp(exp?)

typedef struct CInitDecl {
    unique_ptr_t(CVariableDeclaration) init;
} CInitDecl;

typedef struct CInitExp {
    unique_ptr_t(CExp) init;
} CInitExp;

typedef struct CForInit {
    unique_ptr_impl(AST_T);

    union {
        CInitDecl _CInitDecl;
        CInitExp _CInitExp;
    } get;
} CForInit;

unique_ptr_t(CForInit) make_CForInit(void);
unique_ptr_t(CForInit) make_CInitDecl(unique_ptr_t(CVariableDeclaration) * init);
unique_ptr_t(CForInit) make_CInitExp(unique_ptr_t(CExp) * init);
void free_CForInit(unique_ptr_t(CForInit) * self);
void move_CForInit(unique_ptr_t(CForInit) * self, unique_ptr_t(CForInit) * other);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// block = B(block_item*)

typedef struct CB {
    vector_t(unique_ptr_t(CBlockItem)) block_items;
} CB;

typedef struct CBlock {
    unique_ptr_impl(AST_T);

    union {
        CB _CB;
    } get;
} CBlock;

unique_ptr_t(CBlock) make_CBlock(void);
unique_ptr_t(CBlock) make_CB(vector_t(unique_ptr_t(CBlockItem)) * block_items);
void free_CBlock(unique_ptr_t(CBlock) * self);
void move_CBlock(unique_ptr_t(CBlock) * self, unique_ptr_t(CBlock) * other);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// block_item = S(statement)
//            | D(declaration)

typedef struct CS {
    unique_ptr_t(CStatement) statement;
} CS;

typedef struct CD {
    unique_ptr_t(CDeclaration) declaration;
} CD;

typedef struct CBlockItem {
    unique_ptr_impl(AST_T);

    union {
        CS _CS;
        CD _CD;
    } get;
} CBlockItem;

unique_ptr_t(CBlockItem) make_CBlockItem(void);
unique_ptr_t(CBlockItem) make_CS(unique_ptr_t(CStatement) * statement);
unique_ptr_t(CBlockItem) make_CD(unique_ptr_t(CDeclaration) * declaration);
void free_CBlockItem(unique_ptr_t(CBlockItem) * self);
void move_CBlockItem(unique_ptr_t(CBlockItem) * self, unique_ptr_t(CBlockItem) * other);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// storage_class = Static
//               | Extern

typedef struct CStatic {
    int8_t _empty;
} CStatic;

typedef struct CExtern {
    int8_t _empty;
} CExtern;

typedef struct CStorageClass {
    unique_ptr_impl(AST_T);

    union {
        CStatic _CStatic;
        CExtern _CExtern;
    } get;
} CStorageClass;

unique_ptr_t(CStorageClass) make_CStorageClass(void);
unique_ptr_t(CStorageClass) make_CStatic(void);
unique_ptr_t(CStorageClass) make_CExtern(void);
void free_CStorageClass(unique_ptr_t(CStorageClass) * self);
void move_CStorageClass(unique_ptr_t(CStorageClass) * self, unique_ptr_t(CStorageClass) * other);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// initializer = SingleInit(exp)
//             | CompoundInit(initializer*)

typedef struct CSingleInit {
    unique_ptr_t(CExp) exp;
} CSingleInit;

typedef struct CCompoundInit {
    vector_t(unique_ptr_t(CInitializer)) initializers;
} CCompoundInit;

typedef struct CInitializer {
    unique_ptr_impl(AST_T);
    shared_ptr_t(Type) init_type;

    union {
        CSingleInit _CSingleInit;
        CCompoundInit _CCompoundInit;
    } get;
} CInitializer;

unique_ptr_t(CInitializer) make_CInitializer(void);
unique_ptr_t(CInitializer) make_CSingleInit(unique_ptr_t(CExp) * exp);
unique_ptr_t(CInitializer) make_CCompoundInit(vector_t(unique_ptr_t(CInitializer)) * initializers);
void free_CInitializer(unique_ptr_t(CInitializer) * self);
void move_CInitializer(unique_ptr_t(CInitializer) * self, unique_ptr_t(CInitializer) * other);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// member_declaration = CMemberDeclaration(identifier, type)

typedef struct CMemberDeclaration {
    unique_ptr_impl(AST_T);
    TIdentifier member_name;
    shared_ptr_t(Type) member_type;
    size_t line;

} CMemberDeclaration;

unique_ptr_t(CMemberDeclaration)
    make_CMemberDeclaration(TIdentifier member_name, shared_ptr_t(Type) * member_type, size_t line);
void free_CMemberDeclaration(unique_ptr_t(CMemberDeclaration) * self);
void move_CMemberDeclaration(unique_ptr_t(CMemberDeclaration) * self, unique_ptr_t(CMemberDeclaration) * other);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// struct_declaration = StructDeclaration(identifier, bool, member_declaration*)

typedef struct CStructDeclaration {
    unique_ptr_impl(AST_T);
    TIdentifier tag;
    bool is_union;
    vector_t(unique_ptr_t(CMemberDeclaration)) members;
    size_t line;

} CStructDeclaration;

unique_ptr_t(CStructDeclaration) make_CStructDeclaration(
    TIdentifier tag, bool is_union, vector_t(unique_ptr_t(CMemberDeclaration)) * members, size_t line);
void free_CStructDeclaration(unique_ptr_t(CStructDeclaration) * self);
void move_CStructDeclaration(unique_ptr_t(CStructDeclaration) * self, unique_ptr_t(CStructDeclaration) * other);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// function_declaration = FunctionDeclaration(identifier, identifier*, block?, type, storage_class?)

typedef struct CFunctionDeclaration {
    unique_ptr_impl(AST_T);
    TIdentifier name;
    vector_t(TIdentifier) params;
    unique_ptr_t(CBlock) body;
    shared_ptr_t(Type) fun_type;
    unique_ptr_t(CStorageClass) storage_class;
    size_t line;

} CFunctionDeclaration;

unique_ptr_t(CFunctionDeclaration)
    make_CFunctionDeclaration(TIdentifier name, vector_t(TIdentifier) * params, unique_ptr_t(CBlock) * body,
        shared_ptr_t(Type) * fun_type, unique_ptr_t(CStorageClass) * storage_class, size_t line);
void free_CFunctionDeclaration(unique_ptr_t(CFunctionDeclaration) * self);
void move_CFunctionDeclaration(unique_ptr_t(CFunctionDeclaration) * self, unique_ptr_t(CFunctionDeclaration) * other);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// variable_declaration = VariableDeclaration(identifier, initializer?, type, storage_class?)

typedef struct CVariableDeclaration {
    unique_ptr_impl(AST_T);
    TIdentifier name;
    unique_ptr_t(CInitializer) init;
    shared_ptr_t(Type) var_type;
    unique_ptr_t(CStorageClass) storage_class;
    size_t line;

} CVariableDeclaration;

unique_ptr_t(CVariableDeclaration) make_CVariableDeclaration(TIdentifier name, unique_ptr_t(CInitializer) * init,
    shared_ptr_t(Type) * var_type, unique_ptr_t(CStorageClass) * storage_class, size_t line);
void free_CVariableDeclaration(unique_ptr_t(CVariableDeclaration) * self);
void move_CVariableDeclaration(unique_ptr_t(CVariableDeclaration) * self, unique_ptr_t(CVariableDeclaration) * other);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// declaration = FunDecl(function_declaration)
//             | VarDecl(variable_declaration)
//             | StructDecl(struct_declaration)

typedef struct CFunDecl {
    unique_ptr_t(CFunctionDeclaration) fun_decl;
} CFunDecl;

typedef struct CVarDecl {
    unique_ptr_t(CVariableDeclaration) var_decl;
} CVarDecl;

typedef struct CStructDecl {
    unique_ptr_t(CStructDeclaration) struct_decl;
} CStructDecl;

typedef struct CDeclaration {
    unique_ptr_impl(AST_T);

    union {
        CFunDecl _CFunDecl;
        CVarDecl _CVarDecl;
        CStructDecl _CStructDecl;
    } get;
} CDeclaration;

unique_ptr_t(CDeclaration) make_CDeclaration(void);
unique_ptr_t(CDeclaration) make_CFunDecl(unique_ptr_t(CFunctionDeclaration) * fun_decl);
unique_ptr_t(CDeclaration) make_CVarDecl(unique_ptr_t(CVariableDeclaration) * var_decl);
unique_ptr_t(CDeclaration) make_CStructDecl(unique_ptr_t(CStructDeclaration) * struct_decl);
void free_CDeclaration(unique_ptr_t(CDeclaration) * self);
void move_CDeclaration(unique_ptr_t(CDeclaration) * self, unique_ptr_t(CDeclaration) * other);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// AST = Program(declaration*)

typedef struct CProgram {
    unique_ptr_impl(AST_T);
    vector_t(unique_ptr_t(CDeclaration)) declarations;

} CProgram;

unique_ptr_t(CProgram) make_CProgram(vector_t(unique_ptr_t(CDeclaration)) * declarations);
void free_CProgram(unique_ptr_t(CProgram) * self);
void move_CProgram(unique_ptr_t(CProgram) * self, unique_ptr_t(CProgram) * other);

#endif
