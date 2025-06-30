#include <algorithm>
#include <inttypes.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "util/str2t.hpp"
#include "util/throw.hpp"

#include "ast/ast.hpp"
#include "ast/front_ast.hpp"
#include "ast/front_symt.hpp"

#include "frontend/parser/errors.hpp"
#include "frontend/parser/lexer.hpp"
#include "frontend/parser/parser.hpp"

#include "frontend/intermediate/idents.hpp"

struct AbstractDeclarator {
    std::shared_ptr<Type> derived_type;
};

struct Declarator {
    TIdentifier name;
    std::shared_ptr<Type> derived_type;
    std::vector<TIdentifier> params;
};

struct ParserContext {
    ErrorsContext* errors;
    IdentifierContext* identifiers;
    // Parser
    size_t pop_idx;
    Token* next_tok;
    Token* peek_tok;
    Token* next_tok_i;
    Token* peek_tok_i;
    std::vector<Token>* p_toks;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Parser

typedef ParserContext* Ctx;

static void /* TODO TRY */ expect_next(Ctx ctx, Token* next_tok, TOKEN_KIND expect_tok) {
    if (next_tok->tok_kind != expect_tok) {
        THROW_AT_LINE_EX(GET_PARSER_MSG(MSG_unexpected_next_tok, get_tok_fmt(ctx->identifiers, next_tok),
                             get_tok_kind_fmt(expect_tok)),
            next_tok->line);
    }
}

static void /* TODO TRY */ pop_next(Ctx ctx) {
    if (ctx->pop_idx >= ctx->p_toks->size()) {
        THROW_AT_LINE_EX(GET_PARSER_MSG_0(MSG_reached_eof), ctx->p_toks->back().line);
    }

    ctx->next_tok = &(*ctx->p_toks)[ctx->pop_idx];
    ctx->pop_idx++;
}

static void /* TODO TRY */ pop_next_i(Ctx ctx, size_t i) {
    if (i == 0) {
        /* TODO TRY */ pop_next(ctx);
        ctx->next_tok_i = ctx->next_tok;
        return;
    }
    if (ctx->pop_idx + i >= ctx->p_toks->size()) {
        THROW_AT_LINE_EX(GET_PARSER_MSG_0(MSG_reached_eof), ctx->p_toks->back().line);
    }

    if (i == 1) {
        std::swap((*ctx->p_toks)[ctx->pop_idx], (*ctx->p_toks)[ctx->pop_idx + 1]);
    }
    else {
        Token swap_token_i = std::move((*ctx->p_toks)[ctx->pop_idx + i]);
        for (size_t j = ctx->pop_idx + i; j-- > ctx->pop_idx;) {
            (*ctx->p_toks)[j + 1] = std::move((*ctx->p_toks)[j]);
        }
        (*ctx->p_toks)[ctx->pop_idx] = std::move(swap_token_i);
    }
    /* TODO TRY */ pop_next(ctx);
    ctx->next_tok_i = &(*ctx->p_toks)[ctx->pop_idx - 1];
}

static void /* TODO TRY */ peek_next(Ctx ctx) {
    if (ctx->pop_idx >= ctx->p_toks->size()) {
        THROW_AT_LINE_EX(GET_PARSER_MSG_0(MSG_reached_eof), ctx->p_toks->back().line);
    }

    ctx->peek_tok = &(*ctx->p_toks)[ctx->pop_idx];
}

static void /* TODO TRY */ peek_next_i(Ctx ctx, size_t i) {
    if (i == 0) {
        /* TODO TRY */ peek_next(ctx);
        ctx->peek_tok_i = ctx->peek_tok;
        return;
    }
    if (ctx->pop_idx + i >= ctx->p_toks->size()) {
        THROW_AT_LINE_EX(GET_PARSER_MSG_0(MSG_reached_eof), ctx->p_toks->back().line);
    }

    ctx->peek_tok_i = &(*ctx->p_toks)[ctx->pop_idx + i];
}

// <identifier> ::= ? An identifier token ?
static void /* TODO TRY */ parse_identifier(Ctx ctx, size_t i, return_t(TIdentifier) identifier) {
    /* TODO TRY */ pop_next_i(ctx, i);
    *identifier = ctx->next_tok_i->tok;
}

// string = StringLiteral(int*)
// <string> ::= ? A string token ?
static void /* TODO TRY */ parse_string_literal(Ctx ctx, return_t(std::shared_ptr<CStringLiteral>) literal) {
    std::vector<TChar> value;
    {
        string_to_literal(ctx->identifiers->hash_table[ctx->next_tok->tok], value);
        /* TODO TRY */ peek_next(ctx);
        while (ctx->peek_tok->tok_kind == TOK_string_literal) {
            /* TODO TRY */ pop_next(ctx);
            string_to_literal(ctx->identifiers->hash_table[ctx->next_tok->tok], value);
            /* TODO TRY */ peek_next(ctx);
        }
    }
    *literal = std::make_shared<CStringLiteral>(std::move(value));
}

// <int> ::= ? An int constant token ?
static std::shared_ptr<CConstInt> parse_int_const(intmax_t intmax) {
    TInt value = intmax_to_int32(intmax);
    return std::make_shared<CConstInt>(value);
}

// <char> ::= ? A char token ?
static std::shared_ptr<CConstInt> parse_char_const(Ctx ctx) {
    TInt value = string_to_char_ascii(ctx->identifiers->hash_table[ctx->next_tok->tok]);
    return std::make_shared<CConstInt>(value);
}

// <long> ::= ? An int or long constant token ?
static std::shared_ptr<CConstLong> parse_long_const(intmax_t intmax) {
    TLong value = intmax_to_int64(intmax);
    return std::make_shared<CConstLong>(value);
}

// <double> ::= ? A floating-point constant token ?
static void /* TODO TRY */ parse_dbl_const(Ctx ctx, return_t(std::shared_ptr<CConst>) constant) {
    TDouble value;
    /* TODO TRY */ string_to_dbl(
        ctx->errors, ctx->identifiers->hash_table[ctx->next_tok->tok].c_str(), ctx->next_tok->line, &value);
    *constant = std::make_shared<CConstDouble>(value);
}

// <unsigned int> ::= ? An unsigned int constant token ?
static std::shared_ptr<CConstUInt> parse_uint_const(uintmax_t uintmax) {
    TUInt value = uintmax_to_uint32(uintmax);
    return std::make_shared<CConstUInt>(value);
}

// <unsigned long> ::= ? An unsigned int or unsigned long constant token ?
static std::shared_ptr<CConstULong> parse_ulong_const(uintmax_t uintmax) {
    TULong value = uintmax_to_uint64(uintmax);
    return std::make_shared<CConstULong>(value);
}

// <const> ::= <int> | <long> | <double> | <char>
// (signed) const = ConstInt(int) | ConstLong(long) | ConstDouble(double) | ConstChar(int)
static void /* TODO TRY */ parse_const(Ctx ctx, return_t(std::shared_ptr<CConst>) constant) {
    /* TODO TRY */ pop_next(ctx);
    switch (ctx->next_tok->tok_kind) {
        case TOK_char_const: {
            *constant = parse_char_const(ctx);
            return;
        }
        case TOK_dbl_const:
            /* TODO TRY */ parse_dbl_const(ctx, constant);
            return;
        default:
            break;
    }

    intmax_t value;
    /* TODO TRY */ string_to_intmax(
        ctx->errors, ctx->identifiers->hash_table[ctx->next_tok->tok].c_str(), ctx->next_tok->line, &value);
    if (value > 9223372036854775807ll) {
        THROW_AT_LINE_EX(
            GET_PARSER_MSG(MSG_overflow_long_const, ctx->identifiers->hash_table[ctx->next_tok->tok].c_str()),
            ctx->next_tok->line);
    }
    if (ctx->next_tok->tok_kind == TOK_int_const && value <= 2147483647l) {
        *constant = parse_int_const(value);
    }
    else {
        *constant = parse_long_const(value);
    }
}

// <const> ::= <unsigned int> | <unsigned long>
// (unsigned) const = ConstUInt(uint) | ConstULong(ulong) | ConstUChar(int)
static void /* TODO TRY */ parse_unsigned_const(Ctx ctx, return_t(std::shared_ptr<CConst>) constant) {
    /* TODO TRY */ pop_next(ctx);

    uintmax_t value;
    /* TODO TRY */ string_to_uintmax(
        ctx->errors, ctx->identifiers->hash_table[ctx->next_tok->tok].c_str(), ctx->next_tok->line, &value);
    if (value > 18446744073709551615ull) {
        THROW_AT_LINE_EX(
            GET_PARSER_MSG(MSG_overflow_ulong_const, ctx->identifiers->hash_table[ctx->next_tok->tok].c_str()),
            ctx->next_tok->line);
    }
    if (ctx->next_tok->tok_kind == TOK_uint_const && value <= 4294967295ul) {
        *constant = parse_uint_const(value);
    }
    else {
        *constant = parse_ulong_const(value);
    }
}

static void /* TODO TRY */ parse_arr_size(Ctx ctx, return_t(TLong) size) {
    /* TODO TRY */ pop_next(ctx);
    std::shared_ptr<CConst> constant;
    /* TODO TRY */ peek_next(ctx);
    switch (ctx->peek_tok->tok_kind) {
        case TOK_int_const:
        case TOK_long_const:
        case TOK_char_const:
            /* TODO TRY */ parse_const(ctx, &constant);
            break;
        case TOK_uint_const:
        case TOK_ulong_const:
            /* TODO TRY */ parse_unsigned_const(ctx, &constant);
            break;
        default:
            THROW_AT_LINE_EX(GET_PARSER_MSG(MSG_arr_size_not_int_const, get_tok_fmt(ctx->identifiers, ctx->peek_tok)),
                ctx->peek_tok->line);
    }
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_close_bracket);
    switch (constant->type()) {
        case AST_CConstInt_t: {
            *size = (TLong)(static_cast<CConstInt*>(constant.get())->value);
            break;
        }
        case AST_CConstLong_t: {
            *size = static_cast<CConstLong*>(constant.get())->value;
            break;
        }
        case AST_CConstUInt_t: {
            *size = (TLong)(static_cast<CConstUInt*>(constant.get())->value);
            break;
        }
        case AST_CConstULong_t: {
            *size = (TLong)(static_cast<CConstULong*>(constant.get())->value);
            break;
        }
        default:
            THROW_ABORT;
    }
}

