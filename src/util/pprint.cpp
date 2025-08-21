#include "util/pprint.hpp"
#ifndef __NDEBUG__
#include <iostream>
#include <string>

#include "util/c_std.hpp"
#include "util/str2t.hpp"
#include "util/throw.hpp"

#include "ast/ast.hpp"
#include "ast/back_ast.hpp"
#include "ast/back_symt.hpp"
#include "ast/front_ast.hpp"
#include "ast/front_symt.hpp"
#include "ast/interm_ast.hpp"

#include "frontend/parser/errors.hpp"
#include "frontend/parser/lexer.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Pretty print

static void print_title(const char* title) { std::cout << "+\n+\n@@ " << std::string(title) << " @@"; }

void pprint_toks(IdentifierContext* ctx, vector_t(Token) tokens) {
    print_title("Tokens");
    std::cout << "\nList[" << std::to_string(vec_size(tokens)) << "]:";
    for (size_t i = 0; i < vec_size(tokens); ++i) {
        std::cout << "\n  ";
        switch (tokens[i].tok_kind) {
            case TOK_identifier:
            case TOK_string_literal:
            case TOK_char_const:
            case TOK_int_const:
            case TOK_long_const:
            case TOK_uint_const:
            case TOK_ulong_const:
            case TOK_dbl_const:
                std::cout << get_tok_kind_fmt(tokens[i].tok_kind) << "(" << get_tok_fmt(ctx, &tokens[i]) << ")";
                break;
            default:
                std::cout << get_tok_fmt(ctx, &tokens[i]);
                break;
        }
    }
    std::cout << std::endl;
}

typedef IdentifierContext* Ctx;

static void print_CDeclarator(Ctx ctx, CDeclarator* node, size_t tab);
static void print_CForInit(Ctx ctx, CForInit* node, size_t tab);
static void print_CBlock(Ctx ctx, CBlock* node, size_t tab);
static void print_CBlockItem(Ctx ctx, CBlockItem* node, size_t tab);
static void print_CVariableDeclaration(Ctx ctx, CVariableDeclaration* node, size_t tab);
static void print_CDeclaration(Ctx ctx, CDeclaration* node, size_t tab);
static void print_AsmOperand(Ctx ctx, AsmOperand* node, size_t tab);

#define assert_print(X, Y)        \
    if (!X) {                     \
        print_field(Y, "None: "); \
        return;                   \
    }

#define tab_size 2
#define print_field(X, ...)                             \
    do {                                                \
        printf("\n%*s", (int)(((X)-1) * tab_size), ""); \
        printf(__VA_ARGS__);                            \
    }                                                   \
    while (0)

static void print_CConst(CConst* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_CConst_t:
            print_field(++tab, "CConst: ");
            break;
        case AST_CConstInt_t:
            print_field(++tab, "CConstInt: ");
            print_field(tab + 1, "TInt: %i", static_cast<CConstInt*>(node)->value);
            break;
        case AST_CConstLong_t:
            print_field(++tab, "CConstLong: ");
            print_field(tab + 1, "TLong: %zi", (ssize_t) static_cast<CConstLong*>(node)->value);
            break;
        case AST_CConstUInt_t:
            print_field(++tab, "CConstUInt: ");
            print_field(tab + 1, "TUInt: %u", static_cast<CConstUInt*>(node)->value);
            break;
        case AST_CConstULong_t:
            print_field(++tab, "CConstULong: ");
            print_field(tab + 1, "TULong: %zu", (size_t) static_cast<CConstULong*>(node)->value);
            break;
        case AST_CConstDouble_t:
            print_field(++tab, "CConstDouble: ");
            print_field(tab + 1, "TDouble: %lf", static_cast<CConstDouble*>(node)->value);
            break;
        case AST_CConstChar_t:
            print_field(++tab, "CConstChar: ");
            print_field(tab + 1, "TChar: %c", static_cast<CConstChar*>(node)->value);
            break;
        case AST_CConstUChar_t:
            print_field(++tab, "CConstUChar: ");
            print_field(tab + 1, "TUChar: %u", (unsigned int)static_cast<CConstUChar*>(node)->value);
            break;
        default:
            THROW_ABORT;
    }
}

static void print_CStringLiteral(CStringLiteral* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_CStringLiteral_t:
            print_field(++tab, "CStringLiteral: ");
            break;
        default:
            THROW_ABORT;
    }
    print_field(tab + 1, "List[%zu]: ", vec_size(node->value));
    for (size_t i = 0; i < vec_size(node->value); ++i) {
        print_field(tab + 2, "TChar: %c", node->value[i]);
    }
}

static void print_Type(Ctx ctx, Type* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_Type_t:
            print_field(++tab, "Type: ");
            break;
        case AST_Char_t:
            print_field(++tab, "Char: ");
            break;
        case AST_SChar_t:
            print_field(++tab, "SChar: ");
            break;
        case AST_UChar_t:
            print_field(++tab, "UChar: ");
            break;
        case AST_Int_t:
            print_field(++tab, "Int: ");
            break;
        case AST_Long_t:
            print_field(++tab, "Long: ");
            break;
        case AST_UInt_t:
            print_field(++tab, "UInt: ");
            break;
        case AST_ULong_t:
            print_field(++tab, "ULong: ");
            break;
        case AST_Double_t:
            print_field(++tab, "Double: ");
            break;
        case AST_Void_t:
            print_field(++tab, "Void: ");
            break;
        case AST_FunType_t:
            print_field(++tab, "FunType: ");
            print_field(tab + 1, "TULong: %zu", (size_t) static_cast<FunType*>(node)->param_reg_mask);
            print_field(tab + 1, "TULong: %zu", (size_t) static_cast<FunType*>(node)->ret_reg_mask);
            print_field(tab + 1, "List[%zu]: ", vec_size(static_cast<FunType*>(node)->param_types));
            for (size_t i = 0; i < vec_size(static_cast<FunType*>(node)->param_types); ++i) {
                print_Type(ctx, static_cast<Type*>(static_cast<FunType*>(node)->param_types[i].get()), tab + 1);
            }
            print_Type(ctx, static_cast<Type*>(static_cast<FunType*>(node)->ret_type.get()), tab);
            break;
        case AST_Pointer_t:
            print_field(++tab, "Pointer: ");
            print_Type(ctx, static_cast<Type*>(static_cast<Pointer*>(node)->ref_type.get()), tab);
            break;
        case AST_Array_t:
            print_field(++tab, "Array: ");
            print_field(tab + 1, "TLong: %zi", (ssize_t) static_cast<Array*>(node)->size);
            print_Type(ctx, static_cast<Type*>(static_cast<Array*>(node)->elem_type.get()), tab);
            break;
        case AST_Structure_t:
            print_field(++tab, "Structure: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<Structure*>(node)->tag));
            print_field(tab + 1, "Bool: %s", static_cast<Structure*>(node)->is_union ? "True" : "False");
            break;
        default:
            THROW_ABORT;
    }
}

static void print_StaticInit(Ctx ctx, StaticInit* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_StaticInit_t:
            print_field(++tab, "StaticInit: ");
            break;
        case AST_IntInit_t:
            print_field(++tab, "IntInit: ");
            print_field(tab + 1, "TInt: %i", static_cast<IntInit*>(node)->value);
            break;
        case AST_LongInit_t:
            print_field(++tab, "LongInit: ");
            print_field(tab + 1, "TLong: %zi", (ssize_t) static_cast<LongInit*>(node)->value);
            break;
        case AST_UIntInit_t:
            print_field(++tab, "UIntInit: ");
            print_field(tab + 1, "TUInt: %u", static_cast<UIntInit*>(node)->value);
            break;
        case AST_ULongInit_t:
            print_field(++tab, "ULongInit: ");
            print_field(tab + 1, "TULong: %zu", (size_t) static_cast<ULongInit*>(node)->value);
            break;
        case AST_CharInit_t:
            print_field(++tab, "CharInit: ");
            print_field(tab + 1, "TChar: %c", static_cast<CharInit*>(node)->value);
            break;
        case AST_UCharInit_t:
            print_field(++tab, "UCharInit: ");
            print_field(tab + 1, "TUChar: %u", (unsigned int)static_cast<UCharInit*>(node)->value);
            break;
        case AST_DoubleInit_t:
            print_field(++tab, "DoubleInit: ");
            print_field(
                tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<DoubleInit*>(node)->dbl_const));
            break;
        case AST_ZeroInit_t:
            print_field(++tab, "ZeroInit: ");
            print_field(tab + 1, "TLong: %zi", (ssize_t) static_cast<ZeroInit*>(node)->byte);
            break;
        case AST_StringInit_t:
            print_field(++tab, "StringInit: ");
            print_field(
                tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<StringInit*>(node)->string_const));
            print_field(tab + 1, "Bool: %s", static_cast<StringInit*>(node)->is_null_term ? "True" : "False");
            print_CStringLiteral(static_cast<CStringLiteral*>(static_cast<StringInit*>(node)->literal.get()), tab);
            break;
        case AST_PointerInit_t:
            print_field(++tab, "PointerInit: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<PointerInit*>(node)->name));
            break;
        default:
            THROW_ABORT;
    }
}

static void print_InitialValue(Ctx ctx, InitialValue* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_InitialValue_t:
            print_field(++tab, "InitialValue: ");
            break;
        case AST_Tentative_t:
            print_field(++tab, "Tentative: ");
            break;
        case AST_Initial_t:
            print_field(++tab, "Initial: ");
            print_field(tab + 1, "List[%zu]: ", vec_size(static_cast<Initial*>(node)->static_inits));
            for (size_t i = 0; i < vec_size(static_cast<Initial*>(node)->static_inits); ++i) {
                print_StaticInit(
                    ctx, static_cast<StaticInit*>(static_cast<Initial*>(node)->static_inits[i].get()), tab + 1);
            }
            break;
        case AST_NoInitializer_t:
            print_field(++tab, "NoInitializer: ");
            break;
        default:
            THROW_ABORT;
    }
}

static void print_IdentifierAttr(Ctx ctx, IdentifierAttr* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_IdentifierAttr_t:
            print_field(++tab, "IdentifierAttr: ");
            break;
        case AST_FunAttr_t:
            print_field(++tab, "FunAttr: ");
            print_field(tab + 1, "Bool: %s", static_cast<FunAttr*>(node)->is_def ? "True" : "False");
            print_field(tab + 1, "Bool: %s", static_cast<FunAttr*>(node)->is_glob ? "True" : "False");
            break;
        case AST_StaticAttr_t:
            print_field(++tab, "StaticAttr: ");
            print_field(tab + 1, "Bool: %s", static_cast<StaticAttr*>(node)->is_glob ? "True" : "False");
            print_InitialValue(ctx, static_cast<InitialValue*>(static_cast<StaticAttr*>(node)->init.get()), tab);
            break;
        case AST_ConstantAttr_t:
            print_field(++tab, "ConstantAttr: ");
            print_StaticInit(ctx, static_cast<StaticInit*>(static_cast<ConstantAttr*>(node)->static_init.get()), tab);
            break;
        case AST_LocalAttr_t:
            print_field(++tab, "LocalAttr: ");
            break;
        default:
            THROW_ABORT;
    }
}

static void print_Symbol(Ctx ctx, Symbol* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_Symbol_t:
            print_field(++tab, "Symbol: ");
            break;
        default:
            THROW_ABORT;
    }
    print_Type(ctx, static_cast<Type*>(node->type_t.get()), tab);
    print_IdentifierAttr(ctx, static_cast<IdentifierAttr*>(node->attrs.get()), tab);
}

static void print_StructMember(Ctx ctx, StructMember* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_StructMember_t:
            print_field(++tab, "StructMember: ");
            break;
        default:
            THROW_ABORT;
    }
    print_field(tab + 1, "TLong: %zi", (ssize_t)node->offset);
    print_Type(ctx, static_cast<Type*>(node->member_type.get()), tab);
}

static void print_StructTypedef(Ctx ctx, StructTypedef* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_StructTypedef_t:
            print_field(++tab, "StructTypedef: ");
            break;
        default:
            THROW_ABORT;
    }
    print_field(tab + 1, "TInt: %i", node->alignment);
    print_field(tab + 1, "TLong: %zi", (ssize_t)node->size);
    print_field(tab + 1, "List[%zu]: ", vec_size(node->member_names));
    for (size_t i = 0; i < vec_size(node->member_names); ++i) {
        print_field(tab + 2, "TIdentifier: %s", map_get(ctx->hash_table, node->member_names[i]));
    }
    print_field(tab + 1, "Dict[%zu]: ", map_size(node->members));
    for (size_t i = 0; i < map_size(node->members); ++i) {
        print_field(tab + 2, "[%s]: ", map_get(ctx->hash_table, pair_first(node->members[i])));
        print_StructMember(ctx, static_cast<StructMember*>(pair_second(node->members[i]).get()), tab + 2);
    }
}

static void print_CUnaryOp(CUnaryOp* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_CUnaryOp_t:
            print_field(++tab, "CUnaryOp: ");
            break;
        case AST_CComplement_t:
            print_field(++tab, "CComplement: ");
            break;
        case AST_CNegate_t:
            print_field(++tab, "CNegate: ");
            break;
        case AST_CNot_t:
            print_field(++tab, "CNot: ");
            break;
        case AST_CPrefix_t:
            print_field(++tab, "CPrefix: ");
            break;
        case AST_CPostfix_t:
            print_field(++tab, "CPostfix: ");
            break;
        default:
            THROW_ABORT;
    }
}

static void print_CBinaryOp(CBinaryOp* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_CBinaryOp_t:
            print_field(++tab, "CBinaryOp: ");
            break;
        case AST_CAdd_t:
            print_field(++tab, "CAdd: ");
            break;
        case AST_CSubtract_t:
            print_field(++tab, "CSubtract: ");
            break;
        case AST_CMultiply_t:
            print_field(++tab, "CMultiply: ");
            break;
        case AST_CDivide_t:
            print_field(++tab, "CDivide: ");
            break;
        case AST_CRemainder_t:
            print_field(++tab, "CRemainder: ");
            break;
        case AST_CBitAnd_t:
            print_field(++tab, "CBitAnd: ");
            break;
        case AST_CBitOr_t:
            print_field(++tab, "CBitOr: ");
            break;
        case AST_CBitXor_t:
            print_field(++tab, "CBitXor: ");
            break;
        case AST_CBitShiftLeft_t:
            print_field(++tab, "CBitShiftLeft: ");
            break;
        case AST_CBitShiftRight_t:
            print_field(++tab, "CBitShiftRight: ");
            break;
        case AST_CBitShrArithmetic_t:
            print_field(++tab, "CBitShrArithmetic: ");
            break;
        case AST_CAnd_t:
            print_field(++tab, "CAnd: ");
            break;
        case AST_COr_t:
            print_field(++tab, "COr: ");
            break;
        case AST_CEqual_t:
            print_field(++tab, "CEqual: ");
            break;
        case AST_CNotEqual_t:
            print_field(++tab, "CNotEqual: ");
            break;
        case AST_CLessThan_t:
            print_field(++tab, "CLessThan: ");
            break;
        case AST_CLessOrEqual_t:
            print_field(++tab, "CLessOrEqual: ");
            break;
        case AST_CGreaterThan_t:
            print_field(++tab, "CGreaterThan: ");
            break;
        case AST_CGreaterOrEqual_t:
            print_field(++tab, "CGreaterOrEqual: ");
            break;
        default:
            THROW_ABORT;
    }
}

static void print_CAbstractDeclarator(Ctx ctx, CAbstractDeclarator* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_CAbstractDeclarator_t:
            print_field(++tab, "CAbstractDeclarator: ");
            break;
        case AST_CAbstractPointer_t:
            print_field(++tab, "CAbstractPointer: ");
            print_CAbstractDeclarator(ctx,
                static_cast<CAbstractDeclarator*>(static_cast<CAbstractPointer*>(node)->abstract_decltor.get()), tab);
            break;
        case AST_CAbstractArray_t:
            print_field(++tab, "CAbstractArray: ");
            print_field(tab + 1, "TLong: %zi", (ssize_t) static_cast<CAbstractArray*>(node)->size);
            print_CAbstractDeclarator(ctx,
                static_cast<CAbstractDeclarator*>(static_cast<CAbstractArray*>(node)->abstract_decltor.get()), tab);
            break;
        case AST_CAbstractBase_t:
            print_field(++tab, "CAbstractBase: ");
            break;
        default:
            THROW_ABORT;
    }
}

static void print_CParam(Ctx ctx, CParam* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_CParam_t:
            print_field(++tab, "CParam: ");
            break;
        default:
            THROW_ABORT;
    }
    print_CDeclarator(ctx, static_cast<CDeclarator*>(node->decltor.get()), tab);
    print_Type(ctx, static_cast<Type*>(node->param_type.get()), tab);
}

static void print_CDeclarator(Ctx ctx, CDeclarator* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_CDeclarator_t:
            print_field(++tab, "CDeclarator: ");
            break;
        case AST_CIdent_t:
            print_field(++tab, "CIdent: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<CIdent*>(node)->name));
            break;
        case AST_CPointerDeclarator_t:
            print_field(++tab, "CPointerDeclarator: ");
            print_CDeclarator(
                ctx, static_cast<CDeclarator*>(static_cast<CPointerDeclarator*>(node)->decltor.get()), tab);
            break;
        case AST_CArrayDeclarator_t:
            print_field(++tab, "CArrayDeclarator: ");
            print_field(tab + 1, "TLong: %zi", (ssize_t) static_cast<CArrayDeclarator*>(node)->size);
            print_CDeclarator(ctx, static_cast<CDeclarator*>(static_cast<CArrayDeclarator*>(node)->decltor.get()), tab);
            break;
        case AST_CFunDeclarator_t:
            print_field(++tab, "CFunDeclarator: ");
            print_field(tab + 1, "List[%zu]: ", vec_size(static_cast<CFunDeclarator*>(node)->param_list));
            for (size_t i = 0; i < vec_size(static_cast<CFunDeclarator*>(node)->param_list); ++i) {
                print_CParam(
                    ctx, static_cast<CParam*>(static_cast<CFunDeclarator*>(node)->param_list[i].get()), tab + 1);
            }
            print_CDeclarator(ctx, static_cast<CDeclarator*>(static_cast<CFunDeclarator*>(node)->decltor.get()), tab);
            break;
        default:
            THROW_ABORT;
    }
}

