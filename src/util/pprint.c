#include "util/pprint.h"
#ifdef __NDEBUG__
typedef int empty_translation_unit;
#else
#include "util/c_std.h"
#include "util/str2t.h"
#include "util/throw.h"

#include "ast/ast.h"
#include "ast/back_ast.h"
#include "ast/back_symt.h"
#include "ast/front_ast.h"
#include "ast/front_symt.h"
#include "ast/interm_ast.h"

#include "frontend/parser/errors.h"
#include "frontend/parser/lexer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Pretty print

static void print_title(const char* title) { printf("+\n+\n@@ %s @@", title); }

void pprint_toks(IdentifierContext* ctx, vector_t(Token) tokens) {
    print_title("Tokens");
    printf("\nList[%zu]:", vec_size(tokens));
    for (size_t i = 0; i < vec_size(tokens); ++i) {
        printf("\n  ");
        switch (tokens[i].tok_kind) {
            case TOK_identifier:
            case TOK_string_literal:
            case TOK_char_const:
            case TOK_int_const:
            case TOK_long_const:
            case TOK_uint_const:
            case TOK_ulong_const:
            case TOK_dbl_const:
                printf("%s(%s)", get_tok_kind_fmt(tokens[i].tok_kind), get_tok_fmt(ctx, &tokens[i]));
                break;
            default:
                printf("%s", get_tok_fmt(ctx, &tokens[i]));
                break;
        }
    }
    printf("\n");
}

typedef IdentifierContext* Ctx;

static void print_CDeclarator(Ctx ctx, const CDeclarator* node, size_t tab);
static void print_CForInit(Ctx ctx, const CForInit* node, size_t tab);
static void print_CBlock(Ctx ctx, const CBlock* node, size_t tab);
static void print_CBlockItem(Ctx ctx, const CBlockItem* node, size_t tab);
static void print_CVariableDeclaration(Ctx ctx, const CVariableDeclaration* node, size_t tab);
static void print_CDeclaration(Ctx ctx, const CDeclaration* node, size_t tab);
static void print_AsmOperand(Ctx ctx, const AsmOperand* node, size_t tab);

#define TAB_SIZE 2
#define print_field(X, ...)                             \
    do {                                                \
        printf("\n%*s", (int)(((X)-1) * TAB_SIZE), ""); \
        printf(__VA_ARGS__);                            \
    }                                                   \
    while (0)

#define print_null(X, Y)          \
    if (!X) {                     \
        print_field(Y, "None: "); \
        return;                   \
    }

static void print_CConst(const CConst* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
        case AST_CConst_t:
            print_field(++tab, "CConst: ");
            break;
        case AST_CConstInt_t:
            print_field(++tab, "CConstInt: ");
            print_field(tab + 1, "TInt: %i", node->get._CConstInt.value);
            break;
        case AST_CConstLong_t:
            print_field(++tab, "CConstLong: ");
            print_field(tab + 1, "TLong: %zi", (ssize_t)node->get._CConstLong.value);
            break;
        case AST_CConstUInt_t:
            print_field(++tab, "CConstUInt: ");
            print_field(tab + 1, "TUInt: %u", node->get._CConstUInt.value);
            break;
        case AST_CConstULong_t:
            print_field(++tab, "CConstULong: ");
            print_field(tab + 1, "TULong: %zu", (size_t)node->get._CConstULong.value);
            break;
        case AST_CConstDouble_t:
            print_field(++tab, "CConstDouble: ");
            print_field(tab + 1, "TDouble: %lf", node->get._CConstDouble.value);
            break;
        case AST_CConstChar_t:
            print_field(++tab, "CConstChar: ");
            print_field(tab + 1, "TChar: %i", (int)node->get._CConstChar.value);
            break;
        case AST_CConstUChar_t:
            print_field(++tab, "CConstUChar: ");
            print_field(tab + 1, "TUChar: %u", (unsigned int)node->get._CConstUChar.value);
            break;
        default:
            THROW_ABORT;
    }
}

static void print_CStringLiteral(const CStringLiteral* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
        case AST_CStringLiteral_t:
            print_field(++tab, "CStringLiteral: ");
            break;
        default:
            THROW_ABORT;
    }
    print_field(tab + 1, "List[%zu]: ", vec_size(node->value));
    for (size_t i = 0; i < vec_size(node->value); ++i) {
        print_field(tab + 2, "TChar: %i", (int)node->value[i]);
    }
}

static void print_Type(Ctx ctx, const Type* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
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
            print_field(tab + 1, "TULong: %zu", (size_t)node->get._FunType.param_reg_mask);
            print_field(tab + 1, "TULong: %zu", (size_t)node->get._FunType.ret_reg_mask);
            print_field(tab + 1, "List[%zu]: ", vec_size(node->get._FunType.param_types));
            for (size_t i = 0; i < vec_size(node->get._FunType.param_types); ++i) {
                print_Type(ctx, node->get._FunType.param_types[i], tab + 1);
            }
            print_Type(ctx, node->get._FunType.ret_type, tab);
            break;
        case AST_Pointer_t:
            print_field(++tab, "Pointer: ");
            print_Type(ctx, node->get._Pointer.ref_type, tab);
            break;
        case AST_Array_t:
            print_field(++tab, "Array: ");
            print_field(tab + 1, "TLong: %zi", (ssize_t)node->get._Array.size);
            print_Type(ctx, node->get._Array.elem_type, tab);
            break;
        case AST_Structure_t:
            print_field(++tab, "Structure: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._Structure.tag));
            print_field(tab + 1, "Bool: %s", node->get._Structure.is_union ? "1" : "0");
            break;
        default:
            THROW_ABORT;
    }
}

