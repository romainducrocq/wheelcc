#include <string>

#include "util/throw.hpp"

#include "ast/ast.hpp"
#include "ast/front_ast.hpp"
#include "ast/front_symt.hpp"

#include "frontend/intermediate/names.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Names

TIdentifier resolve_label_identifier(const TIdentifier& label) {
    TIdentifier name = label;
    name += ".";
    name += std::to_string(frontend->label_counter);
    frontend->label_counter++;
    return name;
}

TIdentifier resolve_variable_identifier(const TIdentifier& variable) {
    TIdentifier name = variable;
    name += ".";
    name += std::to_string(frontend->variable_counter);
    frontend->variable_counter++;
    return name;
}

TIdentifier resolve_structure_tag(const TIdentifier& structure) {
    TIdentifier name = structure;
    name += ".";
    name += std::to_string(frontend->structure_counter);
    frontend->structure_counter++;
    return name;
}

TIdentifier represent_label_identifier(LABEL_KIND label_kind) {
    TIdentifier name;
    switch (label_kind) {
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
        case LABEL_KIND::Lswitch: {
            name = "switch";
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
    name += ".";
    name += std::to_string(frontend->label_counter);
    frontend->label_counter++;
    return name;
}

TIdentifier represent_variable_identifier(CExp* node) {
    TIdentifier name;
    switch (node->type()) {
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
        case AST_T::CDot_t: {
            name = "smem";
            break;
        }
        case AST_T::CArrow_t: {
            name = "sptr";
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    name += ".";
    name += std::to_string(frontend->variable_counter);
    frontend->variable_counter++;
    return name;
}