static void print_CExp(Ctx ctx, CExp* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_CExp_t:
            print_field(++tab, "CExp: ");
            break;
        case AST_CConstant_t:
            print_field(++tab, "CConstant: ");
            print_CConst(static_cast<CConst*>(static_cast<CConstant*>(node)->constant.get()), tab);
            break;
        case AST_CString_t:
            print_field(++tab, "CString: ");
            print_CStringLiteral(static_cast<CStringLiteral*>(static_cast<CString*>(node)->literal.get()), tab);
            break;
        case AST_CVar_t:
            print_field(++tab, "CVar: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<CVar*>(node)->name));
            break;
        case AST_CCast_t:
            print_field(++tab, "CCast: ");
            print_CExp(ctx, static_cast<CExp*>(static_cast<CCast*>(node)->exp.get()), tab);
            print_Type(ctx, static_cast<Type*>(static_cast<CCast*>(node)->target_type.get()), tab);
            break;
        case AST_CUnary_t:
            print_field(++tab, "CUnary: ");
            print_CUnaryOp(static_cast<CUnaryOp*>(static_cast<CUnary*>(node)->unop.get()), tab);
            print_CExp(ctx, static_cast<CExp*>(static_cast<CUnary*>(node)->exp.get()), tab);
            break;
        case AST_CBinary_t:
            print_field(++tab, "CBinary: ");
            print_CBinaryOp(static_cast<CBinaryOp*>(static_cast<CBinary*>(node)->binop.get()), tab);
            print_CExp(ctx, static_cast<CExp*>(static_cast<CBinary*>(node)->exp_left.get()), tab);
            print_CExp(ctx, static_cast<CExp*>(static_cast<CBinary*>(node)->exp_right.get()), tab);
            break;
        case AST_CAssignment_t:
            print_field(++tab, "CAssignment: ");
            print_CUnaryOp(static_cast<CUnaryOp*>(static_cast<CAssignment*>(node)->unop.get()), tab);
            print_CExp(ctx, static_cast<CExp*>(static_cast<CAssignment*>(node)->exp_left.get()), tab);
            print_CExp(ctx, static_cast<CExp*>(static_cast<CAssignment*>(node)->exp_right.get()), tab);
            break;
        case AST_CConditional_t:
            print_field(++tab, "CConditional: ");
            print_CExp(ctx, static_cast<CExp*>(static_cast<CConditional*>(node)->condition.get()), tab);
            print_CExp(ctx, static_cast<CExp*>(static_cast<CConditional*>(node)->exp_middle.get()), tab);
            print_CExp(ctx, static_cast<CExp*>(static_cast<CConditional*>(node)->exp_right.get()), tab);
            break;
        case AST_CFunctionCall_t:
            print_field(++tab, "CFunctionCall: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<CFunctionCall*>(node)->name));
            print_field(tab + 1, "List[%zu]: ", vec_size(static_cast<CFunctionCall*>(node)->args));
            for (size_t i = 0; i < vec_size(static_cast<CFunctionCall*>(node)->args); ++i) {
                print_CExp(ctx, static_cast<CExp*>(static_cast<CFunctionCall*>(node)->args[i].get()), tab + 1);
            }
            break;
        case AST_CDereference_t:
            print_field(++tab, "CDereference: ");
            print_CExp(ctx, static_cast<CExp*>(static_cast<CDereference*>(node)->exp.get()), tab);
            break;
        case AST_CAddrOf_t:
            print_field(++tab, "CAddrOf: ");
            print_CExp(ctx, static_cast<CExp*>(static_cast<CAddrOf*>(node)->exp.get()), tab);
            break;
        case AST_CSubscript_t:
            print_field(++tab, "CSubscript: ");
            print_CExp(ctx, static_cast<CExp*>(static_cast<CSubscript*>(node)->primary_exp.get()), tab);
            print_CExp(ctx, static_cast<CExp*>(static_cast<CSubscript*>(node)->subscript_exp.get()), tab);
            break;
        case AST_CSizeOf_t:
            print_field(++tab, "CSizeOf: ");
            print_CExp(ctx, static_cast<CExp*>(static_cast<CSizeOf*>(node)->exp.get()), tab);
            break;
        case AST_CSizeOfT_t:
            print_field(++tab, "CSizeOfT: ");
            print_Type(ctx, static_cast<Type*>(static_cast<CSizeOfT*>(node)->target_type.get()), tab);
            break;
        case AST_CDot_t:
            print_field(++tab, "CDot: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<CDot*>(node)->member));
            print_CExp(ctx, static_cast<CExp*>(static_cast<CDot*>(node)->structure.get()), tab);
            break;
        case AST_CArrow_t:
            print_field(++tab, "CArrow: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<CArrow*>(node)->member));
            print_CExp(ctx, static_cast<CExp*>(static_cast<CArrow*>(node)->pointer.get()), tab);
            break;
        default:
            THROW_ABORT;
    }
    print_Type(ctx, static_cast<Type*>(node->exp_type.get()), tab);
}

static void print_CStatement(Ctx ctx, CStatement* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_CStatement_t:
            print_field(++tab, "CStatement: ");
            break;
        case AST_CReturn_t:
            print_field(++tab, "CReturn: ");
            print_CExp(ctx, static_cast<CExp*>(static_cast<CReturn*>(node)->exp.get()), tab);
            break;
        case AST_CExpression_t:
            print_field(++tab, "CExpression: ");
            print_CExp(ctx, static_cast<CExp*>(static_cast<CExpression*>(node)->exp.get()), tab);
            break;
        case AST_CIf_t:
            print_field(++tab, "CIf: ");
            print_CExp(ctx, static_cast<CExp*>(static_cast<CIf*>(node)->condition.get()), tab);
            print_CStatement(ctx, static_cast<CStatement*>(static_cast<CIf*>(node)->then.get()), tab);
            print_CStatement(ctx, static_cast<CStatement*>(static_cast<CIf*>(node)->else_fi.get()), tab);
            break;
        case AST_CGoto_t:
            print_field(++tab, "CGoto: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<CGoto*>(node)->target));
            break;
        case AST_CLabel_t:
            print_field(++tab, "CLabel: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<CLabel*>(node)->target));
            print_CStatement(ctx, static_cast<CStatement*>(static_cast<CLabel*>(node)->jump_to.get()), tab);
            break;
        case AST_CCompound_t:
            print_field(++tab, "CCompound: ");
            print_CBlock(ctx, static_cast<CBlock*>(static_cast<CCompound*>(node)->block.get()), tab);
            break;
        case AST_CWhile_t:
            print_field(++tab, "CWhile: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<CWhile*>(node)->target));
            print_CExp(ctx, static_cast<CExp*>(static_cast<CWhile*>(node)->condition.get()), tab);
            print_CStatement(ctx, static_cast<CStatement*>(static_cast<CWhile*>(node)->body.get()), tab);
            break;
        case AST_CDoWhile_t:
            print_field(++tab, "CDoWhile: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<CDoWhile*>(node)->target));
            print_CExp(ctx, static_cast<CExp*>(static_cast<CDoWhile*>(node)->condition.get()), tab);
            print_CStatement(ctx, static_cast<CStatement*>(static_cast<CDoWhile*>(node)->body.get()), tab);
            break;
        case AST_CFor_t:
            print_field(++tab, "CFor: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<CFor*>(node)->target));
            print_CForInit(ctx, static_cast<CForInit*>(static_cast<CFor*>(node)->init.get()), tab);
            print_CExp(ctx, static_cast<CExp*>(static_cast<CFor*>(node)->condition.get()), tab);
            print_CExp(ctx, static_cast<CExp*>(static_cast<CFor*>(node)->post.get()), tab);
            print_CStatement(ctx, static_cast<CStatement*>(static_cast<CFor*>(node)->body.get()), tab);
            break;
        case AST_CSwitch_t:
            print_field(++tab, "CSwitch: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<CSwitch*>(node)->target));
            print_field(tab + 1, "Bool: %s", static_cast<CSwitch*>(node)->is_default ? "True" : "False");
            print_CExp(ctx, static_cast<CExp*>(static_cast<CSwitch*>(node)->match.get()), tab);
            print_CStatement(ctx, static_cast<CStatement*>(static_cast<CSwitch*>(node)->body.get()), tab);
            print_field(tab + 1, "List[%zu]: ", vec_size(static_cast<CSwitch*>(node)->cases));
            for (size_t i = 0; i < vec_size(static_cast<CSwitch*>(node)->cases); ++i) {
                print_CExp(ctx, static_cast<CExp*>(static_cast<CSwitch*>(node)->cases[i].get()), tab + 1);
            }
            break;
        case AST_CCase_t:
            print_field(++tab, "CCase: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<CCase*>(node)->target));
            print_CExp(ctx, static_cast<CExp*>(static_cast<CCase*>(node)->value.get()), tab);
            print_CStatement(ctx, static_cast<CStatement*>(static_cast<CCase*>(node)->jump_to.get()), tab);
            break;
        case AST_CDefault_t:
            print_field(++tab, "CDefault: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<CDefault*>(node)->target));
            print_CStatement(ctx, static_cast<CStatement*>(static_cast<CDefault*>(node)->jump_to.get()), tab);
            break;
        case AST_CBreak_t:
            print_field(++tab, "CBreak: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<CBreak*>(node)->target));
            break;
        case AST_CContinue_t:
            print_field(++tab, "CContinue: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<CContinue*>(node)->target));
            break;
        case AST_CNull_t:
            print_field(++tab, "CNull: ");
            break;
        default:
            THROW_ABORT;
    }
}

static void print_CForInit(Ctx ctx, CForInit* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_CForInit_t:
            print_field(++tab, "CForInit: ");
            break;
        case AST_CInitDecl_t:
            print_field(++tab, "CInitDecl: ");
            print_CVariableDeclaration(
                ctx, static_cast<CVariableDeclaration*>(static_cast<CInitDecl*>(node)->init.get()), tab);
            break;
        case AST_CInitExp_t:
            print_field(++tab, "CInitExp: ");
            print_CExp(ctx, static_cast<CExp*>(static_cast<CInitExp*>(node)->init.get()), tab);
            break;
        default:
            THROW_ABORT;
    }
}

static void print_CBlock(Ctx ctx, CBlock* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_CBlock_t:
            print_field(++tab, "CBlock: ");
            break;
        case AST_CB_t:
            print_field(++tab, "CB: ");
            print_field(tab + 1, "List[%zu]: ", vec_size(static_cast<CB*>(node)->block_items));
            for (size_t i = 0; i < vec_size(static_cast<CB*>(node)->block_items); ++i) {
                print_CBlockItem(ctx, static_cast<CBlockItem*>(static_cast<CB*>(node)->block_items[i].get()), tab + 1);
            }
            break;
        default:
            THROW_ABORT;
    }
}

static void print_CBlockItem(Ctx ctx, CBlockItem* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_CBlockItem_t:
            print_field(++tab, "CBlockItem: ");
            break;
        case AST_CS_t:
            print_field(++tab, "CS: ");
            print_CStatement(ctx, static_cast<CStatement*>(static_cast<CS*>(node)->statement.get()), tab);
            break;
        case AST_CD_t:
            print_field(++tab, "CD: ");
            print_CDeclaration(ctx, static_cast<CDeclaration*>(static_cast<CD*>(node)->declaration.get()), tab);
            break;
        default:
            THROW_ABORT;
    }
}

static void print_CStorageClass(CStorageClass* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_CStorageClass_t:
            print_field(++tab, "CStorageClass: ");
            break;
        case AST_CStatic_t:
            print_field(++tab, "CStatic: ");
            break;
        case AST_CExtern_t:
            print_field(++tab, "CExtern: ");
            break;
        default:
            THROW_ABORT;
    }
}

static void print_CInitializer(Ctx ctx, CInitializer* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_CInitializer_t:
            print_field(++tab, "CInitializer: ");
            break;
        case AST_CSingleInit_t:
            print_field(++tab, "CSingleInit: ");
            print_CExp(ctx, static_cast<CExp*>(static_cast<CSingleInit*>(node)->exp.get()), tab);
            break;
        case AST_CCompoundInit_t:
            print_field(++tab, "CCompoundInit: ");
            print_field(tab + 1, "List[%zu]: ", vec_size(static_cast<CCompoundInit*>(node)->initializers));
            for (size_t i = 0; i < vec_size(static_cast<CCompoundInit*>(node)->initializers); ++i) {
                print_CInitializer(
                    ctx, static_cast<CInitializer*>(static_cast<CCompoundInit*>(node)->initializers[i].get()), tab + 1);
            }
            break;
        default:
            THROW_ABORT;
    }
    print_Type(ctx, static_cast<Type*>(node->init_type.get()), tab);
}

static void print_CMemberDeclaration(Ctx ctx, CMemberDeclaration* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_CMemberDeclaration_t:
            print_field(++tab, "CMemberDeclaration: ");
            break;
        default:
            THROW_ABORT;
    }
    print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->member_name));
    print_Type(ctx, static_cast<Type*>(node->member_type.get()), tab);
}

static void print_CStructDeclaration(Ctx ctx, CStructDeclaration* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_CStructDeclaration_t:
            print_field(++tab, "CStructDeclaration: ");
            break;
        default:
            THROW_ABORT;
    }
    print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->tag));
    print_field(tab + 1, "Bool: %s", node->is_union ? "True" : "False");
    print_field(tab + 1, "List[%zu]: ", vec_size(node->members));
    for (size_t i = 0; i < vec_size(node->members); ++i) {
        print_CMemberDeclaration(ctx, static_cast<CMemberDeclaration*>(node->members[i].get()), tab + 1);
    }
}

static void print_CFunctionDeclaration(Ctx ctx, CFunctionDeclaration* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_CFunctionDeclaration_t:
            print_field(++tab, "CFunctionDeclaration: ");
            break;
        default:
            THROW_ABORT;
    }
    print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->name));
    print_field(tab + 1, "List[%zu]: ", vec_size(node->params));
    for (size_t i = 0; i < vec_size(node->params); ++i) {
        print_field(tab + 2, "TIdentifier: %s", map_get(ctx->hash_table, node->params[i]));
    }
    print_CBlock(ctx, static_cast<CBlock*>(node->body.get()), tab);
    print_Type(ctx, static_cast<Type*>(node->fun_type.get()), tab);
    print_CStorageClass(static_cast<CStorageClass*>(node->storage_class.get()), tab);
}

static void print_CVariableDeclaration(Ctx ctx, CVariableDeclaration* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_CVariableDeclaration_t:
            print_field(++tab, "CVariableDeclaration: ");
            break;
        default:
            THROW_ABORT;
    }
    print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->name));
    print_CInitializer(ctx, static_cast<CInitializer*>(node->init.get()), tab);
    print_Type(ctx, static_cast<Type*>(node->var_type.get()), tab);
    print_CStorageClass(static_cast<CStorageClass*>(node->storage_class.get()), tab);
}

static void print_CDeclaration(Ctx ctx, CDeclaration* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_CDeclaration_t:
            print_field(++tab, "CDeclaration: ");
            break;
        case AST_CFunDecl_t:
            print_field(++tab, "CFunDecl: ");
            print_CFunctionDeclaration(
                ctx, static_cast<CFunctionDeclaration*>(static_cast<CFunDecl*>(node)->fun_decl.get()), tab);
            break;
        case AST_CVarDecl_t:
            print_field(++tab, "CVarDecl: ");
            print_CVariableDeclaration(
                ctx, static_cast<CVariableDeclaration*>(static_cast<CVarDecl*>(node)->var_decl.get()), tab);
            break;
        case AST_CStructDecl_t:
            print_field(++tab, "CStructDecl: ");
            print_CStructDeclaration(
                ctx, static_cast<CStructDeclaration*>(static_cast<CStructDecl*>(node)->struct_decl.get()), tab);
            break;
        default:
            THROW_ABORT;
    }
}

static void print_CProgram(Ctx ctx, CProgram* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_CProgram_t:
            print_field(++tab, "CProgram: ");
            break;
        default:
            THROW_ABORT;
    }
    print_field(tab + 1, "List[%zu]: ", vec_size(node->declarations));
    for (size_t i = 0; i < vec_size(node->declarations); ++i) {
        print_CDeclaration(ctx, static_cast<CDeclaration*>(node->declarations[i].get()), tab + 1);
    }
}

static void print_TacUnaryOp(TacUnaryOp* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_TacUnaryOp_t:
            print_field(++tab, "TacUnaryOp: ");
            break;
        case AST_TacComplement_t:
            print_field(++tab, "TacComplement: ");
            break;
        case AST_TacNegate_t:
            print_field(++tab, "TacNegate: ");
            break;
        case AST_TacNot_t:
            print_field(++tab, "TacNot: ");
            break;
        default:
            THROW_ABORT;
    }
}

