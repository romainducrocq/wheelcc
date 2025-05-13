#include <string>

#include "util/str2t.hpp"
#include "util/throw.hpp"

#include "ast/ast.hpp"
#include "ast/front_ast.hpp"

#include "frontend/intermediate/idents.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Identifiers

typedef IdentifierContext* Ctx;

TIdentifier rslv_label_identifier(Ctx ctx, TIdentifier label) {
    std::string name = ctx->hash_table[label];
    return make_label_identifier(ctx, std::move(name));
}

TIdentifier rslv_var_identifier(Ctx ctx, TIdentifier variable) {
    std::string name = ctx->hash_table[variable];
    return make_var_identifier(ctx, std::move(name));
}

TIdentifier rslv_struct_tag(Ctx ctx, TIdentifier structure) {
    std::string name = ctx->hash_table[structure];
    return make_struct_identifier(ctx, std::move(name));
}

TIdentifier repr_label_identifier(Ctx ctx, LABEL_KIND label_kind) {
    std::string name;
    switch (label_kind) {
        case LBL_Land_false: {
            name = "and_false";
            break;
        }
        case LBL_Land_true: {
            name = "and_true";
            break;
        }
        case LBL_Ldo_while: {
            name = "do_while";
            break;
        }
        case LBL_Ldo_while_start: {
            name = "do_while_start";
            break;
        }
        case LBL_Lfor: {
            name = "for";
            break;
        }
        case LBL_Lswitch: {
            name = "switch";
            break;
        }
        case LBL_Lfor_start: {
            name = "for_start";
            break;
        }
        case LBL_Lif_else: {
            name = "if_else";
            break;
        }
        case LBL_Lif_false: {
            name = "if_false";
            break;
        }
        case LBL_Lor_false: {
            name = "or_false";
            break;
        }
        case LBL_Lor_true: {
            name = "or_true";
            break;
        }
        case LBL_Lstring: {
            name = "string";
            break;
        }
        case LBL_Lternary_else: {
            name = "ternary_else";
            break;
        }
        case LBL_Lternary_false: {
            name = "ternary_false";
            break;
        }
        case LBL_Lwhile: {
            name = "while";
            break;
        }
        default:
            THROW_ABORT;
    }
    return make_label_identifier(ctx, std::move(name));
}

TIdentifier repr_loop_identifier(Ctx ctx, LABEL_KIND label_kind, TIdentifier target) {
    std::string name;
    switch (label_kind) {
        case LBL_Lbreak: {
            name = "break_";
            break;
        }
        case LBL_Lcase: {
            name = "case_";
            break;
        }
        case LBL_Lcontinue: {
            name = "continue_";
            break;
        }
        case LBL_Ldefault: {
            name = "default_";
            break;
        }
        default:
            THROW_ABORT;
    }
    name += ctx->hash_table[target];
    return make_string_identifier(ctx, std::move(name));
}

TIdentifier repr_case_identifier(Ctx ctx, TIdentifier target, bool is_label, size_t i) {
    std::string name = is_label ? "case_" : "";
    name += std::to_string(i);
    name += ctx->hash_table[target];
    return make_string_identifier(ctx, std::move(name));
}

TIdentifier repr_var_identifier(Ctx ctx, CExp* node) {
    std::string name;
    switch (node->type()) {
        case AST_CConstant_t: {
            name = "const";
            break;
        }
        case AST_CString_t: {
            name = "string";
            break;
        }
        case AST_CVar_t: {
            name = "var";
            break;
        }
        case AST_CCast_t: {
            name = "cast";
            break;
        }
        case AST_CUnary_t: {
            name = "unop";
            break;
        }
        case AST_CBinary_t: {
            name = "binop";
            break;
        }
        case AST_CAssignment_t: {
            name = "assign";
            break;
        }
        case AST_CConditional_t: {
            name = "ternop";
            break;
        }
        case AST_CFunctionCall_t: {
            name = "call";
            break;
        }
        case AST_CDereference_t: {
            name = "deref";
            break;
        }
        case AST_CAddrOf_t: {
            name = "addr";
            break;
        }
        case AST_CSubscript_t: {
            name = "subscr";
            break;
        }
        case AST_CDot_t: {
            name = "smem";
            break;
        }
        case AST_CArrow_t: {
            name = "sptr";
            break;
        }
        default:
            THROW_ABORT;
    }
    return make_var_identifier(ctx, std::move(name));
}
