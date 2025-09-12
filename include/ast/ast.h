#ifndef _AST_AST_H
#define _AST_AST_H

#include "util/c_std.h"

#include "ast_t.h" // ast

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Abstract syntax tree

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct CConst CConst;
typedef struct CStringLiteral CStringLiteral;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// const = ConstInt(int)
//       | ConstLong(int)
//       | ConstUInt(int)
//       | ConstULong(int)
//       | ConstDouble(double)
//       | ConstChar(int)
//       | ConstUChar(int)

typedef struct CConstInt {
    TInt value;
} CConstInt;

typedef struct CConstLong {
    TLong value;
} CConstLong;

typedef struct CConstUInt {
    TUInt value;
} CConstUInt;

typedef struct CConstULong {
    TULong value;
} CConstULong;

typedef struct CConstDouble {
    TDouble value;
} CConstDouble;

typedef struct CConstChar {
    TChar value;
} CConstChar;

typedef struct CConstUChar {
    TUChar value;
} CConstUChar;

typedef struct CConst {
    shared_ptr_impl(AST_T);

    union {
        CConstInt _CConstInt;
        CConstLong _CConstLong;
        CConstUInt _CConstUInt;
        CConstULong _CConstULong;
        CConstDouble _CConstDouble;
        CConstChar _CConstChar;
        CConstUChar _CConstUChar;
    } get;
} CConst;

#ifdef __cplusplus
extern "C" {
#endif
shared_ptr_t(CConst) make_CConst(void);
shared_ptr_t(CConst) make_CConstInt(TInt value);
shared_ptr_t(CConst) make_CConstLong(TLong value);
shared_ptr_t(CConst) make_CConstUInt(TUInt value);
shared_ptr_t(CConst) make_CConstULong(TULong value);
shared_ptr_t(CConst) make_CConstDouble(TDouble value);
shared_ptr_t(CConst) make_CConstChar(TChar value);
shared_ptr_t(CConst) make_CConstUChar(TUChar value);
void free_CConst(shared_ptr_t(CConst) * self);
#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// string = StringLiteral(int*)

typedef struct CStringLiteral {
    shared_ptr_impl(AST_T);
    vector_t(TChar) value;
} CStringLiteral;

#ifdef __cplusplus
extern "C" {
#endif
shared_ptr_t(CStringLiteral) make_CStringLiteral(vector_t(TChar) * value);
void free_CStringLiteral(shared_ptr_t(CStringLiteral) * self);
#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PairKeyValue(TIdentifier, string_t);

typedef struct IdentifierContext {
    uint32_t label_count;
    uint32_t var_count;
    uint32_t struct_count;
    hashmap_t(TIdentifier, string_t) hash_table;
} IdentifierContext;

#ifdef __cplusplus
extern "C" {
#endif
TIdentifier make_string_identifier(IdentifierContext* ctx, string_t* value);
TIdentifier make_label_identifier(IdentifierContext* ctx, string_t* name);
TIdentifier make_var_identifier(IdentifierContext* ctx, string_t* name);
TIdentifier make_struct_identifier(IdentifierContext* ctx, string_t* name);
#ifdef __cplusplus
}
#endif
#define UID_SEPARATOR "."

#endif
