#include "frontend/intermediate/names.hpp"
#include "util/error.hpp"
#include "ast/ast.hpp"
#include "ast/front_ast.hpp"

#include <inttypes.h>
#include <string>

static int32_t label_counter = 0;
static int32_t variable_counter = 0;

TIdentifier represent_label_identifier(const TIdentifier& label);

TIdentifier resolve_label_identifier(const TIdentifier& label) {
    return represent_label_identifier(label);
}

TIdentifier resolve_variable_identifier(const TIdentifier& variable) {
    variable_counter += 1;
    TIdentifier name = variable + "." + std::to_string(variable_counter - 1);
    return name;
}

TIdentifier represent_label_identifier(const TIdentifier& label) {
    label_counter += 1;
    TIdentifier name = label + "." + std::to_string(label_counter - 1);
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

    variable_counter += 1;
    name += "." + std::to_string(variable_counter - 1);

    return name;
}