static void print_TacBinaryOp(TacBinaryOp* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_TacBinaryOp_t:
            print_field(++tab, "TacBinaryOp: ");
            break;
        case AST_TacAdd_t:
            print_field(++tab, "TacAdd: ");
            break;
        case AST_TacSubtract_t:
            print_field(++tab, "TacSubtract: ");
            break;
        case AST_TacMultiply_t:
            print_field(++tab, "TacMultiply: ");
            break;
        case AST_TacDivide_t:
            print_field(++tab, "TacDivide: ");
            break;
        case AST_TacRemainder_t:
            print_field(++tab, "TacRemainder: ");
            break;
        case AST_TacBitAnd_t:
            print_field(++tab, "TacBitAnd: ");
            break;
        case AST_TacBitOr_t:
            print_field(++tab, "TacBitOr: ");
            break;
        case AST_TacBitXor_t:
            print_field(++tab, "TacBitXor: ");
            break;
        case AST_TacBitShiftLeft_t:
            print_field(++tab, "TacBitShiftLeft: ");
            break;
        case AST_TacBitShiftRight_t:
            print_field(++tab, "TacBitShiftRight: ");
            break;
        case AST_TacBitShrArithmetic_t:
            print_field(++tab, "TacBitShrArithmetic: ");
            break;
        case AST_TacEqual_t:
            print_field(++tab, "TacEqual: ");
            break;
        case AST_TacNotEqual_t:
            print_field(++tab, "TacNotEqual: ");
            break;
        case AST_TacLessThan_t:
            print_field(++tab, "TacLessThan: ");
            break;
        case AST_TacLessOrEqual_t:
            print_field(++tab, "TacLessOrEqual: ");
            break;
        case AST_TacGreaterThan_t:
            print_field(++tab, "TacGreaterThan: ");
            break;
        case AST_TacGreaterOrEqual_t:
            print_field(++tab, "TacGreaterOrEqual: ");
            break;
        default:
            THROW_ABORT;
    }
}

static void print_TacValue(Ctx ctx, TacValue* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_TacValue_t:
            print_field(++tab, "TacValue: ");
            break;
        case AST_TacConstant_t:
            print_field(++tab, "TacConstant: ");
            print_CConst(static_cast<CConst*>(static_cast<TacConstant*>(node)->constant.get()), tab);
            break;
        case AST_TacVariable_t:
            print_field(++tab, "TacVariable: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<TacVariable*>(node)->name));
            break;
        default:
            THROW_ABORT;
    }
}

static void print_TacInstruction(Ctx ctx, TacInstruction* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_TacInstruction_t:
            print_field(++tab, "TacInstruction: ");
            break;
        case AST_TacReturn_t:
            print_field(++tab, "TacReturn: ");
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacReturn*>(node)->val.get()), tab);
            break;
        case AST_TacSignExtend_t:
            print_field(++tab, "TacSignExtend: ");
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacSignExtend*>(node)->src.get()), tab);
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacSignExtend*>(node)->dst.get()), tab);
            break;
        case AST_TacTruncate_t:
            print_field(++tab, "TacTruncate: ");
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacTruncate*>(node)->src.get()), tab);
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacTruncate*>(node)->dst.get()), tab);
            break;
        case AST_TacZeroExtend_t:
            print_field(++tab, "TacZeroExtend: ");
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacZeroExtend*>(node)->src.get()), tab);
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacZeroExtend*>(node)->dst.get()), tab);
            break;
        case AST_TacDoubleToInt_t:
            print_field(++tab, "TacDoubleToInt: ");
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacDoubleToInt*>(node)->src.get()), tab);
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacDoubleToInt*>(node)->dst.get()), tab);
            break;
        case AST_TacDoubleToUInt_t:
            print_field(++tab, "TacDoubleToUInt: ");
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacDoubleToUInt*>(node)->src.get()), tab);
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacDoubleToUInt*>(node)->dst.get()), tab);
            break;
        case AST_TacIntToDouble_t:
            print_field(++tab, "TacIntToDouble: ");
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacIntToDouble*>(node)->src.get()), tab);
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacIntToDouble*>(node)->dst.get()), tab);
            break;
        case AST_TacUIntToDouble_t:
            print_field(++tab, "TacUIntToDouble: ");
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacUIntToDouble*>(node)->src.get()), tab);
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacUIntToDouble*>(node)->dst.get()), tab);
            break;
        case AST_TacFunCall_t:
            print_field(++tab, "TacFunCall: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<TacFunCall*>(node)->name));
            print_field(tab + 1, "List[%zu]: ", vec_size(static_cast<TacFunCall*>(node)->args));
            for (size_t i = 0; i < vec_size(static_cast<TacFunCall*>(node)->args); ++i) {
                print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacFunCall*>(node)->args[i].get()), tab + 1);
            }
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacFunCall*>(node)->dst.get()), tab);
            break;
        case AST_TacUnary_t:
            print_field(++tab, "TacUnary: ");
            print_TacUnaryOp(static_cast<TacUnaryOp*>(static_cast<TacUnary*>(node)->unop.get()), tab);
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacUnary*>(node)->src.get()), tab);
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacUnary*>(node)->dst.get()), tab);
            break;
        case AST_TacBinary_t:
            print_field(++tab, "TacBinary: ");
            print_TacBinaryOp(static_cast<TacBinaryOp*>(static_cast<TacBinary*>(node)->binop.get()), tab);
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacBinary*>(node)->src1.get()), tab);
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacBinary*>(node)->src2.get()), tab);
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacBinary*>(node)->dst.get()), tab);
            break;
        case AST_TacCopy_t:
            print_field(++tab, "TacCopy: ");
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacCopy*>(node)->src.get()), tab);
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacCopy*>(node)->dst.get()), tab);
            break;
        case AST_TacGetAddress_t:
            print_field(++tab, "TacGetAddress: ");
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacGetAddress*>(node)->src.get()), tab);
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacGetAddress*>(node)->dst.get()), tab);
            break;
        case AST_TacLoad_t:
            print_field(++tab, "TacLoad: ");
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacLoad*>(node)->src_ptr.get()), tab);
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacLoad*>(node)->dst.get()), tab);
            break;
        case AST_TacStore_t:
            print_field(++tab, "TacStore: ");
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacStore*>(node)->src.get()), tab);
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacStore*>(node)->dst_ptr.get()), tab);
            break;
        case AST_TacAddPtr_t:
            print_field(++tab, "TacAddPtr: ");
            print_field(tab + 1, "TLong: %zi", (ssize_t) static_cast<TacAddPtr*>(node)->scale);
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacAddPtr*>(node)->src_ptr.get()), tab);
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacAddPtr*>(node)->idx.get()), tab);
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacAddPtr*>(node)->dst.get()), tab);
            break;
        case AST_TacCopyToOffset_t:
            print_field(++tab, "TacCopyToOffset: ");
            print_field(
                tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<TacCopyToOffset*>(node)->dst_name));
            print_field(tab + 1, "TLong: %zi", (ssize_t) static_cast<TacCopyToOffset*>(node)->offset);
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacCopyToOffset*>(node)->src.get()), tab);
            break;
        case AST_TacCopyFromOffset_t:
            print_field(++tab, "TacCopyFromOffset: ");
            print_field(
                tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<TacCopyFromOffset*>(node)->src_name));
            print_field(tab + 1, "TLong: %zi", (ssize_t) static_cast<TacCopyFromOffset*>(node)->offset);
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacCopyFromOffset*>(node)->dst.get()), tab);
            break;
        case AST_TacJump_t:
            print_field(++tab, "TacJump: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<TacJump*>(node)->target));
            break;
        case AST_TacJumpIfZero_t:
            print_field(++tab, "TacJumpIfZero: ");
            print_field(
                tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<TacJumpIfZero*>(node)->target));
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacJumpIfZero*>(node)->condition.get()), tab);
            break;
        case AST_TacJumpIfNotZero_t:
            print_field(++tab, "TacJumpIfNotZero: ");
            print_field(
                tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<TacJumpIfNotZero*>(node)->target));
            print_TacValue(ctx, static_cast<TacValue*>(static_cast<TacJumpIfNotZero*>(node)->condition.get()), tab);
            break;
        case AST_TacLabel_t:
            print_field(++tab, "TacLabel: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<TacLabel*>(node)->name));
            break;
        default:
            THROW_ABORT;
    }
}

static void print_TacTopLevel(Ctx ctx, TacTopLevel* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_TacTopLevel_t:
            print_field(++tab, "TacTopLevel: ");
            break;
        case AST_TacFunction_t:
            print_field(++tab, "TacFunction: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<TacFunction*>(node)->name));
            print_field(tab + 1, "Bool: %s", static_cast<TacFunction*>(node)->is_glob ? "True" : "False");
            print_field(tab + 1, "List[%zu]: ", vec_size(static_cast<TacFunction*>(node)->params));
            for (size_t i = 0; i < vec_size(static_cast<TacFunction*>(node)->params); ++i) {
                print_field(
                    tab + 2, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<TacFunction*>(node)->params[i]));
            }
            print_field(tab + 1, "List[%zu]: ", vec_size(static_cast<TacFunction*>(node)->body));
            for (size_t i = 0; i < vec_size(static_cast<TacFunction*>(node)->body); ++i) {
                print_TacInstruction(
                    ctx, static_cast<TacInstruction*>(static_cast<TacFunction*>(node)->body[i].get()), tab + 1);
            }
            break;
        case AST_TacStaticVariable_t:
            print_field(++tab, "TacStaticVariable: ");
            print_field(
                tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<TacStaticVariable*>(node)->name));
            print_field(tab + 1, "Bool: %s", static_cast<TacStaticVariable*>(node)->is_glob ? "True" : "False");
            print_Type(ctx, static_cast<Type*>(static_cast<TacStaticVariable*>(node)->static_init_type.get()), tab);
            print_field(tab + 1, "List[%zu]: ", vec_size(static_cast<TacStaticVariable*>(node)->static_inits));
            for (size_t i = 0; i < vec_size(static_cast<TacStaticVariable*>(node)->static_inits); ++i) {
                print_StaticInit(ctx,
                    static_cast<StaticInit*>(static_cast<TacStaticVariable*>(node)->static_inits[i].get()), tab + 1);
            }
            break;
        case AST_TacStaticConstant_t:
            print_field(++tab, "TacStaticConstant: ");
            print_field(
                tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<TacStaticConstant*>(node)->name));
            print_Type(ctx, static_cast<Type*>(static_cast<TacStaticConstant*>(node)->static_init_type.get()), tab);
            print_StaticInit(
                ctx, static_cast<StaticInit*>(static_cast<TacStaticConstant*>(node)->static_init.get()), tab);
            break;
        default:
            THROW_ABORT;
    }
}

static void print_TacProgram(Ctx ctx, TacProgram* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_TacProgram_t:
            print_field(++tab, "TacProgram: ");
            break;
        default:
            THROW_ABORT;
    }
    print_field(tab + 1, "List[%zu]: ", vec_size(node->static_const_toplvls));
    for (size_t i = 0; i < vec_size(node->static_const_toplvls); ++i) {
        print_TacTopLevel(ctx, static_cast<TacTopLevel*>(node->static_const_toplvls[i].get()), tab + 1);
    }
    print_field(tab + 1, "List[%zu]: ", vec_size(node->static_var_toplvls));
    for (size_t i = 0; i < vec_size(node->static_var_toplvls); ++i) {
        print_TacTopLevel(ctx, static_cast<TacTopLevel*>(node->static_var_toplvls[i].get()), tab + 1);
    }
    print_field(tab + 1, "List[%zu]: ", vec_size(node->fun_toplvls));
    for (size_t i = 0; i < vec_size(node->fun_toplvls); ++i) {
        print_TacTopLevel(ctx, static_cast<TacTopLevel*>(node->fun_toplvls[i].get()), tab + 1);
    }
}

static void print_AssemblyType(AssemblyType* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_AssemblyType_t:
            print_field(++tab, "AssemblyType: ");
            break;
        case AST_Byte_t:
            print_field(++tab, "Byte: ");
            break;
        case AST_LongWord_t:
            print_field(++tab, "LongWord: ");
            break;
        case AST_QuadWord_t:
            print_field(++tab, "QuadWord: ");
            break;
        case AST_BackendDouble_t:
            print_field(++tab, "BackendDouble: ");
            break;
        case AST_ByteArray_t:
            print_field(++tab, "ByteArray: ");
            print_field(tab + 1, "TLong: %zi", (ssize_t) static_cast<ByteArray*>(node)->size);
            print_field(tab + 1, "TInt: %i", static_cast<ByteArray*>(node)->alignment);
            break;
        default:
            THROW_ABORT;
    }
}

static void print_BackendSymbol(Ctx ctx, BackendSymbol* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_BackendSymbol_t:
            print_field(++tab, "BackendSymbol: ");
            break;
        case AST_BackendObj_t:
            print_field(++tab, "BackendObj: ");
            print_field(tab + 1, "Bool: %s", static_cast<BackendObj*>(node)->is_static ? "True" : "False");
            print_field(tab + 1, "Bool: %s", static_cast<BackendObj*>(node)->is_const ? "True" : "False");
            print_AssemblyType(static_cast<AssemblyType*>(static_cast<BackendObj*>(node)->asm_type.get()), tab);
            break;
        case AST_BackendFun_t:
            print_field(++tab, "BackendFun: ");
            print_field(tab + 1, "Bool: %s", static_cast<BackendFun*>(node)->is_def ? "True" : "False");
            print_field(tab + 1, "List[%zu]: ", vec_size(static_cast<BackendFun*>(node)->callee_saved_regs));
            for (size_t i = 0; i < vec_size(static_cast<BackendFun*>(node)->callee_saved_regs); ++i) {
                print_AsmOperand(
                    ctx, static_cast<AsmOperand*>(static_cast<BackendFun*>(node)->callee_saved_regs[i].get()), tab + 1);
            }
            break;
        default:
            THROW_ABORT;
    }
}

static void print_AsmReg(AsmReg* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_AsmReg_t:
            print_field(++tab, "AsmReg: ");
            break;
        case AST_AsmAx_t:
            print_field(++tab, "AsmAx: ");
            break;
        case AST_AsmBx_t:
            print_field(++tab, "AsmBx: ");
            break;
        case AST_AsmCx_t:
            print_field(++tab, "AsmCx: ");
            break;
        case AST_AsmDx_t:
            print_field(++tab, "AsmDx: ");
            break;
        case AST_AsmDi_t:
            print_field(++tab, "AsmDi: ");
            break;
        case AST_AsmSi_t:
            print_field(++tab, "AsmSi: ");
            break;
        case AST_AsmR8_t:
            print_field(++tab, "AsmR8: ");
            break;
        case AST_AsmR9_t:
            print_field(++tab, "AsmR9: ");
            break;
        case AST_AsmR10_t:
            print_field(++tab, "AsmR10: ");
            break;
        case AST_AsmR11_t:
            print_field(++tab, "AsmR11: ");
            break;
        case AST_AsmR12_t:
            print_field(++tab, "AsmR12: ");
            break;
        case AST_AsmR13_t:
            print_field(++tab, "AsmR13: ");
            break;
        case AST_AsmR14_t:
            print_field(++tab, "AsmR14: ");
            break;
        case AST_AsmR15_t:
            print_field(++tab, "AsmR15: ");
            break;
        case AST_AsmSp_t:
            print_field(++tab, "AsmSp: ");
            break;
        case AST_AsmBp_t:
            print_field(++tab, "AsmBp: ");
            break;
        case AST_AsmXMM0_t:
            print_field(++tab, "AsmXMM0: ");
            break;
        case AST_AsmXMM1_t:
            print_field(++tab, "AsmXMM1: ");
            break;
        case AST_AsmXMM2_t:
            print_field(++tab, "AsmXMM2: ");
            break;
        case AST_AsmXMM3_t:
            print_field(++tab, "AsmXMM3: ");
            break;
        case AST_AsmXMM4_t:
            print_field(++tab, "AsmXMM4: ");
            break;
        case AST_AsmXMM5_t:
            print_field(++tab, "AsmXMM5: ");
            break;
        case AST_AsmXMM6_t:
            print_field(++tab, "AsmXMM6: ");
            break;
        case AST_AsmXMM7_t:
            print_field(++tab, "AsmXMM7: ");
            break;
        case AST_AsmXMM8_t:
            print_field(++tab, "AsmXMM8: ");
            break;
        case AST_AsmXMM9_t:
            print_field(++tab, "AsmXMM9: ");
            break;
        case AST_AsmXMM10_t:
            print_field(++tab, "AsmXMM10: ");
            break;
        case AST_AsmXMM11_t:
            print_field(++tab, "AsmXMM11: ");
            break;
        case AST_AsmXMM12_t:
            print_field(++tab, "AsmXMM12: ");
            break;
        case AST_AsmXMM13_t:
            print_field(++tab, "AsmXMM13: ");
            break;
        case AST_AsmXMM14_t:
            print_field(++tab, "AsmXMM14: ");
            break;
        case AST_AsmXMM15_t:
            print_field(++tab, "AsmXMM15: ");
            break;
        default:
            THROW_ABORT;
    }
}