static void print_StaticInit(Ctx ctx, const StaticInit* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
        case AST_StaticInit_t:
            print_field(++tab, "StaticInit: ");
            break;
        case AST_IntInit_t:
            print_field(++tab, "IntInit: ");
            print_field(tab + 1, "TInt: %i", node->get._IntInit.value);
            break;
        case AST_LongInit_t:
            print_field(++tab, "LongInit: ");
            print_field(tab + 1, "TLong: %zi", (ssize_t)node->get._LongInit.value);
            break;
        case AST_UIntInit_t:
            print_field(++tab, "UIntInit: ");
            print_field(tab + 1, "TUInt: %u", node->get._UIntInit.value);
            break;
        case AST_ULongInit_t:
            print_field(++tab, "ULongInit: ");
            print_field(tab + 1, "TULong: %zu", (size_t)node->get._ULongInit.value);
            break;
        case AST_CharInit_t:
            print_field(++tab, "CharInit: ");
            print_field(tab + 1, "TChar: %i", (int)node->get._CharInit.value);
            break;
        case AST_UCharInit_t:
            print_field(++tab, "UCharInit: ");
            print_field(tab + 1, "TUChar: %u", (unsigned int)node->get._UCharInit.value);
            break;
        case AST_DoubleInit_t:
            print_field(++tab, "DoubleInit: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._DoubleInit.dbl_const));
            break;
        case AST_ZeroInit_t:
            print_field(++tab, "ZeroInit: ");
            print_field(tab + 1, "TLong: %zi", (ssize_t)node->get._ZeroInit.byte);
            break;
        case AST_StringInit_t:
            print_field(++tab, "StringInit: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._StringInit.string_const));
            print_field(tab + 1, "Bool: %s", node->get._StringInit.is_null_term ? "1" : "0");
            print_CStringLiteral(node->get._StringInit.literal, tab);
            break;
        case AST_PointerInit_t:
            print_field(++tab, "PointerInit: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._PointerInit.name));
            break;
        default:
            THROW_ABORT;
    }
}

static void print_InitialValue(Ctx ctx, const InitialValue* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
        case AST_InitialValue_t:
            print_field(++tab, "InitialValue: ");
            break;
        case AST_Tentative_t:
            print_field(++tab, "Tentative: ");
            break;
        case AST_Initial_t:
            print_field(++tab, "Initial: ");
            print_field(tab + 1, "List[%zu]: ", vec_size(node->get._Initial.static_inits));
            for (size_t i = 0; i < vec_size(node->get._Initial.static_inits); ++i) {
                print_StaticInit(ctx, node->get._Initial.static_inits[i], tab + 1);
            }
            break;
        case AST_NoInitializer_t:
            print_field(++tab, "NoInitializer: ");
            break;
        default:
            THROW_ABORT;
    }
}

static void print_IdentifierAttr(Ctx ctx, const IdentifierAttr* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
        case AST_IdentifierAttr_t:
            print_field(++tab, "IdentifierAttr: ");
            break;
        case AST_FunAttr_t:
            print_field(++tab, "FunAttr: ");
            print_field(tab + 1, "Bool: %s", node->get._FunAttr.is_def ? "1" : "0");
            print_field(tab + 1, "Bool: %s", node->get._FunAttr.is_glob ? "1" : "0");
            break;
        case AST_StaticAttr_t:
            print_field(++tab, "StaticAttr: ");
            print_field(tab + 1, "Bool: %s", node->get._StaticAttr.is_glob ? "1" : "0");
            print_InitialValue(ctx, node->get._StaticAttr.init, tab);
            break;
        case AST_ConstantAttr_t:
            print_field(++tab, "ConstantAttr: ");
            print_StaticInit(ctx, node->get._ConstantAttr.static_init, tab);
            break;
        case AST_LocalAttr_t:
            print_field(++tab, "LocalAttr: ");
            break;
        default:
            THROW_ABORT;
    }
}

static void print_Symbol(Ctx ctx, const Symbol* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
        case AST_Symbol_t:
            print_field(++tab, "Symbol: ");
            break;
        default:
            THROW_ABORT;
    }
    print_Type(ctx, node->type_t, tab);
    print_IdentifierAttr(ctx, node->attrs, tab);
}

static void print_StructMember(Ctx ctx, const StructMember* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
        case AST_StructMember_t:
            print_field(++tab, "StructMember: ");
            break;
        default:
            THROW_ABORT;
    }
    print_field(tab + 1, "TLong: %zi", (ssize_t)node->offset);
    print_Type(ctx, node->member_type, tab);
}

static void print_StructTypedef(Ctx ctx, const StructTypedef* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
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
        print_StructMember(ctx, pair_second(node->members[i]), tab + 2);
    }
}

