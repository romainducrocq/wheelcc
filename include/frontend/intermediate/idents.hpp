#ifndef _FRONTEND_INTERMEDIATE_IDENTS_HPP
#define _FRONTEND_INTERMEDIATE_IDENTS_HPP

#include <string>

#include "ast_t.hpp" // ast

#include "intermediate/labels.hpp" // frontend

struct CExp;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Identifiers

TIdentifier rslv_label_identifier(TIdentifier label);
TIdentifier rslv_var_identifier(TIdentifier variable);
TIdentifier rslv_struct_tag(TIdentifier structure);
TIdentifier repr_label_identifier(LABEL_KIND label_kind);
TIdentifier repr_loop_identifier(LABEL_KIND label_kind, TIdentifier target);
TIdentifier repr_case_identifier(TIdentifier target, bool is_label, size_t i);
TIdentifier repr_var_identifier(CExp* node);

#endif
