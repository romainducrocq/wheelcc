#include "frontend/intermediate/names.hpp"
#include "util/throw.hpp"
#include "ast/ast.hpp"
#include "ast/front_ast.hpp"

#include <inttypes.h>
#include <string>

static int32_t label_counter = 0;
static int32_t variable_counter = 0;
static int32_t structure_counter = 0;

TIdentifier resolve_label_identifier(const TIdentifier& label) {
    TIdentifier name = label + "." + std::to_string(label_counter);
    label_counter += 1;
    return name;
}

TIdentifier resolve_variable_identifier(const TIdentifier& variable) {
    TIdentifier name = variable + "." + std::to_string(variable_counter);
    variable_counter += 1;
    return name;
}

TIdentifier resolve_structure_tag(const TIdentifier& structure) {
    TIdentifier name = structure + "." + std::to_string(structure_counter);
    structure_counter += 1;
    return name;
}

TIdentifier represent_label_identifier(LABEL_KIND label_kind) {
    TIdentifier name;
    switch(label_kind) {
        case LABEL_KIND::Land_false: {
            name = "and_false";
            break;
        }
        case LABEL_KIND::Land_true: {
            name = "and_true";
            break;
        }
        case LABEL_KIND::Lcomisd_nan: {
            name = "comisd_nan";
            break;
        }
        case LABEL_KIND::Ldouble: {
            name = "double";
            break;
        }
        case LABEL_KIND::Ldo_while: {
            name = "do_while";
            break;
        }
        case LABEL_KIND::Ldo_while_start: {
            name = "do_while_start";
            break;
        }
        case LABEL_KIND::Lfor: {
            name = "for";
            break;
        }
        case LABEL_KIND::Lfor_start: {
            name = "for_start";
            break;
        }
        case LABEL_KIND::Lif_else: {
            name = "if_else";
            break;
        }
        case LABEL_KIND::Lif_false: {
            name = "if_false";
            break;
        }
        case LABEL_KIND::Lor_false: {
            name = "or_false";
            break;
        }
        case LABEL_KIND::Lor_true: {
            name = "or_true";
            break;
        }
        case LABEL_KIND::Lsd2si_after: {
            name = "sd2si_after";
            break;
        }
        case LABEL_KIND::Lsd2si_out_of_range: {
            name = "sd2si_out_of_range";
            break;
        }
        case LABEL_KIND::Lsi2sd_after: {
            name = "si2sd_after";
            break;
        }
        case LABEL_KIND::Lsi2sd_out_of_range: {
            name = "si2sd_out_of_range";
            break;
        }
        case LABEL_KIND::Lstring: {
            name = "string";
            break;
        }
        case LABEL_KIND::Lternary_else: {
            name = "ternary_else";
            break;
        }
        case LABEL_KIND::Lternary_false: {
            name = "ternary_false";
            break;
        }
        case LABEL_KIND::Lwhile: {
            name = "while";
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }

    name += "." + std::to_string(label_counter);
    label_counter += 1;
    return name;
}

TIdentifier represent_variable_identifier(CExp* node) {
    TIdentifier name;
    switch(node->type()) {
        case AST_T::CConstant_t: {
            name = "const";
            break;
        }
        case AST_T::CString_t: {
            name = "string";
            break;
        }
        case AST_T::CVar_t: {
            name = "var";
            break;
        }
        case AST_T::CCast_t: {
            name = "cast";
            break;
        }
        case AST_T::CUnary_t: {
            name = "unop";
            break;
        }
        case AST_T::CBinary_t: {
            name = "binop";
            break;
        }
        case AST_T::CAssignment_t: {
            name = "assign";
            break;
        }
        case AST_T::CConditional_t: {
            name = "ternop";
            break;
        }
        case AST_T::CFunctionCall_t: {
            name = "call";
            break;
        }
        case AST_T::CDereference_t: {
            name = "deref";
            break;
        }
        case AST_T::CAddrOf_t: {
            name = "addr";
            break;
        }
        case AST_T::CSubscript_t: {
            name = "subscr";
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }

    name += "." + std::to_string(variable_counter);
    variable_counter += 1;
    return name;
}