static void print_AsmCondCode(AsmCondCode* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_AsmCondCode_t:
            print_field(++tab, "AsmCondCode: ");
            break;
        case AST_AsmE_t:
            print_field(++tab, "AsmE: ");
            break;
        case AST_AsmNE_t:
            print_field(++tab, "AsmNE: ");
            break;
        case AST_AsmG_t:
            print_field(++tab, "AsmG: ");
            break;
        case AST_AsmGE_t:
            print_field(++tab, "AsmGE: ");
            break;
        case AST_AsmL_t:
            print_field(++tab, "AsmL: ");
            break;
        case AST_AsmLE_t:
            print_field(++tab, "AsmLE: ");
            break;
        case AST_AsmA_t:
            print_field(++tab, "AsmA: ");
            break;
        case AST_AsmAE_t:
            print_field(++tab, "AsmAE: ");
            break;
        case AST_AsmB_t:
            print_field(++tab, "AsmB: ");
            break;
        case AST_AsmBE_t:
            print_field(++tab, "AsmBE: ");
            break;
        case AST_AsmP_t:
            print_field(++tab, "AsmP: ");
            break;
        default:
            THROW_ABORT;
    }
}

static void print_AsmOperand(Ctx ctx, AsmOperand* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_AsmOperand_t:
            print_field(++tab, "AsmOperand: ");
            break;
        case AST_AsmImm_t:
            print_field(++tab, "AsmImm: ");
            print_field(tab + 1, "TULong: %zu", (size_t) static_cast<AsmImm*>(node)->value);
            print_field(tab + 1, "Bool: %s", static_cast<AsmImm*>(node)->is_byte ? "True" : "False");
            print_field(tab + 1, "Bool: %s", static_cast<AsmImm*>(node)->is_quad ? "True" : "False");
            print_field(tab + 1, "Bool: %s", static_cast<AsmImm*>(node)->is_neg ? "True" : "False");
            break;
        case AST_AsmRegister_t:
            print_field(++tab, "AsmRegister: ");
            print_AsmReg(static_cast<AsmReg*>(static_cast<AsmRegister*>(node)->reg.get()), tab);
            break;
        case AST_AsmPseudo_t:
            print_field(++tab, "AsmPseudo: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<AsmPseudo*>(node)->name));
            break;
        case AST_AsmMemory_t:
            print_field(++tab, "AsmMemory: ");
            print_field(tab + 1, "TLong: %zi", (ssize_t) static_cast<AsmMemory*>(node)->value);
            print_AsmReg(static_cast<AsmReg*>(static_cast<AsmMemory*>(node)->reg.get()), tab);
            break;
        case AST_AsmData_t:
            print_field(++tab, "AsmData: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<AsmData*>(node)->name));
            print_field(tab + 1, "TLong: %zi", (ssize_t) static_cast<AsmData*>(node)->offset);
            break;
        case AST_AsmPseudoMem_t:
            print_field(++tab, "AsmPseudoMem: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<AsmPseudoMem*>(node)->name));
            print_field(tab + 1, "TLong: %zi", (ssize_t) static_cast<AsmPseudoMem*>(node)->offset);
            break;
        case AST_AsmIndexed_t:
            print_field(++tab, "AsmIndexed: ");
            print_field(tab + 1, "TLong: %zi", (ssize_t) static_cast<AsmIndexed*>(node)->scale);
            print_AsmReg(static_cast<AsmReg*>(static_cast<AsmIndexed*>(node)->reg_base.get()), tab);
            print_AsmReg(static_cast<AsmReg*>(static_cast<AsmIndexed*>(node)->reg_index.get()), tab);
            break;
        default:
            THROW_ABORT;
    }
}

static void print_AsmBinaryOp(AsmBinaryOp* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_AsmBinaryOp_t:
            print_field(++tab, "AsmBinaryOp: ");
            break;
        case AST_AsmAdd_t:
            print_field(++tab, "AsmAdd: ");
            break;
        case AST_AsmSub_t:
            print_field(++tab, "AsmSub: ");
            break;
        case AST_AsmMult_t:
            print_field(++tab, "AsmMult: ");
            break;
        case AST_AsmDivDouble_t:
            print_field(++tab, "AsmDivDouble: ");
            break;
        case AST_AsmBitAnd_t:
            print_field(++tab, "AsmBitAnd: ");
            break;
        case AST_AsmBitOr_t:
            print_field(++tab, "AsmBitOr: ");
            break;
        case AST_AsmBitXor_t:
            print_field(++tab, "AsmBitXor: ");
            break;
        case AST_AsmBitShiftLeft_t:
            print_field(++tab, "AsmBitShiftLeft: ");
            break;
        case AST_AsmBitShiftRight_t:
            print_field(++tab, "AsmBitShiftRight: ");
            break;
        case AST_AsmBitShrArithmetic_t:
            print_field(++tab, "AsmBitShrArithmetic: ");
            break;
        default:
            THROW_ABORT;
    }
}

static void print_AsmUnaryOp(AsmUnaryOp* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_AsmUnaryOp_t:
            print_field(++tab, "AsmUnaryOp: ");
            break;
        case AST_AsmNot_t:
            print_field(++tab, "AsmNot: ");
            break;
        case AST_AsmNeg_t:
            print_field(++tab, "AsmNeg: ");
            break;
        case AST_AsmShr_t:
            print_field(++tab, "AsmShr: ");
            break;
        default:
            THROW_ABORT;
    }
}

