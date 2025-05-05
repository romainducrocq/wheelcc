#ifndef _AST_AST_H
#define _AST_AST_H

#include <inttypes.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

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
    CStringLiteral() = default;
    CStringLiteral(std::vector<TChar> value);

    std::vector<TChar> value;
};

/*
struct Dummy : Ast {
};
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct IdentifierContext {
    IdentifierContext();

    uint32_t label_counter;
    uint32_t variable_counter;
    uint32_t structure_counter;
    std::unordered_map<TIdentifier, std::string> hash_table;
};

extern std::unique_ptr<IdentifierContext> identifiers;
#define INIT_IDENTIFIER_CTX identifiers = std::make_unique<IdentifierContext>()
#define FREE_IDENTIFIER_CTX identifiers.reset()

TIdentifier make_string_identifier(std::string&& value);
TIdentifier make_label_identifier(std::string&& name);
TIdentifier make_var_identifier(std::string&& name);
TIdentifier make_struct_identifier(std::string&& name);
#define UID_SEPARATOR "."

#endif
