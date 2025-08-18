#include "util/c_std.hpp"
#include "util/throw.hpp"

#include "ast/ast.hpp"
#include "ast/front_ast.hpp"

#include "frontend/intermediate/idents.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Identifiers

typedef IdentifierContext* Ctx;

TIdentifier rslv_label_identifier(Ctx ctx, TIdentifier label) {
    string_t name = str_new(NULL);
    str_copy(map_get(ctx->hash_table, label), name);
    return make_label_identifier(ctx, &name);
}

TIdentifier rslv_var_identifier(Ctx ctx, TIdentifier variable) {
    string_t name = str_new(NULL);
    str_copy(map_get(ctx->hash_table, variable), name);
    return make_var_identifier(ctx, &name);
}

TIdentifier rslv_struct_tag(Ctx ctx, TIdentifier structure) {
    string_t name = str_new(NULL);
    str_copy(map_get(ctx->hash_table, structure), name);
    return make_struct_identifier(ctx, &name);
}

TIdentifier repr_label_identifier(Ctx ctx, LABEL_KIND label_kind) {
    string_t name = str_new(NULL);
    switch (label_kind) {
        case LBL_Land_false: {
            name = str_new("and_false");
            break;
        }
        case LBL_Land_true: {
            name = str_new("and_true");
            break;
        }
        case LBL_Ldo_while: {
            name = str_new("do_while");
            break;
        }
        case LBL_Ldo_while_start: {
            name = str_new("do_while_start");
            break;
        }
        case LBL_Lfor: {
            name = str_new("for");
            break;
        }
        case LBL_Lswitch: {
            name = str_new("switch");
            break;
        }
        case LBL_Lfor_start: {
            name = str_new("for_start");
            break;
        }
        case LBL_Lif_else: {
            name = str_new("if_else");
            break;
        }
        case LBL_Lif_false: {
            name = str_new("if_false");
            break;
        }
        case LBL_Lor_false: {
            name = str_new("or_false");
            break;
        }
        case LBL_Lor_true: {
            name = str_new("or_true");
            break;
        }
        case LBL_Lstring: {
            name = str_new("string");
            break;
        }
        case LBL_Lternary_else: {
            name = str_new("ternary_else");
            break;
        }
        case LBL_Lternary_false: {
            name = str_new("ternary_false");
            break;
        }
        case LBL_Lwhile: {
            name = str_new("while");
            break;
        }
        default:
            THROW_ABORT;
    }
    return make_label_identifier(ctx, &name);
}

TIdentifier repr_loop_identifier(Ctx ctx, LABEL_KIND label_kind, TIdentifier target) {
    string_t name = str_new(NULL);
    switch (label_kind) {
        case LBL_Lbreak: {
            name = str_new("break_");
            break;
        }
        case LBL_Lcase: {
            name = str_new("case_");
            break;
        }
        case LBL_Lcontinue: {
            name = str_new("continue_");
            break;
        }
        case LBL_Ldefault: {
            name = str_new("default_");
            break;
        }
        default:
            THROW_ABORT;
    }
    str_append(name, map_get(ctx->hash_table, target));
    return make_string_identifier(ctx, &name);
}

TIdentifier repr_case_identifier(Ctx ctx, TIdentifier target, bool is_label, size_t i) {
    string_t name = is_label ? str_new("case_") : str_new("");
    {
        string_t strto_i = str_to_string(i);
        str_append(name, strto_i);
        str_delete(strto_i);
    }
    str_append(name, map_get(ctx->hash_table, target));
    return make_string_identifier(ctx, &name);
}

TIdentifier repr_var_identifier(Ctx ctx, CExp* node) {
    string_t name = str_new(NULL);
    switch (node->type()) {
        case AST_CConstant_t: {
            name = str_new("const");
            break;
        }
        case AST_CString_t: {
            name = str_new("string");
            break;
        }
        case AST_CVar_t: {
            name = str_new("var");
            break;
        }
        case AST_CCast_t: {
            name = str_new("cast");
            break;
        }
        case AST_CUnary_t: {
            name = str_new("unop");
            break;
        }
        case AST_CBinary_t: {
            name = str_new("binop");
            break;
        }
        case AST_CAssignment_t: {
            name = str_new("assign");
            break;
        }
        case AST_CConditional_t: {
            name = str_new("ternop");
            break;
        }
        case AST_CFunctionCall_t: {
            name = str_new("call");
            break;
        }
        case AST_CDereference_t: {
            name = str_new("deref");
            break;
        }
        case AST_CAddrOf_t: {
            name = str_new("addr");
            break;
        }
        case AST_CSubscript_t: {
            name = str_new("subscr");
            break;
        }
        case AST_CDot_t: {
            name = str_new("smem");
            break;
        }
        case AST_CArrow_t: {
            name = str_new("sptr");
            break;
        }
        default:
            THROW_ABORT;
    }
    return make_var_identifier(ctx, &name);
}
