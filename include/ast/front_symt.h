#ifndef _AST_FRONT_SYMT_H
#define _AST_FRONT_SYMT_H

#include "util/c_std.h"

#include "ast/ast.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Frontend symbol table

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct Type Type;
typedef struct StaticInit StaticInit;
typedef struct InitialValue InitialValue;
typedef struct IdentifierAttr IdentifierAttr;
typedef struct Symbol Symbol;
typedef struct StructMember StructMember;
typedef struct StructTypedef StructTypedef;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// type = Char
//      | SChar
//      | UChar
//      | Int
//      | Long
//      | UInt
//      | ULong
//      | Double
//      | Void
//      | FunType(int, int, type*, type)
//      | Pointer(type)
//      | Array(int, type)
//      | Structure(identifier, bool)

typedef struct Char {
    int8_t _empty;
} Char;

typedef struct SChar {
    int8_t _empty;
} SChar;

typedef struct UChar {
    int8_t _empty;
} UChar;

typedef struct Int {
    int8_t _empty;
} Int;

typedef struct Long {
    int8_t _empty;
} Long;

typedef struct UInt {
    int8_t _empty;
} UInt;

typedef struct ULong {
    int8_t _empty;
} ULong;

typedef struct Double {
    int8_t _empty;
} Double;

typedef struct Void {
    int8_t _empty;
} Void;

typedef struct FunType {
    TULong param_reg_mask;
    TULong ret_reg_mask;
    vector_t(shared_ptr_t(Type)) param_types;
    shared_ptr_t(Type) ret_type;
} FunType;

typedef struct Pointer {
    shared_ptr_t(Type) ref_type;
} Pointer;

typedef struct Array {
    TLong size;
    shared_ptr_t(Type) elem_type;
} Array;

typedef struct Structure {
    TIdentifier tag;
    bool is_union;
} Structure;

typedef struct Type {
    shared_ptr_impl(AST_T);

    union {
        Char _Char;
        SChar _SChar;
        UChar _UChar;
        Int _Int;
        Long _Long;
        UInt _UInt;
        ULong _ULong;
        Double _Double;
        Void _Void;
        FunType _FunType;
        Pointer _Pointer;
        Array _Array;
        Structure _Structure;
    } get;
} Type;

