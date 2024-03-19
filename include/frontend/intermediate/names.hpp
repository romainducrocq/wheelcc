#ifndef _FRONTEND_INTERMEDIATE_NAMES_HPP
#define _FRONTEND_INTERMEDIATE_NAMES_HPP

#include "ast/ast.hpp"
#include "ast/front_ast.hpp"

enum LABEL_KIND {
    Land_false,
    Land_true,
    Lcomisd_nan,
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
    Lternary_else,
    Lternary_false,
    Lwhile
};

TIdentifier resolve_label_identifier(const TIdentifier& label);
TIdentifier resolve_variable_identifier(const TIdentifier& variable);
TIdentifier represent_label_identifier(LABEL_KIND label_kind);
TIdentifier represent_variable_identifier(CExp* node);

#endif