// <unop> ::= "-" | "~" | "!" | "*" | "&" | "++" | "--"
// unary_operator = Complement | Negate | Not | Prefix | Postfix
static void /* TODO TRY */ parse_unop(Ctx ctx, return_t(std::unique_ptr<CUnaryOp>) unop) {
    /* TODO TRY */ pop_next(ctx);
    switch (ctx->next_tok->tok_kind) {
        case TOK_unop_complement: {
            *unop = std::make_unique<CComplement>();
            break;
        }
        case TOK_unop_neg: {
            *unop = std::make_unique<CNegate>();
            break;
        }
        case TOK_unop_not: {
            *unop = std::make_unique<CNot>();
            break;
        }
        default:
            THROW_AT_LINE_EX(
                GET_PARSER_MSG(MSG_expect_unop, get_tok_fmt(ctx->identifiers, ctx->next_tok)), ctx->next_tok->line);
    }
}

// <binop> ::= "-" | "+" | "*" | "/" | "%" | "&" | "|" | "^" | "<<" | ">>" | "&&" | "||" | "==" | "!=" | "<"
//           | "<=" | ">" | ">=" | "=" | "-=" | "+=" | "*=" | "/=" | "%=" | "&=" | "|=" | "^=" | "<<=" | ">>="
// binary_operator = Add | Subtract | Multiply | Divide | Remainder | BitAnd | BitOr | BitXor | BitShiftLeft
//                 | BitShiftRight | BitShrArithmetic | And | Or | Equal | NotEqual | LessThan | LessOrEqual
//                 | GreaterThan | GreaterOrEqual
static void /* TODO TRY */ parse_binop(Ctx ctx, return_t(std::unique_ptr<CBinaryOp>) binop) {
    /* TODO TRY */ pop_next(ctx);
    switch (ctx->next_tok->tok_kind) {
        case TOK_binop_add:
        case TOK_assign_add:
        case TOK_unop_incr: {
            *binop = std::make_unique<CAdd>();
            break;
        }
        case TOK_unop_neg:
        case TOK_assign_subtract:
        case TOK_unop_decr: {
            *binop = std::make_unique<CSubtract>();
            break;
        }
        case TOK_binop_multiply:
        case TOK_assign_multiply: {
            *binop = std::make_unique<CMultiply>();
            break;
        }
        case TOK_binop_divide:
        case TOK_assign_divide: {
            *binop = std::make_unique<CDivide>();
            break;
        }
        case TOK_binop_remainder:
        case TOK_assign_remainder: {
            *binop = std::make_unique<CRemainder>();
            break;
        }
        case TOK_binop_bitand:
        case TOK_assign_bitand: {
            *binop = std::make_unique<CBitAnd>();
            break;
        }
        case TOK_binop_bitor:
        case TOK_assign_bitor: {
            *binop = std::make_unique<CBitOr>();
            break;
        }
        case TOK_binop_xor:
        case TOK_assign_xor: {
            *binop = std::make_unique<CBitXor>();
            break;
        }
        case TOK_binop_shiftleft:
        case TOK_assign_shiftleft: {
            *binop = std::make_unique<CBitShiftLeft>();
            break;
        }
        case TOK_binop_shiftright:
        case TOK_assign_shiftright: {
            *binop = std::make_unique<CBitShiftRight>();
            break;
        }
        case TOK_binop_and: {
            *binop = std::make_unique<CAnd>();
            break;
        }
        case TOK_binop_or: {
            *binop = std::make_unique<COr>();
            break;
        }
        case TOK_binop_eq: {
            *binop = std::make_unique<CEqual>();
            break;
        }
        case TOK_binop_ne: {
            *binop = std::make_unique<CNotEqual>();
            break;
        }
        case TOK_binop_lt: {
            *binop = std::make_unique<CLessThan>();
            break;
        }
        case TOK_binop_le: {
            *binop = std::make_unique<CLessOrEqual>();
            break;
        }
        case TOK_binop_gt: {
            *binop = std::make_unique<CGreaterThan>();
            break;
        }
        case TOK_binop_ge: {
            *binop = std::make_unique<CGreaterOrEqual>();
            break;
        }
        default:
            THROW_AT_LINE_EX(
                GET_PARSER_MSG(MSG_expect_binop, get_tok_fmt(ctx->identifiers, ctx->next_tok)), ctx->next_tok->line);
    }
}

static void proc_abstract_decltor(
    CAbstractDeclarator* node, std::shared_ptr<Type>&& base_type, AbstractDeclarator& abstract_decltor);

static void proc_ptr_abstract_decltor(
    CAbstractPointer* node, std::shared_ptr<Type>&& base_type, AbstractDeclarator& abstract_decltor) {
    std::shared_ptr<Type> derived_type = std::make_shared<Pointer>(std::move(base_type));
    proc_abstract_decltor(node->abstract_decltor.get(), std::move(derived_type), abstract_decltor);
}

static void proc_arr_abstract_decltor(
    CAbstractArray* node, std::shared_ptr<Type>&& base_type, AbstractDeclarator& abstract_decltor) {
    TLong size = node->size;
    std::shared_ptr<Type> derived_type = std::make_shared<Array>(size, std::move(base_type));
    proc_abstract_decltor(node->abstract_decltor.get(), std::move(derived_type), abstract_decltor);
}

static void proc_base_abstract_decltor(std::shared_ptr<Type>&& base_type, AbstractDeclarator& abstract_decltor) {
    abstract_decltor.derived_type = std::move(base_type);
}

static void proc_abstract_decltor(
    CAbstractDeclarator* node, std::shared_ptr<Type>&& base_type, AbstractDeclarator& abstract_decltor) {
    switch (node->type()) {
        case AST_CAbstractPointer_t:
            proc_ptr_abstract_decltor(static_cast<CAbstractPointer*>(node), std::move(base_type), abstract_decltor);
            break;
        case AST_CAbstractArray_t:
            proc_arr_abstract_decltor(static_cast<CAbstractArray*>(node), std::move(base_type), abstract_decltor);
            break;
        case AST_CAbstractBase_t:
            proc_base_abstract_decltor(std::move(base_type), abstract_decltor);
            break;
        default:
            THROW_ABORT;
    }
}

static void /* TODO TRY */ parse_abstract_decltor(
    Ctx ctx, return_t(std::unique_ptr<CAbstractDeclarator>) abstract_decltor);

// (array) <direct-abstract-declarator> ::= { "[" <const> "]" }+
static void /* TODO TRY */ parse_arr_abstract_decltor(
    Ctx ctx, return_t(std::unique_ptr<CAbstractDeclarator>) abstract_decltor) {
    *abstract_decltor = std::make_unique<CAbstractBase>();
    do {
        TLong size;
        /* TODO TRY */ parse_arr_size(ctx, &size);
        *abstract_decltor = std::make_unique<CAbstractArray>(size, std::move(*abstract_decltor));
        /* TODO TRY */ peek_next(ctx);
    }
    while (ctx->peek_tok->tok_kind == TOK_open_bracket);
}

// (direct) <direct-abstract-declarator> ::= "(" <abstract-declarator> ")" { "[" <const> "]" }
static void /* TODO TRY */ parse_direct_abstract_decltor(
    Ctx ctx, return_t(std::unique_ptr<CAbstractDeclarator>) abstract_decltor) {
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ parse_abstract_decltor(ctx, abstract_decltor);
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_close_paren);
    /* TODO TRY */ peek_next(ctx);
    while (ctx->peek_tok->tok_kind == TOK_open_bracket) {
        TLong size;
        /* TODO TRY */ parse_arr_size(ctx, &size);
        *abstract_decltor = std::make_unique<CAbstractArray>(size, std::move(*abstract_decltor));
        /* TODO TRY */ peek_next(ctx);
    }
}

static void /* TODO TRY */ parse_ptr_abstract_decltor(
    Ctx ctx, return_t(std::unique_ptr<CAbstractDeclarator>) abstract_decltor) {
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ peek_next(ctx);
    if (ctx->peek_tok->tok_kind == TOK_close_paren) {
        *abstract_decltor = std::make_unique<CAbstractBase>();
    }
    else {
        /* TODO TRY */ parse_abstract_decltor(ctx, abstract_decltor);
    }
    *abstract_decltor = std::make_unique<CAbstractPointer>(std::move(*abstract_decltor));
}

// <abstract-declarator> ::= "*" [ <abstract-declarator> ] | <direct-abstract-declarator>
// abstract_declarator = AbstractPointer(abstract_declarator) | AbstractArray(int, abstract_declarator) | AbstractBase
static void /* TODO TRY */ parse_abstract_decltor(
    Ctx ctx, return_t(std::unique_ptr<CAbstractDeclarator>) abstract_decltor) {
    /* TODO TRY */ peek_next(ctx);
    switch (ctx->peek_tok->tok_kind) {
        case TOK_binop_multiply:
            /* TODO TRY */ parse_ptr_abstract_decltor(ctx, abstract_decltor);
            break;
        case TOK_open_paren:
            /* TODO TRY */ parse_direct_abstract_decltor(ctx, abstract_decltor);
            break;
        case TOK_open_bracket:
            /* TODO TRY */ parse_arr_abstract_decltor(ctx, abstract_decltor);
            break;
        default:
            THROW_AT_LINE_EX(GET_PARSER_MSG(MSG_expect_abstract_decltor, get_tok_fmt(ctx->identifiers, ctx->peek_tok)),
                ctx->peek_tok->line);
    }
}

static void /* TODO TRY */ parse_type_specifier(Ctx ctx, return_t(std::shared_ptr<Type>) type_specifier);

static void /* TODO TRY */ parse_unary_exp_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp);
static void /* TODO TRY */ parse_cast_exp_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp);
static void /* TODO TRY */ parse_exp(Ctx ctx, int32_t min_precedence, return_t(std::unique_ptr<CExp>) exp);

static void /* TODO TRY */ parse_decltor_cast_factor(Ctx ctx, return_t(std::shared_ptr<Type>) target_type) {
    AbstractDeclarator abstract_decltor;
    std::unique_ptr<CAbstractDeclarator> abstract_decltor_1;
    /* TODO TRY */ parse_abstract_decltor(ctx, &abstract_decltor_1);
    proc_abstract_decltor(abstract_decltor_1.get(), std::move(*target_type), abstract_decltor);
    *target_type = std::move(abstract_decltor.derived_type);
}

// <type-name> ::= { <type-specifier> }+ [ <abstract-declarator> ]
static void /* TODO TRY */ parse_type_name(Ctx ctx, return_t(std::shared_ptr<Type>) target_type) {
    /* TODO TRY */ parse_type_specifier(ctx, target_type);
    /* TODO TRY */ peek_next(ctx);
    switch (ctx->peek_tok->tok_kind) {
        case TOK_binop_multiply:
        case TOK_open_paren:
        case TOK_open_bracket:
            /* TODO TRY */ parse_decltor_cast_factor(ctx, target_type);
            break;
        default:
            break;
    }
}