static void print_AsmInstruction(Ctx ctx, AsmInstruction* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_AsmInstruction_t:
            print_field(++tab, "AsmInstruction: ");
            break;
        case AST_AsmMov_t:
            print_field(++tab, "AsmMov: ");
            print_AssemblyType(static_cast<AssemblyType*>(static_cast<AsmMov*>(node)->asm_type.get()), tab);
            print_AsmOperand(ctx, static_cast<AsmOperand*>(static_cast<AsmMov*>(node)->src.get()), tab);
            print_AsmOperand(ctx, static_cast<AsmOperand*>(static_cast<AsmMov*>(node)->dst.get()), tab);
            break;
        case AST_AsmMovSx_t:
            print_field(++tab, "AsmMovSx: ");
            print_AssemblyType(static_cast<AssemblyType*>(static_cast<AsmMovSx*>(node)->asm_type_src.get()), tab);
            print_AssemblyType(static_cast<AssemblyType*>(static_cast<AsmMovSx*>(node)->asm_type_dst.get()), tab);
            print_AsmOperand(ctx, static_cast<AsmOperand*>(static_cast<AsmMovSx*>(node)->src.get()), tab);
            print_AsmOperand(ctx, static_cast<AsmOperand*>(static_cast<AsmMovSx*>(node)->dst.get()), tab);
            break;
        case AST_AsmMovZeroExtend_t:
            print_field(++tab, "AsmMovZeroExtend: ");
            print_AssemblyType(
                static_cast<AssemblyType*>(static_cast<AsmMovZeroExtend*>(node)->asm_type_src.get()), tab);
            print_AssemblyType(
                static_cast<AssemblyType*>(static_cast<AsmMovZeroExtend*>(node)->asm_type_dst.get()), tab);
            print_AsmOperand(ctx, static_cast<AsmOperand*>(static_cast<AsmMovZeroExtend*>(node)->src.get()), tab);
            print_AsmOperand(ctx, static_cast<AsmOperand*>(static_cast<AsmMovZeroExtend*>(node)->dst.get()), tab);
            break;
        case AST_AsmLea_t:
            print_field(++tab, "AsmLea: ");
            print_AsmOperand(ctx, static_cast<AsmOperand*>(static_cast<AsmLea*>(node)->src.get()), tab);
            print_AsmOperand(ctx, static_cast<AsmOperand*>(static_cast<AsmLea*>(node)->dst.get()), tab);
            break;
        case AST_AsmCvttsd2si_t:
            print_field(++tab, "AsmCvttsd2si: ");
            print_AssemblyType(static_cast<AssemblyType*>(static_cast<AsmCvttsd2si*>(node)->asm_type.get()), tab);
            print_AsmOperand(ctx, static_cast<AsmOperand*>(static_cast<AsmCvttsd2si*>(node)->src.get()), tab);
            print_AsmOperand(ctx, static_cast<AsmOperand*>(static_cast<AsmCvttsd2si*>(node)->dst.get()), tab);
            break;
        case AST_AsmCvtsi2sd_t:
            print_field(++tab, "AsmCvtsi2sd: ");
            print_AssemblyType(static_cast<AssemblyType*>(static_cast<AsmCvtsi2sd*>(node)->asm_type.get()), tab);
            print_AsmOperand(ctx, static_cast<AsmOperand*>(static_cast<AsmCvtsi2sd*>(node)->src.get()), tab);
            print_AsmOperand(ctx, static_cast<AsmOperand*>(static_cast<AsmCvtsi2sd*>(node)->dst.get()), tab);
            break;
        case AST_AsmUnary_t:
            print_field(++tab, "AsmUnary: ");
            print_AsmUnaryOp(static_cast<AsmUnaryOp*>(static_cast<AsmUnary*>(node)->unop.get()), tab);
            print_AssemblyType(static_cast<AssemblyType*>(static_cast<AsmUnary*>(node)->asm_type.get()), tab);
            print_AsmOperand(ctx, static_cast<AsmOperand*>(static_cast<AsmUnary*>(node)->dst.get()), tab);
            break;
        case AST_AsmBinary_t:
            print_field(++tab, "AsmBinary: ");
            print_AsmBinaryOp(static_cast<AsmBinaryOp*>(static_cast<AsmBinary*>(node)->binop.get()), tab);
            print_AssemblyType(static_cast<AssemblyType*>(static_cast<AsmBinary*>(node)->asm_type.get()), tab);
            print_AsmOperand(ctx, static_cast<AsmOperand*>(static_cast<AsmBinary*>(node)->src.get()), tab);
            print_AsmOperand(ctx, static_cast<AsmOperand*>(static_cast<AsmBinary*>(node)->dst.get()), tab);
            break;
        case AST_AsmCmp_t:
            print_field(++tab, "AsmCmp: ");
            print_AssemblyType(static_cast<AssemblyType*>(static_cast<AsmCmp*>(node)->asm_type.get()), tab);
            print_AsmOperand(ctx, static_cast<AsmOperand*>(static_cast<AsmCmp*>(node)->src.get()), tab);
            print_AsmOperand(ctx, static_cast<AsmOperand*>(static_cast<AsmCmp*>(node)->dst.get()), tab);
            break;
        case AST_AsmIdiv_t:
            print_field(++tab, "AsmIdiv: ");
            print_AssemblyType(static_cast<AssemblyType*>(static_cast<AsmIdiv*>(node)->asm_type.get()), tab);
            print_AsmOperand(ctx, static_cast<AsmOperand*>(static_cast<AsmIdiv*>(node)->src.get()), tab);
            break;
        case AST_AsmDiv_t:
            print_field(++tab, "AsmDiv: ");
            print_AssemblyType(static_cast<AssemblyType*>(static_cast<AsmDiv*>(node)->asm_type.get()), tab);
            print_AsmOperand(ctx, static_cast<AsmOperand*>(static_cast<AsmDiv*>(node)->src.get()), tab);
            break;
        case AST_AsmCdq_t:
            print_field(++tab, "AsmCdq: ");
            print_AssemblyType(static_cast<AssemblyType*>(static_cast<AsmCdq*>(node)->asm_type.get()), tab);
            break;
        case AST_AsmJmp_t:
            print_field(++tab, "AsmJmp: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<AsmJmp*>(node)->target));
            break;
        case AST_AsmJmpCC_t:
            print_field(++tab, "AsmJmpCC: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<AsmJmpCC*>(node)->target));
            print_AsmCondCode(static_cast<AsmCondCode*>(static_cast<AsmJmpCC*>(node)->cond_code.get()), tab);
            break;
        case AST_AsmSetCC_t:
            print_field(++tab, "AsmSetCC: ");
            print_AsmCondCode(static_cast<AsmCondCode*>(static_cast<AsmSetCC*>(node)->cond_code.get()), tab);
            print_AsmOperand(ctx, static_cast<AsmOperand*>(static_cast<AsmSetCC*>(node)->dst.get()), tab);
            break;
        case AST_AsmLabel_t:
            print_field(++tab, "AsmLabel: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<AsmLabel*>(node)->name));
            break;
        case AST_AsmPush_t:
            print_field(++tab, "AsmPush: ");
            print_AsmOperand(ctx, static_cast<AsmOperand*>(static_cast<AsmPush*>(node)->src.get()), tab);
            break;
        case AST_AsmPop_t:
            print_field(++tab, "AsmPop: ");
            print_AsmReg(static_cast<AsmReg*>(static_cast<AsmPop*>(node)->reg.get()), tab);
            break;
        case AST_AsmCall_t:
            print_field(++tab, "AsmCall: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<AsmCall*>(node)->name));
            break;
        case AST_AsmRet_t:
            print_field(++tab, "AsmRet: ");
            break;
        default:
            THROW_ABORT;
    }
}

static void print_AsmTopLevel(Ctx ctx, AsmTopLevel* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_AsmTopLevel_t:
            print_field(++tab, "AsmTopLevel: ");
            break;
        case AST_AsmFunction_t:
            print_field(++tab, "AsmFunction: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<AsmFunction*>(node)->name));
            print_field(tab + 1, "Bool: %s", static_cast<AsmFunction*>(node)->is_glob ? "True" : "False");
            print_field(tab + 1, "Bool: %s", static_cast<AsmFunction*>(node)->is_ret_memory ? "True" : "False");
            print_field(tab + 1, "List[%zu]: ", vec_size(static_cast<AsmFunction*>(node)->instructions));
            for (size_t i = 0; i < vec_size(static_cast<AsmFunction*>(node)->instructions); ++i) {
                print_AsmInstruction(
                    ctx, static_cast<AsmInstruction*>(static_cast<AsmFunction*>(node)->instructions[i].get()), tab + 1);
            }
            break;
        case AST_AsmStaticVariable_t:
            print_field(++tab, "AsmStaticVariable: ");
            print_field(
                tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<AsmStaticVariable*>(node)->name));
            print_field(tab + 1, "TInt: %i", static_cast<AsmStaticVariable*>(node)->alignment);
            print_field(tab + 1, "Bool: %s", static_cast<AsmStaticVariable*>(node)->is_glob ? "True" : "False");
            print_field(tab + 1, "List[%zu]: ", vec_size(static_cast<AsmStaticVariable*>(node)->static_inits));
            for (size_t i = 0; i < vec_size(static_cast<AsmStaticVariable*>(node)->static_inits); ++i) {
                print_StaticInit(ctx,
                    static_cast<StaticInit*>(static_cast<AsmStaticVariable*>(node)->static_inits[i].get()), tab + 1);
            }
            break;
        case AST_AsmStaticConstant_t:
            print_field(++tab, "AsmStaticConstant: ");
            print_field(
                tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, static_cast<AsmStaticConstant*>(node)->name));
            print_field(tab + 1, "TInt: %i", static_cast<AsmStaticConstant*>(node)->alignment);
            print_StaticInit(
                ctx, static_cast<StaticInit*>(static_cast<AsmStaticConstant*>(node)->static_init.get()), tab);
            break;
        default:
            THROW_ABORT;
    }
}

static void print_AsmProgram(Ctx ctx, AsmProgram* node, size_t tab) {
    assert_print(node, ++tab);
    switch (node->type()) {
        case AST_AsmProgram_t:
            print_field(++tab, "AsmProgram: ");
            break;
        default:
            THROW_ABORT;
    }
    print_field(tab + 1, "List[%zu]: ", vec_size(node->static_const_toplvls));
    for (size_t i = 0; i < vec_size(node->static_const_toplvls); ++i) {
        print_AsmTopLevel(ctx, static_cast<AsmTopLevel*>(node->static_const_toplvls[i].get()), tab + 1);
    }
    print_field(tab + 1, "List[%zu]: ", vec_size(node->top_levels));
    for (size_t i = 0; i < vec_size(node->top_levels); ++i) {
        print_AsmTopLevel(ctx, static_cast<AsmTopLevel*>(node->top_levels[i].get()), tab + 1);
    }
}

// static void print_field(const std::string& name, const std::string& value, size_t t) {
//     std::cout << "\n";
//     for (size_t i = 0; i < t - 1; ++i) {
//         std::cout << "  ";
//     }
//     std::cout << std::string(name) << ": ";
//     if (!value.empty()) {
//         std::cout << value;
//     }
// }

// static void print_ast(IdentifierContext* /*ctx*/, Ast* node, size_t t) {
//     if (!node) {
//         print_field("None", "", ++t);
//         return;
//     }
//     switch (node->type()) {
//         case AST_CConst_t: {
//             print_field("CConst", "", ++t);
//             break;
//         }
//         case AST_CConstInt_t: {
//             print_field("CConstInt", "", ++t);
//             CConstInt* p_node = static_cast<CConstInt*>(node);
//             print_field("TInt", std::to_string(p_node->value), t + 1);
//             break;
//         }
//         case AST_CConstLong_t: {
//             print_field("CConstLong", "", ++t);
//             CConstLong* p_node = static_cast<CConstLong*>(node);
//             print_field("TLong", std::to_string(p_node->value), t + 1);
//             break;
//         }
//         case AST_CConstUInt_t: {
//             print_field("CConstUInt", "", ++t);
//             CConstUInt* p_node = static_cast<CConstUInt*>(node);
//             print_field("TUInt", std::to_string(p_node->value), t + 1);
//             break;
//         }
//         case AST_CConstULong_t: {
//             print_field("CConstULong", "", ++t);
//             CConstULong* p_node = static_cast<CConstULong*>(node);
//             print_field("TULong", std::to_string(p_node->value), t + 1);
//             break;
//         }
//         case AST_CConstDouble_t: {
//             print_field("CConstDouble", "", ++t);
//             CConstDouble* p_node = static_cast<CConstDouble*>(node);
//             print_field("TDouble", std::to_string(p_node->value), t + 1);
//             break;
//         }
//         case AST_CConstChar_t: {
//             print_field("CConstChar", "", ++t);
//             CConstChar* p_node = static_cast<CConstChar*>(node);
//             print_field("TChar", std::to_string(p_node->value), t + 1);
//             break;
//         }
//         case AST_CConstUChar_t: {
//             print_field("CConstUChar", "", ++t);
//             CConstUChar* p_node = static_cast<CConstUChar*>(node);
//             print_field("TUChar", std::to_string(p_node->value), t + 1);
//             break;
//         }
//         case AST_CStringLiteral_t: {
//             print_field("CStringLiteral", "", ++t);
//             CStringLiteral* p_node = static_cast<CStringLiteral*>(node);
//             print_field("List[" + std::to_string(vec_size(p_node->value)) + "]", "", t + 1);
//             for (size_t i = 0; i < vec_size(p_node->value); ++i) {
//                 print_field("TChar", std::to_string(p_node->value[i]), t + 2);
//             }
//             break;
//         }
//         case AST_Type_t: {
//             print_field("Type", "", ++t);
//             break;
//         }
//         case AST_Char_t: {
//             print_field("Char", "", ++t);
//             break;
//         }
//         case AST_SChar_t: {
//             print_field("SChar", "", ++t);
//             break;
//         }
//         case AST_UChar_t: {
//             print_field("UChar", "", ++t);
//             break;
//         }
//         case AST_Int_t: {
//             print_field("Int", "", ++t);
//             break;
//         }
//         case AST_Long_t: {
//             print_field("Long", "", ++t);
//             break;
//         }
//         case AST_UInt_t: {
//             print_field("UInt", "", ++t);
//             break;
//         }
//         case AST_ULong_t: {
//             print_field("ULong", "", ++t);
//             break;
//         }
//         case AST_Double_t: {
//             print_field("Double", "", ++t);
//             break;
//         }
//         case AST_Void_t: {
//             print_field("Void", "", ++t);
//             break;
//         }
//         case AST_FunType_t: {
//             print_field("FunType", "", ++t);
//             FunType* p_node = static_cast<FunType*>(node);
//             print_field("TULong", std::to_string(p_node->param_reg_mask), t + 1);
//             print_field("TULong", std::to_string(p_node->ret_reg_mask), t + 1);
//             print_field("List[" + std::to_string(vec_size(p_node->param_types)) + "]", "", t + 1);
//             for (size_t i = 0; i < vec_size(p_node->param_types); ++i) {
//                 print_ast(ctx, p_node->param_types[i].get(), t + 1);
//             }
//             print_ast(ctx, p_node->ret_type.get(), t);
//             break;
//         }
//         case AST_Pointer_t: {
//             print_field("Pointer", "", ++t);
//             Pointer* p_node = static_cast<Pointer*>(node);
//             print_ast(ctx, p_node->ref_type.get(), t);
//             break;
//         }
//         case AST_Array_t: {
//             print_field("Array", "", ++t);
//             Array* p_node = static_cast<Array*>(node);
//             print_field("TLong", std::to_string(p_node->size), t + 1);
//             print_ast(ctx, p_node->elem_type.get(), t);
//             break;
//         }
//         case AST_Structure_t: {
//             print_field("Structure", "", ++t);
//             Structure* p_node = static_cast<Structure*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->tag)), t + 1);
//             print_field("Bool", std::to_string(p_node->is_union), t + 1);
//             break;
//         }
//         case AST_StaticInit_t: {
//             print_field("StaticInit", "", ++t);
//             break;
//         }
//         case AST_IntInit_t: {
//             print_field("IntInit", "", ++t);
//             IntInit* p_node = static_cast<IntInit*>(node);
//             print_field("TInt", std::to_string(p_node->value), t + 1);
//             break;
//         }
//         case AST_LongInit_t: {
//             print_field("LongInit", "", ++t);
//             LongInit* p_node = static_cast<LongInit*>(node);
//             print_field("TLong", std::to_string(p_node->value), t + 1);
//             break;
//         }
//         case AST_UIntInit_t: {
//             print_field("UIntInit", "", ++t);
//             UIntInit* p_node = static_cast<UIntInit*>(node);
//             print_field("TUInt", std::to_string(p_node->value), t + 1);
//             break;
//         }
//         case AST_ULongInit_t: {
//             print_field("ULongInit", "", ++t);
//             ULongInit* p_node = static_cast<ULongInit*>(node);
//             print_field("TULong", std::to_string(p_node->value), t + 1);
//             break;
//         }
//         case AST_CharInit_t: {
//             print_field("CharInit", "", ++t);
//             CharInit* p_node = static_cast<CharInit*>(node);
//             print_field("TChar", std::to_string(p_node->value), t + 1);
//             break;
//         }
//         case AST_UCharInit_t: {
//             print_field("UCharInit", "", ++t);
//             UCharInit* p_node = static_cast<UCharInit*>(node);
//             print_field("TUChar", std::to_string(p_node->value), t + 1);
//             break;
//         }
//         case AST_DoubleInit_t: {
//             print_field("DoubleInit", "", ++t);
//             DoubleInit* p_node = static_cast<DoubleInit*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->dbl_const)), t + 1);
//             break;
//         }
//         case AST_ZeroInit_t: {
//             print_field("ZeroInit", "", ++t);
//             ZeroInit* p_node = static_cast<ZeroInit*>(node);
//             print_field("TLong", std::to_string(p_node->byte), t + 1);
//             break;
//         }
//         case AST_StringInit_t: {
//             print_field("StringInit", "", ++t);
//             StringInit* p_node = static_cast<StringInit*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->string_const)), t + 1);
//             print_field("Bool", std::to_string(p_node->is_null_term), t + 1);
//             print_ast(ctx, p_node->literal.get(), t);
//             break;
//         }
//         case AST_PointerInit_t: {
//             print_field("PointerInit", "", ++t);
//             PointerInit* p_node = static_cast<PointerInit*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->name)), t + 1);
//             break;
//         }
//         case AST_InitialValue_t: {
//             print_field("InitialValue", "", ++t);
//             break;
//         }
//         case AST_Tentative_t: {
//             print_field("Tentative", "", ++t);
//             break;
//         }
//         case AST_Initial_t: {
//             print_field("Initial", "", ++t);
//             Initial* p_node = static_cast<Initial*>(node);
//             print_field("List[" + std::to_string(vec_size(p_node->static_inits)) + "]", "", t + 1);
//             for (size_t i = 0; i < vec_size(p_node->static_inits); ++i) {
//                 print_ast(ctx, p_node->static_inits[i].get(), t + 1);
//             }
//             break;
//         }
//         case AST_NoInitializer_t: {
//             print_field("NoInitializer", "", ++t);
//             break;
//         }
//         case AST_IdentifierAttr_t: {
//             print_field("IdentifierAttr", "", ++t);
//             break;
//         }
//         case AST_FunAttr_t: {
//             print_field("FunAttr", "", ++t);
//             FunAttr* p_node = static_cast<FunAttr*>(node);
//             print_field("Bool", std::to_string(p_node->is_def), t + 1);
//             print_field("Bool", std::to_string(p_node->is_glob), t + 1);
//             break;
//         }
//         case AST_StaticAttr_t: {
//             print_field("StaticAttr", "", ++t);
//             StaticAttr* p_node = static_cast<StaticAttr*>(node);
//             print_field("Bool", std::to_string(p_node->is_glob), t + 1);
//             print_ast(ctx, p_node->init.get(), t);
//             break;
//         }
//         case AST_ConstantAttr_t: {
//             print_field("ConstantAttr", "", ++t);
//             ConstantAttr* p_node = static_cast<ConstantAttr*>(node);
//             print_ast(ctx, p_node->static_init.get(), t);
//             break;
//         }
//         case AST_LocalAttr_t: {
//             print_field("LocalAttr", "", ++t);
//             break;
//         }
//         case AST_Symbol_t: {
//             print_field("Symbol", "", ++t);
//             Symbol* p_node = static_cast<Symbol*>(node);
//             print_ast(ctx, p_node->type_t.get(), t);
//             print_ast(ctx, p_node->attrs.get(), t);
//             break;
//         }
//         case AST_StructMember_t: {
//             print_field("StructMember", "", ++t);
//             StructMember* p_node = static_cast<StructMember*>(node);
//             print_field("TLong", std::to_string(p_node->offset), t + 1);
//             print_ast(ctx, p_node->member_type.get(), t);
//             break;
//         }
//         case AST_StructTypedef_t: {
//             print_field("StructTypedef", "", ++t);
//             StructTypedef* p_node = static_cast<StructTypedef*>(node);
//             print_field("TInt", std::to_string(p_node->alignment), t + 1);
//             print_field("TLong", std::to_string(p_node->size), t + 1);
//             print_field("List[" + std::to_string(vec_size(p_node->member_names)) + "]", "", t + 1);
//             for (size_t i = 0; i < vec_size(p_node->member_names); ++i) {
//                 print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->member_names[i])), t + 2);
//             }
//             print_field("Dict[" + std::to_string(map_size(p_node->members)) + "]", "", t + 1);
//             for (size_t i = 0; i < map_size(p_node->members); ++i) {
//                 print_field(
//                     "[" + std::string(map_get(ctx->hash_table, pair_first(p_node->members[i]))) + "]", "", t + 2);
//                 print_ast(ctx, pair_second(p_node->members[i]).get(), t + 2);
//             }
//             break;
//         }
//         case AST_AssemblyType_t: {
//             print_field("AssemblyType", "", ++t);
//             break;
//         }
//         case AST_Byte_t: {
//             print_field("Byte", "", ++t);
//             break;
//         }
//         case AST_LongWord_t: {
//             print_field("LongWord", "", ++t);
//             break;
//         }
//         case AST_QuadWord_t: {
//             print_field("QuadWord", "", ++t);
//             break;
//         }
//         case AST_BackendDouble_t: {
//             print_field("BackendDouble", "", ++t);
//             break;
//         }
//         case AST_ByteArray_t: {
//             print_field("ByteArray", "", ++t);
//             ByteArray* p_node = static_cast<ByteArray*>(node);
//             print_field("TLong", std::to_string(p_node->size), t + 1);
//             print_field("TInt", std::to_string(p_node->alignment), t + 1);
//             break;
//         }
//         case AST_BackendSymbol_t: {
//             print_field("BackendSymbol", "", ++t);
//             break;
//         }
//         case AST_BackendObj_t: {
//             print_field("BackendObj", "", ++t);
//             BackendObj* p_node = static_cast<BackendObj*>(node);
//             print_field("Bool", std::to_string(p_node->is_static), t + 1);
//             print_field("Bool", std::to_string(p_node->is_const), t + 1);
//             print_ast(ctx, p_node->asm_type.get(), t);
//             break;
//         }
//         case AST_BackendFun_t: {
//             print_field("BackendFun", "", ++t);
//             BackendFun* p_node = static_cast<BackendFun*>(node);
//             print_field("Bool", std::to_string(p_node->is_def), t + 1);
//             print_field("List[" + std::to_string(vec_size(p_node->callee_saved_regs)) + "]", "", t + 1);
//             for (size_t i = 0; i < vec_size(p_node->callee_saved_regs); ++i) {
//                 print_ast(ctx, p_node->callee_saved_regs[i].get(), t + 1);
//             }
//             break;
//         }
//         case AST_CUnaryOp_t: {
//             print_field("CUnaryOp", "", ++t);
//             break;
//         }
//         case AST_CComplement_t: {
//             print_field("CComplement", "", ++t);
//             break;
//         }
//         case AST_CNegate_t: {
//             print_field("CNegate", "", ++t);
//             break;
//         }
//         case AST_CNot_t: {
//             print_field("CNot", "", ++t);
//             break;
//         }
//         case AST_CPrefix_t: {
//             print_field("CPrefix", "", ++t);
//             break;
//         }
//         case AST_CPostfix_t: {
//             print_field("CPostfix", "", ++t);
//             break;
//         }
//         case AST_CBinaryOp_t: {
//             print_field("CBinaryOp", "", ++t);
//             break;
//         }
//         case AST_CAdd_t: {
//             print_field("CAdd", "", ++t);
//             break;
//         }
//         case AST_CSubtract_t: {
//             print_field("CSubtract", "", ++t);
//             break;
//         }
//         case AST_CMultiply_t: {
//             print_field("CMultiply", "", ++t);
//             break;
//         }
//         case AST_CDivide_t: {
//             print_field("CDivide", "", ++t);
//             break;
//         }
//         case AST_CRemainder_t: {
//             print_field("CRemainder", "", ++t);
//             break;
//         }
//         case AST_CBitAnd_t: {
//             print_field("CBitAnd", "", ++t);
//             break;
//         }
//         case AST_CBitOr_t: {
//             print_field("CBitOr", "", ++t);
//             break;
//         }
//         case AST_CBitXor_t: {
//             print_field("CBitXor", "", ++t);
//             break;
//         }
//         case AST_CBitShiftLeft_t: {
//             print_field("CBitShiftLeft", "", ++t);
//             break;
//         }
//         case AST_CBitShiftRight_t: {
//             print_field("CBitShiftRight", "", ++t);
//             break;
//         }
//         case AST_CBitShrArithmetic_t: {
//             print_field("CBitShrArithmetic", "", ++t);
//             break;
//         }
//         case AST_CAnd_t: {
//             print_field("CAnd", "", ++t);
//             break;
//         }
//         case AST_COr_t: {
//             print_field("COr", "", ++t);
//             break;
//         }
//         case AST_CEqual_t: {
//             print_field("CEqual", "", ++t);
//             break;
//         }
//         case AST_CNotEqual_t: {
//             print_field("CNotEqual", "", ++t);
//             break;
//         }
//         case AST_CLessThan_t: {
//             print_field("CLessThan", "", ++t);
//             break;
//         }
//         case AST_CLessOrEqual_t: {
//             print_field("CLessOrEqual", "", ++t);
//             break;
//         }
//         case AST_CGreaterThan_t: {
//             print_field("CGreaterThan", "", ++t);
//             break;
//         }
//         case AST_CGreaterOrEqual_t: {
//             print_field("CGreaterOrEqual", "", ++t);
//             break;
//         }
//         case AST_CAbstractDeclarator_t: {
//             print_field("CAbstractDeclarator", "", ++t);
//             break;
//         }
//         case AST_CAbstractPointer_t: {
//             print_field("CAbstractPointer", "", ++t);
//             CAbstractPointer* p_node = static_cast<CAbstractPointer*>(node);
//             print_ast(ctx, p_node->abstract_decltor.get(), t);
//             break;
//         }
//         case AST_CAbstractArray_t: {
//             print_field("CAbstractArray", "", ++t);
//             CAbstractArray* p_node = static_cast<CAbstractArray*>(node);
//             print_field("TLong", std::to_string(p_node->size), t + 1);
//             print_ast(ctx, p_node->abstract_decltor.get(), t);
//             break;
//         }
//         case AST_CAbstractBase_t: {
//             print_field("CAbstractBase", "", ++t);
//             break;
//         }
//         case AST_CParam_t: {
//             print_field("CParam", "", ++t);
//             CParam* p_node = static_cast<CParam*>(node);
//             print_ast(ctx, p_node->decltor.get(), t);
//             print_ast(ctx, p_node->param_type.get(), t);
//             break;
//         }
//         case AST_CDeclarator_t: {
//             print_field("CDeclarator", "", ++t);
//             break;
//         }
//         case AST_CIdent_t: {
//             print_field("CIdent", "", ++t);
//             CIdent* p_node = static_cast<CIdent*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->name)), t + 1);
//             break;
//         }
//         case AST_CPointerDeclarator_t: {
//             print_field("CPointerDeclarator", "", ++t);
//             CPointerDeclarator* p_node = static_cast<CPointerDeclarator*>(node);
//             print_ast(ctx, p_node->decltor.get(), t);
//             break;
//         }
//         case AST_CArrayDeclarator_t: {
//             print_field("CArrayDeclarator", "", ++t);
//             CArrayDeclarator* p_node = static_cast<CArrayDeclarator*>(node);
//             print_field("TLong", std::to_string(p_node->size), t + 1);
//             print_ast(ctx, p_node->decltor.get(), t);
//             break;
//         }
//         case AST_CFunDeclarator_t: {
//             print_field("CFunDeclarator", "", ++t);
//             CFunDeclarator* p_node = static_cast<CFunDeclarator*>(node);
//             print_field("List[" + std::to_string(vec_size(p_node->param_list)) + "]", "", t + 1);
//             for (size_t i = 0; i < vec_size(p_node->param_list); ++i) {
//                 print_ast(ctx, p_node->param_list[i].get(), t + 1);
//             }
//             print_ast(ctx, p_node->decltor.get(), t);
//             break;
//         }
//         case AST_CExp_t: {
//             print_field("CExp", "", ++t);
//             CExp* p_node = static_cast<CExp*>(node);
//             print_ast(ctx, p_node->exp_type.get(), t);
//             break;
//         }
//         case AST_CConstant_t: {
//             print_field("CConstant", "", ++t);
//             CConstant* p_node = static_cast<CConstant*>(node);
//             print_ast(ctx, p_node->constant.get(), t);
//             print_ast(ctx, p_node->exp_type.get(), t);
//             break;
//         }
//         case AST_CString_t: {
//             print_field("CString", "", ++t);
//             CString* p_node = static_cast<CString*>(node);
//             print_ast(ctx, p_node->literal.get(), t);
//             print_ast(ctx, p_node->exp_type.get(), t);
//             break;
//         }
//         case AST_CVar_t: {
//             print_field("CVar", "", ++t);
//             CVar* p_node = static_cast<CVar*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->name)), t + 1);
//             print_ast(ctx, p_node->exp_type.get(), t);
//             break;
//         }
//         case AST_CCast_t: {
//             print_field("CCast", "", ++t);
//             CCast* p_node = static_cast<CCast*>(node);
//             print_ast(ctx, p_node->exp.get(), t);
//             print_ast(ctx, p_node->target_type.get(), t);
//             print_ast(ctx, p_node->exp_type.get(), t);
//             break;
//         }
//         case AST_CUnary_t: {
//             print_field("CUnary", "", ++t);
//             CUnary* p_node = static_cast<CUnary*>(node);
//             print_ast(ctx, p_node->unop.get(), t);
//             print_ast(ctx, p_node->exp.get(), t);
//             print_ast(ctx, p_node->exp_type.get(), t);
//             break;
//         }
//         case AST_CBinary_t: {
//             print_field("CBinary", "", ++t);
//             CBinary* p_node = static_cast<CBinary*>(node);
//             print_ast(ctx, p_node->binop.get(), t);
//             print_ast(ctx, p_node->exp_left.get(), t);
//             print_ast(ctx, p_node->exp_right.get(), t);
//             print_ast(ctx, p_node->exp_type.get(), t);
//             break;
//         }
//         case AST_CAssignment_t: {
//             print_field("CAssignment", "", ++t);
//             CAssignment* p_node = static_cast<CAssignment*>(node);
//             print_ast(ctx, p_node->unop.get(), t);
//             print_ast(ctx, p_node->exp_left.get(), t);
//             print_ast(ctx, p_node->exp_right.get(), t);
//             print_ast(ctx, p_node->exp_type.get(), t);
//             break;
//         }
//         case AST_CConditional_t: {
//             print_field("CConditional", "", ++t);
//             CConditional* p_node = static_cast<CConditional*>(node);
//             print_ast(ctx, p_node->condition.get(), t);
//             print_ast(ctx, p_node->exp_middle.get(), t);
//             print_ast(ctx, p_node->exp_right.get(), t);
//             print_ast(ctx, p_node->exp_type.get(), t);
//             break;
//         }
//         case AST_CFunctionCall_t: {
//             print_field("CFunctionCall", "", ++t);
//             CFunctionCall* p_node = static_cast<CFunctionCall*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->name)), t + 1);
//             print_field("List[" + std::to_string(vec_size(p_node->args)) + "]", "", t + 1);
//             for (size_t i = 0; i < vec_size(p_node->args); ++i) {
//                 print_ast(ctx, p_node->args[i].get(), t + 1);
//             }
//             print_ast(ctx, p_node->exp_type.get(), t);
//             break;
//         }
//         case AST_CDereference_t: {
//             print_field("CDereference", "", ++t);
//             CDereference* p_node = static_cast<CDereference*>(node);
//             print_ast(ctx, p_node->exp.get(), t);
//             print_ast(ctx, p_node->exp_type.get(), t);
//             break;
//         }
//         case AST_CAddrOf_t: {
//             print_field("CAddrOf", "", ++t);
//             CAddrOf* p_node = static_cast<CAddrOf*>(node);
//             print_ast(ctx, p_node->exp.get(), t);
//             print_ast(ctx, p_node->exp_type.get(), t);
//             break;
//         }
//         case AST_CSubscript_t: {
//             print_field("CSubscript", "", ++t);
//             CSubscript* p_node = static_cast<CSubscript*>(node);
//             print_ast(ctx, p_node->primary_exp.get(), t);
//             print_ast(ctx, p_node->subscript_exp.get(), t);
//             print_ast(ctx, p_node->exp_type.get(), t);
//             break;
//         }
//         case AST_CSizeOf_t: {
//             print_field("CSizeOf", "", ++t);
//             CSizeOf* p_node = static_cast<CSizeOf*>(node);
//             print_ast(ctx, p_node->exp.get(), t);
//             print_ast(ctx, p_node->exp_type.get(), t);
//             break;
//         }
//         case AST_CSizeOfT_t: {
//             print_field("CSizeOfT", "", ++t);
//             CSizeOfT* p_node = static_cast<CSizeOfT*>(node);
//             print_ast(ctx, p_node->target_type.get(), t);
//             print_ast(ctx, p_node->exp_type.get(), t);
//             break;
//         }
//         case AST_CDot_t: {
//             print_field("CDot", "", ++t);
//             CDot* p_node = static_cast<CDot*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->member)), t + 1);
//             print_ast(ctx, p_node->structure.get(), t);
//             print_ast(ctx, p_node->exp_type.get(), t);
//             break;
//         }
//         case AST_CArrow_t: {
//             print_field("CArrow", "", ++t);
//             CArrow* p_node = static_cast<CArrow*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->member)), t + 1);
//             print_ast(ctx, p_node->pointer.get(), t);
//             print_ast(ctx, p_node->exp_type.get(), t);
//             break;
//         }
//         case AST_CStatement_t: {
//             print_field("CStatement", "", ++t);
//             break;
//         }
//         case AST_CReturn_t: {
//             print_field("CReturn", "", ++t);
//             CReturn* p_node = static_cast<CReturn*>(node);
//             print_ast(ctx, p_node->exp.get(), t);
//             break;
//         }
//         case AST_CExpression_t: {
//             print_field("CExpression", "", ++t);
//             CExpression* p_node = static_cast<CExpression*>(node);
//             print_ast(ctx, p_node->exp.get(), t);
//             break;
//         }
//         case AST_CIf_t: {
//             print_field("CIf", "", ++t);
//             CIf* p_node = static_cast<CIf*>(node);
//             print_ast(ctx, p_node->condition.get(), t);
//             print_ast(ctx, p_node->then.get(), t);
//             print_ast(ctx, p_node->else_fi.get(), t);
//             break;
//         }
//         case AST_CGoto_t: {
//             print_field("CGoto", "", ++t);
//             CGoto* p_node = static_cast<CGoto*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->target)), t + 1);
//             break;
//         }
//         case AST_CLabel_t: {
//             print_field("CLabel", "", ++t);
//             CLabel* p_node = static_cast<CLabel*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->target)), t + 1);
//             print_ast(ctx, p_node->jump_to.get(), t);
//             break;
//         }
//         case AST_CCompound_t: {
//             print_field("CCompound", "", ++t);
//             CCompound* p_node = static_cast<CCompound*>(node);
//             print_ast(ctx, p_node->block.get(), t);
//             break;
//         }
//         case AST_CWhile_t: {
//             print_field("CWhile", "", ++t);
//             CWhile* p_node = static_cast<CWhile*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->target)), t + 1);
//             print_ast(ctx, p_node->condition.get(), t);
//             print_ast(ctx, p_node->body.get(), t);
//             break;
//         }
//         case AST_CDoWhile_t: {
//             print_field("CDoWhile", "", ++t);
//             CDoWhile* p_node = static_cast<CDoWhile*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->target)), t + 1);
//             print_ast(ctx, p_node->condition.get(), t);
//             print_ast(ctx, p_node->body.get(), t);
//             break;
//         }
//         case AST_CFor_t: {
//             print_field("CFor", "", ++t);
//             CFor* p_node = static_cast<CFor*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->target)), t + 1);
//             print_ast(ctx, p_node->init.get(), t);
//             print_ast(ctx, p_node->condition.get(), t);
//             print_ast(ctx, p_node->post.get(), t);
//             print_ast(ctx, p_node->body.get(), t);
//             break;
//         }
//         case AST_CSwitch_t: {
//             print_field("CSwitch", "", ++t);
//             CSwitch* p_node = static_cast<CSwitch*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->target)), t + 1);
//             print_field("Bool", std::to_string(p_node->is_default), t + 1);
//             print_ast(ctx, p_node->match.get(), t);
//             print_ast(ctx, p_node->body.get(), t);
//             print_field("List[" + std::to_string(vec_size(p_node->cases)) + "]", "", t + 1);
//             for (size_t i = 0; i < vec_size(p_node->cases); ++i) {
//                 print_ast(ctx, p_node->cases[i].get(), t + 1);
//             }
//             break;
//         }
//         case AST_CCase_t: {
//             print_field("CCase", "", ++t);
//             CCase* p_node = static_cast<CCase*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->target)), t + 1);
//             print_ast(ctx, p_node->value.get(), t);
//             print_ast(ctx, p_node->jump_to.get(), t);
//             break;
//         }
//         case AST_CDefault_t: {
//             print_field("CDefault", "", ++t);
//             CDefault* p_node = static_cast<CDefault*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->target)), t + 1);
//             print_ast(ctx, p_node->jump_to.get(), t);
//             break;
//         }
//         case AST_CBreak_t: {
//             print_field("CBreak", "", ++t);
//             CBreak* p_node = static_cast<CBreak*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->target)), t + 1);
//             break;
//         }
//         case AST_CContinue_t: {
//             print_field("CContinue", "", ++t);
//             CContinue* p_node = static_cast<CContinue*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->target)), t + 1);
//             break;
//         }
//         case AST_CNull_t: {
//             print_field("CNull", "", ++t);
//             break;
//         }
//         case AST_CForInit_t: {
//             print_field("CForInit", "", ++t);
//             break;
//         }
//         case AST_CInitDecl_t: {
//             print_field("CInitDecl", "", ++t);
//             CInitDecl* p_node = static_cast<CInitDecl*>(node);
//             print_ast(ctx, p_node->init.get(), t);
//             break;
//         }
//         case AST_CInitExp_t: {
//             print_field("CInitExp", "", ++t);
//             CInitExp* p_node = static_cast<CInitExp*>(node);
//             print_ast(ctx, p_node->init.get(), t);
//             break;
//         }
//         case AST_CBlock_t: {
//             print_field("CBlock", "", ++t);
//             break;
//         }
//         case AST_CB_t: {
//             print_field("CB", "", ++t);
//             CB* p_node = static_cast<CB*>(node);
//             print_field("List[" + std::to_string(vec_size(p_node->block_items)) + "]", "", t + 1);
//             for (size_t i = 0; i < vec_size(p_node->block_items); ++i) {
//                 print_ast(ctx, p_node->block_items[i].get(), t + 1);
//             }
//             break;
//         }
//         case AST_CBlockItem_t: {
//             print_field("CBlockItem", "", ++t);
//             break;
//         }
//         case AST_CS_t: {
//             print_field("CS", "", ++t);
//             CS* p_node = static_cast<CS*>(node);
//             print_ast(ctx, p_node->statement.get(), t);
//             break;
//         }
//         case AST_CD_t: {
//             print_field("CD", "", ++t);
//             CD* p_node = static_cast<CD*>(node);
//             print_ast(ctx, p_node->declaration.get(), t);
//             break;
//         }
//         case AST_CStorageClass_t: {
//             print_field("CStorageClass", "", ++t);
//             break;
//         }
//         case AST_CStatic_t: {
//             print_field("CStatic", "", ++t);
//             break;
//         }
//         case AST_CExtern_t: {
//             print_field("CExtern", "", ++t);
//             break;
//         }
//         case AST_CInitializer_t: {
//             print_field("CInitializer", "", ++t);
//             CInitializer* p_node = static_cast<CInitializer*>(node);
//             print_ast(ctx, p_node->init_type.get(), t);
//             break;
//         }
//         case AST_CSingleInit_t: {
//             print_field("CSingleInit", "", ++t);
//             CSingleInit* p_node = static_cast<CSingleInit*>(node);
//             print_ast(ctx, p_node->exp.get(), t);
//             print_ast(ctx, p_node->init_type.get(), t);
//             break;
//         }
//         case AST_CCompoundInit_t: {
//             print_field("CCompoundInit", "", ++t);
//             CCompoundInit* p_node = static_cast<CCompoundInit*>(node);
//             print_field("List[" + std::to_string(vec_size(p_node->initializers)) + "]", "", t + 1);
//             for (size_t i = 0; i < vec_size(p_node->initializers); ++i) {
//                 print_ast(ctx, p_node->initializers[i].get(), t + 1);
//             }
//             print_ast(ctx, p_node->init_type.get(), t);
//             break;
//         }
//         case AST_CMemberDeclaration_t: {
//             print_field("CMemberDeclaration", "", ++t);
//             CMemberDeclaration* p_node = static_cast<CMemberDeclaration*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->member_name)), t + 1);
//             print_ast(ctx, p_node->member_type.get(), t);
//             break;
//         }
//         case AST_CStructDeclaration_t: {
//             print_field("CStructDeclaration", "", ++t);
//             CStructDeclaration* p_node = static_cast<CStructDeclaration*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->tag)), t + 1);
//             print_field("Bool", std::to_string(p_node->is_union), t + 1);
//             print_field("List[" + std::to_string(vec_size(p_node->members)) + "]", "", t + 1);
//             for (size_t i = 0; i < vec_size(p_node->members); ++i) {
//                 print_ast(ctx, p_node->members[i].get(), t + 1);
//             }
//             break;
//         }
//         case AST_CFunctionDeclaration_t: {
//             print_field("CFunctionDeclaration", "", ++t);
//             CFunctionDeclaration* p_node = static_cast<CFunctionDeclaration*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->name)), t + 1);
//             print_field("List[" + std::to_string(vec_size(p_node->params)) + "]", "", t + 1);
//             for (size_t i = 0; i < vec_size(p_node->params); ++i) {
//                 print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->params[i])), t + 2);
//             }
//             print_ast(ctx, p_node->body.get(), t);
//             print_ast(ctx, p_node->fun_type.get(), t);
//             print_ast(ctx, p_node->storage_class.get(), t);
//             break;
//         }
//         case AST_CVariableDeclaration_t: {
//             print_field("CVariableDeclaration", "", ++t);
//             CVariableDeclaration* p_node = static_cast<CVariableDeclaration*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->name)), t + 1);
//             print_ast(ctx, p_node->init.get(), t);
//             print_ast(ctx, p_node->var_type.get(), t);
//             print_ast(ctx, p_node->storage_class.get(), t);
//             break;
//         }
//         case AST_CDeclaration_t: {
//             print_field("CDeclaration", "", ++t);
//             break;
//         }
//         case AST_CFunDecl_t: {
//             print_field("CFunDecl", "", ++t);
//             CFunDecl* p_node = static_cast<CFunDecl*>(node);
//             print_ast(ctx, p_node->fun_decl.get(), t);
//             break;
//         }
//         case AST_CVarDecl_t: {
//             print_field("CVarDecl", "", ++t);
//             CVarDecl* p_node = static_cast<CVarDecl*>(node);
//             print_ast(ctx, p_node->var_decl.get(), t);
//             break;
//         }
//         case AST_CStructDecl_t: {
//             print_field("CStructDecl", "", ++t);
//             CStructDecl* p_node = static_cast<CStructDecl*>(node);
//             print_ast(ctx, p_node->struct_decl.get(), t);
//             break;
//         }
//         case AST_CProgram_t: {
//             print_field("CProgram", "", ++t);
//             CProgram* p_node = static_cast<CProgram*>(node);
//             print_field("List[" + std::to_string(vec_size(p_node->declarations)) + "]", "", t + 1);
//             for (size_t i = 0; i < vec_size(p_node->declarations); ++i) {
//                 print_ast(ctx, p_node->declarations[i].get(), t + 1);
//             }
//             break;
//         }
//         case AST_TacUnaryOp_t: {
//             print_field("TacUnaryOp", "", ++t);
//             break;
//         }
//         case AST_TacComplement_t: {
//             print_field("TacComplement", "", ++t);
//             break;
//         }
//         case AST_TacNegate_t: {
//             print_field("TacNegate", "", ++t);
//             break;
//         }
//         case AST_TacNot_t: {
//             print_field("TacNot", "", ++t);
//             break;
//         }
//         case AST_TacBinaryOp_t: {
//             print_field("TacBinaryOp", "", ++t);
//             break;
//         }
//         case AST_TacAdd_t: {
//             print_field("TacAdd", "", ++t);
//             break;
//         }
//         case AST_TacSubtract_t: {
//             print_field("TacSubtract", "", ++t);
//             break;
//         }
//         case AST_TacMultiply_t: {
//             print_field("TacMultiply", "", ++t);
//             break;
//         }
//         case AST_TacDivide_t: {
//             print_field("TacDivide", "", ++t);
//             break;
//         }
//         case AST_TacRemainder_t: {
//             print_field("TacRemainder", "", ++t);
//             break;
//         }
//         case AST_TacBitAnd_t: {
//             print_field("TacBitAnd", "", ++t);
//             break;
//         }
//         case AST_TacBitOr_t: {
//             print_field("TacBitOr", "", ++t);
//             break;
//         }
//         case AST_TacBitXor_t: {
//             print_field("TacBitXor", "", ++t);
//             break;
//         }
//         case AST_TacBitShiftLeft_t: {
//             print_field("TacBitShiftLeft", "", ++t);
//             break;
//         }
//         case AST_TacBitShiftRight_t: {
//             print_field("TacBitShiftRight", "", ++t);
//             break;
//         }
//         case AST_TacBitShrArithmetic_t: {
//             print_field("TacBitShrArithmetic", "", ++t);
//             break;
//         }
//         case AST_TacEqual_t: {
//             print_field("TacEqual", "", ++t);
//             break;
//         }
//         case AST_TacNotEqual_t: {
//             print_field("TacNotEqual", "", ++t);
//             break;
//         }
//         case AST_TacLessThan_t: {
//             print_field("TacLessThan", "", ++t);
//             break;
//         }
//         case AST_TacLessOrEqual_t: {
//             print_field("TacLessOrEqual", "", ++t);
//             break;
//         }
//         case AST_TacGreaterThan_t: {
//             print_field("TacGreaterThan", "", ++t);
//             break;
//         }
//         case AST_TacGreaterOrEqual_t: {
//             print_field("TacGreaterOrEqual", "", ++t);
//             break;
//         }
//         case AST_TacValue_t: {
//             print_field("TacValue", "", ++t);
//             break;
//         }
//         case AST_TacConstant_t: {
//             print_field("TacConstant", "", ++t);
//             TacConstant* p_node = static_cast<TacConstant*>(node);
//             print_ast(ctx, p_node->constant.get(), t);
//             break;
//         }
//         case AST_TacVariable_t: {
//             print_field("TacVariable", "", ++t);
//             TacVariable* p_node = static_cast<TacVariable*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->name)), t + 1);
//             break;
//         }
//         case AST_TacExpResult_t: {
//             print_field("TacExpResult", "", ++t);
//             break;
//         }
//         case AST_TacPlainOperand_t: {
//             print_field("TacPlainOperand", "", ++t);
//             TacPlainOperand* p_node = static_cast<TacPlainOperand*>(node);
//             print_ast(ctx, p_node->val.get(), t);
//             break;
//         }
//         case AST_TacDereferencedPointer_t: {
//             print_field("TacDereferencedPointer", "", ++t);
//             TacDereferencedPointer* p_node = static_cast<TacDereferencedPointer*>(node);
//             print_ast(ctx, p_node->val.get(), t);
//             break;
//         }
//         case AST_TacSubObject_t: {
//             print_field("TacSubObject", "", ++t);
//             TacSubObject* p_node = static_cast<TacSubObject*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->base_name)), t + 1);
//             print_field("TLong", std::to_string(p_node->offset), t + 1);
//             break;
//         }
//         case AST_TacInstruction_t: {
//             print_field("TacInstruction", "", ++t);
//             break;
//         }
//         case AST_TacReturn_t: {
//             print_field("TacReturn", "", ++t);
//             TacReturn* p_node = static_cast<TacReturn*>(node);
//             print_ast(ctx, p_node->val.get(), t);
//             break;
//         }
//         case AST_TacSignExtend_t: {
//             print_field("TacSignExtend", "", ++t);
//             TacSignExtend* p_node = static_cast<TacSignExtend*>(node);
//             print_ast(ctx, p_node->src.get(), t);
//             print_ast(ctx, p_node->dst.get(), t);
//             break;
//         }
//         case AST_TacTruncate_t: {
//             print_field("TacTruncate", "", ++t);
//             TacTruncate* p_node = static_cast<TacTruncate*>(node);
//             print_ast(ctx, p_node->src.get(), t);
//             print_ast(ctx, p_node->dst.get(), t);
//             break;
//         }
//         case AST_TacZeroExtend_t: {
//             print_field("TacZeroExtend", "", ++t);
//             TacZeroExtend* p_node = static_cast<TacZeroExtend*>(node);
//             print_ast(ctx, p_node->src.get(), t);
//             print_ast(ctx, p_node->dst.get(), t);
//             break;
//         }
//         case AST_TacDoubleToInt_t: {
//             print_field("TacDoubleToInt", "", ++t);
//             TacDoubleToInt* p_node = static_cast<TacDoubleToInt*>(node);
//             print_ast(ctx, p_node->src.get(), t);
//             print_ast(ctx, p_node->dst.get(), t);
//             break;
//         }
//         case AST_TacDoubleToUInt_t: {
//             print_field("TacDoubleToUInt", "", ++t);
//             TacDoubleToUInt* p_node = static_cast<TacDoubleToUInt*>(node);
//             print_ast(ctx, p_node->src.get(), t);
//             print_ast(ctx, p_node->dst.get(), t);
//             break;
//         }
//         case AST_TacIntToDouble_t: {
//             print_field("TacIntToDouble", "", ++t);
//             TacIntToDouble* p_node = static_cast<TacIntToDouble*>(node);
//             print_ast(ctx, p_node->src.get(), t);
//             print_ast(ctx, p_node->dst.get(), t);
//             break;
//         }
//         case AST_TacUIntToDouble_t: {
//             print_field("TacUIntToDouble", "", ++t);
//             TacUIntToDouble* p_node = static_cast<TacUIntToDouble*>(node);
//             print_ast(ctx, p_node->src.get(), t);
//             print_ast(ctx, p_node->dst.get(), t);
//             break;
//         }
//         case AST_TacFunCall_t: {
//             print_field("TacFunCall", "", ++t);
//             TacFunCall* p_node = static_cast<TacFunCall*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->name)), t + 1);
//             print_field("List[" + std::to_string(vec_size(p_node->args)) + "]", "", t + 1);
//             for (size_t i = 0; i < vec_size(p_node->args); ++i) {
//                 print_ast(ctx, p_node->args[i].get(), t + 1);
//             }
//             print_ast(ctx, p_node->dst.get(), t);
//             break;
//         }
//         case AST_TacUnary_t: {
//             print_field("TacUnary", "", ++t);
//             TacUnary* p_node = static_cast<TacUnary*>(node);
//             print_ast(ctx, p_node->unop.get(), t);
//             print_ast(ctx, p_node->src.get(), t);
//             print_ast(ctx, p_node->dst.get(), t);
//             break;
//         }
//         case AST_TacBinary_t: {
//             print_field("TacBinary", "", ++t);
//             TacBinary* p_node = static_cast<TacBinary*>(node);
//             print_ast(ctx, p_node->binop.get(), t);
//             print_ast(ctx, p_node->src1.get(), t);
//             print_ast(ctx, p_node->src2.get(), t);
//             print_ast(ctx, p_node->dst.get(), t);
//             break;
//         }
//         case AST_TacCopy_t: {
//             print_field("TacCopy", "", ++t);
//             TacCopy* p_node = static_cast<TacCopy*>(node);
//             print_ast(ctx, p_node->src.get(), t);
//             print_ast(ctx, p_node->dst.get(), t);
//             break;
//         }
//         case AST_TacGetAddress_t: {
//             print_field("TacGetAddress", "", ++t);
//             TacGetAddress* p_node = static_cast<TacGetAddress*>(node);
//             print_ast(ctx, p_node->src.get(), t);
//             print_ast(ctx, p_node->dst.get(), t);
//             break;
//         }
//         case AST_TacLoad_t: {
//             print_field("TacLoad", "", ++t);
//             TacLoad* p_node = static_cast<TacLoad*>(node);
//             print_ast(ctx, p_node->src_ptr.get(), t);
//             print_ast(ctx, p_node->dst.get(), t);
//             break;
//         }
//         case AST_TacStore_t: {
//             print_field("TacStore", "", ++t);
//             TacStore* p_node = static_cast<TacStore*>(node);
//             print_ast(ctx, p_node->src.get(), t);
//             print_ast(ctx, p_node->dst_ptr.get(), t);
//             break;
//         }
//         case AST_TacAddPtr_t: {
//             print_field("TacAddPtr", "", ++t);
//             TacAddPtr* p_node = static_cast<TacAddPtr*>(node);
//             print_field("TLong", std::to_string(p_node->scale), t + 1);
//             print_ast(ctx, p_node->src_ptr.get(), t);
//             print_ast(ctx, p_node->idx.get(), t);
//             print_ast(ctx, p_node->dst.get(), t);
//             break;
//         }
//         case AST_TacCopyToOffset_t: {
//             print_field("TacCopyToOffset", "", ++t);
//             TacCopyToOffset* p_node = static_cast<TacCopyToOffset*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->dst_name)), t + 1);
//             print_field("TLong", std::to_string(p_node->offset), t + 1);
//             print_ast(ctx, p_node->src.get(), t);
//             break;
//         }
//         case AST_TacCopyFromOffset_t: {
//             print_field("TacCopyFromOffset", "", ++t);
//             TacCopyFromOffset* p_node = static_cast<TacCopyFromOffset*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->src_name)), t + 1);
//             print_field("TLong", std::to_string(p_node->offset), t + 1);
//             print_ast(ctx, p_node->dst.get(), t);
//             break;
//         }
//         case AST_TacJump_t: {
//             print_field("TacJump", "", ++t);
//             TacJump* p_node = static_cast<TacJump*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->target)), t + 1);
//             break;
//         }
//         case AST_TacJumpIfZero_t: {
//             print_field("TacJumpIfZero", "", ++t);
//             TacJumpIfZero* p_node = static_cast<TacJumpIfZero*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->target)), t + 1);
//             print_ast(ctx, p_node->condition.get(), t);
//             break;
//         }
//         case AST_TacJumpIfNotZero_t: {
//             print_field("TacJumpIfNotZero", "", ++t);
//             TacJumpIfNotZero* p_node = static_cast<TacJumpIfNotZero*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->target)), t + 1);
//             print_ast(ctx, p_node->condition.get(), t);
//             break;
//         }
//         case AST_TacLabel_t: {
//             print_field("TacLabel", "", ++t);
//             TacLabel* p_node = static_cast<TacLabel*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->name)), t + 1);
//             break;
//         }
//         case AST_TacTopLevel_t: {
//             print_field("TacTopLevel", "", ++t);
//             break;
//         }
//         case AST_TacFunction_t: {
//             print_field("TacFunction", "", ++t);
//             TacFunction* p_node = static_cast<TacFunction*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->name)), t + 1);
//             print_field("Bool", std::to_string(p_node->is_glob), t + 1);
//             print_field("List[" + std::to_string(vec_size(p_node->params)) + "]", "", t + 1);
//             for (size_t i = 0; i < vec_size(p_node->params); ++i) {
//                 print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->params[i])), t + 2);
//             }
//             print_field("List[" + std::to_string(vec_size(p_node->body)) + "]", "", t + 1);
//             for (size_t i = 0; i < vec_size(p_node->body); ++i) {
//                 print_ast(ctx, p_node->body[i].get(), t + 1);
//             }
//             break;
//         }
//         case AST_TacStaticVariable_t: {
//             print_field("TacStaticVariable", "", ++t);
//             TacStaticVariable* p_node = static_cast<TacStaticVariable*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->name)), t + 1);
//             print_field("Bool", std::to_string(p_node->is_glob), t + 1);
//             print_ast(ctx, p_node->static_init_type.get(), t);
//             print_field("List[" + std::to_string(vec_size(p_node->static_inits)) + "]", "", t + 1);
//             for (size_t i = 0; i < vec_size(p_node->static_inits); ++i) {
//                 print_ast(ctx, p_node->static_inits[i].get(), t + 1);
//             }
//             break;
//         }
//         case AST_TacStaticConstant_t: {
//             print_field("TacStaticConstant", "", ++t);
//             TacStaticConstant* p_node = static_cast<TacStaticConstant*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->name)), t + 1);
//             print_ast(ctx, p_node->static_init_type.get(), t);
//             print_ast(ctx, p_node->static_init.get(), t);
//             break;
//         }
//         case AST_TacProgram_t: {
//             print_field("TacProgram", "", ++t);
//             TacProgram* p_node = static_cast<TacProgram*>(node);
//             print_field("List[" + std::to_string(vec_size(p_node->static_const_toplvls)) + "]", "", t + 1);
//             for (size_t i = 0; i < vec_size(p_node->static_const_toplvls); ++i) {
//                 print_ast(ctx, p_node->static_const_toplvls[i].get(), t + 1);
//             }
//             print_field("List[" + std::to_string(vec_size(p_node->static_var_toplvls)) + "]", "", t + 1);
//             for (size_t i = 0; i < vec_size(p_node->static_var_toplvls); ++i) {
//                 print_ast(ctx, p_node->static_var_toplvls[i].get(), t + 1);
//             }
//             print_field("List[" + std::to_string(vec_size(p_node->fun_toplvls)) + "]", "", t + 1);
//             for (size_t i = 0; i < vec_size(p_node->fun_toplvls); ++i) {
//                 print_ast(ctx, p_node->fun_toplvls[i].get(), t + 1);
//             }
//             break;
//         }
//         case AST_AsmReg_t: {
//             print_field("AsmReg", "", ++t);
//             break;
//         }
//         case AST_AsmAx_t: {
//             print_field("AsmAx", "", ++t);
//             break;
//         }
//         case AST_AsmBx_t: {
//             print_field("AsmBx", "", ++t);
//             break;
//         }
//         case AST_AsmCx_t: {
//             print_field("AsmCx", "", ++t);
//             break;
//         }
//         case AST_AsmDx_t: {
//             print_field("AsmDx", "", ++t);
//             break;
//         }
//         case AST_AsmDi_t: {
//             print_field("AsmDi", "", ++t);
//             break;
//         }
//         case AST_AsmSi_t: {
//             print_field("AsmSi", "", ++t);
//             break;
//         }
//         case AST_AsmR8_t: {
//             print_field("AsmR8", "", ++t);
//             break;
//         }
//         case AST_AsmR9_t: {
//             print_field("AsmR9", "", ++t);
//             break;
//         }
//         case AST_AsmR10_t: {
//             print_field("AsmR10", "", ++t);
//             break;
//         }
//         case AST_AsmR11_t: {
//             print_field("AsmR11", "", ++t);
//             break;
//         }
//         case AST_AsmR12_t: {
//             print_field("AsmR12", "", ++t);
//             break;
//         }
//         case AST_AsmR13_t: {
//             print_field("AsmR13", "", ++t);
//             break;
//         }
//         case AST_AsmR14_t: {
//             print_field("AsmR14", "", ++t);
//             break;
//         }
//         case AST_AsmR15_t: {
//             print_field("AsmR15", "", ++t);
//             break;
//         }
//         case AST_AsmSp_t: {
//             print_field("AsmSp", "", ++t);
//             break;
//         }
//         case AST_AsmBp_t: {
//             print_field("AsmBp", "", ++t);
//             break;
//         }
//         case AST_AsmXMM0_t: {
//             print_field("AsmXMM0", "", ++t);
//             break;
//         }
//         case AST_AsmXMM1_t: {
//             print_field("AsmXMM1", "", ++t);
//             break;
//         }
//         case AST_AsmXMM2_t: {
//             print_field("AsmXMM2", "", ++t);
//             break;
//         }
//         case AST_AsmXMM3_t: {
//             print_field("AsmXMM3", "", ++t);
//             break;
//         }
//         case AST_AsmXMM4_t: {
//             print_field("AsmXMM4", "", ++t);
//             break;
//         }
//         case AST_AsmXMM5_t: {
//             print_field("AsmXMM5", "", ++t);
//             break;
//         }
//         case AST_AsmXMM6_t: {
//             print_field("AsmXMM6", "", ++t);
//             break;
//         }
//         case AST_AsmXMM7_t: {
//             print_field("AsmXMM7", "", ++t);
//             break;
//         }
//         case AST_AsmXMM8_t: {
//             print_field("AsmXMM8", "", ++t);
//             break;
//         }
//         case AST_AsmXMM9_t: {
//             print_field("AsmXMM9", "", ++t);
//             break;
//         }
//         case AST_AsmXMM10_t: {
//             print_field("AsmXMM10", "", ++t);
//             break;
//         }
//         case AST_AsmXMM11_t: {
//             print_field("AsmXMM11", "", ++t);
//             break;
//         }
//         case AST_AsmXMM12_t: {
//             print_field("AsmXMM12", "", ++t);
//             break;
//         }
//         case AST_AsmXMM13_t: {
//             print_field("AsmXMM13", "", ++t);
//             break;
//         }
//         case AST_AsmXMM14_t: {
//             print_field("AsmXMM14", "", ++t);
//             break;
//         }
//         case AST_AsmXMM15_t: {
//             print_field("AsmXMM15", "", ++t);
//             break;
//         }
//         case AST_AsmCondCode_t: {
//             print_field("AsmCondCode", "", ++t);
//             break;
//         }
//         case AST_AsmE_t: {
//             print_field("AsmE", "", ++t);
//             break;
//         }
//         case AST_AsmNE_t: {
//             print_field("AsmNE", "", ++t);
//             break;
//         }
//         case AST_AsmG_t: {
//             print_field("AsmG", "", ++t);
//             break;
//         }
//         case AST_AsmGE_t: {
//             print_field("AsmGE", "", ++t);
//             break;
//         }
//         case AST_AsmL_t: {
//             print_field("AsmL", "", ++t);
//             break;
//         }
//         case AST_AsmLE_t: {
//             print_field("AsmLE", "", ++t);
//             break;
//         }
//         case AST_AsmA_t: {
//             print_field("AsmA", "", ++t);
//             break;
//         }
//         case AST_AsmAE_t: {
//             print_field("AsmAE", "", ++t);
//             break;
//         }
//         case AST_AsmB_t: {
//             print_field("AsmB", "", ++t);
//             break;
//         }
//         case AST_AsmBE_t: {
//             print_field("AsmBE", "", ++t);
//             break;
//         }
//         case AST_AsmP_t: {
//             print_field("AsmP", "", ++t);
//             break;
//         }
//         case AST_AsmOperand_t: {
//             print_field("AsmOperand", "", ++t);
//             break;
//         }
//         case AST_AsmImm_t: {
//             print_field("AsmImm", "", ++t);
//             AsmImm* p_node = static_cast<AsmImm*>(node);
//             print_field("TULong", std::to_string(p_node->value), t + 1);
//             print_field("Bool", std::to_string(p_node->is_byte), t + 1);
//             print_field("Bool", std::to_string(p_node->is_quad), t + 1);
//             print_field("Bool", std::to_string(p_node->is_neg), t + 1);
//             break;
//         }
//         case AST_AsmRegister_t: {
//             print_field("AsmRegister", "", ++t);
//             AsmRegister* p_node = static_cast<AsmRegister*>(node);
//             print_ast(ctx, p_node->reg.get(), t);
//             break;
//         }
//         case AST_AsmPseudo_t: {
//             print_field("AsmPseudo", "", ++t);
//             AsmPseudo* p_node = static_cast<AsmPseudo*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->name)), t + 1);
//             break;
//         }
//         case AST_AsmMemory_t: {
//             print_field("AsmMemory", "", ++t);
//             AsmMemory* p_node = static_cast<AsmMemory*>(node);
//             print_field("TLong", std::to_string(p_node->value), t + 1);
//             print_ast(ctx, p_node->reg.get(), t);
//             break;
//         }
//         case AST_AsmData_t: {
//             print_field("AsmData", "", ++t);
//             AsmData* p_node = static_cast<AsmData*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->name)), t + 1);
//             print_field("TLong", std::to_string(p_node->offset), t + 1);
//             break;
//         }
//         case AST_AsmPseudoMem_t: {
//             print_field("AsmPseudoMem", "", ++t);
//             AsmPseudoMem* p_node = static_cast<AsmPseudoMem*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->name)), t + 1);
//             print_field("TLong", std::to_string(p_node->offset), t + 1);
//             break;
//         }
//         case AST_AsmIndexed_t: {
//             print_field("AsmIndexed", "", ++t);
//             AsmIndexed* p_node = static_cast<AsmIndexed*>(node);
//             print_field("TLong", std::to_string(p_node->scale), t + 1);
//             print_ast(ctx, p_node->reg_base.get(), t);
//             print_ast(ctx, p_node->reg_index.get(), t);
//             break;
//         }
//         case AST_AsmBinaryOp_t: {
//             print_field("AsmBinaryOp", "", ++t);
//             break;
//         }
//         case AST_AsmAdd_t: {
//             print_field("AsmAdd", "", ++t);
//             break;
//         }
//         case AST_AsmSub_t: {
//             print_field("AsmSub", "", ++t);
//             break;
//         }
//         case AST_AsmMult_t: {
//             print_field("AsmMult", "", ++t);
//             break;
//         }
//         case AST_AsmDivDouble_t: {
//             print_field("AsmDivDouble", "", ++t);
//             break;
//         }
//         case AST_AsmBitAnd_t: {
//             print_field("AsmBitAnd", "", ++t);
//             break;
//         }
//         case AST_AsmBitOr_t: {
//             print_field("AsmBitOr", "", ++t);
//             break;
//         }
//         case AST_AsmBitXor_t: {
//             print_field("AsmBitXor", "", ++t);
//             break;
//         }
//         case AST_AsmBitShiftLeft_t: {
//             print_field("AsmBitShiftLeft", "", ++t);
//             break;
//         }
//         case AST_AsmBitShiftRight_t: {
//             print_field("AsmBitShiftRight", "", ++t);
//             break;
//         }
//         case AST_AsmBitShrArithmetic_t: {
//             print_field("AsmBitShrArithmetic", "", ++t);
//             break;
//         }
//         case AST_AsmUnaryOp_t: {
//             print_field("AsmUnaryOp", "", ++t);
//             break;
//         }
//         case AST_AsmNot_t: {
//             print_field("AsmNot", "", ++t);
//             break;
//         }
//         case AST_AsmNeg_t: {
//             print_field("AsmNeg", "", ++t);
//             break;
//         }
//         case AST_AsmShr_t: {
//             print_field("AsmShr", "", ++t);
//             break;
//         }
//         case AST_AsmInstruction_t: {
//             print_field("AsmInstruction", "", ++t);
//             break;
//         }
//         case AST_AsmMov_t: {
//             print_field("AsmMov", "", ++t);
//             AsmMov* p_node = static_cast<AsmMov*>(node);
//             print_ast(ctx, p_node->asm_type.get(), t);
//             print_ast(ctx, p_node->src.get(), t);
//             print_ast(ctx, p_node->dst.get(), t);
//             break;
//         }
//         case AST_AsmMovSx_t: {
//             print_field("AsmMovSx", "", ++t);
//             AsmMovSx* p_node = static_cast<AsmMovSx*>(node);
//             print_ast(ctx, p_node->asm_type_src.get(), t);
//             print_ast(ctx, p_node->asm_type_dst.get(), t);
//             print_ast(ctx, p_node->src.get(), t);
//             print_ast(ctx, p_node->dst.get(), t);
//             break;
//         }
//         case AST_AsmMovZeroExtend_t: {
//             print_field("AsmMovZeroExtend", "", ++t);
//             AsmMovZeroExtend* p_node = static_cast<AsmMovZeroExtend*>(node);
//             print_ast(ctx, p_node->asm_type_src.get(), t);
//             print_ast(ctx, p_node->asm_type_dst.get(), t);
//             print_ast(ctx, p_node->src.get(), t);
//             print_ast(ctx, p_node->dst.get(), t);
//             break;
//         }
//         case AST_AsmLea_t: {
//             print_field("AsmLea", "", ++t);
//             AsmLea* p_node = static_cast<AsmLea*>(node);
//             print_ast(ctx, p_node->src.get(), t);
//             print_ast(ctx, p_node->dst.get(), t);
//             break;
//         }
//         case AST_AsmCvttsd2si_t: {
//             print_field("AsmCvttsd2si", "", ++t);
//             AsmCvttsd2si* p_node = static_cast<AsmCvttsd2si*>(node);
//             print_ast(ctx, p_node->asm_type.get(), t);
//             print_ast(ctx, p_node->src.get(), t);
//             print_ast(ctx, p_node->dst.get(), t);
//             break;
//         }
//         case AST_AsmCvtsi2sd_t: {
//             print_field("AsmCvtsi2sd", "", ++t);
//             AsmCvtsi2sd* p_node = static_cast<AsmCvtsi2sd*>(node);
//             print_ast(ctx, p_node->asm_type.get(), t);
//             print_ast(ctx, p_node->src.get(), t);
//             print_ast(ctx, p_node->dst.get(), t);
//             break;
//         }
//         case AST_AsmUnary_t: {
//             print_field("AsmUnary", "", ++t);
//             AsmUnary* p_node = static_cast<AsmUnary*>(node);
//             print_ast(ctx, p_node->unop.get(), t);
//             print_ast(ctx, p_node->asm_type.get(), t);
//             print_ast(ctx, p_node->dst.get(), t);
//             break;
//         }
//         case AST_AsmBinary_t: {
//             print_field("AsmBinary", "", ++t);
//             AsmBinary* p_node = static_cast<AsmBinary*>(node);
//             print_ast(ctx, p_node->binop.get(), t);
//             print_ast(ctx, p_node->asm_type.get(), t);
//             print_ast(ctx, p_node->src.get(), t);
//             print_ast(ctx, p_node->dst.get(), t);
//             break;
//         }
//         case AST_AsmCmp_t: {
//             print_field("AsmCmp", "", ++t);
//             AsmCmp* p_node = static_cast<AsmCmp*>(node);
//             print_ast(ctx, p_node->asm_type.get(), t);
//             print_ast(ctx, p_node->src.get(), t);
//             print_ast(ctx, p_node->dst.get(), t);
//             break;
//         }
//         case AST_AsmIdiv_t: {
//             print_field("AsmIdiv", "", ++t);
//             AsmIdiv* p_node = static_cast<AsmIdiv*>(node);
//             print_ast(ctx, p_node->asm_type.get(), t);
//             print_ast(ctx, p_node->src.get(), t);
//             break;
//         }
//         case AST_AsmDiv_t: {
//             print_field("AsmDiv", "", ++t);
//             AsmDiv* p_node = static_cast<AsmDiv*>(node);
//             print_ast(ctx, p_node->asm_type.get(), t);
//             print_ast(ctx, p_node->src.get(), t);
//             break;
//         }
//         case AST_AsmCdq_t: {
//             print_field("AsmCdq", "", ++t);
//             AsmCdq* p_node = static_cast<AsmCdq*>(node);
//             print_ast(ctx, p_node->asm_type.get(), t);
//             break;
//         }
//         case AST_AsmJmp_t: {
//             print_field("AsmJmp", "", ++t);
//             AsmJmp* p_node = static_cast<AsmJmp*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->target)), t + 1);
//             break;
//         }
//         case AST_AsmJmpCC_t: {
//             print_field("AsmJmpCC", "", ++t);
//             AsmJmpCC* p_node = static_cast<AsmJmpCC*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->target)), t + 1);
//             print_ast(ctx, p_node->cond_code.get(), t);
//             break;
//         }
//         case AST_AsmSetCC_t: {
//             print_field("AsmSetCC", "", ++t);
//             AsmSetCC* p_node = static_cast<AsmSetCC*>(node);
//             print_ast(ctx, p_node->cond_code.get(), t);
//             print_ast(ctx, p_node->dst.get(), t);
//             break;
//         }
//         case AST_AsmLabel_t: {
//             print_field("AsmLabel", "", ++t);
//             AsmLabel* p_node = static_cast<AsmLabel*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->name)), t + 1);
//             break;
//         }
//         case AST_AsmPush_t: {
//             print_field("AsmPush", "", ++t);
//             AsmPush* p_node = static_cast<AsmPush*>(node);
//             print_ast(ctx, p_node->src.get(), t);
//             break;
//         }
//         case AST_AsmPop_t: {
//             print_field("AsmPop", "", ++t);
//             AsmPop* p_node = static_cast<AsmPop*>(node);
//             print_ast(ctx, p_node->reg.get(), t);
//             break;
//         }
//         case AST_AsmCall_t: {
//             print_field("AsmCall", "", ++t);
//             AsmCall* p_node = static_cast<AsmCall*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->name)), t + 1);
//             break;
//         }
//         case AST_AsmRet_t: {
//             print_field("AsmRet", "", ++t);
//             break;
//         }
//         case AST_AsmTopLevel_t: {
//             print_field("AsmTopLevel", "", ++t);
//             break;
//         }
//         case AST_AsmFunction_t: {
//             print_field("AsmFunction", "", ++t);
//             AsmFunction* p_node = static_cast<AsmFunction*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->name)), t + 1);
//             print_field("Bool", std::to_string(p_node->is_glob), t + 1);
//             print_field("Bool", std::to_string(p_node->is_ret_memory), t + 1);
//             print_field("List[" + std::to_string(vec_size(p_node->instructions)) + "]", "", t + 1);
//             for (size_t i = 0; i < vec_size(p_node->instructions); ++i) {
//                 print_ast(ctx, p_node->instructions[i].get(), t + 1);
//             }
//             break;
//         }
//         case AST_AsmStaticVariable_t: {
//             print_field("AsmStaticVariable", "", ++t);
//             AsmStaticVariable* p_node = static_cast<AsmStaticVariable*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->name)), t + 1);
//             print_field("TInt", std::to_string(p_node->alignment), t + 1);
//             print_field("Bool", std::to_string(p_node->is_glob), t + 1);
//             print_field("List[" + std::to_string(vec_size(p_node->static_inits)) + "]", "", t + 1);
//             for (size_t i = 0; i < vec_size(p_node->static_inits); ++i) {
//                 print_ast(ctx, p_node->static_inits[i].get(), t + 1);
//             }
//             break;
//         }
//         case AST_AsmStaticConstant_t: {
//             print_field("AsmStaticConstant", "", ++t);
//             AsmStaticConstant* p_node = static_cast<AsmStaticConstant*>(node);
//             print_field("TIdentifier", std::string(map_get(ctx->hash_table, p_node->name)), t + 1);
//             print_field("TInt", std::to_string(p_node->alignment), t + 1);
//             print_ast(ctx, p_node->static_init.get(), t);
//             break;
//         }
//         case AST_AsmProgram_t: {
//             print_field("AsmProgram", "", ++t);
//             AsmProgram* p_node = static_cast<AsmProgram*>(node);
//             print_field("List[" + std::to_string(vec_size(p_node->static_const_toplvls)) + "]", "", t + 1);
//             for (size_t i = 0; i < vec_size(p_node->static_const_toplvls); ++i) {
//                 print_ast(ctx, p_node->static_const_toplvls[i].get(), t + 1);
//             }
//             print_field("List[" + std::to_string(vec_size(p_node->top_levels)) + "]", "", t + 1);
//             for (size_t i = 0; i < vec_size(p_node->top_levels); ++i) {
//                 print_ast(ctx, p_node->top_levels[i].get(), t + 1);
//             }
//             break;
//         }
//         default:
//             THROW_ABORT;
//     }
// }

void pprint_c_ast(IdentifierContext* ctx, CProgram* node) {
    print_title("C AST");
    print_CProgram(ctx, node, 0);
    std::cout << std::endl;
}

void pprint_tac_ast(IdentifierContext* ctx, TacProgram* node) {
    print_title("TAC AST");
    print_TacProgram(ctx, node, 0);
    std::cout << std::endl;
}

void pprint_asm_ast(IdentifierContext* ctx, AsmProgram* node) {
    print_title("ASM AST");
    print_AsmProgram(ctx, node, 0);
    std::cout << std::endl;
}

void pprint_ast(IdentifierContext* /*ctx*/, Ast* /*node*/, const char* name) {
    print_title(name);
    // print_ast(ctx, node, 0);
    std::cout << std::endl;
}

void pprint_addressed_set(IdentifierContext* /*ctx*/, FrontEndContext* frontend) {
    print_title("Addressed Set");
    std::cout << "\nSet(" << std::to_string(set_size(frontend->addressed_set)) << "):";
    for (size_t i = 0; i < set_size(frontend->addressed_set); ++i) {
        // TIdentifier name = element_get(frontend->addressed_set[i]);
        // print_field("", std::string(map_get(ctx->hash_table, name)), 2);
    }
    std::cout << std::endl;
}

void pprint_string_const_table(IdentifierContext* /*ctx*/, FrontEndContext* frontend) {
    print_title("String Constant Table");
    std::cout << "\nDict(" << std::to_string(map_size(frontend->string_const_table)) << "):";
    for (size_t i = 0; i < map_size(frontend->string_const_table); ++i) {
        const pair_t(TIdentifier, TIdentifier)* static_const = &frontend->string_const_table[i];
        // print_field("[" + std::string(map_get(ctx->hash_table, pair_first(*static_const))) + "]", "", 2);
        if (map_find(frontend->symbol_table, pair_second(*static_const)) != map_end()
            && map_get(frontend->symbol_table, pair_second(*static_const))->attrs->type() == AST_ConstantAttr_t) {
            ConstantAttr* constant_attr =
                static_cast<ConstantAttr*>(map_get(frontend->symbol_table, pair_second(*static_const))->attrs.get());
            if (constant_attr->static_init->type() == AST_StringInit_t) {
                std::cout << "\n    String: \"";
                StringInit* string_init = static_cast<StringInit*>(constant_attr->static_init.get());
                for (size_t i = 0; i < vec_size(string_init->literal.get()->value); ++i) {
                    TChar byte = string_init->literal.get()->value[i];
                    switch (byte) {
                        case 39:
                            std::cout << "\\'";
                            break;
                        case 34:
                            std::cout << "\\\"";
                            break;
                        case 63:
                            std::cout << "\\?";
                            break;
                        case 92:
                            std::cout << "\\\\";
                            break;
                        case 7:
                            std::cout << "\\a";
                            break;
                        case 8:
                            std::cout << "\\b";
                            break;
                        case 12:
                            std::cout << "\\f";
                            break;
                        case 10:
                            std::cout << "\\n";
                            break;
                        case 13:
                            std::cout << "\\r";
                            break;
                        case 9:
                            std::cout << "\\t";
                            break;
                        case 11:
                            std::cout << "\\v";
                            break;
                        default:
                            std::cout << byte;
                            break;
                    }
                }
                std::cout << "\"";
                continue;
            }
        }
        THROW_ABORT;
    }
    std::cout << std::endl;
}

void pprint_struct_typedef_table(IdentifierContext* /*ctx*/, FrontEndContext* frontend) {
    print_title("Structure Typedef Table");
    std::cout << "\nDict(" << std::to_string(map_size(frontend->struct_typedef_table)) << "):";
    for (size_t i = 0; i < map_size(frontend->struct_typedef_table); ++i) {
        // const pair_t(TIdentifier, UPtrStructTypedef)* struct_typedef = &frontend->struct_typedef_table[i];
        // print_field("[" + std::string(map_get(ctx->hash_table, pair_first(*struct_typedef))) + "]", "", 2);
        // print_ast(ctx, pair_second(*struct_typedef).get(), 2);
    }
    std::cout << std::endl;
}

void pprint_symbol_table(IdentifierContext* /*ctx*/, FrontEndContext* frontend) {
    print_title("Symbol Table");
    std::cout << "\nDict(" << std::to_string(map_size(frontend->symbol_table)) << "):";
    for (size_t i = 0; i < map_size(frontend->symbol_table); ++i) {
        // const pair_t(TIdentifier, UPtrSymbol)* symbol = &frontend->symbol_table[i];
        // print_field("[" + std::string(map_get(ctx->hash_table, pair_first(*symbol))) + "]", "", 2);
        // print_ast(ctx, pair_second(*symbol).get(), 2);
    }
    std::cout << std::endl;
}

void pprint_backend_symbol_table(IdentifierContext* /*ctx*/, BackEndContext* backend) {
    print_title("Backend Symbol Table");
    std::cout << "\nDict(" << std::to_string(map_size(backend->symbol_table)) << "):";
    for (size_t i = 0; i < map_size(backend->symbol_table); ++i) {
        // const pair_t(TIdentifier, UPtrBackendSymbol)* bakend_symbol = &backend->symbol_table[i];
        // print_field("[" + std::string(map_get(ctx->hash_table, pair_first(*bakend_symbol))) + "]", "", 2);
        // print_ast(ctx, pair_second(*bakend_symbol).get(), 2);
    }
    std::cout << std::endl;
}
#endif
