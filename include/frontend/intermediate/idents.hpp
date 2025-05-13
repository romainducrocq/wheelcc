#ifndef _FRONT_INTERMEDIATE_IDENTS_H
#define _FRONT_INTERMEDIATE_IDENTS_H

#include "ast_t.hpp" // ast

#include "intermediate/labels.hpp" // frontend

struct CExp;
struct IdentifierContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Identifiers

TIdentifier rslv_label_identifier(IdentifierContext* ctx, TIdentifier label);
TIdentifier rslv_var_identifier(IdentifierContext* ctx, TIdentifier variable);
TIdentifier rslv_struct_tag(IdentifierContext* ctx, TIdentifier structure);
TIdentifier repr_label_identifier(IdentifierContext* ctx, LABEL_KIND label_kind);
TIdentifier repr_loop_identifier(IdentifierContext* ctx, LABEL_KIND label_kind, TIdentifier target);
TIdentifier repr_case_identifier(IdentifierContext* ctx, TIdentifier target, bool is_label, size_t i);
TIdentifier repr_var_identifier(IdentifierContext* ctx, CExp* node);

#endif
