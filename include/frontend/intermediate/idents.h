#ifndef _FRONT_INTERMEDIATE_IDENTS_H
#define _FRONT_INTERMEDIATE_IDENTS_H

#include "ast_t.h" // ast

#include "intermediate/labels.h" // frontend

typedef struct CExp CExp;
typedef struct IdentifierContext IdentifierContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Identifiers

#ifdef __cplusplus
extern "C" {
#endif
TIdentifier rslv_label_identifier(IdentifierContext* ctx, TIdentifier label);
TIdentifier rslv_var_identifier(IdentifierContext* ctx, TIdentifier variable);
TIdentifier rslv_struct_tag(IdentifierContext* ctx, TIdentifier structure);
TIdentifier repr_label_identifier(IdentifierContext* ctx, LABEL_KIND label_kind);
TIdentifier repr_loop_identifier(IdentifierContext* ctx, LABEL_KIND label_kind, TIdentifier target);
TIdentifier repr_case_identifier(IdentifierContext* ctx, TIdentifier target, bool is_label, size_t i);
TIdentifier repr_var_identifier(IdentifierContext* ctx, const CExp* node);
#ifdef __cplusplus
}
#endif

#endif