// <argument-list> ::= <exp> { "," <exp> }
static void /* TODO TRY */ parse_arg_list(Ctx ctx, return_t(std::vector<std::unique_ptr<CExp>>) args) {
    {
        std::unique_ptr<CExp> arg;
        /* TODO TRY */ parse_exp(ctx, 0, &arg);
        args->push_back(std::move(arg));
    }
    /* TODO TRY */ peek_next(ctx);
    while (ctx->peek_tok->tok_kind == TOK_comma_separator) {
        /* TODO TRY */ pop_next(ctx);
        std::unique_ptr<CExp> arg;
        /* TODO TRY */ parse_exp(ctx, 0, &arg);
        args->push_back(std::move(arg));
        /* TODO TRY */ peek_next(ctx);
    }
}

static void /* TODO TRY */ parse_const_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    size_t line = ctx->peek_tok->line;
    std::shared_ptr<CConst> constant;
    /* TODO TRY */ parse_const(ctx, &constant);
    *exp = std::make_unique<CConstant>(std::move(constant), line);
}

static void /* TODO TRY */ parse_unsigned_const_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    size_t line = ctx->peek_tok->line;
    std::shared_ptr<CConst> constant;
    /* TODO TRY */ parse_unsigned_const(ctx, &constant);
    *exp = std::make_unique<CConstant>(std::move(constant), line);
}

static void /* TODO TRY */ parse_string_literal_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    size_t line = ctx->peek_tok->line;
    /* TODO TRY */ pop_next(ctx);
    std::shared_ptr<CStringLiteral> literal;
    /* TODO TRY */ parse_string_literal(ctx, &literal);
    *exp = std::make_unique<CString>(std::move(literal), line);
}

static void /* TODO TRY */ parse_var_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    size_t line = ctx->peek_tok->line;
    TIdentifier name;
    /* TODO TRY */ parse_identifier(ctx, 0, &name);
    *exp = std::make_unique<CVar>(name, line);
}

static void /* TODO TRY */ parse_call_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    size_t line = ctx->peek_tok->line;
    TIdentifier name;
    /* TODO TRY */ parse_identifier(ctx, 0, &name);
    /* TODO TRY */ pop_next(ctx);
    std::vector<std::unique_ptr<CExp>> args;
    /* TODO TRY */ peek_next(ctx);
    if (ctx->peek_tok->tok_kind != TOK_close_paren) {
        /* TODO TRY */ parse_arg_list(ctx, &args);
    }
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_close_paren);
    *exp = std::make_unique<CFunctionCall>(name, std::move(args), line);
}

static void /* TODO TRY */ parse_inner_exp_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) inner_exp) {
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ parse_exp(ctx, 0, inner_exp);
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_close_paren);
}

static void /* TODO TRY */ parse_subscript_factor(
    Ctx ctx, std::unique_ptr<CExp>&& primary_exp, return_t(std::unique_ptr<CExp>) subscript_exp) {
    size_t line = ctx->peek_tok->line;
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ parse_exp(ctx, 0, subscript_exp);
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_close_bracket);
    *subscript_exp = std::make_unique<CSubscript>(std::move(primary_exp), std::move(*subscript_exp), line);
}

static void /* TODO TRY */ parse_dot_factor(
    Ctx ctx, std::unique_ptr<CExp>&& structure, return_t(std::unique_ptr<CExp>) exp) {
    size_t line = ctx->peek_tok->line;
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ peek_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->peek_tok, TOK_identifier);
    TIdentifier member;
    /* TODO TRY */ parse_identifier(ctx, 0, &member);
    *exp = std::make_unique<CDot>(member, std::move(structure), line);
}

static void /* TODO TRY */ parse_arrow_factor(
    Ctx ctx, std::unique_ptr<CExp>&& pointer, return_t(std::unique_ptr<CExp>) exp) {
    size_t line = ctx->peek_tok->line;
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ peek_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->peek_tok, TOK_identifier);
    TIdentifier member;
    /* TODO TRY */ parse_identifier(ctx, 0, &member);
    *exp = std::make_unique<CArrow>(member, std::move(pointer), line);
}

static void /* TODO TRY */ parse_postfix_incr_factor(
    Ctx ctx, std::unique_ptr<CExp>&& exp_left, return_t(std::unique_ptr<CExp>) exp) {
    size_t line = ctx->peek_tok->line;
    std::unique_ptr<CUnaryOp> unop = std::make_unique<CPostfix>();
    std::unique_ptr<CExp> exp_left_1;
    std::unique_ptr<CExp> exp_right_1;
    {
        std::unique_ptr<CBinaryOp> binop;
        /* TODO TRY */ parse_binop(ctx, &binop);
        std::unique_ptr<CExp> exp_right;
        {
            std::shared_ptr<CConst> constant = std::make_shared<CConstInt>(1);
            exp_right = std::make_unique<CConstant>(std::move(constant), line);
        }
        exp_right_1 = std::make_unique<CBinary>(std::move(binop), std::move(exp_left), std::move(exp_right), line);
    }
    *exp = std::make_unique<CAssignment>(std::move(unop), std::move(exp_left_1), std::move(exp_right_1), line);
}

static void /* TODO TRY */ parse_unary_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    size_t line = ctx->peek_tok->line;
    std::unique_ptr<CUnaryOp> unop;
    /* TODO TRY */ parse_unop(ctx, &unop);
    std::unique_ptr<CExp> cast_exp;
    /* TODO TRY */ parse_cast_exp_factor(ctx, &cast_exp);
    *exp = std::make_unique<CUnary>(std::move(unop), std::move(cast_exp), line);
}

static void /* TODO TRY */ parse_incr_unary_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    size_t line = ctx->peek_tok->line;
    std::unique_ptr<CUnaryOp> unop = std::make_unique<CPrefix>();
    std::unique_ptr<CExp> exp_left_1;
    std::unique_ptr<CExp> exp_right_1;
    {
        std::unique_ptr<CBinaryOp> binop;
        /* TODO TRY */ parse_binop(ctx, &binop);
        std::unique_ptr<CExp> exp_left;
        /* TODO TRY */ parse_cast_exp_factor(ctx, &exp_left);
        std::unique_ptr<CExp> exp_right;
        {
            std::shared_ptr<CConst> constant = std::make_shared<CConstInt>(1);
            exp_right = std::make_unique<CConstant>(std::move(constant), line);
        }
        exp_right_1 = std::make_unique<CBinary>(std::move(binop), std::move(exp_left), std::move(exp_right), line);
    }
    *exp = std::make_unique<CAssignment>(std::move(unop), std::move(exp_left_1), std::move(exp_right_1), line);
}

static void /* TODO TRY */ parse_deref_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    size_t line = ctx->next_tok->line;
    std::unique_ptr<CExp> cast_exp;
    /* TODO TRY */ parse_cast_exp_factor(ctx, &cast_exp);
    *exp = std::make_unique<CDereference>(std::move(cast_exp), line);
}

static void /* TODO TRY */ parse_addrof_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    size_t line = ctx->next_tok->line;
    std::unique_ptr<CExp> cast_exp;
    /* TODO TRY */ parse_cast_exp_factor(ctx, &cast_exp);
    *exp = std::make_unique<CAddrOf>(std::move(cast_exp), line);
}

static void /* TODO TRY */ parse_ptr_unary_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    /* TODO TRY */ pop_next(ctx);
    switch (ctx->next_tok->tok_kind) {
        case TOK_binop_multiply:
            /* TODO TRY */ parse_deref_factor(ctx, exp);
            break;
        case TOK_binop_bitand:
            /* TODO TRY */ parse_addrof_factor(ctx, exp);
            break;
        default:
            THROW_AT_LINE_EX(GET_PARSER_MSG(MSG_expect_ptr_unary_factor, get_tok_fmt(ctx->identifiers, ctx->next_tok)),
                ctx->next_tok->line);
    }
}

static void /* TODO TRY */ parse_sizeoft_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    size_t line = ctx->peek_tok->line;
    /* TODO TRY */ pop_next(ctx);
    std::shared_ptr<Type> target_type;
    /* TODO TRY */ parse_type_name(ctx, &target_type);
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_close_paren);
    *exp = std::make_unique<CSizeOfT>(std::move(target_type), line);
}

static void /* TODO TRY */ parse_sizeof_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    size_t line = ctx->peek_tok->line;
    std::unique_ptr<CExp> unary_exp;
    /* TODO TRY */ parse_unary_exp_factor(ctx, &unary_exp);
    *exp = std::make_unique<CSizeOf>(std::move(unary_exp), line);
}

static void /* TODO TRY */ parse_sizeof_unary_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ peek_next(ctx);
    if (ctx->peek_tok->tok_kind == TOK_open_paren) {
        /* TODO TRY */ peek_next_i(ctx, 1);
        switch (ctx->peek_tok_i->tok_kind) {
            case TOK_key_char:
            case TOK_key_int:
            case TOK_key_long:
            case TOK_key_double:
            case TOK_key_unsigned:
            case TOK_key_signed:
            case TOK_key_void:
            case TOK_key_struct:
            case TOK_key_union:
                /* TODO TRY */ parse_sizeoft_factor(ctx, exp);
                return;
            default:
                break;
        }
    }
    /* TODO TRY */ parse_sizeof_factor(ctx, exp);
}

static void /* TODO TRY */ parse_cast_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    size_t line = ctx->peek_tok->line;
    /* TODO TRY */ pop_next(ctx);
    std::shared_ptr<Type> target_type;
    /* TODO TRY */ parse_type_name(ctx, &target_type);
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_close_paren);
    std::unique_ptr<CExp> cast_exp;
    /* TODO TRY */ parse_cast_exp_factor(ctx, &cast_exp);
    *exp = std::make_unique<CCast>(std::move(cast_exp), std::move(target_type), line);
}

// <primary-exp> ::= <const> | <identifier> | "(" <exp> ")" | { <string> }+ | <identifier> "(" [ <argument-list> ] ")"
static void /* TODO TRY */ parse_primary_exp_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    /* TODO TRY */ peek_next(ctx);
    switch (ctx->peek_tok->tok_kind) {
        case TOK_int_const:
        case TOK_long_const:
        case TOK_char_const:
        case TOK_dbl_const:
            /* TODO TRY */ parse_const_factor(ctx, exp);
            break;
        case TOK_uint_const:
        case TOK_ulong_const:
            /* TODO TRY */ parse_unsigned_const_factor(ctx, exp);
            break;
        case TOK_identifier: {
            /* TODO TRY */ peek_next_i(ctx, 1);
            if (ctx->peek_tok_i->tok_kind == TOK_open_paren) {
                /* TODO TRY */ parse_call_factor(ctx, exp);
            }
            else {
                /* TODO TRY */ parse_var_factor(ctx, exp);
            }
            break;
        }
        case TOK_string_literal:
            /* TODO TRY */ parse_string_literal_factor(ctx, exp);
            break;
        case TOK_open_paren:
            /* TODO TRY */ parse_inner_exp_factor(ctx, exp);
            break;
        default:
            THROW_AT_LINE_EX(
                GET_PARSER_MSG(MSG_expect_primary_exp_factor, get_tok_fmt(ctx->identifiers, ctx->peek_tok)),
                ctx->peek_tok->line);
    }
}