static void print_CUnaryOp(const CUnaryOp* node, size_t tab) {
    print_null((node->type != AST_CUnaryOp_t), ++tab); // TODO remove
    print_null(node, ++tab);
    switch (node->type) {
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

static void print_CBinaryOp(const CBinaryOp* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
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

static void print_CParam(Ctx ctx, const CParam* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
        case AST_CParam_t:
            print_field(++tab, "CParam: ");
            break;
        default:
            THROW_ABORT;
    }
    print_CDeclarator(ctx, node->decltor, tab);
    print_Type(ctx, node->param_type, tab);
}

static void print_CDeclarator(Ctx ctx, const CDeclarator* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
        case AST_CDeclarator_t:
            print_field(++tab, "CDeclarator: ");
            break;
        case AST_CIdent_t:
            print_field(++tab, "CIdent: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._CIdent.name));
            break;
        case AST_CPointerDeclarator_t:
            print_field(++tab, "CPointerDeclarator: ");
            print_CDeclarator(ctx, node->get._CPointerDeclarator.decltor, tab);
            break;
        case AST_CArrayDeclarator_t:
            print_field(++tab, "CArrayDeclarator: ");
            print_field(tab + 1, "TLong: %zi", (ssize_t)node->get._CArrayDeclarator.size);
            print_CDeclarator(ctx, node->get._CArrayDeclarator.decltor, tab);
            break;
        case AST_CFunDeclarator_t:
            print_field(++tab, "CFunDeclarator: ");
            print_field(tab + 1, "List[%zu]: ", vec_size(node->get._CFunDeclarator.param_list));
            for (size_t i = 0; i < vec_size(node->get._CFunDeclarator.param_list); ++i) {
                print_CParam(ctx, node->get._CFunDeclarator.param_list[i], tab + 1);
            }
            print_CDeclarator(ctx, node->get._CFunDeclarator.decltor, tab);
            break;
        default:
            THROW_ABORT;
    }
}

static void print_CExp(Ctx ctx, const CExp* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
        case AST_CExp_t:
            print_field(++tab, "CExp: ");
            break;
        case AST_CConstant_t:
            print_field(++tab, "CConstant: ");
            print_CConst(node->get._CConstant.constant, tab);
            break;
        case AST_CString_t:
            print_field(++tab, "CString: ");
            print_CStringLiteral(node->get._CString.literal, tab);
            break;
        case AST_CVar_t:
            print_field(++tab, "CVar: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._CVar.name));
            break;
        case AST_CCast_t:
            print_field(++tab, "CCast: ");
            print_CExp(ctx, node->get._CCast.exp, tab);
            print_Type(ctx, node->get._CCast.target_type, tab);
            break;
        case AST_CUnary_t:
            print_field(++tab, "CUnary: ");
            print_CUnaryOp(&node->get._CUnary.unop, tab);
            print_CExp(ctx, node->get._CUnary.exp, tab);
            break;
        case AST_CBinary_t:
            print_field(++tab, "CBinary: ");
            print_CBinaryOp(&node->get._CBinary.binop, tab);
            print_CExp(ctx, node->get._CBinary.exp_left, tab);
            print_CExp(ctx, node->get._CBinary.exp_right, tab);
            break;
        case AST_CAssignment_t:
            print_field(++tab, "CAssignment: ");
            print_CUnaryOp(&node->get._CAssignment.unop, tab);
            print_CExp(ctx, node->get._CAssignment.exp_left, tab);
            print_CExp(ctx, node->get._CAssignment.exp_right, tab);
            break;
        case AST_CConditional_t:
            print_field(++tab, "CConditional: ");
            print_CExp(ctx, node->get._CConditional.condition, tab);
            print_CExp(ctx, node->get._CConditional.exp_middle, tab);
            print_CExp(ctx, node->get._CConditional.exp_right, tab);
            break;
        case AST_CFunctionCall_t:
            print_field(++tab, "CFunctionCall: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._CFunctionCall.name));
            print_field(tab + 1, "List[%zu]: ", vec_size(node->get._CFunctionCall.args));
            for (size_t i = 0; i < vec_size(node->get._CFunctionCall.args); ++i) {
                print_CExp(ctx, node->get._CFunctionCall.args[i], tab + 1);
            }
            break;
        case AST_CDereference_t:
            print_field(++tab, "CDereference: ");
            print_CExp(ctx, node->get._CDereference.exp, tab);
            break;
        case AST_CAddrOf_t:
            print_field(++tab, "CAddrOf: ");
            print_CExp(ctx, node->get._CAddrOf.exp, tab);
            break;
        case AST_CSubscript_t:
            print_field(++tab, "CSubscript: ");
            print_CExp(ctx, node->get._CSubscript.primary_exp, tab);
            print_CExp(ctx, node->get._CSubscript.subscript_exp, tab);
            break;
        case AST_CSizeOf_t:
            print_field(++tab, "CSizeOf: ");
            print_CExp(ctx, node->get._CSizeOf.exp, tab);
            break;
        case AST_CSizeOfT_t:
            print_field(++tab, "CSizeOfT: ");
            print_Type(ctx, node->get._CSizeOfT.target_type, tab);
            break;
        case AST_CDot_t:
            print_field(++tab, "CDot: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._CDot.member));
            print_CExp(ctx, node->get._CDot.structure, tab);
            break;
        case AST_CArrow_t:
            print_field(++tab, "CArrow: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._CArrow.member));
            print_CExp(ctx, node->get._CArrow.pointer, tab);
            break;
        default:
            THROW_ABORT;
    }
    print_Type(ctx, node->exp_type, tab);
}

static void print_CStatement(Ctx ctx, const CStatement* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
        case AST_CStatement_t:
            print_field(++tab, "CStatement: ");
            break;
        case AST_CReturn_t:
            print_field(++tab, "CReturn: ");
            print_CExp(ctx, node->get._CReturn.exp, tab);
            break;
        case AST_CExpression_t:
            print_field(++tab, "CExpression: ");
            print_CExp(ctx, node->get._CExpression.exp, tab);
            break;
        case AST_CIf_t:
            print_field(++tab, "CIf: ");
            print_CExp(ctx, node->get._CIf.condition, tab);
            print_CStatement(ctx, node->get._CIf.then, tab);
            print_CStatement(ctx, node->get._CIf.else_fi, tab);
            break;
        case AST_CGoto_t:
            print_field(++tab, "CGoto: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._CGoto.target));
            break;
        case AST_CLabel_t:
            print_field(++tab, "CLabel: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._CLabel.target));
            print_CStatement(ctx, node->get._CLabel.jump_to, tab);
            break;
        case AST_CCompound_t:
            print_field(++tab, "CCompound: ");
            print_CBlock(ctx, node->get._CCompound.block, tab);
            break;
        case AST_CWhile_t:
            print_field(++tab, "CWhile: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._CWhile.target));
            print_CExp(ctx, node->get._CWhile.condition, tab);
            print_CStatement(ctx, node->get._CWhile.body, tab);
            break;
        case AST_CDoWhile_t:
            print_field(++tab, "CDoWhile: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._CDoWhile.target));
            print_CExp(ctx, node->get._CDoWhile.condition, tab);
            print_CStatement(ctx, node->get._CDoWhile.body, tab);
            break;
        case AST_CFor_t:
            print_field(++tab, "CFor: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._CFor.target));
            print_CForInit(ctx, node->get._CFor.init, tab);
            print_CExp(ctx, node->get._CFor.condition, tab);
            print_CExp(ctx, node->get._CFor.post, tab);
            print_CStatement(ctx, node->get._CFor.body, tab);
            break;
        case AST_CSwitch_t:
            print_field(++tab, "CSwitch: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._CSwitch.target));
            print_field(tab + 1, "Bool: %s", node->get._CSwitch.is_default ? "1" : "0");
            print_CExp(ctx, node->get._CSwitch.match, tab);
            print_CStatement(ctx, node->get._CSwitch.body, tab);
            print_field(tab + 1, "List[%zu]: ", vec_size(node->get._CSwitch.cases));
            for (size_t i = 0; i < vec_size(node->get._CSwitch.cases); ++i) {
                print_CExp(ctx, node->get._CSwitch.cases[i], tab + 1);
            }
            break;
        case AST_CCase_t:
            print_field(++tab, "CCase: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._CCase.target));
            print_CExp(ctx, node->get._CCase.value, tab);
            print_CStatement(ctx, node->get._CCase.jump_to, tab);
            break;
        case AST_CDefault_t:
            print_field(++tab, "CDefault: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._CDefault.target));
            print_CStatement(ctx, node->get._CDefault.jump_to, tab);
            break;
        case AST_CBreak_t:
            print_field(++tab, "CBreak: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._CBreak.target));
            break;
        case AST_CContinue_t:
            print_field(++tab, "CContinue: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._CContinue.target));
            break;
        case AST_CNull_t:
            print_field(++tab, "CNull: ");
            break;
        default:
            THROW_ABORT;
    }
}

static void print_CForInit(Ctx ctx, const CForInit* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
        case AST_CForInit_t:
            print_field(++tab, "CForInit: ");
            break;
        case AST_CInitDecl_t:
            print_field(++tab, "CInitDecl: ");
            print_CVariableDeclaration(ctx, node->get._CInitDecl.init, tab);
            break;
        case AST_CInitExp_t:
            print_field(++tab, "CInitExp: ");
            print_CExp(ctx, node->get._CInitExp.init, tab);
            break;
        default:
            THROW_ABORT;
    }
}

static void print_CBlock(Ctx ctx, const CBlock* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
        case AST_CBlock_t:
            print_field(++tab, "CBlock: ");
            break;
        case AST_CB_t:
            print_field(++tab, "CB: ");
            print_field(tab + 1, "List[%zu]: ", vec_size(node->get._CB.block_items));
            for (size_t i = 0; i < vec_size(node->get._CB.block_items); ++i) {
                print_CBlockItem(ctx, node->get._CB.block_items[i], tab + 1);
            }
            break;
        default:
            THROW_ABORT;
    }
}

static void print_CBlockItem(Ctx ctx, const CBlockItem* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
        case AST_CBlockItem_t:
            print_field(++tab, "CBlockItem: ");
            break;
        case AST_CS_t:
            print_field(++tab, "CS: ");
            print_CStatement(ctx, node->get._CS.statement, tab);
            break;
        case AST_CD_t:
            print_field(++tab, "CD: ");
            print_CDeclaration(ctx, node->get._CD.declaration, tab);
            break;
        default:
            THROW_ABORT;
    }
}

static void print_CStorageClass(const CStorageClass* node, size_t tab) {
    print_null((node->type != AST_CStorageClass_t), ++tab); // TODO remove
    print_null(node, ++tab);
    switch (node->type) {
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

static void print_CInitializer(Ctx ctx, const CInitializer* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
        case AST_CInitializer_t:
            print_field(++tab, "CInitializer: ");
            break;
        case AST_CSingleInit_t:
            print_field(++tab, "CSingleInit: ");
            print_CExp(ctx, node->get._CSingleInit.exp, tab);
            break;
        case AST_CCompoundInit_t:
            print_field(++tab, "CCompoundInit: ");
            print_field(tab + 1, "List[%zu]: ", vec_size(node->get._CCompoundInit.initializers));
            for (size_t i = 0; i < vec_size(node->get._CCompoundInit.initializers); ++i) {
                print_CInitializer(ctx, node->get._CCompoundInit.initializers[i], tab + 1);
            }
            break;
        default:
            THROW_ABORT;
    }
    print_Type(ctx, node->init_type, tab);
}

static void print_CMemberDeclaration(Ctx ctx, const CMemberDeclaration* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
        case AST_CMemberDeclaration_t:
            print_field(++tab, "CMemberDeclaration: ");
            break;
        default:
            THROW_ABORT;
    }
    print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->member_name));
    print_Type(ctx, node->member_type, tab);
}

static void print_CStructDeclaration(Ctx ctx, const CStructDeclaration* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
        case AST_CStructDeclaration_t:
            print_field(++tab, "CStructDeclaration: ");
            break;
        default:
            THROW_ABORT;
    }
    print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->tag));
    print_field(tab + 1, "Bool: %s", node->is_union ? "1" : "0");
    print_field(tab + 1, "List[%zu]: ", vec_size(node->members));
    for (size_t i = 0; i < vec_size(node->members); ++i) {
        print_CMemberDeclaration(ctx, node->members[i], tab + 1);
    }
}

static void print_CFunctionDeclaration(Ctx ctx, const CFunctionDeclaration* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
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
    print_CBlock(ctx, node->body, tab);
    print_Type(ctx, node->fun_type, tab);
    print_CStorageClass(&node->storage_class, tab);
}

static void print_CVariableDeclaration(Ctx ctx, const CVariableDeclaration* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
        case AST_CVariableDeclaration_t:
            print_field(++tab, "CVariableDeclaration: ");
            break;
        default:
            THROW_ABORT;
    }
    print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->name));
    print_CInitializer(ctx, node->init, tab);
    print_Type(ctx, node->var_type, tab);
    print_CStorageClass(&node->storage_class, tab);
}

static void print_CDeclaration(Ctx ctx, const CDeclaration* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
        case AST_CDeclaration_t:
            print_field(++tab, "CDeclaration: ");
            break;
        case AST_CFunDecl_t:
            print_field(++tab, "CFunDecl: ");
            print_CFunctionDeclaration(ctx, node->get._CFunDecl.fun_decl, tab);
            break;
        case AST_CVarDecl_t:
            print_field(++tab, "CVarDecl: ");
            print_CVariableDeclaration(ctx, node->get._CVarDecl.var_decl, tab);
            break;
        case AST_CStructDecl_t:
            print_field(++tab, "CStructDecl: ");
            print_CStructDeclaration(ctx, node->get._CStructDecl.struct_decl, tab);
            break;
        default:
            THROW_ABORT;
    }
}

static void print_CProgram(Ctx ctx, const CProgram* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
        case AST_CProgram_t:
            print_field(++tab, "CProgram: ");
            break;
        default:
            THROW_ABORT;
    }
    print_field(tab + 1, "List[%zu]: ", vec_size(node->declarations));
    for (size_t i = 0; i < vec_size(node->declarations); ++i) {
        print_CDeclaration(ctx, node->declarations[i], tab + 1);
    }
}

static void print_TacUnaryOp(const TacUnaryOp* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
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

static void print_TacBinaryOp(const TacBinaryOp* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
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

static void print_TacValue(Ctx ctx, const TacValue* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
        case AST_TacValue_t:
            print_field(++tab, "TacValue: ");
            break;
        case AST_TacConstant_t:
            print_field(++tab, "TacConstant: ");
            print_CConst(node->get._TacConstant.constant, tab);
            break;
        case AST_TacVariable_t:
            print_field(++tab, "TacVariable: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._TacVariable.name));
            break;
        default:
            THROW_ABORT;
    }
}

static void print_TacInstruction(Ctx ctx, const TacInstruction* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
        case AST_TacInstruction_t:
            print_field(++tab, "TacInstruction: ");
            break;
        case AST_TacReturn_t:
            print_field(++tab, "TacReturn: ");
            print_TacValue(ctx, node->get._TacReturn.val, tab);
            break;
        case AST_TacSignExtend_t:
            print_field(++tab, "TacSignExtend: ");
            print_TacValue(ctx, node->get._TacSignExtend.src, tab);
            print_TacValue(ctx, node->get._TacSignExtend.dst, tab);
            break;
        case AST_TacTruncate_t:
            print_field(++tab, "TacTruncate: ");
            print_TacValue(ctx, node->get._TacTruncate.src, tab);
            print_TacValue(ctx, node->get._TacTruncate.dst, tab);
            break;
        case AST_TacZeroExtend_t:
            print_field(++tab, "TacZeroExtend: ");
            print_TacValue(ctx, node->get._TacZeroExtend.src, tab);
            print_TacValue(ctx, node->get._TacZeroExtend.dst, tab);
            break;
        case AST_TacDoubleToInt_t:
            print_field(++tab, "TacDoubleToInt: ");
            print_TacValue(ctx, node->get._TacDoubleToInt.src, tab);
            print_TacValue(ctx, node->get._TacDoubleToInt.dst, tab);
            break;
        case AST_TacDoubleToUInt_t:
            print_field(++tab, "TacDoubleToUInt: ");
            print_TacValue(ctx, node->get._TacDoubleToUInt.src, tab);
            print_TacValue(ctx, node->get._TacDoubleToUInt.dst, tab);
            break;
        case AST_TacIntToDouble_t:
            print_field(++tab, "TacIntToDouble: ");
            print_TacValue(ctx, node->get._TacIntToDouble.src, tab);
            print_TacValue(ctx, node->get._TacIntToDouble.dst, tab);
            break;
        case AST_TacUIntToDouble_t:
            print_field(++tab, "TacUIntToDouble: ");
            print_TacValue(ctx, node->get._TacUIntToDouble.src, tab);
            print_TacValue(ctx, node->get._TacUIntToDouble.dst, tab);
            break;
        case AST_TacFunCall_t:
            print_field(++tab, "TacFunCall: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._TacFunCall.name));
            print_field(tab + 1, "List[%zu]: ", vec_size(node->get._TacFunCall.args));
            for (size_t i = 0; i < vec_size(node->get._TacFunCall.args); ++i) {
                print_TacValue(ctx, node->get._TacFunCall.args[i], tab + 1);
            }
            print_TacValue(ctx, node->get._TacFunCall.dst, tab);
            break;
        case AST_TacUnary_t:
            print_field(++tab, "TacUnary: ");
            print_TacUnaryOp(&node->get._TacUnary.unop, tab);
            print_TacValue(ctx, node->get._TacUnary.src, tab);
            print_TacValue(ctx, node->get._TacUnary.dst, tab);
            break;
        case AST_TacBinary_t:
            print_field(++tab, "TacBinary: ");
            print_TacBinaryOp(&node->get._TacBinary.binop, tab);
            print_TacValue(ctx, node->get._TacBinary.src1, tab);
            print_TacValue(ctx, node->get._TacBinary.src2, tab);
            print_TacValue(ctx, node->get._TacBinary.dst, tab);
            break;
        case AST_TacCopy_t:
            print_field(++tab, "TacCopy: ");
            print_TacValue(ctx, node->get._TacCopy.src, tab);
            print_TacValue(ctx, node->get._TacCopy.dst, tab);
            break;
        case AST_TacGetAddress_t:
            print_field(++tab, "TacGetAddress: ");
            print_TacValue(ctx, node->get._TacGetAddress.src, tab);
            print_TacValue(ctx, node->get._TacGetAddress.dst, tab);
            break;
        case AST_TacLoad_t:
            print_field(++tab, "TacLoad: ");
            print_TacValue(ctx, node->get._TacLoad.src_ptr, tab);
            print_TacValue(ctx, node->get._TacLoad.dst, tab);
            break;
        case AST_TacStore_t:
            print_field(++tab, "TacStore: ");
            print_TacValue(ctx, node->get._TacStore.src, tab);
            print_TacValue(ctx, node->get._TacStore.dst_ptr, tab);
            break;
        case AST_TacAddPtr_t:
            print_field(++tab, "TacAddPtr: ");
            print_field(tab + 1, "TLong: %zi", (ssize_t)node->get._TacAddPtr.scale);
            print_TacValue(ctx, node->get._TacAddPtr.src_ptr, tab);
            print_TacValue(ctx, node->get._TacAddPtr.idx, tab);
            print_TacValue(ctx, node->get._TacAddPtr.dst, tab);
            break;
        case AST_TacCopyToOffset_t:
            print_field(++tab, "TacCopyToOffset: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._TacCopyToOffset.dst_name));
            print_field(tab + 1, "TLong: %zi", (ssize_t)node->get._TacCopyToOffset.offset);
            print_TacValue(ctx, node->get._TacCopyToOffset.src, tab);
            break;
        case AST_TacCopyFromOffset_t:
            print_field(++tab, "TacCopyFromOffset: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._TacCopyFromOffset.src_name));
            print_field(tab + 1, "TLong: %zi", (ssize_t)node->get._TacCopyFromOffset.offset);
            print_TacValue(ctx, node->get._TacCopyFromOffset.dst, tab);
            break;
        case AST_TacJump_t:
            print_field(++tab, "TacJump: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._TacJump.target));
            break;
        case AST_TacJumpIfZero_t:
            print_field(++tab, "TacJumpIfZero: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._TacJumpIfZero.target));
            print_TacValue(ctx, node->get._TacJumpIfZero.condition, tab);
            break;
        case AST_TacJumpIfNotZero_t:
            print_field(++tab, "TacJumpIfNotZero: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._TacJumpIfNotZero.target));
            print_TacValue(ctx, node->get._TacJumpIfNotZero.condition, tab);
            break;
        case AST_TacLabel_t:
            print_field(++tab, "TacLabel: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._TacLabel.name));
            break;
        default:
            THROW_ABORT;
    }
}

static void print_TacTopLevel(Ctx ctx, const TacTopLevel* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
        case AST_TacTopLevel_t:
            print_field(++tab, "TacTopLevel: ");
            break;
        case AST_TacFunction_t:
            print_field(++tab, "TacFunction: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._TacFunction.name));
            print_field(tab + 1, "Bool: %s", node->get._TacFunction.is_glob ? "1" : "0");
            print_field(tab + 1, "List[%zu]: ", vec_size(node->get._TacFunction.params));
            for (size_t i = 0; i < vec_size(node->get._TacFunction.params); ++i) {
                print_field(tab + 2, "TIdentifier: %s", map_get(ctx->hash_table, node->get._TacFunction.params[i]));
            }
            print_field(tab + 1, "List[%zu]: ", vec_size(node->get._TacFunction.body));
            for (size_t i = 0; i < vec_size(node->get._TacFunction.body); ++i) {
                print_TacInstruction(ctx, node->get._TacFunction.body[i], tab + 1);
            }
            break;
        case AST_TacStaticVariable_t:
            print_field(++tab, "TacStaticVariable: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._TacStaticVariable.name));
            print_field(tab + 1, "Bool: %s", node->get._TacStaticVariable.is_glob ? "1" : "0");
            print_Type(ctx, node->get._TacStaticVariable.static_init_type, tab);
            print_field(tab + 1, "List[%zu]: ", vec_size(node->get._TacStaticVariable.static_inits));
            for (size_t i = 0; i < vec_size(node->get._TacStaticVariable.static_inits); ++i) {
                print_StaticInit(ctx, node->get._TacStaticVariable.static_inits[i], tab + 1);
            }
            break;
        case AST_TacStaticConstant_t:
            print_field(++tab, "TacStaticConstant: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._TacStaticConstant.name));
            print_Type(ctx, node->get._TacStaticConstant.static_init_type, tab);
            print_StaticInit(ctx, node->get._TacStaticConstant.static_init, tab);
            break;
        default:
            THROW_ABORT;
    }
}

static void print_TacProgram(Ctx ctx, const TacProgram* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
        case AST_TacProgram_t:
            print_field(++tab, "TacProgram: ");
            break;
        default:
            THROW_ABORT;
    }
    print_field(tab + 1, "List[%zu]: ", vec_size(node->static_const_toplvls));
    for (size_t i = 0; i < vec_size(node->static_const_toplvls); ++i) {
        print_TacTopLevel(ctx, node->static_const_toplvls[i], tab + 1);
    }
    print_field(tab + 1, "List[%zu]: ", vec_size(node->static_var_toplvls));
    for (size_t i = 0; i < vec_size(node->static_var_toplvls); ++i) {
        print_TacTopLevel(ctx, node->static_var_toplvls[i], tab + 1);
    }
    print_field(tab + 1, "List[%zu]: ", vec_size(node->fun_toplvls));
    for (size_t i = 0; i < vec_size(node->fun_toplvls); ++i) {
        print_TacTopLevel(ctx, node->fun_toplvls[i], tab + 1);
    }
}

static void print_AssemblyType(const AssemblyType* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
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
            print_field(tab + 1, "TLong: %zi", (ssize_t)node->get._ByteArray.size);
            print_field(tab + 1, "TInt: %i", node->get._ByteArray.alignment);
            break;
        default:
            THROW_ABORT;
    }
}

static void print_BackendSymbol(Ctx ctx, const BackendSymbol* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
        case AST_BackendSymbol_t:
            print_field(++tab, "BackendSymbol: ");
            break;
        case AST_BackendObj_t:
            print_field(++tab, "BackendObj: ");
            print_field(tab + 1, "Bool: %s", node->get._BackendObj.is_static ? "1" : "0");
            print_field(tab + 1, "Bool: %s", node->get._BackendObj.is_const ? "1" : "0");
            print_AssemblyType(node->get._BackendObj.asm_type, tab);
            break;
        case AST_BackendFun_t:
            print_field(++tab, "BackendFun: ");
            print_field(tab + 1, "Bool: %s", node->get._BackendFun.is_def ? "1" : "0");
            print_field(tab + 1, "List[%zu]: ", vec_size(node->get._BackendFun.callee_saved_regs));
            for (size_t i = 0; i < vec_size(node->get._BackendFun.callee_saved_regs); ++i) {
                print_AsmOperand(ctx, node->get._BackendFun.callee_saved_regs[i], tab + 1);
            }
            break;
        default:
            THROW_ABORT;
    }
}

static void print_AsmReg(const AsmReg* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
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

static void print_AsmCondCode(const AsmCondCode* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
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

static void print_AsmOperand(Ctx ctx, const AsmOperand* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
        case AST_AsmOperand_t:
            print_field(++tab, "AsmOperand: ");
            break;
        case AST_AsmImm_t:
            print_field(++tab, "AsmImm: ");
            print_field(tab + 1, "TULong: %zu", (size_t)node->get._AsmImm.value);
            print_field(tab + 1, "Bool: %s", node->get._AsmImm.is_byte ? "1" : "0");
            print_field(tab + 1, "Bool: %s", node->get._AsmImm.is_quad ? "1" : "0");
            print_field(tab + 1, "Bool: %s", node->get._AsmImm.is_neg ? "1" : "0");
            break;
        case AST_AsmRegister_t:
            print_field(++tab, "AsmRegister: ");
            print_AsmReg(&node->get._AsmRegister.reg, tab);
            break;
        case AST_AsmPseudo_t:
            print_field(++tab, "AsmPseudo: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._AsmPseudo.name));
            break;
        case AST_AsmMemory_t:
            print_field(++tab, "AsmMemory: ");
            print_field(tab + 1, "TLong: %zi", (ssize_t)node->get._AsmMemory.value);
            print_AsmReg(&node->get._AsmMemory.reg, tab);
            break;
        case AST_AsmData_t:
            print_field(++tab, "AsmData: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._AsmData.name));
            print_field(tab + 1, "TLong: %zi", (ssize_t)node->get._AsmData.offset);
            break;
        case AST_AsmPseudoMem_t:
            print_field(++tab, "AsmPseudoMem: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._AsmPseudoMem.name));
            print_field(tab + 1, "TLong: %zi", (ssize_t)node->get._AsmPseudoMem.offset);
            break;
        case AST_AsmIndexed_t:
            print_field(++tab, "AsmIndexed: ");
            print_field(tab + 1, "TLong: %zi", (ssize_t)node->get._AsmIndexed.scale);
            print_AsmReg(&node->get._AsmIndexed.reg_base, tab);
            print_AsmReg(&node->get._AsmIndexed.reg_index, tab);
            break;
        default:
            THROW_ABORT;
    }
}

static void print_AsmBinaryOp(const AsmBinaryOp* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
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

static void print_AsmUnaryOp(const AsmUnaryOp* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
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

static void print_AsmInstruction(Ctx ctx, const AsmInstruction* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
        case AST_AsmInstruction_t:
            print_field(++tab, "AsmInstruction: ");
            break;
        case AST_AsmMov_t:
            print_field(++tab, "AsmMov: ");
            print_AssemblyType(node->get._AsmMov.asm_type, tab);
            print_AsmOperand(ctx, node->get._AsmMov.src, tab);
            print_AsmOperand(ctx, node->get._AsmMov.dst, tab);
            break;
        case AST_AsmMovSx_t:
            print_field(++tab, "AsmMovSx: ");
            print_AssemblyType(node->get._AsmMovSx.asm_type_src, tab);
            print_AssemblyType(node->get._AsmMovSx.asm_type_dst, tab);
            print_AsmOperand(ctx, node->get._AsmMovSx.src, tab);
            print_AsmOperand(ctx, node->get._AsmMovSx.dst, tab);
            break;
        case AST_AsmMovZeroExtend_t:
            print_field(++tab, "AsmMovZeroExtend: ");
            print_AssemblyType(node->get._AsmMovZeroExtend.asm_type_src, tab);
            print_AssemblyType(node->get._AsmMovZeroExtend.asm_type_dst, tab);
            print_AsmOperand(ctx, node->get._AsmMovZeroExtend.src, tab);
            print_AsmOperand(ctx, node->get._AsmMovZeroExtend.dst, tab);
            break;
        case AST_AsmLea_t:
            print_field(++tab, "AsmLea: ");
            print_AsmOperand(ctx, node->get._AsmLea.src, tab);
            print_AsmOperand(ctx, node->get._AsmLea.dst, tab);
            break;
        case AST_AsmCvttsd2si_t:
            print_field(++tab, "AsmCvttsd2si: ");
            print_AssemblyType(node->get._AsmCvttsd2si.asm_type, tab);
            print_AsmOperand(ctx, node->get._AsmCvttsd2si.src, tab);
            print_AsmOperand(ctx, node->get._AsmCvttsd2si.dst, tab);
            break;
        case AST_AsmCvtsi2sd_t:
            print_field(++tab, "AsmCvtsi2sd: ");
            print_AssemblyType(node->get._AsmCvtsi2sd.asm_type, tab);
            print_AsmOperand(ctx, node->get._AsmCvtsi2sd.src, tab);
            print_AsmOperand(ctx, node->get._AsmCvtsi2sd.dst, tab);
            break;
        case AST_AsmUnary_t:
            print_field(++tab, "AsmUnary: ");
            print_AsmUnaryOp(&node->get._AsmUnary.unop, tab);
            print_AssemblyType(node->get._AsmUnary.asm_type, tab);
            print_AsmOperand(ctx, node->get._AsmUnary.dst, tab);
            break;
        case AST_AsmBinary_t:
            print_field(++tab, "AsmBinary: ");
            print_AsmBinaryOp(&node->get._AsmBinary.binop, tab);
            print_AssemblyType(node->get._AsmBinary.asm_type, tab);
            print_AsmOperand(ctx, node->get._AsmBinary.src, tab);
            print_AsmOperand(ctx, node->get._AsmBinary.dst, tab);
            break;
        case AST_AsmCmp_t:
            print_field(++tab, "AsmCmp: ");
            print_AssemblyType(node->get._AsmCmp.asm_type, tab);
            print_AsmOperand(ctx, node->get._AsmCmp.src, tab);
            print_AsmOperand(ctx, node->get._AsmCmp.dst, tab);
            break;
        case AST_AsmIdiv_t:
            print_field(++tab, "AsmIdiv: ");
            print_AssemblyType(node->get._AsmIdiv.asm_type, tab);
            print_AsmOperand(ctx, node->get._AsmIdiv.src, tab);
            break;
        case AST_AsmDiv_t:
            print_field(++tab, "AsmDiv: ");
            print_AssemblyType(node->get._AsmDiv.asm_type, tab);
            print_AsmOperand(ctx, node->get._AsmDiv.src, tab);
            break;
        case AST_AsmCdq_t:
            print_field(++tab, "AsmCdq: ");
            print_AssemblyType(node->get._AsmCdq.asm_type, tab);
            break;
        case AST_AsmJmp_t:
            print_field(++tab, "AsmJmp: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._AsmJmp.target));
            break;
        case AST_AsmJmpCC_t:
            print_field(++tab, "AsmJmpCC: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._AsmJmpCC.target));
            print_AsmCondCode(&node->get._AsmJmpCC.cond_code, tab);
            break;
        case AST_AsmSetCC_t:
            print_field(++tab, "AsmSetCC: ");
            print_AsmCondCode(&node->get._AsmSetCC.cond_code, tab);
            print_AsmOperand(ctx, node->get._AsmSetCC.dst, tab);
            break;
        case AST_AsmLabel_t:
            print_field(++tab, "AsmLabel: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._AsmLabel.name));
            break;
        case AST_AsmPush_t:
            print_field(++tab, "AsmPush: ");
            print_AsmOperand(ctx, node->get._AsmPush.src, tab);
            break;
        case AST_AsmPop_t:
            print_field(++tab, "AsmPop: ");
            print_AsmReg(&node->get._AsmPop.reg, tab);
            break;
        case AST_AsmCall_t:
            print_field(++tab, "AsmCall: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._AsmCall.name));
            break;
        case AST_AsmRet_t:
            print_field(++tab, "AsmRet: ");
            break;
        default:
            THROW_ABORT;
    }
}

static void print_AsmTopLevel(Ctx ctx, const AsmTopLevel* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
        case AST_AsmTopLevel_t:
            print_field(++tab, "AsmTopLevel: ");
            break;
        case AST_AsmFunction_t:
            print_field(++tab, "AsmFunction: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._AsmFunction.name));
            print_field(tab + 1, "Bool: %s", node->get._AsmFunction.is_glob ? "1" : "0");
            print_field(tab + 1, "Bool: %s", node->get._AsmFunction.is_ret_memory ? "1" : "0");
            print_field(tab + 1, "List[%zu]: ", vec_size(node->get._AsmFunction.instructions));
            for (size_t i = 0; i < vec_size(node->get._AsmFunction.instructions); ++i) {
                print_AsmInstruction(ctx, node->get._AsmFunction.instructions[i], tab + 1);
            }
            break;
        case AST_AsmStaticVariable_t:
            print_field(++tab, "AsmStaticVariable: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._AsmStaticVariable.name));
            print_field(tab + 1, "TInt: %i", node->get._AsmStaticVariable.alignment);
            print_field(tab + 1, "Bool: %s", node->get._AsmStaticVariable.is_glob ? "1" : "0");
            print_field(tab + 1, "List[%zu]: ", vec_size(node->get._AsmStaticVariable.static_inits));
            for (size_t i = 0; i < vec_size(node->get._AsmStaticVariable.static_inits); ++i) {
                print_StaticInit(ctx, node->get._AsmStaticVariable.static_inits[i], tab + 1);
            }
            break;
        case AST_AsmStaticConstant_t:
            print_field(++tab, "AsmStaticConstant: ");
            print_field(tab + 1, "TIdentifier: %s", map_get(ctx->hash_table, node->get._AsmStaticConstant.name));
            print_field(tab + 1, "TInt: %i", node->get._AsmStaticConstant.alignment);
            print_StaticInit(ctx, node->get._AsmStaticConstant.static_init, tab);
            break;
        default:
            THROW_ABORT;
    }
}

static void print_AsmProgram(Ctx ctx, const AsmProgram* node, size_t tab) {
    print_null(node, ++tab);
    switch (node->type) {
        case AST_AsmProgram_t:
            print_field(++tab, "AsmProgram: ");
            break;
        default:
            THROW_ABORT;
    }
    print_field(tab + 1, "List[%zu]: ", vec_size(node->static_const_toplvls));
    for (size_t i = 0; i < vec_size(node->static_const_toplvls); ++i) {
        print_AsmTopLevel(ctx, node->static_const_toplvls[i], tab + 1);
    }
    print_field(tab + 1, "List[%zu]: ", vec_size(node->top_levels));
    for (size_t i = 0; i < vec_size(node->top_levels); ++i) {
        print_AsmTopLevel(ctx, node->top_levels[i], tab + 1);
    }
}

void pprint_c_ast(IdentifierContext* ctx, CProgram* node) {
    print_title("C AST");
    print_CProgram(ctx, node, 0);
    printf("\n");
}

void pprint_tac_ast(IdentifierContext* ctx, TacProgram* node) {
    print_title("TAC AST");
    print_TacProgram(ctx, node, 0);
    printf("\n");
}

void pprint_asm_ast(IdentifierContext* ctx, AsmProgram* node) {
    print_title("ASM AST");
    print_AsmProgram(ctx, node, 0);
    printf("\n");
}

void pprint_addressed_set(IdentifierContext* ctx, FrontEndContext* frontend) {
    print_title("Addressed Set");
    printf("\nSet(%zu):", set_size(frontend->addressed_set));
    for (size_t i = 0; i < set_size(frontend->addressed_set); ++i) {
        TIdentifier name = element_get(frontend->addressed_set[i]);
        print_field(2, ": %s", map_get(ctx->hash_table, name));
    }
    printf("\n");
}

void pprint_string_const_table(IdentifierContext* ctx, FrontEndContext* frontend) {
    print_title("String Constant Table");
    printf("\nDict(%zu):", map_size(frontend->string_const_table));
    for (size_t i = 0; i < map_size(frontend->string_const_table); ++i) {
        const pair_t(TIdentifier, TIdentifier)* static_const = &frontend->string_const_table[i];
        print_field(2, "[%s]: ", map_get(ctx->hash_table, pair_first(*static_const)));
        ssize_t map_it = map_find(frontend->symbol_table, pair_second(*static_const));
        if (map_it != map_end() && pair_second(frontend->symbol_table[map_it])->attrs->type == AST_ConstantAttr_t) {
            ConstantAttr* constant_attr = &pair_second(frontend->symbol_table[map_it])->attrs->get._ConstantAttr;
            if (constant_attr->static_init->type == AST_StringInit_t) {
                printf("\n    String: \"");
                StringInit* string_init = &constant_attr->static_init->get._StringInit;
                for (size_t i = 0; i < vec_size(string_init->literal->value); ++i) {
                    TChar byte = string_init->literal->value[i];
                    switch (byte) {
                        case 39:
                            printf("\\'");
                            break;
                        case 34:
                            printf("\\\"");
                            break;
                        case 63:
                            printf("\\?");
                            break;
                        case 92:
                            printf("\\\\");
                            break;
                        case 7:
                            printf("\\a");
                            break;
                        case 8:
                            printf("\\b");
                            break;
                        case 12:
                            printf("\\f");
                            break;
                        case 10:
                            printf("\\n");
                            break;
                        case 13:
                            printf("\\r");
                            break;
                        case 9:
                            printf("\\t");
                            break;
                        case 11:
                            printf("\\v");
                            break;
                        default:
                            printf("%c", (char)byte);
                            break;
                    }
                }
                printf("\"");
                continue;
            }
        }
        THROW_ABORT;
    }
    printf("\n");
}

void pprint_struct_typedef_table(IdentifierContext* ctx, FrontEndContext* frontend) {
    print_title("Structure Typedef Table");
    printf("\nDict(%zu):", map_size(frontend->struct_typedef_table));
    for (size_t i = 0; i < map_size(frontend->struct_typedef_table); ++i) {
        const pair_t(TIdentifier, UPtrStructTypedef)* struct_typedef = &frontend->struct_typedef_table[i];
        print_field(2, "[%s]: ", map_get(ctx->hash_table, pair_first(*struct_typedef)));
        print_StructTypedef(ctx, pair_second(*struct_typedef), 2);
    }
    printf("\n");
}

void pprint_symbol_table(IdentifierContext* ctx, FrontEndContext* frontend) {
    print_title("Symbol Table");
    printf("\nDict(%zu):", map_size(frontend->symbol_table));
    for (size_t i = 0; i < map_size(frontend->symbol_table); ++i) {
        const pair_t(TIdentifier, UPtrSymbol)* symbol = &frontend->symbol_table[i];
        print_field(2, "[%s]: ", map_get(ctx->hash_table, pair_first(*symbol)));
        print_Symbol(ctx, pair_second(*symbol), 2);
    }
    printf("\n");
}

void pprint_backend_symbol_table(IdentifierContext* ctx, BackEndContext* backend) {
    print_title("Backend Symbol Table");
    printf("\nDict(%zu):", map_size(backend->symbol_table));
    for (size_t i = 0; i < map_size(backend->symbol_table); ++i) {
        const pair_t(TIdentifier, UPtrBackendSymbol)* bakend_symbol = &backend->symbol_table[i];
        print_field(2, "[%s]: ", map_get(ctx->hash_table, pair_first(*bakend_symbol)));
        print_BackendSymbol(ctx, pair_second(*bakend_symbol), 2);
    }
    printf("\n");
}
#endif
