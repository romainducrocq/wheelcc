#ifndef _FRONTEND_INTERMEDIATE_IDENTS_HPP
#define _FRONTEND_INTERMEDIATE_IDENTS_HPP

#include <string>

#include "ast/ast.hpp"

#include "intermediate/labels.hpp" // frontend

struct CExp;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Identifiers

TIdentifier resolve_label_identifier(TIdentifier label);
TIdentifier resolve_variable_identifier(TIdentifier variable);
TIdentifier resolve_structure_tag(TIdentifier structure);
TIdentifier represent_label_identifier(LABEL_KIND label_kind);
TIdentifier represent_loop_identifier(LABEL_KIND label_kind, TIdentifier target);
TIdentifier represent_case_identifier(TIdentifier target, bool is_label, size_t i);
TIdentifier represent_variable_identifier(CExp* node);

#endif