// <postfix-op> ::= "[" <exp> "]" | "." <identifier> | "->" <identifier>
static void /* TODO TRY */ parse_postfix_op_exp_factor(
    Ctx ctx, std::unique_ptr<CExp>&& primary_exp, return_t(std::unique_ptr<CExp>) exp) {
    std::unique_ptr<CExp> postfix_exp;
    /* TODO TRY */ peek_next(ctx);
    switch (ctx->peek_tok->tok_kind) {
        case TOK_open_bracket:
            /* TODO TRY */ parse_subscript_factor(ctx, std::move(primary_exp), &postfix_exp);
            break;
        case TOK_structop_member:
            /* TODO TRY */ parse_dot_factor(ctx, std::move(primary_exp), &postfix_exp);
            break;
        case TOK_structop_ptr:
            /* TODO TRY */ parse_arrow_factor(ctx, std::move(primary_exp), &postfix_exp);
            break;
        case TOK_unop_incr:
        case TOK_unop_decr:
            /* TODO TRY */ parse_postfix_incr_factor(ctx, std::move(primary_exp), &postfix_exp);
            break;
        default: {
            *exp = std::move(primary_exp);
            return;
        }
    }
    /* TODO TRY */ parse_postfix_op_exp_factor(ctx, std::move(postfix_exp), exp);
}

// <postfix-exp> ::= <primary-exp> { <postfix-op> }
static void /* TODO TRY */ parse_postfix_exp_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    std::unique_ptr<CExp> primary_exp;
    /* TODO TRY */ parse_primary_exp_factor(ctx, &primary_exp);
    /* TODO TRY */ peek_next(ctx);
    switch (ctx->peek_tok->tok_kind) {
        case TOK_open_bracket:
        case TOK_structop_member:
        case TOK_structop_ptr:
        case TOK_unop_incr:
        case TOK_unop_decr:
            /* TODO TRY */ parse_postfix_op_exp_factor(ctx, std::move(primary_exp), exp);
            break;
        default: {
            *exp = std::move(primary_exp);
            break;
        }
    }
}

//<unary-exp> ::= <unop> <cast-exp> | "sizeof" <unary-exp> | "sizeof" "(" <type-name> ")" | <postfix-exp>
static void /* TODO TRY */ parse_unary_exp_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    /* TODO TRY */ peek_next(ctx);
    switch (ctx->peek_tok->tok_kind) {
        case TOK_unop_complement:
        case TOK_unop_neg:
        case TOK_unop_not:
            /* TODO TRY */ parse_unary_factor(ctx, exp);
            break;
        case TOK_unop_incr:
        case TOK_unop_decr:
            /* TODO TRY */ parse_incr_unary_factor(ctx, exp);
            break;
        case TOK_binop_multiply:
        case TOK_binop_bitand:
            /* TODO TRY */ parse_ptr_unary_factor(ctx, exp);
            break;
        case TOK_key_sizeof:
            /* TODO TRY */ parse_sizeof_unary_factor(ctx, exp);
            break;
        default:
            /* TODO TRY */ parse_postfix_exp_factor(ctx, exp);
            break;
    }
}

// <cast-exp> ::= "(" <type-name> ")" <cast-exp> | <unary-exp>
static void /* TODO TRY */ parse_cast_exp_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    /* TODO TRY */ peek_next(ctx);
    if (ctx->peek_tok->tok_kind == TOK_open_paren) {
        /* TODO TRY */ peek_next_i(ctx, 1);
        switch (ctx->peek_tok_i->tok_kind) {
            case TOK_key_char:
            case TOK_key_int:
            case TOK_key_long:
            case TOK_key_double:
            case TOK_key_unsigned:
            case TOK_key_signed:
            case TOK_key_void:
            case TOK_key_struct:
            case TOK_key_union:
                /* TODO TRY */ parse_cast_factor(ctx, exp);
                return;
            default:
                break;
        }
    }
    /* TODO TRY */ parse_unary_exp_factor(ctx, exp);
}

static void /* TODO TRY */ parse_assign_exp(
    Ctx ctx, std::unique_ptr<CExp>&& exp_left, int32_t precedence, return_t(std::unique_ptr<CExp>) exp) {
    size_t line = ctx->peek_tok->line;
    /* TODO TRY */ pop_next(ctx);
    std::unique_ptr<CUnaryOp> unop;
    std::unique_ptr<CExp> exp_right;
    /* TODO TRY */ parse_exp(ctx, precedence, &exp_right);
    *exp = std::make_unique<CAssignment>(std::move(unop), std::move(exp_left), std::move(exp_right), line);
}

static void /* TODO TRY */ parse_assign_compound_exp(
    Ctx ctx, std::unique_ptr<CExp>&& exp_left, int32_t precedence, return_t(std::unique_ptr<CExp>) exp) {
    size_t line = ctx->peek_tok->line;
    std::unique_ptr<CUnaryOp> unop;
    std::unique_ptr<CExp> exp_left_1;
    std::unique_ptr<CExp> exp_right_1;
    {
        std::unique_ptr<CBinaryOp> binop;
        /* TODO TRY */ parse_binop(ctx, &binop);
        std::unique_ptr<CExp> exp_right;
        /* TODO TRY */ parse_exp(ctx, precedence, &exp_right);
        exp_right_1 = std::make_unique<CBinary>(std::move(binop), std::move(exp_left), std::move(exp_right), line);
    }
    *exp = std::make_unique<CAssignment>(std::move(unop), std::move(exp_left_1), std::move(exp_right_1), line);
}

static void /* TODO TRY */ parse_binary_exp(
    Ctx ctx, std::unique_ptr<CExp>&& exp_left, int32_t precedence, return_t(std::unique_ptr<CExp>) exp) {
    size_t line = ctx->peek_tok->line;
    std::unique_ptr<CBinaryOp> binop;
    /* TODO TRY */ parse_binop(ctx, &binop);
    std::unique_ptr<CExp> exp_right;
    /* TODO TRY */ parse_exp(ctx, precedence + 1, &exp_right);
    *exp = std::make_unique<CBinary>(std::move(binop), std::move(exp_left), std::move(exp_right), line);
}

static void /* TODO TRY */ parse_ternary_exp(
    Ctx ctx, std::unique_ptr<CExp>&& exp_left, int32_t precedence, return_t(std::unique_ptr<CExp>) exp) {
    size_t line = ctx->peek_tok->line;
    /* TODO TRY */ pop_next(ctx);
    std::unique_ptr<CExp> exp_middle;
    /* TODO TRY */ parse_exp(ctx, 0, &exp_middle);
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_ternary_else);
    std::unique_ptr<CExp> exp_right;
    /* TODO TRY */ parse_exp(ctx, precedence, &exp_right);
    *exp = std::make_unique<CConditional>(std::move(exp_left), std::move(exp_middle), std::move(exp_right), line);
}

static int32_t get_tok_precedence(TOKEN_KIND tok_kind) {
    switch (tok_kind) {
        case TOK_binop_multiply:
        case TOK_binop_divide:
        case TOK_binop_remainder:
            return 50;
        case TOK_unop_neg:
        case TOK_binop_add:
            return 45;
        case TOK_binop_shiftleft:
        case TOK_binop_shiftright:
            return 40;
        case TOK_binop_lt:
        case TOK_binop_le:
        case TOK_binop_gt:
        case TOK_binop_ge:
            return 35;
        case TOK_binop_eq:
        case TOK_binop_ne:
            return 30;
        case TOK_binop_bitand:
            return 25;
        case TOK_binop_xor:
            return 20;
        case TOK_binop_bitor:
            return 15;
        case TOK_binop_and:
            return 10;
        case TOK_binop_or:
            return 5;
        case TOK_ternary_if:
            return 3;
        case TOK_assign:
        case TOK_assign_add:
        case TOK_assign_subtract:
        case TOK_assign_multiply:
        case TOK_assign_divide:
        case TOK_assign_remainder:
        case TOK_assign_bitand:
        case TOK_assign_bitor:
        case TOK_assign_xor:
        case TOK_assign_shiftleft:
        case TOK_assign_shiftright:
            return 1;
        default:
            return -1;
    }
}

// <exp> ::= <cast-exp> | <exp> <binop> <exp> | <exp> "?" <exp> ":" <exp>
// exp = Constant(const, type) | String(string, type) | Var(identifier, type) | Cast(type, exp, type)
//     | Unary(unary_operator, exp, type) | Binary(binary_operator, exp, exp, type)
//     | Assignment(unary_operator, exp, exp, type) | Conditional(exp, exp, exp, type)
//     | FunctionCall(identifier, exp*, type) | Dereference(exp, type) | AddrOf(exp, type)
//     | Subscript(exp, exp, type) | SizeOf(exp, type) | SizeOfT(type, type) | Dot(exp, identifier, type)
//     | Arrow(exp, identifier, type)
static void /* TODO TRY */ parse_exp(Ctx ctx, int32_t min_precedence, return_t(std::unique_ptr<CExp>) exp) {
    /* TODO TRY */ parse_cast_exp_factor(ctx, exp);
    while (true) {
        /* TODO TRY */ peek_next(ctx);
        int32_t precedence = get_tok_precedence(ctx->peek_tok->tok_kind);
        if (precedence < min_precedence) {
            break;
        }
        std::unique_ptr<CExp> exp_left = std::move(*exp);
        switch (ctx->peek_tok->tok_kind) {
            case TOK_binop_add:
            case TOK_unop_neg:
            case TOK_binop_multiply:
            case TOK_binop_divide:
            case TOK_binop_remainder:
            case TOK_binop_bitand:
            case TOK_binop_bitor:
            case TOK_binop_xor:
            case TOK_binop_shiftleft:
            case TOK_binop_shiftright:
            case TOK_binop_lt:
            case TOK_binop_le:
            case TOK_binop_gt:
            case TOK_binop_ge:
            case TOK_binop_eq:
            case TOK_binop_ne:
            case TOK_binop_and:
            case TOK_binop_or:
                /* TODO TRY */ parse_binary_exp(ctx, std::move(exp_left), precedence, exp);
                break;
            case TOK_assign:
                /* TODO TRY */ parse_assign_exp(ctx, std::move(exp_left), precedence, exp);
                break;
            case TOK_assign_add:
            case TOK_assign_subtract:
            case TOK_assign_multiply:
            case TOK_assign_divide:
            case TOK_assign_remainder:
            case TOK_assign_bitand:
            case TOK_assign_bitor:
            case TOK_assign_xor:
            case TOK_assign_shiftleft:
            case TOK_assign_shiftright:
                /* TODO TRY */ parse_assign_compound_exp(ctx, std::move(exp_left), precedence, exp);
                break;
            case TOK_ternary_if:
                /* TODO TRY */ parse_ternary_exp(ctx, std::move(exp_left), precedence, exp);
                break;
            default:
                THROW_AT_LINE_EX(
                    GET_PARSER_MSG(MSG_expect_exp, get_tok_fmt(ctx->identifiers, ctx->peek_tok)), ctx->peek_tok->line);
        }
    }
}

