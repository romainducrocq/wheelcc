#include <string>

#include "util/str2t.hpp"
#include "util/throw.hpp"

#include "ast/ast.hpp"
#include "ast/front_ast.hpp"

#include "frontend/intermediate/idents.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Identifiers

TIdentifier resolve_label_identifier(TIdentifier label) {
    std::string name = identifiers->hash_table[label];
    name += ".";
    name += std::to_string(identifiers->label_counter);
    identifiers->label_counter++;
    return make_string_identifier(std::move(name));
}

TIdentifier resolve_variable_identifier(TIdentifier variable) {
    std::string name = identifiers->hash_table[variable];
    name += ".";
    name += std::to_string(identifiers->variable_counter);
    identifiers->variable_counter++;
    return make_string_identifier(std::move(name));
}

TIdentifier resolve_structure_tag(TIdentifier structure) {
    std::string name = identifiers->hash_table[structure];
    name += ".";
    name += std::to_string(identifiers->structure_counter);
    identifiers->structure_counter++;
    return make_string_identifier(std::move(name));
}

TIdentifier represent_label_identifier(LABEL_KIND label_kind) {
    std::string name;
    switch (label_kind) {
        case LABEL_KIND::Land_false: {
            name = "and_false";
            break;
        }
        case LABEL_KIND::Land_true: {
            name = "and_true";
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
    name += std::to_string(identifiers->label_counter);
    identifiers->label_counter++;
    return make_string_identifier(std::move(name));
}

TIdentifier represent_loop_identifier(LABEL_KIND label_kind, TIdentifier target) {
    std::string name;
    switch (label_kind) {
        case LABEL_KIND::Lbreak: {
            name = "break_";
            break;
        }
        case LABEL_KIND::Lcase: {
            name = "case_";
            break;
        }
        case LABEL_KIND::Lcontinue: {
            name = "continue_";
            break;
        }
        case LABEL_KIND::Ldefault: {
            name = "default_";
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    name += identifiers->hash_table[target];
    return make_string_identifier(std::move(name));
}

TIdentifier represent_case_identifier(TIdentifier target, bool is_label, size_t i) {
    std::string name = is_label ? "case_" : "";
    name += std::to_string(i);
    name += identifiers->hash_table[target];
    return make_string_identifier(std::move(name));
}

TIdentifier represent_variable_identifier(CExp* node) {
    std::string name;
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
    name += std::to_string(identifiers->variable_counter);
    identifiers->variable_counter++;
    return make_string_identifier(std::move(name));
}
