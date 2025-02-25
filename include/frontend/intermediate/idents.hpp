#ifndef _FRONTEND_INTERMEDIATE_IDENTS_HPP
#define _FRONTEND_INTERMEDIATE_IDENTS_HPP

#include <string>

#include "ast/ast.hpp"

struct CExp;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Identifiers

enum LABEL_KIND {
    Land_false,
    Land_true,
    Lbreak,
    Lcase,
    Lcomisd_nan,
    Lcontinue,
    Ldefault,
    Ldouble,
    Ldo_while,
    Ldo_while_start,
    Lfor,
    Lfor_start,
    Lif_else,
    Lif_false,
    Lor_false,
    Lor_true,
    Lsd2si_after,
    Lsd2si_out_of_range,
    Lsi2sd_after,
    Lsi2sd_out_of_range,
    Lstring,
    Lswitch,
    Lternary_else,
    Lternary_false,
    Lwhile
};

TIdentifier make_string_identifier(std::string&& value);
TIdentifier resolve_label_identifier(TIdentifier label);
TIdentifier resolve_variable_identifier(TIdentifier variable);
TIdentifier resolve_structure_tag(TIdentifier structure);
TIdentifier represent_label_identifier(LABEL_KIND label_kind);
TIdentifier represent_loop_identifier(LABEL_KIND label_kind, TIdentifier target);
TIdentifier represent_case_identifier(TIdentifier target, bool is_label, size_t i);
TIdentifier represent_variable_identifier(CExp* node);

#endif