static void /* TODO TRY */ parse_for_init(Ctx ctx, return_t(std::unique_ptr<CForInit>) for_init);
static void /* TODO TRY */ parse_block(Ctx ctx, return_t(std::unique_ptr<CBlock>) block);
static void /* TODO TRY */ parse_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement);

static void /* TODO TRY */ parse_ret_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    size_t line = ctx->peek_tok->line;
    /* TODO TRY */ pop_next(ctx);
    std::unique_ptr<CExp> exp;
    /* TODO TRY */ peek_next(ctx);
    if (ctx->peek_tok->tok_kind != TOK_semicolon) {
        /* TODO TRY */ parse_exp(ctx, 0, &exp);
    }
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_semicolon);
    *statement = std::make_unique<CReturn>(std::move(exp), line);
}

static void /* TODO TRY */ parse_exp_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    std::unique_ptr<CExp> exp;
    /* TODO TRY */ parse_exp(ctx, 0, &exp);
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_semicolon);
    *statement = std::make_unique<CExpression>(std::move(exp));
}

static void /* TODO TRY */ parse_if_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_open_paren);
    std::unique_ptr<CExp> condition;
    /* TODO TRY */ parse_exp(ctx, 0, &condition);
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_close_paren);
    /* TODO TRY */ peek_next(ctx);
    std::unique_ptr<CStatement> then;
    /* TODO TRY */ parse_statement(ctx, &then);
    std::unique_ptr<CStatement> else_fi;
    /* TODO TRY */ peek_next(ctx);
    if (ctx->peek_tok->tok_kind == TOK_key_else) {
        /* TODO TRY */ pop_next(ctx);
        /* TODO TRY */ peek_next(ctx);
        /* TODO TRY */ parse_statement(ctx, &else_fi);
    }
    *statement = std::make_unique<CIf>(std::move(condition), std::move(then), std::move(else_fi));
}

static void /* TODO TRY */ parse_goto_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    size_t line = ctx->peek_tok->line;
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ peek_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->peek_tok, TOK_identifier);
    TIdentifier target;
    /* TODO TRY */ parse_identifier(ctx, 0, &target);
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_semicolon);
    *statement = std::make_unique<CGoto>(target, line);
}

static void /* TODO TRY */ parse_label_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    size_t line = ctx->peek_tok->line;
    TIdentifier target;
    /* TODO TRY */ parse_identifier(ctx, 0, &target);
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ peek_next(ctx);
    std::unique_ptr<CStatement> jump_to;
    /* TODO TRY */ parse_statement(ctx, &jump_to);
    *statement = std::make_unique<CLabel>(target, std::move(jump_to), line);
}

static void /* TODO TRY */ parse_compound_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    std::unique_ptr<CBlock> block;
    /* TODO TRY */ parse_block(ctx, &block);
    *statement = std::make_unique<CCompound>(std::move(block));
}

static void /* TODO TRY */ parse_while_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_open_paren);
    std::unique_ptr<CExp> condition;
    /* TODO TRY */ parse_exp(ctx, 0, &condition);
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_close_paren);
    /* TODO TRY */ peek_next(ctx);
    std::unique_ptr<CStatement> body;
    /* TODO TRY */ parse_statement(ctx, &body);
    *statement = std::make_unique<CWhile>(std::move(condition), std::move(body));
}

static void /* TODO TRY */ parse_do_while_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ peek_next(ctx);
    std::unique_ptr<CStatement> body;
    /* TODO TRY */ parse_statement(ctx, &body);
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_key_while);
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_open_paren);
    std::unique_ptr<CExp> condition;
    /* TODO TRY */ parse_exp(ctx, 0, &condition);
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_close_paren);
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_semicolon);
    *statement = std::make_unique<CDoWhile>(std::move(condition), std::move(body));
}

static void /* TODO TRY */ parse_for_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_open_paren);
    std::unique_ptr<CForInit> for_init;
    /* TODO TRY */ parse_for_init(ctx, &for_init);
    std::unique_ptr<CExp> condition;
    /* TODO TRY */ peek_next(ctx);
    if (ctx->peek_tok->tok_kind != TOK_semicolon) {
        /* TODO TRY */ parse_exp(ctx, 0, &condition);
    }
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_semicolon);
    std::unique_ptr<CExp> post;
    /* TODO TRY */ peek_next(ctx);
    if (ctx->peek_tok->tok_kind != TOK_close_paren) {
        /* TODO TRY */ parse_exp(ctx, 0, &post);
    }
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_close_paren);
    /* TODO TRY */ peek_next(ctx);
    std::unique_ptr<CStatement> body;
    /* TODO TRY */ parse_statement(ctx, &body);
    *statement = std::make_unique<CFor>(std::move(for_init), std::move(condition), std::move(post), std::move(body));
}

static void /* TODO TRY */ parse_switch_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_open_paren);
    std::unique_ptr<CExp> match;
    /* TODO TRY */ parse_exp(ctx, 0, &match);
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_close_paren);
    /* TODO TRY */ peek_next(ctx);
    std::unique_ptr<CStatement> body;
    /* TODO TRY */ parse_statement(ctx, &body);
    *statement = std::make_unique<CSwitch>(std::move(match), std::move(body));
}

static void /* TODO TRY */ parse_case_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    /* TODO TRY */ pop_next(ctx);
    std::unique_ptr<CExp> value;
    {
        size_t line = ctx->peek_tok->line;
        std::shared_ptr<CConst> constant;
        /* TODO TRY */ peek_next(ctx);
        switch (ctx->peek_tok->tok_kind) {
            case TOK_int_const:
            case TOK_long_const:
            case TOK_char_const:
                /* TODO TRY */ parse_const(ctx, &constant);
                break;
            case TOK_uint_const:
            case TOK_ulong_const:
                /* TODO TRY */ parse_unsigned_const(ctx, &constant);
                break;
            default:
                THROW_AT_LINE_EX(
                    GET_PARSER_MSG(MSG_case_value_not_int_const, get_tok_fmt(ctx->identifiers, ctx->peek_tok)),
                    ctx->peek_tok->line);
        }
        value = std::make_unique<CConstant>(std::move(constant), line);
    }
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_ternary_else);
    /* TODO TRY */ peek_next(ctx);
    std::unique_ptr<CStatement> jump_to;
    /* TODO TRY */ parse_statement(ctx, &jump_to);
    *statement = std::make_unique<CCase>(std::move(value), std::move(jump_to));
}

static void /* TODO TRY */ parse_default_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    size_t line = ctx->peek_tok->line;
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_ternary_else);
    /* TODO TRY */ peek_next(ctx);
    std::unique_ptr<CStatement> jump_to;
    /* TODO TRY */ parse_statement(ctx, &jump_to);
    *statement = std::make_unique<CDefault>(std::move(jump_to), line);
}

static void /* TODO TRY */ parse_break_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    size_t line = ctx->peek_tok->line;
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_semicolon);
    *statement = std::make_unique<CBreak>(line);
}

static void /* TODO TRY */ parse_continue_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    size_t line = ctx->peek_tok->line;
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_semicolon);
    *statement = std::make_unique<CContinue>(line);
}

static void /* TODO TRY */ parse_null_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    /* TODO TRY */ pop_next(ctx);
    *statement = std::make_unique<CNull>();
}

// <statement> ::= ";" | "return" [ <exp> ] ";" | "if" "(" <exp> ")" <statement> [ "else" <statement> ]
//               | "goto" <identifier> ";" | <identifier> ":" | <block> | "do" <statement> "while" "(" <exp> ")" ";"
//               | "while" "(" <exp> ")" <statement> | "for" "(" <for-init> [ <exp> ] ";" [ <exp> ] ")" <statement>
//               | "switch" "(" <exp> ")" <statement> | "case" <const> ":" <statement> | "default" ":" <statement>
//               | "break" ";" | "continue" ";" | <exp> ";"
// statement = Return(exp?) | Expression(exp) | If(exp, statement, statement?) | Goto(identifier)
//           | Label(identifier, target) | Compound(block) | While(exp, statement, identifier)
//           | DoWhile(statement, exp, identifier) | For(for_init, exp?, exp?, statement, identifier)
//           | Switch(identifier, bool, exp, statement, exp*) | Case(identifier, exp, statement)
//           | Default(identifier, statement) | Break(identifier) | Continue(identifier) | Null
static void /* TODO TRY */ parse_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    switch (ctx->peek_tok->tok_kind) {
        case TOK_key_return:
            /* TODO TRY */ parse_ret_statement(ctx, statement);
            return;
        case TOK_key_if:
            /* TODO TRY */ parse_if_statement(ctx, statement);
            return;
        case TOK_key_goto:
            /* TODO TRY */ parse_goto_statement(ctx, statement);
            return;
        case TOK_identifier: {
            /* TODO TRY */ peek_next_i(ctx, 1);
            if (ctx->peek_tok_i->tok_kind == TOK_ternary_else) {
                /* TODO TRY */ parse_label_statement(ctx, statement);
                return;
            }
            break;
        }
        case TOK_open_brace:
            /* TODO TRY */ parse_compound_statement(ctx, statement);
            return;
        case TOK_key_while:
            /* TODO TRY */ parse_while_statement(ctx, statement);
            return;
        case TOK_key_do:
            /* TODO TRY */ parse_do_while_statement(ctx, statement);
            return;
        case TOK_key_for:
            /* TODO TRY */ parse_for_statement(ctx, statement);
            return;
        case TOK_key_switch:
            /* TODO TRY */ parse_switch_statement(ctx, statement);
            return;
        case TOK_key_case:
            /* TODO TRY */ parse_case_statement(ctx, statement);
            return;
        case TOK_key_default:
            /* TODO TRY */ parse_default_statement(ctx, statement);
            return;
        case TOK_key_break:
            /* TODO TRY */ parse_break_statement(ctx, statement);
            return;
        case TOK_key_continue:
            /* TODO TRY */ parse_continue_statement(ctx, statement);
            return;
        case TOK_semicolon:
            /* TODO TRY */ parse_null_statement(ctx, statement);
            return;
        default:
            break;
    }
    /* TODO TRY */ parse_exp_statement(ctx, statement);
}

static std::unique_ptr<CStorageClass> /* TODO TRY */ parse_decltor_decl(Ctx ctx, Declarator& decltor);
static std::unique_ptr<CVariableDeclaration> /* TODO TRY */ parse_var_declaration(
    Ctx ctx, std::unique_ptr<CStorageClass>&& storage_class, Declarator&& decltor);

