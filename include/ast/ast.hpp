#ifndef _AST_AST_H
#define _AST_AST_H

#include <inttypes.h>
#include <unordered_map>

#include "util/c_std.hpp"

#include "ast_t.hpp" // ast

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Abstract syntax tree

struct Ast {
    virtual ~Ast() = 0;
    virtual AST_T type() = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// const = ConstInt(int)
//       | ConstLong(int)
//       | ConstUInt(int)
//       | ConstULong(int)
//       | ConstDouble(double)
//       | ConstChar(int)
//       | ConstUChar(int)
struct CConst : Ast {
    AST_T type() override;
};

struct CConstInt : CConst {
    AST_T type() override;
    CConstInt() = default;
    CConstInt(TInt value);

    TInt value;
};

struct CConstLong : CConst {
    AST_T type() override;
    CConstLong() = default;
    CConstLong(TLong value);

    TLong value;
};

struct CConstUInt : CConst {
    AST_T type() override;
    CConstUInt() = default;
    CConstUInt(TUInt value);

    TUInt value;
};

struct CConstULong : CConst {
    AST_T type() override;
    CConstULong() = default;
    CConstULong(TULong value);

    TULong value;
};

struct CConstDouble : CConst {
    AST_T type() override;
    CConstDouble() = default;
    CConstDouble(TDouble value);

    TDouble value;
};

struct CConstChar : CConst {
    AST_T type() override;
    CConstChar() = default;
    CConstChar(TChar value);

    TChar value;
};

struct CConstUChar : CConst {
    AST_T type() override;
    CConstUChar() = default;
    CConstUChar(TUChar value);

    TUChar value;
};

// string = StringLiteral(int*)
struct CStringLiteral : Ast {
    AST_T type() override;
    CStringLiteral();
    CStringLiteral(vector_t(TChar) * value);
    ~CStringLiteral();

    vector_t(TChar) value;
};

/*
struct Dummy : Ast {
};
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PairKeyValue(TIdentifier, string_t);

struct IdentifierContext {
    uint32_t label_count;
    uint32_t var_count;
    uint32_t struct_count;
    std::unordered_map<TIdentifier, string_t> hash_table;
};

TIdentifier make_string_identifier(IdentifierContext* ctx, string_t* value);
TIdentifier make_label_identifier(IdentifierContext* ctx, string_t* name);
TIdentifier make_var_identifier(IdentifierContext* ctx, string_t* name);
TIdentifier make_struct_identifier(IdentifierContext* ctx, string_t* name);
#define UID_SEPARATOR "."

#endif
