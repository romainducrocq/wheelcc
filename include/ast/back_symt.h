#ifndef _AST_BACK_SYMT_H
#define _AST_BACK_SYMT_H

#include "util/c_std.h"

#include "ast/ast.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Backend symbol table

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct AssemblyType AssemblyType;
typedef struct BackendSymbol BackendSymbol;
typedef struct AsmOperand AsmOperand;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// assembly_type = Byte
//               | LongWord
//               | QuadWord
//               | BackendDouble
//               | ByteArray(int, int)

typedef struct Byte {
    int8_t _empty;
} Byte;

typedef struct LongWord {
    int8_t _empty;
} LongWord;

typedef struct QuadWord {
    int8_t _empty;
} QuadWord;

typedef struct BackendDouble {
    int8_t _empty;
} BackendDouble;

typedef struct ByteArray {
    TLong size;
    TInt alignment;
} ByteArray;

typedef struct AssemblyType {
    shared_ptr_impl(AST_T);

    union {
        Byte _Byte;
        LongWord _LongWord;
        QuadWord _QuadWord;
        BackendDouble _BackendDouble;
        ByteArray _ByteArray;
    } get;
} AssemblyType;

shared_ptr_t(AssemblyType) make_AssemblyType(void);
shared_ptr_t(AssemblyType) make_Byte(void);
shared_ptr_t(AssemblyType) make_LongWord(void);
shared_ptr_t(AssemblyType) make_QuadWord(void);
shared_ptr_t(AssemblyType) make_BackendDouble(void);
shared_ptr_t(AssemblyType) make_ByteArray(TLong size, TInt alignment);
void free_AssemblyType(shared_ptr_t(AssemblyType) * self);
void move_AssemblyType(shared_ptr_t(AssemblyType) * self, shared_ptr_t(AssemblyType) * other);
void copy_AssemblyType(shared_ptr_t(AssemblyType) * self, shared_ptr_t(AssemblyType) * other);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// symbol = Obj(assembly_type, bool, bool)
//        | Fun(bool, operand*)

typedef struct BackendObj {
    bool is_static;
    bool is_const;
    shared_ptr_t(AssemblyType) asm_type;
} BackendObj;

typedef struct BackendFun {
    bool is_def;
    vector_t(shared_ptr_t(AsmOperand)) callee_saved_regs;
} BackendFun;

typedef struct BackendSymbol {
    unique_ptr_impl(AST_T);

    union {
        BackendObj _BackendObj;
        BackendFun _BackendFun;
    } get;
} BackendSymbol;

unique_ptr_t(BackendSymbol) make_BackendSymbol(void);
unique_ptr_t(BackendSymbol) make_BackendObj(bool is_static, bool is_const, shared_ptr_t(AssemblyType) * asm_type);
unique_ptr_t(BackendSymbol) make_BackendFun(bool is_def);
void free_BackendSymbol(unique_ptr_t(BackendSymbol) * self);
void move_BackendSymbol(unique_ptr_t(BackendSymbol) * self, unique_ptr_t(BackendSymbol) * other);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef unique_ptr_t(BackendSymbol) UPtrBackendSymbol;
PairKeyValue(TIdentifier, UPtrBackendSymbol);

typedef struct BackEndContext {
    hashmap_t(TIdentifier, UPtrBackendSymbol) symbol_table;
} BackEndContext;

#endif