static void /* TODO TRY */ parse_for_init_decl(Ctx ctx, return_t(std::unique_ptr<CForInit>) for_init) {
    Declarator decltor;
    std::unique_ptr<CStorageClass> storage_class = /* TODO TRY */ parse_decltor_decl(ctx, decltor);
    if (decltor.derived_type->type() == AST_FunType_t) {
        THROW_AT_LINE_EX(GET_PARSER_MSG(MSG_for_init_decl_as_fun, ctx->identifiers->hash_table[decltor.name].c_str()),
            ctx->next_tok->line);
    }
    std::unique_ptr<CVariableDeclaration> init =
        /* TODO TRY */ parse_var_declaration(ctx, std::move(storage_class), std::move(decltor));
    *for_init = std::make_unique<CInitDecl>(std::move(init));
}

static void /* TODO TRY */ parse_for_init_exp(Ctx ctx, return_t(std::unique_ptr<CForInit>) for_init) {
    std::unique_ptr<CExp> init;
    /* TODO TRY */ peek_next(ctx);
    if (ctx->peek_tok->tok_kind != TOK_semicolon) {
        /* TODO TRY */ parse_exp(ctx, 0, &init);
    }
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_semicolon);
    *for_init = std::make_unique<CInitExp>(std::move(init));
}

// <for-init> ::= <variable-declaration> | [ <exp> ] ";"
// for_init = InitDecl(variable_declaration) | InitExp(exp?)
static void /* TODO TRY */ parse_for_init(Ctx ctx, return_t(std::unique_ptr<CForInit>) for_init) {
    /* TODO TRY */ peek_next(ctx);
    switch (ctx->peek_tok->tok_kind) {
        case TOK_key_char:
        case TOK_key_int:
        case TOK_key_long:
        case TOK_key_double:
        case TOK_key_unsigned:
        case TOK_key_signed:
        case TOK_key_void:
        case TOK_key_struct:
        case TOK_key_union:
        case TOK_key_static:
        case TOK_key_extern:
            /* TODO TRY */ parse_for_init_decl(ctx, for_init);
            break;
        default:
            /* TODO TRY */ parse_for_init_exp(ctx, for_init);
            break;
    }
}

static std::unique_ptr<CDeclaration> /* TODO TRY */ parse_declaration(Ctx ctx);

static void /* TODO TRY */ parse_s_block_item(Ctx ctx, return_t(std::unique_ptr<CBlockItem>) block_item) {
    std::unique_ptr<CStatement> statement;
    /* TODO TRY */ parse_statement(ctx, &statement);
    *block_item = std::make_unique<CS>(std::move(statement));
}

static void /* TODO TRY */ parse_d_block_item(Ctx ctx, return_t(std::unique_ptr<CBlockItem>) block_item) {
    std::unique_ptr<CDeclaration> declaration = /* TODO TRY */ parse_declaration(ctx);
    *block_item = std::make_unique<CD>(std::move(declaration));
}

// <block-item> ::= <statement> | <declaration>
// block_item = S(statement) | D(declaration)
static void /* TODO TRY */ parse_block_item(Ctx ctx, return_t(std::unique_ptr<CBlockItem>) block_item) {
    switch (ctx->peek_tok->tok_kind) {
        case TOK_key_char:
        case TOK_key_int:
        case TOK_key_long:
        case TOK_key_double:
        case TOK_key_unsigned:
        case TOK_key_signed:
        case TOK_key_void:
        case TOK_key_struct:
        case TOK_key_union:
        case TOK_key_static:
        case TOK_key_extern:
            /* TODO TRY */ parse_d_block_item(ctx, block_item);
            break;
        default:
            /* TODO TRY */ parse_s_block_item(ctx, block_item);
            break;
    }
}

static void /* TODO TRY */ parse_b_block(Ctx ctx, return_t(std::unique_ptr<CBlock>) block) {
    std::vector<std::unique_ptr<CBlockItem>> block_items;
    /* TODO TRY */ peek_next(ctx);
    while (ctx->peek_tok->tok_kind != TOK_close_brace) {
        std::unique_ptr<CBlockItem> block_item;
        /* TODO TRY */ parse_block_item(ctx, &block_item);
        block_items.push_back(std::move(block_item));
        /* TODO TRY */ peek_next(ctx);
    }
    *block = std::make_unique<CB>(std::move(block_items));
}

// <block> ::= "{" { <block-item> } "}"
// block = B(block_item*)
static void /* TODO TRY */ parse_block(Ctx ctx, return_t(std::unique_ptr<CBlock>) block) {
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ parse_b_block(ctx, block);
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_close_brace);
}

// <type-specifier> ::= "int" | "long" | "signed" | "unsigned" | "double" | "char" | "void"
//                    | ("struct" | "union") <identifier>
static void /* TODO TRY */ parse_type_specifier(Ctx ctx, return_t(std::shared_ptr<Type>) type_specifier) {
    size_t i = 0;
    /* TODO TRY */ peek_next(ctx);
    size_t line = ctx->peek_tok->line;
    std::vector<TOKEN_KIND> type_tok_kinds;
    while (true) {
        /* TODO TRY */ peek_next_i(ctx, i);
        switch (ctx->peek_tok_i->tok_kind) {
            case TOK_identifier:
            case TOK_close_paren:
                goto Lbreak;
            case TOK_key_char:
            case TOK_key_int:
            case TOK_key_long:
            case TOK_key_double:
            case TOK_key_unsigned:
            case TOK_key_signed:
            case TOK_key_void:
                /* TODO TRY */ pop_next_i(ctx, i);
                type_tok_kinds.push_back(ctx->next_tok_i->tok_kind);
                break;
            case TOK_key_struct:
            case TOK_key_union:
                /* TODO TRY */ pop_next_i(ctx, i);
                type_tok_kinds.push_back(ctx->next_tok_i->tok_kind);
                /* TODO TRY */ peek_next_i(ctx, i);
                /* TODO TRY */ expect_next(ctx, ctx->peek_tok_i, TOK_identifier);
                break;
            case TOK_key_static:
            case TOK_key_extern:
            case TOK_binop_multiply:
            case TOK_open_paren:
                i++;
                break;
            case TOK_open_bracket: {
                do {
                    i++;
                    /* TODO TRY */ peek_next_i(ctx, i);
                }
                while (ctx->peek_tok_i->tok_kind != TOK_close_bracket);
                i++;
                break;
            }
            default:
                THROW_AT_LINE_EX(GET_PARSER_MSG(MSG_expect_specifier, get_tok_fmt(ctx->identifiers, ctx->peek_tok_i)),
                    ctx->peek_tok_i->line);
        }
    }
Lbreak:
    switch (type_tok_kinds.size()) {
        case 1: {
            switch (type_tok_kinds[0]) {
                case TOK_key_char: {
                    *type_specifier = std::make_shared<Char>();
                    return;
                }
                case TOK_key_int: {
                    *type_specifier = std::make_shared<Int>();
                    return;
                }
                case TOK_key_long: {
                    *type_specifier = std::make_shared<Long>();
                    return;
                }
                case TOK_key_double: {
                    *type_specifier = std::make_shared<Double>();
                    return;
                }
                case TOK_key_unsigned: {
                    *type_specifier = std::make_shared<UInt>();
                    return;
                }
                case TOK_key_signed: {
                    *type_specifier = std::make_shared<Int>();
                    return;
                }
                case TOK_key_void: {
                    *type_specifier = std::make_shared<Void>();
                    return;
                }
                case TOK_key_struct: {
                    TIdentifier tag;
                    /* TODO TRY */ parse_identifier(ctx, i, &tag);
                    *type_specifier = std::make_shared<Structure>(tag, false);
                    return;
                }
                case TOK_key_union: {
                    TIdentifier tag;
                    /* TODO TRY */ parse_identifier(ctx, i, &tag);
                    *type_specifier = std::make_shared<Structure>(tag, true);
                    return;
                }
                default:
                    break;
            }
            break;
        }
        case 2: {
            if (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_unsigned) != type_tok_kinds.end()) {
                if (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_int) != type_tok_kinds.end()) {
                    *type_specifier = std::make_shared<UInt>();
                    return;
                }
                else if (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_long)
                         != type_tok_kinds.end()) {
                    *type_specifier = std::make_shared<ULong>();
                    return;
                }
                else if (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_char)
                         != type_tok_kinds.end()) {
                    *type_specifier = std::make_shared<UChar>();
                    return;
                }
            }
            else if (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_signed) != type_tok_kinds.end()) {
                if (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_int) != type_tok_kinds.end()) {
                    *type_specifier = std::make_shared<Int>();
                    return;
                }
                else if (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_long)
                         != type_tok_kinds.end()) {
                    *type_specifier = std::make_shared<Long>();
                    return;
                }
                else if (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_char)
                         != type_tok_kinds.end()) {
                    *type_specifier = std::make_shared<SChar>();
                    return;
                }
            }
            else if ((std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_int) != type_tok_kinds.end())
                     && (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_long)
                         != type_tok_kinds.end())) {
                *type_specifier = std::make_shared<Long>();
                return;
            }
            break;
        }
        case 3: {
            if ((std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_int) != type_tok_kinds.end())
                && (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_long) != type_tok_kinds.end())) {
                if (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_unsigned) != type_tok_kinds.end()) {
                    *type_specifier = std::make_shared<ULong>();
                    return;
                }
                else if (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_signed)
                         != type_tok_kinds.end()) {
                    *type_specifier = std::make_shared<Long>();
                    return;
                }
            }
            break;
        }
        default:
            break;
    }
    std::string type_tok_kinds_s = "(";
    for (TOKEN_KIND type_tok_kind : type_tok_kinds) {
        type_tok_kinds_s += get_tok_kind_fmt(type_tok_kind);
        type_tok_kinds_s += ", ";
    }
    if (!type_tok_kinds.empty()) {
        type_tok_kinds_s.pop_back();
        type_tok_kinds_s.pop_back();
    }
    type_tok_kinds_s += ")";
    THROW_AT_LINE_EX(GET_PARSER_MSG(MSG_expect_specifier_list, type_tok_kinds_s.c_str()), line);
}

// <specifier> ::= <type-specifier> | "static" | "extern"
// storage_class = Static | Extern
static void /* TODO TRY */ parse_storage_class(Ctx ctx, return_t(std::unique_ptr<CStorageClass>) storage_class) {
    /* TODO TRY */ pop_next(ctx);
    switch (ctx->next_tok->tok_kind) {
        case TOK_key_static: {
            *storage_class = std::make_unique<CStatic>();
            break;
        }
        case TOK_key_extern: {
            *storage_class = std::make_unique<CExtern>();
            break;
        }
        default:
            THROW_AT_LINE_EX(GET_PARSER_MSG(MSG_expect_storage_class, get_tok_fmt(ctx->identifiers, ctx->next_tok)),
                ctx->next_tok->line);
    }
}