#ifdef __cplusplus
extern "C" {
#endif
shared_ptr_t(Type) make_Type(void);
shared_ptr_t(Type) make_Char(void);
shared_ptr_t(Type) make_SChar(void);
shared_ptr_t(Type) make_UChar(void);
shared_ptr_t(Type) make_Int(void);
shared_ptr_t(Type) make_Long(void);
shared_ptr_t(Type) make_UInt(void);
shared_ptr_t(Type) make_ULong(void);
shared_ptr_t(Type) make_Double(void);
shared_ptr_t(Type) make_Void(void);
shared_ptr_t(Type) make_FunType(vector_t(shared_ptr_t(Type)) * param_types, shared_ptr_t(Type) * ret_type);
shared_ptr_t(Type) make_Pointer(shared_ptr_t(Type) * ref_type);
shared_ptr_t(Type) make_Array(TLong size, shared_ptr_t(Type) * elem_type);
shared_ptr_t(Type) make_Structure(TIdentifier tag, bool is_union);
void free_Type(shared_ptr_t(Type) * self);
#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// static_init = IntInit(int)
//             | LongInit(int)
//             | UIntInit(int)
//             | ULongInit(int)
//             | CharInit(int)
//             | UCharInit(int)
//             | DoubleInit(string)
//             | ZeroInit(int)
//             | StringInit(identifier, bool, string)
//             | PointerInit(identifier)

typedef struct IntInit {
    TInt value;
} IntInit;

typedef struct LongInit {
    TLong value;
} LongInit;

typedef struct UIntInit {
    TUInt value;
} UIntInit;

typedef struct ULongInit {
    TULong value;
} ULongInit;

typedef struct CharInit {
    TChar value;
} CharInit;

typedef struct UCharInit {
    TUChar value;
} UCharInit;

typedef struct DoubleInit {
    TIdentifier dbl_const;
} DoubleInit;

typedef struct ZeroInit {
    TLong byte;
} ZeroInit;

typedef struct StringInit {
    TIdentifier string_const;
    bool is_null_term;
    shared_ptr_t(CStringLiteral) literal;
} StringInit;

typedef struct PointerInit {
    TIdentifier name;
} PointerInit;

typedef struct StaticInit {
    shared_ptr_impl(AST_T);

    union {
        IntInit _IntInit;
        LongInit _LongInit;
        UIntInit _UIntInit;
        ULongInit _ULongInit;
        CharInit _CharInit;
        UCharInit _UCharInit;
        DoubleInit _DoubleInit;
        ZeroInit _ZeroInit;
        StringInit _StringInit;
        PointerInit _PointerInit;
    } get;
} StaticInit;

#ifdef __cplusplus
extern "C" {
#endif
shared_ptr_t(StaticInit) make_StaticInit(void);
shared_ptr_t(StaticInit) make_IntInit(TInt value);
shared_ptr_t(StaticInit) make_LongInit(TLong value);
shared_ptr_t(StaticInit) make_UIntInit(TUInt value);
shared_ptr_t(StaticInit) make_ULongInit(TULong value);
shared_ptr_t(StaticInit) make_CharInit(TChar value);
shared_ptr_t(StaticInit) make_UCharInit(TUChar value);
shared_ptr_t(StaticInit) make_DoubleInit(TIdentifier dbl_const);
shared_ptr_t(StaticInit) make_ZeroInit(TLong byte);
shared_ptr_t(StaticInit)
    make_StringInit(TIdentifier string_const, bool is_null_term, shared_ptr_t(CStringLiteral) * literal);
shared_ptr_t(StaticInit) make_PointerInit(TIdentifier name);
void free_StaticInit(shared_ptr_t(StaticInit) * self);
#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// initial_value = Tentative
//               | Initial(static_init*)
//               | NoInitializer

typedef struct Tentative {
    int8_t _empty;
} Tentative;

typedef struct Initial {
    vector_t(shared_ptr_t(StaticInit)) static_inits;
} Initial;

typedef struct NoInitializer {
    int8_t _empty;
} NoInitializer;

typedef struct InitialValue {
    shared_ptr_impl(AST_T);

    union {
        Tentative _Tentative;
        Initial _Initial;
        NoInitializer _NoInitializer;
    } get;
} InitialValue;

#ifdef __cplusplus
extern "C" {
#endif
shared_ptr_t(InitialValue) make_InitialValue(void);
shared_ptr_t(InitialValue) make_Tentative(void);
shared_ptr_t(InitialValue) make_Initial(vector_t(shared_ptr_t(StaticInit)) * static_inits);
shared_ptr_t(InitialValue) make_NoInitializer(void);
void free_InitialValue(shared_ptr_t(InitialValue) * self);
#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// identifier_attrs = FunAttr(bool, bool)
//                  | StaticAttr(initial_value, bool)
//                  | ConstantAttr(static_init)
//                  | LocalAttr

typedef struct FunAttr {
    bool is_def;
    bool is_glob;
} FunAttr;

typedef struct StaticAttr {
    bool is_glob;
    shared_ptr_t(InitialValue) init;
} StaticAttr;

typedef struct ConstantAttr {
    shared_ptr_t(StaticInit) static_init;
} ConstantAttr;

typedef struct LocalAttr {
    int8_t _empty;
} LocalAttr;

typedef struct IdentifierAttr {
    unique_ptr_impl(AST_T);

    union {
        FunAttr _FunAttr;
        StaticAttr _StaticAttr;
        ConstantAttr _ConstantAttr;
        LocalAttr _LocalAttr;
    } get;
} IdentifierAttr;

#ifdef __cplusplus
extern "C" {
#endif
unique_ptr_t(IdentifierAttr) make_IdentifierAttr(void);
unique_ptr_t(IdentifierAttr) make_FunAttr(bool is_def, bool is_glob);
unique_ptr_t(IdentifierAttr) make_StaticAttr(bool is_glob, shared_ptr_t(InitialValue) * init);
unique_ptr_t(IdentifierAttr) make_ConstantAttr(shared_ptr_t(StaticInit) * static_init);
unique_ptr_t(IdentifierAttr) make_LocalAttr(void);
void free_IdentifierAttr(unique_ptr_t(IdentifierAttr) * self);
#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// symbol = Symbol(type, identifier_attrs)

typedef struct Symbol {
    unique_ptr_impl(AST_T);
    shared_ptr_t(Type) type_t;
    unique_ptr_t(IdentifierAttr) attrs;
} Symbol;

#ifdef __cplusplus
extern "C" {
#endif
unique_ptr_t(Symbol) make_Symbol(shared_ptr_t(Type) * type_t, unique_ptr_t(IdentifierAttr) * attrs);
void free_Symbol(unique_ptr_t(Symbol) * self);
#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// struct_member = StructMember(int, type)

typedef struct StructMember {
    unique_ptr_impl(AST_T);
    TLong offset;
    shared_ptr_t(Type) member_type;
} StructMember;

#ifdef __cplusplus
extern "C" {
#endif
unique_ptr_t(StructMember) make_StructMember(TLong offset, shared_ptr_t(Type) * member_type);
void free_StructMember(unique_ptr_t(StructMember) * self);
#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// struct_typedef(int, int, identifier*, struct_member*)

typedef unique_ptr_t(StructMember) UPtrStructMember;
PairKeyValue(TIdentifier, UPtrStructMember);

typedef struct StructTypedef {
    unique_ptr_impl(AST_T);
    TInt alignment;
    TLong size;
    vector_t(TIdentifier) member_names;
    hashmap_t(TIdentifier, UPtrStructMember) members;
} StructTypedef;

#ifdef __cplusplus
extern "C" {
#endif
unique_ptr_t(StructTypedef) make_StructTypedef(TInt alignment, TLong size, vector_t(TIdentifier) * member_names,
    hashmap_t(TIdentifier, UPtrStructMember) * members);
void free_StructTypedef(unique_ptr_t(StructTypedef) * self);
#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PairKeyValue(TIdentifier, size_t);
PairKeyValue(TIdentifier, TIdentifier);
typedef unique_ptr_t(StructTypedef) UPtrStructTypedef;
PairKeyValue(TIdentifier, UPtrStructTypedef);
typedef unique_ptr_t(Symbol) UPtrSymbol;
PairKeyValue(TIdentifier, UPtrSymbol);
ElementKey(TIdentifier);

typedef struct FrontEndContext {
    hashmap_t(TIdentifier, TIdentifier) string_const_table;
    hashmap_t(TIdentifier, UPtrStructTypedef) struct_typedef_table;
    hashmap_t(TIdentifier, UPtrSymbol) symbol_table;
    hashset_t(TIdentifier) addressed_set;
} FrontEndContext;

#ifdef __cplusplus
extern "C" {
#endif
StructMember* get_struct_typedef_member(FrontEndContext* ctx, TIdentifier tag, TIdentifier member_name);
StructMember* get_struct_typedef_back(FrontEndContext* ctx, TIdentifier tag);
#ifdef __cplusplus
}
#endif

#endif