static void /* TODO TRY */ parse_initializer(Ctx ctx, return_t(std::unique_ptr<CInitializer>) initializer);

static void /* TODO TRY */ parse_single_init(Ctx ctx, return_t(std::unique_ptr<CInitializer>) initializer) {
    std::unique_ptr<CExp> exp;
    /* TODO TRY */ parse_exp(ctx, 0, &exp);
    *initializer = std::make_unique<CSingleInit>(std::move(exp));
}

static void /* TODO TRY */ parse_compound_init(Ctx ctx, return_t(std::unique_ptr<CInitializer>) initializer) {
    /* TODO TRY */ pop_next(ctx);
    std::vector<std::unique_ptr<CInitializer>> initializers;
    while (true) {
        /* TODO TRY */ peek_next(ctx);
        if (ctx->peek_tok->tok_kind == TOK_close_brace) {
            break;
        }
        /* TODO TRY */ parse_initializer(ctx, initializer);
        initializers.push_back(std::move(*initializer));
        /* TODO TRY */ peek_next(ctx);
        if (ctx->peek_tok->tok_kind == TOK_close_brace) {
            break;
        }
        /* TODO TRY */ pop_next(ctx);
        /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_comma_separator);
    }
    if (initializers.empty()) {
        THROW_AT_LINE_EX(GET_PARSER_MSG_0(MSG_empty_compound_init), ctx->peek_tok->line);
    }
    /* TODO TRY */ pop_next(ctx);
    *initializer = std::make_unique<CCompoundInit>(std::move(initializers));
}

// <initializer> ::= <exp> | "{" <initializer> { "," <initializer> } [","] "}"
// initializer = SingleInit(exp) | CompoundInit(initializer*)
static void /* TODO TRY */ parse_initializer(Ctx ctx, return_t(std::unique_ptr<CInitializer>) initializer) {
    /* TODO TRY */ peek_next(ctx);
    if (ctx->peek_tok->tok_kind == TOK_open_brace) {
        /* TODO TRY */ parse_compound_init(ctx, initializer);
    }
    else {
        /* TODO TRY */ parse_single_init(ctx, initializer);
    }
}

static std::unique_ptr<CDeclarator> /* TODO TRY */ parse_decltor(Ctx ctx);
static void /* TODO TRY */ proc_decltor(
    Ctx ctx, CDeclarator* node, std::shared_ptr<Type>&& base_type, Declarator& decltor);

static void proc_ident_decltor(CIdent* node, std::shared_ptr<Type>&& base_type, Declarator& decltor) {
    decltor.name = node->name;
    decltor.derived_type = std::move(base_type);
}

static void /* TODO TRY */ proc_ptr_decltor(
    Ctx ctx, CPointerDeclarator* node, std::shared_ptr<Type>&& base_type, Declarator& decltor) {
    std::shared_ptr<Type> derived_type = std::make_shared<Pointer>(std::move(base_type));
    /* TODO TRY */ proc_decltor(ctx, node->decltor.get(), std::move(derived_type), decltor);
}

static void /* TODO TRY */ proc_arr_decltor(
    Ctx ctx, CArrayDeclarator* node, std::shared_ptr<Type>&& base_type, Declarator& decltor) {
    TLong size = node->size;
    std::shared_ptr<Type> derived_type = std::make_shared<Array>(size, std::move(base_type));
    /* TODO TRY */ proc_decltor(ctx, node->decltor.get(), std::move(derived_type), decltor);
}

static void /* TODO TRY */ proc_fun_decltor(
    Ctx ctx, CFunDeclarator* node, std::shared_ptr<Type>&& base_type, Declarator& decltor) {
    if (node->decltor->type() != AST_CIdent_t) {
        THROW_AT_LINE_EX(GET_PARSER_MSG_0(MSG_derived_fun_decl), ctx->next_tok->line);
    }

    std::vector<TIdentifier> params;
    std::vector<std::shared_ptr<Type>> param_types;
    params.reserve(node->param_list.size());
    param_types.reserve(node->param_list.size());
    for (const auto& param : node->param_list) {
        Declarator param_decltor;
        std::shared_ptr<Type> param_type = param->param_type;
        /* TODO TRY */ proc_decltor(ctx, param->decltor.get(), std::move(param_type), param_decltor);
        THROW_ABORT_IF(param_decltor.derived_type->type() == AST_FunType_t);
        params.push_back(param_decltor.name);
        param_types.push_back(std::move(param_decltor.derived_type));
    }
    TIdentifier name = static_cast<CIdent*>(node->decltor.get())->name;
    std::shared_ptr<Type> derived_type = std::make_shared<FunType>(std::move(param_types), std::move(base_type));
    decltor.name = name;
    decltor.derived_type = std::move(derived_type);
    decltor.params = std::move(params);
}

static void /* TODO TRY */ proc_decltor(
    Ctx ctx, CDeclarator* node, std::shared_ptr<Type>&& base_type, Declarator& decltor) {
    switch (node->type()) {
        case AST_CIdent_t:
            proc_ident_decltor(static_cast<CIdent*>(node), std::move(base_type), decltor);
            break;
        case AST_CPointerDeclarator_t:
            /* TODO TRY */ proc_ptr_decltor(ctx, static_cast<CPointerDeclarator*>(node), std::move(base_type), decltor);
            break;
        case AST_CArrayDeclarator_t:
            /* TODO TRY */ proc_arr_decltor(ctx, static_cast<CArrayDeclarator*>(node), std::move(base_type), decltor);
            break;
        case AST_CFunDeclarator_t:
            /* TODO TRY */ proc_fun_decltor(ctx, static_cast<CFunDeclarator*>(node), std::move(base_type), decltor);
            break;
        default:
            THROW_ABORT;
    }
}

static void /* TODO TRY */ parse_ident_decltor(Ctx ctx, return_t(std::unique_ptr<CDeclarator>) decltor) {
    TIdentifier name;
    /* TODO TRY */ parse_identifier(ctx, 0, &name);
    *decltor = std::make_unique<CIdent>(name);
}

static void /* TODO TRY */ parse_simple_decltor(Ctx ctx, return_t(std::unique_ptr<CDeclarator>) decltor) {
    /* TODO TRY */ pop_next(ctx);
    *decltor = /* TODO TRY */ parse_decltor(ctx);
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_close_paren);
}

// <simple-declarator> ::= <identifier> | "(" <declarator> ")"
static void /* TODO TRY */ parse_simple_decltor_decl(Ctx ctx, return_t(std::unique_ptr<CDeclarator>) decltor) {
    /* TODO TRY */ peek_next(ctx);
    switch (ctx->peek_tok->tok_kind) {
        case TOK_identifier:
            /* TODO TRY */ parse_ident_decltor(ctx, decltor);
            break;
        case TOK_open_paren:
            /* TODO TRY */ parse_simple_decltor(ctx, decltor);
            break;
        default:
            THROW_AT_LINE_EX(GET_PARSER_MSG(MSG_expect_simple_decltor, get_tok_fmt(ctx->identifiers, ctx->peek_tok)),
                ctx->peek_tok->line);
    }
}

// <param> ::= { <type-specifier> }+ <declarator>
// param_info = Param(type, declarator)
static void /* TODO TRY */ parse_param(Ctx ctx, return_t(std::unique_ptr<CParam>) param) {
    std::shared_ptr<Type> param_type;
    /* TODO TRY */ parse_type_specifier(ctx, &param_type);
    std::unique_ptr<CDeclarator> decltor = /* TODO TRY */ parse_decltor(ctx);
    *param = std::make_unique<CParam>(std::move(decltor), std::move(param_type));
}

static void /* TODO TRY */ parse_empty_param_list(Ctx ctx) { /* TODO TRY */
    pop_next(ctx);
}

static void /* TODO TRY */ parse_non_empty_param_list(
    Ctx ctx, return_t(std::vector<std::unique_ptr<CParam>>) param_list) {
    {
        std::unique_ptr<CParam> param;
        /* TODO TRY */ parse_param(ctx, &param);
        param_list->push_back(std::move(param));
    }
    /* TODO TRY */ peek_next(ctx);
    while (ctx->peek_tok->tok_kind == TOK_comma_separator) {
        /* TODO TRY */ pop_next(ctx);
        std::unique_ptr<CParam> param;
        /* TODO TRY */ parse_param(ctx, &param);
        param_list->push_back(std::move(param));
        /* TODO TRY */ peek_next(ctx);
    }
}

// <param-list> ::= "(" "void" ")" | "(" <param> { "," <param> } ")"
static void /* TODO TRY */ parse_param_list(Ctx ctx, return_t(std::vector<std::unique_ptr<CParam>>) param_list) {
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ peek_next(ctx);
    switch (ctx->peek_tok->tok_kind) {
        case TOK_key_void: {
            /* TODO TRY */ peek_next_i(ctx, 1);
            if (ctx->peek_tok_i->tok_kind == TOK_close_paren) {
                /* TODO TRY */ parse_empty_param_list(ctx);
            }
            else {
                /* TODO TRY */ parse_non_empty_param_list(ctx, param_list);
            }
            break;
        }
        case TOK_key_char:
        case TOK_key_int:
        case TOK_key_long:
        case TOK_key_double:
        case TOK_key_unsigned:
        case TOK_key_signed:
        case TOK_key_struct:
        case TOK_key_union:
            /* TODO TRY */ parse_non_empty_param_list(ctx, param_list);
            break;
        default:
            THROW_AT_LINE_EX(GET_PARSER_MSG(MSG_expect_param_list, get_tok_fmt(ctx->identifiers, ctx->peek_tok)),
                ctx->peek_tok->line);
    }
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_close_paren);
}

// (fun) <declarator-suffix> ::= <param-list>
static void /* TODO TRY */ parse_fun_decltor_suffix(
    Ctx ctx, std::unique_ptr<CDeclarator>&& decltor, return_t(std::unique_ptr<CDeclarator>) direct_decltor) {
    std::vector<std::unique_ptr<CParam>> param_list;
    /* TODO TRY */ parse_param_list(ctx, &param_list);
    *direct_decltor = std::make_unique<CFunDeclarator>(std::move(param_list), std::move(decltor));
}

// (array) <declarator-suffix> ::= { "[" <const> "]" }+
static std::unique_ptr<CDeclarator> /* TODO TRY */ parse_arr_decltor_suffix(
    Ctx ctx, std::unique_ptr<CDeclarator>&& decltor) {
    do {
        TLong size;
        /* TODO TRY */ parse_arr_size(ctx, &size);
        decltor = std::make_unique<CArrayDeclarator>(size, std::move(decltor));
        /* TODO TRY */ peek_next(ctx);
    }
    while (ctx->peek_tok->tok_kind == TOK_open_bracket);
    return std::move(decltor);
}

// <direct-declarator> ::= <simple-declarator> [ <declarator-suffix> ]
static std::unique_ptr<CDeclarator> /* TODO TRY */ parse_direct_decltor(Ctx ctx) {
    /* TODO return_t */ std::unique_ptr<CDeclarator> direct_decltor;
    std::unique_ptr<CDeclarator> decltor;
    /* TODO TRY */ parse_simple_decltor_decl(ctx, &decltor);
    /* TODO TRY */ peek_next(ctx);
    switch (ctx->peek_tok->tok_kind) {
        case TOK_open_paren:
            /* TODO TRY */ parse_fun_decltor_suffix(ctx, std::move(decltor), &direct_decltor);
            return direct_decltor;
        case TOK_open_bracket:
            return /* TODO TRY */ parse_arr_decltor_suffix(ctx, std::move(decltor));
        default:
            return decltor;
    }
}

static std::unique_ptr<CPointerDeclarator> /* TODO TRY */ parse_ptr_decltor(Ctx ctx) {
    /* TODO TRY */ pop_next(ctx);
    std::unique_ptr<CDeclarator> decltor = /* TODO TRY */ parse_decltor(ctx);
    return std::make_unique<CPointerDeclarator>(std::move(decltor));
}

// <declarator> ::= "*" <declarator> | <direct-declarator>
// declarator = Ident(identifier) | PointerDeclarator(declarator) | ArrayDeclarator(int, declarator)
//            | FunDeclarator(param_info*, declarator)
static std::unique_ptr<CDeclarator> /* TODO TRY */ parse_decltor(Ctx ctx) {
    /* TODO TRY */ peek_next(ctx);
    if (ctx->peek_tok->tok_kind == TOK_binop_multiply) {
        return /* TODO TRY */ parse_ptr_decltor(ctx);
    }
    else {
        return /* TODO TRY */ parse_direct_decltor(ctx);
    }
}

// <function-declaration> ::= { <specifier> }+ <declarator> ( <block> | ";")
// function_declaration = FunctionDeclaration(identifier, identifier*, block?, type, storage_class?)
static std::unique_ptr<CFunctionDeclaration> /* TODO TRY */ parse_fun_declaration(
    Ctx ctx, std::unique_ptr<CStorageClass>&& storage_class, Declarator&& decltor) {
    size_t line = ctx->next_tok->line;
    std::unique_ptr<CBlock> body;
    /* TODO TRY */ peek_next(ctx);
    if (ctx->peek_tok->tok_kind == TOK_semicolon) {
        /* TODO TRY */ pop_next(ctx);
    }
    else {
        /* TODO TRY */ expect_next(ctx, ctx->peek_tok, TOK_open_brace);
        /* TODO TRY */ parse_block(ctx, &body);
    }
    return std::make_unique<CFunctionDeclaration>(decltor.name, std::move(decltor.params), std::move(body),
        std::move(decltor.derived_type), std::move(storage_class), line);
}

// <variable-declaration> ::= { <specifier> }+ <declarator> [ "=" <initializer> ] ";"
// variable_declaration = VariableDeclaration(identifier, initializer?, type, storage_class?)
static std::unique_ptr<CVariableDeclaration> /* TODO TRY */ parse_var_declaration(
    Ctx ctx, std::unique_ptr<CStorageClass>&& storage_class, Declarator&& decltor) {
    size_t line = ctx->next_tok->line;
    std::unique_ptr<CInitializer> initializer;
    /* TODO TRY */ peek_next(ctx);
    if (ctx->peek_tok->tok_kind == TOK_assign) {
        /* TODO TRY */ pop_next(ctx);
        /* TODO TRY */ parse_initializer(ctx, &initializer);
    }
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_semicolon);
    return std::make_unique<CVariableDeclaration>(
        decltor.name, std::move(initializer), std::move(decltor.derived_type), std::move(storage_class), line);
}

// <member-declaration> ::= { <type-specifier> }+ <declarator> ";"
// member_declaration = MemberDeclaration(identifier, type)
static std::unique_ptr<CMemberDeclaration> /* TODO TRY */ parse_member_decl(Ctx ctx) {
    Declarator decltor;
    {
        std::unique_ptr<CStorageClass> storage_class = /* TODO TRY */ parse_decltor_decl(ctx, decltor);
        if (storage_class) {
            THROW_AT_LINE_EX(
                GET_PARSER_MSG(MSG_member_decl_not_auto, ctx->identifiers->hash_table[decltor.name].c_str(),
                    get_storage_class_fmt(storage_class.get())),
                ctx->next_tok->line);
        }
    }
    if (decltor.derived_type->type() == AST_FunType_t) {
        THROW_AT_LINE_EX(GET_PARSER_MSG(MSG_member_decl_as_fun, ctx->identifiers->hash_table[decltor.name].c_str()),
            ctx->next_tok->line);
    }
    size_t line = ctx->next_tok->line;
    /* TODO TRY */ pop_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_semicolon);
    return std::make_unique<CMemberDeclaration>(decltor.name, std::move(decltor.derived_type), line);
}

// <struct-declaration> ::= ("struct" | "union") <identifier> [ "{" { <member-declaration> }+ "}" ] ";"
// struct_declaration = StructDeclaration(identifier, bool, member_declaration*)
static std::unique_ptr<CStructDeclaration> /* TODO TRY */ parse_struct_decl(Ctx ctx) {
    size_t line = ctx->peek_tok->line;
    /* TODO TRY */ pop_next(ctx);
    bool is_union = ctx->next_tok->tok_kind == TOK_key_union;
    /* TODO TRY */ peek_next(ctx);
    /* TODO TRY */ expect_next(ctx, ctx->peek_tok, TOK_identifier);
    TIdentifier tag;
    /* TODO TRY */ parse_identifier(ctx, 0, &tag);
    std::vector<std::unique_ptr<CMemberDeclaration>> members;
    /* TODO TRY */ pop_next(ctx);
    if (ctx->next_tok->tok_kind == TOK_open_brace) {
        do {
            std::unique_ptr<CMemberDeclaration> member = /* TODO TRY */ parse_member_decl(ctx);
            members.push_back(std::move(member));
            /* TODO TRY */ peek_next(ctx);
        }
        while (ctx->peek_tok->tok_kind != TOK_close_brace);
        /* TODO TRY */ pop_next(ctx);
        /* TODO TRY */ pop_next(ctx);
    }
    /* TODO TRY */ expect_next(ctx, ctx->next_tok, TOK_semicolon);
    return std::make_unique<CStructDeclaration>(tag, is_union, std::move(members), line);
}

static std::unique_ptr<CFunDecl> /* TODO TRY */ parse_fun_decl(
    Ctx ctx, std::unique_ptr<CStorageClass>&& storage_class, Declarator&& decltor) {
    std::unique_ptr<CFunctionDeclaration> fun_decl =
        /* TODO TRY */ parse_fun_declaration(ctx, std::move(storage_class), std::move(decltor));
    return std::make_unique<CFunDecl>(std::move(fun_decl));
}

static std::unique_ptr<CVarDecl> /* TODO TRY */ parse_var_decl(
    Ctx ctx, std::unique_ptr<CStorageClass>&& storage_class, Declarator&& decltor) {
    std::unique_ptr<CVariableDeclaration> var_decl =
        /* TODO TRY */ parse_var_declaration(ctx, std::move(storage_class), std::move(decltor));
    return std::make_unique<CVarDecl>(std::move(var_decl));
}

static std::unique_ptr<CStructDecl> /* TODO TRY */ parse_struct_declaration(Ctx ctx) {
    std::unique_ptr<CStructDeclaration> struct_decl = /* TODO TRY */ parse_struct_decl(ctx);
    return std::make_unique<CStructDecl>(std::move(struct_decl));
}

static std::unique_ptr<CStorageClass> /* TODO TRY */ parse_decltor_decl(Ctx ctx, Declarator& decltor) {
    std::shared_ptr<Type> type_specifier;
    /* TODO TRY */ parse_type_specifier(ctx, &type_specifier);
    std::unique_ptr<CStorageClass> storage_class;
    /* TODO TRY */ peek_next(ctx);
    switch (ctx->peek_tok->tok_kind) {
        case TOK_identifier:
        case TOK_binop_multiply:
        case TOK_open_paren:
            break;
        default:
            /* TODO TRY */ parse_storage_class(ctx, &storage_class);
            break;
    }
    std::unique_ptr<CDeclarator> decltor_1 = /* TODO TRY */ parse_decltor(ctx);
    /* TODO TRY */ proc_decltor(ctx, decltor_1.get(), std::move(type_specifier), decltor);
    return storage_class;
}

// <declaration> ::= <variable-declaration> | <function-declaration> | <struct-declaration>
// declaration = FunDecl(function_declaration) | VarDecl(variable_declaration) | StructDecl(struct_declaration)
static std::unique_ptr<CDeclaration> /* TODO TRY */ parse_declaration(Ctx ctx) {
    /* TODO TRY */ peek_next(ctx);
    switch (ctx->peek_tok->tok_kind) {
        case TOK_key_struct:
        case TOK_key_union: {
            /* TODO TRY */ peek_next_i(ctx, 2);
            switch (ctx->peek_tok_i->tok_kind) {
                case TOK_open_brace:
                case TOK_semicolon:
                    return /* TODO TRY */ parse_struct_declaration(ctx);
                default:
                    break;
            }
        }
        default:
            break;
    }
    Declarator decltor;
    std::unique_ptr<CStorageClass> storage_class = /* TODO TRY */ parse_decltor_decl(ctx, decltor);
    if (decltor.derived_type->type() == AST_FunType_t) {
        return /* TODO TRY */ parse_fun_decl(ctx, std::move(storage_class), std::move(decltor));
    }
    else {
        return /* TODO TRY */ parse_var_decl(ctx, std::move(storage_class), std::move(decltor));
    }
}

// <program> ::= { <declaration> }
// AST = Program(declaration*)
static std::unique_ptr<CProgram> /* TODO TRY */ parse_program(Ctx ctx) {
    std::vector<std::unique_ptr<CDeclaration>> declarations;
    while (ctx->pop_idx < ctx->p_toks->size()) {
        std::unique_ptr<CDeclaration> declaration = /* TODO TRY */ parse_declaration(ctx);
        declarations.push_back(std::move(declaration));
    }
    return std::make_unique<CProgram>(std::move(declarations));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<CProgram> /* TODO TRY */ parse_tokens(
    std::vector<Token>&& tokens, ErrorsContext* errors, IdentifierContext* identifiers) {
    ParserContext ctx;
    {
        ctx.errors = errors;
        ctx.identifiers = identifiers;
        ctx.pop_idx = 0;
        ctx.p_toks = &tokens;
    }
    std::unique_ptr<CProgram> c_ast = /* TODO TRY */ parse_program(&ctx);
    THROW_ABORT_IF(ctx.pop_idx != tokens.size());

    std::vector<Token>().swap(tokens);
    THROW_ABORT_IF(!c_ast);
    return c_ast;
}
