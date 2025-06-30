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

// TODO rm
// TR_Y should be 372
#define FINALLY_EXIT \
    EARLY_EXIT;      \
    FINALLY;
#define TEMP_RETURN return 0

typedef ParserContext* Ctx;

static error_t expect_next(Ctx ctx, Token* next_tok, TOKEN_KIND expect_tok) {
    CATCH_ENTER;
    if (next_tok->tok_kind != expect_tok) {
        THROW_AT_LINE_EX(GET_PARSER_MSG(MSG_unexpected_next_tok, get_tok_fmt(ctx->identifiers, next_tok),
                             get_tok_kind_fmt(expect_tok)),
            next_tok->line);
    }
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t pop_next(Ctx ctx) {
    CATCH_ENTER;
    if (ctx->pop_idx >= ctx->p_toks->size()) {
        THROW_AT_LINE_EX(GET_PARSER_MSG_0(MSG_reached_eof), ctx->p_toks->back().line);
    }

    ctx->next_tok = &(*ctx->p_toks)[ctx->pop_idx];
    ctx->pop_idx++;
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t pop_next_i(Ctx ctx, size_t i) {
    CATCH_ENTER;
    if (i == 0) {
        TRY(pop_next(ctx));
        ctx->next_tok_i = ctx->next_tok;
        TEMP_RETURN;
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
    TRY(pop_next(ctx));
    ctx->next_tok_i = &(*ctx->p_toks)[ctx->pop_idx - 1];
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t peek_next(Ctx ctx) {
    CATCH_ENTER;
    if (ctx->pop_idx >= ctx->p_toks->size()) {
        THROW_AT_LINE_EX(GET_PARSER_MSG_0(MSG_reached_eof), ctx->p_toks->back().line);
    }

    ctx->peek_tok = &(*ctx->p_toks)[ctx->pop_idx];
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t peek_next_i(Ctx ctx, size_t i) {
    CATCH_ENTER;
    if (i == 0) {
        TRY(peek_next(ctx));
        ctx->peek_tok_i = ctx->peek_tok;
        TEMP_RETURN;
    }
    if (ctx->pop_idx + i >= ctx->p_toks->size()) {
        THROW_AT_LINE_EX(GET_PARSER_MSG_0(MSG_reached_eof), ctx->p_toks->back().line);
    }

    ctx->peek_tok_i = &(*ctx->p_toks)[ctx->pop_idx + i];
    FINALLY_EXIT;
    CATCH_EXIT;
}

// <identifier> ::= ? An identifier token ?
static error_t parse_identifier(Ctx ctx, size_t i, return_t(TIdentifier) identifier) {
    CATCH_ENTER;
    TRY(pop_next_i(ctx, i));
    *identifier = ctx->next_tok_i->tok;
    FINALLY_EXIT;
    CATCH_EXIT;
}

// string = StringLiteral(int*)
// <string> ::= ? A string token ?
static error_t parse_string_literal(Ctx ctx, return_t(std::shared_ptr<CStringLiteral>) literal) {
    std::vector<TChar> value;
    CATCH_ENTER;
    string_to_literal(ctx->identifiers->hash_table[ctx->next_tok->tok], value);
    TRY(peek_next(ctx));
    while (ctx->peek_tok->tok_kind == TOK_string_literal) {
        TRY(pop_next(ctx));
        string_to_literal(ctx->identifiers->hash_table[ctx->next_tok->tok], value);
        TRY(peek_next(ctx));
    }
    *literal = std::make_shared<CStringLiteral>(std::move(value));
    FINALLY_EXIT;
    CATCH_EXIT;
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
static error_t parse_dbl_const(Ctx ctx, return_t(std::shared_ptr<CConst>) constant) {
    CATCH_ENTER;
    TDouble value;
    TRY(string_to_dbl(
        ctx->errors, ctx->identifiers->hash_table[ctx->next_tok->tok].c_str(), ctx->next_tok->line, &value));
    *constant = std::make_shared<CConstDouble>(value);
    FINALLY_EXIT;
    CATCH_EXIT;
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
static error_t parse_const(Ctx ctx, return_t(std::shared_ptr<CConst>) constant) {
    CATCH_ENTER;
    TRY(pop_next(ctx));
    switch (ctx->next_tok->tok_kind) {
        case TOK_char_const: {
            *constant = parse_char_const(ctx);
            TEMP_RETURN;
        }
        case TOK_dbl_const:
            TRY(parse_dbl_const(ctx, constant));
            TEMP_RETURN;
        default:
            break;
    }

    intmax_t value;
    TRY(string_to_intmax(
        ctx->errors, ctx->identifiers->hash_table[ctx->next_tok->tok].c_str(), ctx->next_tok->line, &value));
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
    FINALLY_EXIT;
    CATCH_EXIT;
}

// <const> ::= <unsigned int> | <unsigned long>
// (unsigned) const = ConstUInt(uint) | ConstULong(ulong) | ConstUChar(int)
static error_t parse_unsigned_const(Ctx ctx, return_t(std::shared_ptr<CConst>) constant) {
    CATCH_ENTER;
    TRY(pop_next(ctx));

    uintmax_t value;
    TRY(string_to_uintmax(
        ctx->errors, ctx->identifiers->hash_table[ctx->next_tok->tok].c_str(), ctx->next_tok->line, &value));
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
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_arr_size(Ctx ctx, return_t(TLong) size) {
    std::shared_ptr<CConst> constant;
    CATCH_ENTER;
    TRY(pop_next(ctx));
    TRY(peek_next(ctx));
    switch (ctx->peek_tok->tok_kind) {
        case TOK_int_const:
        case TOK_long_const:
        case TOK_char_const:
            TRY(parse_const(ctx, &constant));
            break;
        case TOK_uint_const:
        case TOK_ulong_const:
            TRY(parse_unsigned_const(ctx, &constant));
            break;
        default:
            THROW_AT_LINE_EX(GET_PARSER_MSG(MSG_arr_size_not_int_const, get_tok_fmt(ctx->identifiers, ctx->peek_tok)),
                ctx->peek_tok->line);
    }
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_close_bracket));
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
    FINALLY_EXIT;
    CATCH_EXIT;
}

// <unop> ::= "-" | "~" | "!" | "*" | "&" | "++" | "--"
// unary_operator = Complement | Negate | Not | Prefix | Postfix
static error_t parse_unop(Ctx ctx, return_t(std::unique_ptr<CUnaryOp>) unop) {
    CATCH_ENTER;
    TRY(pop_next(ctx));
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
    FINALLY_EXIT;
    CATCH_EXIT;
}

// <binop> ::= "-" | "+" | "*" | "/" | "%" | "&" | "|" | "^" | "<<" | ">>" | "&&" | "||" | "==" | "!=" | "<"
//           | "<=" | ">" | ">=" | "=" | "-=" | "+=" | "*=" | "/=" | "%=" | "&=" | "|=" | "^=" | "<<=" | ">>="
// binary_operator = Add | Subtract | Multiply | Divide | Remainder | BitAnd | BitOr | BitXor | BitShiftLeft
//                 | BitShiftRight | BitShrArithmetic | And | Or | Equal | NotEqual | LessThan | LessOrEqual
//                 | GreaterThan | GreaterOrEqual
static error_t parse_binop(Ctx ctx, return_t(std::unique_ptr<CBinaryOp>) binop) {
    CATCH_ENTER;
    TRY(pop_next(ctx));
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
    FINALLY_EXIT;
    CATCH_EXIT;
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

static error_t parse_abstract_decltor(Ctx ctx, return_t(std::unique_ptr<CAbstractDeclarator>) abstract_decltor);

// (array) <direct-abstract-declarator> ::= { "[" <const> "]" }+
static error_t parse_arr_abstract_decltor(Ctx ctx, return_t(std::unique_ptr<CAbstractDeclarator>) abstract_decltor) {
    CATCH_ENTER;
    *abstract_decltor = std::make_unique<CAbstractBase>();
    do {
        TLong size;
        TRY(parse_arr_size(ctx, &size));
        *abstract_decltor = std::make_unique<CAbstractArray>(size, std::move(*abstract_decltor));
        TRY(peek_next(ctx));
    }
    while (ctx->peek_tok->tok_kind == TOK_open_bracket);
    FINALLY_EXIT;
    CATCH_EXIT;
}

// (direct) <direct-abstract-declarator> ::= "(" <abstract-declarator> ")" { "[" <const> "]" }
static error_t parse_direct_abstract_decltor(Ctx ctx, return_t(std::unique_ptr<CAbstractDeclarator>) abstract_decltor) {
    CATCH_ENTER;
    TRY(pop_next(ctx));
    TRY(parse_abstract_decltor(ctx, abstract_decltor));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_close_paren));
    TRY(peek_next(ctx));
    while (ctx->peek_tok->tok_kind == TOK_open_bracket) {
        TLong size;
        TRY(parse_arr_size(ctx, &size));
        *abstract_decltor = std::make_unique<CAbstractArray>(size, std::move(*abstract_decltor));
        TRY(peek_next(ctx));
    }
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_ptr_abstract_decltor(Ctx ctx, return_t(std::unique_ptr<CAbstractDeclarator>) abstract_decltor) {
    CATCH_ENTER;
    TRY(pop_next(ctx));
    TRY(peek_next(ctx));
    if (ctx->peek_tok->tok_kind == TOK_close_paren) {
        *abstract_decltor = std::make_unique<CAbstractBase>();
    }
    else {
        TRY(parse_abstract_decltor(ctx, abstract_decltor));
    }
    *abstract_decltor = std::make_unique<CAbstractPointer>(std::move(*abstract_decltor));
    FINALLY_EXIT;
    CATCH_EXIT;
}

// <abstract-declarator> ::= "*" [ <abstract-declarator> ] | <direct-abstract-declarator>
// abstract_declarator = AbstractPointer(abstract_declarator) | AbstractArray(int, abstract_declarator) | AbstractBase
static error_t parse_abstract_decltor(Ctx ctx, return_t(std::unique_ptr<CAbstractDeclarator>) abstract_decltor) {
    CATCH_ENTER;
    TRY(peek_next(ctx));
    switch (ctx->peek_tok->tok_kind) {
        case TOK_binop_multiply:
            TRY(parse_ptr_abstract_decltor(ctx, abstract_decltor));
            break;
        case TOK_open_paren:
            TRY(parse_direct_abstract_decltor(ctx, abstract_decltor));
            break;
        case TOK_open_bracket:
            TRY(parse_arr_abstract_decltor(ctx, abstract_decltor));
            break;
        default:
            THROW_AT_LINE_EX(GET_PARSER_MSG(MSG_expect_abstract_decltor, get_tok_fmt(ctx->identifiers, ctx->peek_tok)),
                ctx->peek_tok->line);
    }
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_type_specifier(Ctx ctx, return_t(std::shared_ptr<Type>) type_specifier);

static error_t parse_unary_exp_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp);
static error_t parse_cast_exp_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp);
static error_t parse_exp(Ctx ctx, int32_t min_precedence, return_t(std::unique_ptr<CExp>) exp);

static error_t parse_decltor_cast_factor(Ctx ctx, return_t(std::shared_ptr<Type>) target_type) {
    AbstractDeclarator abstract_decltor;
    std::unique_ptr<CAbstractDeclarator> abstract_decltor_1;
    CATCH_ENTER;
    TRY(parse_abstract_decltor(ctx, &abstract_decltor_1));
    proc_abstract_decltor(abstract_decltor_1.get(), std::move(*target_type), abstract_decltor);
    *target_type = std::move(abstract_decltor.derived_type);
    FINALLY_EXIT;
    CATCH_EXIT;
}

// <type-name> ::= { <type-specifier> }+ [ <abstract-declarator> ]
static error_t parse_type_name(Ctx ctx, return_t(std::shared_ptr<Type>) target_type) {
    CATCH_ENTER;
    TRY(parse_type_specifier(ctx, target_type));
    TRY(peek_next(ctx));
    switch (ctx->peek_tok->tok_kind) {
        case TOK_binop_multiply:
        case TOK_open_paren:
        case TOK_open_bracket:
            TRY(parse_decltor_cast_factor(ctx, target_type));
            break;
        default:
            break;
    }
    FINALLY_EXIT;
    CATCH_EXIT;
}

// <argument-list> ::= <exp> { "," <exp> }
static error_t parse_arg_list(Ctx ctx, return_t(std::vector<std::unique_ptr<CExp>>) args) {
    std::unique_ptr<CExp> arg;
    CATCH_ENTER;
    TRY(parse_exp(ctx, 0, &arg));
    args->push_back(std::move(arg));
    TRY(peek_next(ctx));
    while (ctx->peek_tok->tok_kind == TOK_comma_separator) {
        TRY(pop_next(ctx));
        TRY(parse_exp(ctx, 0, &arg));
        args->push_back(std::move(arg));
        TRY(peek_next(ctx));
    }
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_const_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    std::shared_ptr<CConst> constant;
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(parse_const(ctx, &constant));
    *exp = std::make_unique<CConstant>(std::move(constant), line);
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_unsigned_const_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    std::shared_ptr<CConst> constant;
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(parse_unsigned_const(ctx, &constant));
    *exp = std::make_unique<CConstant>(std::move(constant), line);
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_string_literal_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    std::shared_ptr<CStringLiteral> literal;
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(pop_next(ctx));
    TRY(parse_string_literal(ctx, &literal));
    *exp = std::make_unique<CString>(std::move(literal), line);
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_var_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TIdentifier name;
    TRY(parse_identifier(ctx, 0, &name));
    *exp = std::make_unique<CVar>(name, line);
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_call_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    std::vector<std::unique_ptr<CExp>> args;
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TIdentifier name;
    TRY(parse_identifier(ctx, 0, &name));
    TRY(pop_next(ctx));
    TRY(peek_next(ctx));
    if (ctx->peek_tok->tok_kind != TOK_close_paren) {
        TRY(parse_arg_list(ctx, &args));
    }
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_close_paren));
    *exp = std::make_unique<CFunctionCall>(name, std::move(args), line);
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_inner_exp_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    CATCH_ENTER;
    TRY(pop_next(ctx));
    TRY(parse_exp(ctx, 0, exp));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_close_paren));
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_subscript_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    std::unique_ptr<CExp> subscript_exp;
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(pop_next(ctx));
    TRY(parse_exp(ctx, 0, &subscript_exp));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_close_bracket));
    *exp = std::make_unique<CSubscript>(std::move(*exp), std::move(subscript_exp), line);
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_dot_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(pop_next(ctx));
    TRY(peek_next(ctx));
    TRY(expect_next(ctx, ctx->peek_tok, TOK_identifier));
    TIdentifier member;
    TRY(parse_identifier(ctx, 0, &member));
    *exp = std::make_unique<CDot>(member, std::move(*exp), line);
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_arrow_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(pop_next(ctx));
    TRY(peek_next(ctx));
    TRY(expect_next(ctx, ctx->peek_tok, TOK_identifier));
    TIdentifier member;
    TRY(parse_identifier(ctx, 0, &member));
    *exp = std::make_unique<CArrow>(member, std::move(*exp), line);
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_postfix_incr_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    std::unique_ptr<CExp> exp_right;
    std::unique_ptr<CExp> exp_right_1;
    std::unique_ptr<CUnaryOp> unop;
    std::unique_ptr<CBinaryOp> binop;
    std::shared_ptr<CConst> constant;
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    unop = std::make_unique<CPostfix>();
    TRY(parse_binop(ctx, &binop));
    constant = std::make_shared<CConstInt>(1);
    exp_right = std::make_unique<CConstant>(std::move(constant), line);
    exp_right_1 = std::make_unique<CBinary>(std::move(binop), std::move(*exp), std::move(exp_right), line);
    *exp = std::make_unique<CAssignment>(std::move(unop), nullptr, std::move(exp_right_1), line);
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_unary_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    std::unique_ptr<CExp> cast_exp;
    std::unique_ptr<CUnaryOp> unop;
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(parse_unop(ctx, &unop));
    TRY(parse_cast_exp_factor(ctx, &cast_exp));
    *exp = std::make_unique<CUnary>(std::move(unop), std::move(cast_exp), line);
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_incr_unary_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    std::unique_ptr<CExp> exp_left;
    std::unique_ptr<CExp> exp_right;
    std::unique_ptr<CExp> exp_left_1;
    std::unique_ptr<CExp> exp_right_1;
    std::unique_ptr<CUnaryOp> unop;
    std::unique_ptr<CBinaryOp> binop;
    std::shared_ptr<CConst> constant;
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    unop = std::make_unique<CPrefix>();
    TRY(parse_binop(ctx, &binop));
    TRY(parse_cast_exp_factor(ctx, &exp_left));
    constant = std::make_shared<CConstInt>(1);
    exp_right = std::make_unique<CConstant>(std::move(constant), line);
    exp_right_1 = std::make_unique<CBinary>(std::move(binop), std::move(exp_left), std::move(exp_right), line);
    *exp = std::make_unique<CAssignment>(std::move(unop), std::move(exp_left_1), std::move(exp_right_1), line);
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_deref_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    std::unique_ptr<CExp> cast_exp;
    CATCH_ENTER;
    size_t line = ctx->next_tok->line;
    TRY(parse_cast_exp_factor(ctx, &cast_exp));
    *exp = std::make_unique<CDereference>(std::move(cast_exp), line);
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_addrof_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    std::unique_ptr<CExp> cast_exp;
    CATCH_ENTER;
    size_t line = ctx->next_tok->line;
    TRY(parse_cast_exp_factor(ctx, &cast_exp));
    *exp = std::make_unique<CAddrOf>(std::move(cast_exp), line);
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_ptr_unary_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    CATCH_ENTER;
    TRY(pop_next(ctx));
    switch (ctx->next_tok->tok_kind) {
        case TOK_binop_multiply:
            TRY(parse_deref_factor(ctx, exp));
            break;
        case TOK_binop_bitand:
            TRY(parse_addrof_factor(ctx, exp));
            break;
        default:
            THROW_AT_LINE_EX(GET_PARSER_MSG(MSG_expect_ptr_unary_factor, get_tok_fmt(ctx->identifiers, ctx->next_tok)),
                ctx->next_tok->line);
    }
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_sizeoft_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    std::shared_ptr<Type> target_type;
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(pop_next(ctx));
    TRY(parse_type_name(ctx, &target_type));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_close_paren));
    *exp = std::make_unique<CSizeOfT>(std::move(target_type), line);
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_sizeof_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    std::unique_ptr<CExp> unary_exp;
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(parse_unary_exp_factor(ctx, &unary_exp));
    *exp = std::make_unique<CSizeOf>(std::move(unary_exp), line);
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_sizeof_unary_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    CATCH_ENTER;
    TRY(pop_next(ctx));
    TRY(peek_next(ctx));
    if (ctx->peek_tok->tok_kind == TOK_open_paren) {
        TRY(peek_next_i(ctx, 1));
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
                TRY(parse_sizeoft_factor(ctx, exp));
                TEMP_RETURN;
            default:
                break;
        }
    }
    TRY(parse_sizeof_factor(ctx, exp));
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_cast_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    std::unique_ptr<CExp> cast_exp;
    std::shared_ptr<Type> target_type;
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(pop_next(ctx));
    TRY(parse_type_name(ctx, &target_type));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_close_paren));
    TRY(parse_cast_exp_factor(ctx, &cast_exp));
    *exp = std::make_unique<CCast>(std::move(cast_exp), std::move(target_type), line);
    FINALLY_EXIT;
    CATCH_EXIT;
}

// <primary-exp> ::= <const> | <identifier> | "(" <exp> ")" | { <string> }+ | <identifier> "(" [ <argument-list> ] ")"
static error_t parse_primary_exp_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    CATCH_ENTER;
    TRY(peek_next(ctx));
    switch (ctx->peek_tok->tok_kind) {
        case TOK_int_const:
        case TOK_long_const:
        case TOK_char_const:
        case TOK_dbl_const:
            TRY(parse_const_factor(ctx, exp));
            break;
        case TOK_uint_const:
        case TOK_ulong_const:
            TRY(parse_unsigned_const_factor(ctx, exp));
            break;
        case TOK_identifier: {
            TRY(peek_next_i(ctx, 1));
            if (ctx->peek_tok_i->tok_kind == TOK_open_paren) {
                TRY(parse_call_factor(ctx, exp));
            }
            else {
                TRY(parse_var_factor(ctx, exp));
            }
            break;
        }
        case TOK_string_literal:
            TRY(parse_string_literal_factor(ctx, exp));
            break;
        case TOK_open_paren:
            TRY(parse_inner_exp_factor(ctx, exp));
            break;
        default:
            THROW_AT_LINE_EX(
                GET_PARSER_MSG(MSG_expect_primary_exp_factor, get_tok_fmt(ctx->identifiers, ctx->peek_tok)),
                ctx->peek_tok->line);
    }
    FINALLY_EXIT;
    CATCH_EXIT;
}

// <postfix-op> ::= "[" <exp> "]" | "." <identifier> | "->" <identifier>
static error_t parse_postfix_op_exp_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    CATCH_ENTER;
    TRY(peek_next(ctx));
    switch (ctx->peek_tok->tok_kind) {
        case TOK_open_bracket:
            TRY(parse_subscript_factor(ctx, exp));
            break;
        case TOK_structop_member:
            TRY(parse_dot_factor(ctx, exp));
            break;
        case TOK_structop_ptr:
            TRY(parse_arrow_factor(ctx, exp));
            break;
        case TOK_unop_incr:
        case TOK_unop_decr:
            TRY(parse_postfix_incr_factor(ctx, exp));
            break;
        default:
            TEMP_RETURN;
    }
    TRY(parse_postfix_op_exp_factor(ctx, exp));
    FINALLY_EXIT;
    CATCH_EXIT;
}

// <postfix-exp> ::= <primary-exp> { <postfix-op> }
static error_t parse_postfix_exp_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    CATCH_ENTER;
    TRY(parse_primary_exp_factor(ctx, exp));
    TRY(peek_next(ctx));
    switch (ctx->peek_tok->tok_kind) {
        case TOK_open_bracket:
        case TOK_structop_member:
        case TOK_structop_ptr:
        case TOK_unop_incr:
        case TOK_unop_decr:
            TRY(parse_postfix_op_exp_factor(ctx, exp));
            break;
        default:
            break;
    }
    FINALLY_EXIT;
    CATCH_EXIT;
}

//<unary-exp> ::= <unop> <cast-exp> | "sizeof" <unary-exp> | "sizeof" "(" <type-name> ")" | <postfix-exp>
static error_t parse_unary_exp_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    CATCH_ENTER;
    TRY(peek_next(ctx));
    switch (ctx->peek_tok->tok_kind) {
        case TOK_unop_complement:
        case TOK_unop_neg:
        case TOK_unop_not:
            TRY(parse_unary_factor(ctx, exp));
            break;
        case TOK_unop_incr:
        case TOK_unop_decr:
            TRY(parse_incr_unary_factor(ctx, exp));
            break;
        case TOK_binop_multiply:
        case TOK_binop_bitand:
            TRY(parse_ptr_unary_factor(ctx, exp));
            break;
        case TOK_key_sizeof:
            TRY(parse_sizeof_unary_factor(ctx, exp));
            break;
        default:
            TRY(parse_postfix_exp_factor(ctx, exp));
            break;
    }
    FINALLY_EXIT;
    CATCH_EXIT;
}

// <cast-exp> ::= "(" <type-name> ")" <cast-exp> | <unary-exp>
static error_t parse_cast_exp_factor(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    CATCH_ENTER;
    TRY(peek_next(ctx));
    if (ctx->peek_tok->tok_kind == TOK_open_paren) {
        TRY(peek_next_i(ctx, 1));
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
                TRY(parse_cast_factor(ctx, exp));
                TEMP_RETURN;
            default:
                break;
        }
    }
    TRY(parse_unary_exp_factor(ctx, exp));
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_assign_exp(Ctx ctx, int32_t precedence, return_t(std::unique_ptr<CExp>) exp_left) {
    std::unique_ptr<CExp> exp_right;
    std::unique_ptr<CUnaryOp> unop;
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(pop_next(ctx));
    TRY(parse_exp(ctx, precedence, &exp_right));
    *exp_left = std::make_unique<CAssignment>(std::move(unop), std::move(*exp_left), std::move(exp_right), line);
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_assign_compound_exp(Ctx ctx, int32_t precedence, return_t(std::unique_ptr<CExp>) exp_left) {
    std::unique_ptr<CExp> exp_right;
    std::unique_ptr<CExp> exp_right_1;
    std::unique_ptr<CBinaryOp> binop;
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(parse_binop(ctx, &binop));
    TRY(parse_exp(ctx, precedence, &exp_right));
    exp_right_1 = std::make_unique<CBinary>(std::move(binop), std::move(*exp_left), std::move(exp_right), line);
    *exp_left = std::make_unique<CAssignment>(nullptr, nullptr, std::move(exp_right_1), line);
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_binary_exp(Ctx ctx, int32_t precedence, return_t(std::unique_ptr<CExp>) exp_left) {
    std::unique_ptr<CExp> exp_right;
    std::unique_ptr<CBinaryOp> binop;
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(parse_binop(ctx, &binop));
    TRY(parse_exp(ctx, precedence + 1, &exp_right));
    *exp_left = std::make_unique<CBinary>(std::move(binop), std::move(*exp_left), std::move(exp_right), line);
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_ternary_exp(Ctx ctx, int32_t precedence, return_t(std::unique_ptr<CExp>) exp_left) {
    std::unique_ptr<CExp> exp_middle;
    std::unique_ptr<CExp> exp_right;
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(pop_next(ctx));
    TRY(parse_exp(ctx, 0, &exp_middle));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_ternary_else));
    TRY(parse_exp(ctx, precedence, &exp_right));
    *exp_left = std::make_unique<CConditional>(std::move(*exp_left), std::move(exp_middle), std::move(exp_right), line);
    FINALLY_EXIT;
    CATCH_EXIT;
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
static error_t parse_exp(Ctx ctx, int32_t min_precedence, return_t(std::unique_ptr<CExp>) exp) {
    CATCH_ENTER;
    TRY(parse_cast_exp_factor(ctx, exp));
    while (true) {
        TRY(peek_next(ctx));
        int32_t precedence = get_tok_precedence(ctx->peek_tok->tok_kind);
        if (precedence < min_precedence) {
            break;
        }
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
                TRY(parse_binary_exp(ctx, precedence, exp));
                break;
            case TOK_assign:
                TRY(parse_assign_exp(ctx, precedence, exp));
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
                TRY(parse_assign_compound_exp(ctx, precedence, exp));
                break;
            case TOK_ternary_if:
                TRY(parse_ternary_exp(ctx, precedence, exp));
                break;
            default:
                THROW_AT_LINE_EX(
                    GET_PARSER_MSG(MSG_expect_exp, get_tok_fmt(ctx->identifiers, ctx->peek_tok)), ctx->peek_tok->line);
        }
    }
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_for_init(Ctx ctx, return_t(std::unique_ptr<CForInit>) for_init);
static error_t parse_block(Ctx ctx, return_t(std::unique_ptr<CBlock>) block);
static error_t parse_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement);

static error_t parse_ret_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    std::unique_ptr<CExp> exp;
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(pop_next(ctx));
    TRY(peek_next(ctx));
    if (ctx->peek_tok->tok_kind != TOK_semicolon) {
        TRY(parse_exp(ctx, 0, &exp));
    }
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_semicolon));
    *statement = std::make_unique<CReturn>(std::move(exp), line);
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_exp_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    std::unique_ptr<CExp> exp;
    CATCH_ENTER;
    TRY(parse_exp(ctx, 0, &exp));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_semicolon));
    *statement = std::make_unique<CExpression>(std::move(exp));
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_if_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    std::unique_ptr<CExp> condition;
    std::unique_ptr<CStatement> then;
    std::unique_ptr<CStatement> else_fi;
    CATCH_ENTER;
    TRY(pop_next(ctx));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_open_paren));
    TRY(parse_exp(ctx, 0, &condition));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_close_paren));
    TRY(peek_next(ctx));
    TRY(parse_statement(ctx, &then));
    TRY(peek_next(ctx));
    if (ctx->peek_tok->tok_kind == TOK_key_else) {
        TRY(pop_next(ctx));
        TRY(peek_next(ctx));
        TRY(parse_statement(ctx, &else_fi));
    }
    *statement = std::make_unique<CIf>(std::move(condition), std::move(then), std::move(else_fi));
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_goto_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(pop_next(ctx));
    TRY(peek_next(ctx));
    TRY(expect_next(ctx, ctx->peek_tok, TOK_identifier));
    TIdentifier target;
    TRY(parse_identifier(ctx, 0, &target));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_semicolon));
    *statement = std::make_unique<CGoto>(target, line);
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_label_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    std::unique_ptr<CStatement> jump_to;
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TIdentifier target;
    TRY(parse_identifier(ctx, 0, &target));
    TRY(pop_next(ctx));
    TRY(peek_next(ctx));
    TRY(parse_statement(ctx, &jump_to));
    *statement = std::make_unique<CLabel>(target, std::move(jump_to), line);
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_compound_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    std::unique_ptr<CBlock> block;
    CATCH_ENTER;
    TRY(parse_block(ctx, &block));
    *statement = std::make_unique<CCompound>(std::move(block));
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_while_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    std::unique_ptr<CExp> condition;
    std::unique_ptr<CStatement> body;
    CATCH_ENTER;
    TRY(pop_next(ctx));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_open_paren));
    TRY(parse_exp(ctx, 0, &condition));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_close_paren));
    TRY(peek_next(ctx));
    TRY(parse_statement(ctx, &body));
    *statement = std::make_unique<CWhile>(std::move(condition), std::move(body));
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_do_while_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    std::unique_ptr<CStatement> body;
    std::unique_ptr<CExp> condition;
    CATCH_ENTER;
    TRY(pop_next(ctx));
    TRY(peek_next(ctx));
    TRY(parse_statement(ctx, &body));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_key_while));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_open_paren));
    TRY(parse_exp(ctx, 0, &condition));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_close_paren));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_semicolon));
    *statement = std::make_unique<CDoWhile>(std::move(condition), std::move(body));
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_for_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    std::unique_ptr<CForInit> for_init;
    std::unique_ptr<CExp> condition;
    std::unique_ptr<CExp> post;
    std::unique_ptr<CStatement> body;
    CATCH_ENTER;
    TRY(pop_next(ctx));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_open_paren));
    TRY(parse_for_init(ctx, &for_init));
    TRY(peek_next(ctx));
    if (ctx->peek_tok->tok_kind != TOK_semicolon) {
        TRY(parse_exp(ctx, 0, &condition));
    }
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_semicolon));
    TRY(peek_next(ctx));
    if (ctx->peek_tok->tok_kind != TOK_close_paren) {
        TRY(parse_exp(ctx, 0, &post));
    }
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_close_paren));
    TRY(peek_next(ctx));
    TRY(parse_statement(ctx, &body));
    *statement = std::make_unique<CFor>(std::move(for_init), std::move(condition), std::move(post), std::move(body));
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_switch_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    std::unique_ptr<CExp> match;
    std::unique_ptr<CStatement> body;
    CATCH_ENTER;
    TRY(pop_next(ctx));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_open_paren));
    TRY(parse_exp(ctx, 0, &match));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_close_paren));
    TRY(peek_next(ctx));
    TRY(parse_statement(ctx, &body));
    *statement = std::make_unique<CSwitch>(std::move(match), std::move(body));
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_case_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    std::unique_ptr<CExp> value;
    std::unique_ptr<CStatement> jump_to;
    std::shared_ptr<CConst> constant;
    CATCH_ENTER;
    size_t line;
    TRY(pop_next(ctx));
    line = ctx->peek_tok->line;
    TRY(peek_next(ctx));
    switch (ctx->peek_tok->tok_kind) {
        case TOK_int_const:
        case TOK_long_const:
        case TOK_char_const:
            TRY(parse_const(ctx, &constant));
            break;
        case TOK_uint_const:
        case TOK_ulong_const:
            TRY(parse_unsigned_const(ctx, &constant));
            break;
        default:
            THROW_AT_LINE_EX(GET_PARSER_MSG(MSG_case_value_not_int_const, get_tok_fmt(ctx->identifiers, ctx->peek_tok)),
                ctx->peek_tok->line);
    }
    value = std::make_unique<CConstant>(std::move(constant), line);
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_ternary_else));
    TRY(peek_next(ctx));
    TRY(parse_statement(ctx, &jump_to));
    *statement = std::make_unique<CCase>(std::move(value), std::move(jump_to));
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_default_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    std::unique_ptr<CStatement> jump_to;
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(pop_next(ctx));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_ternary_else));
    TRY(peek_next(ctx));
    TRY(parse_statement(ctx, &jump_to));
    *statement = std::make_unique<CDefault>(std::move(jump_to), line);
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_break_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(pop_next(ctx));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_semicolon));
    *statement = std::make_unique<CBreak>(line);
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_continue_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(pop_next(ctx));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_semicolon));
    *statement = std::make_unique<CContinue>(line);
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_null_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    CATCH_ENTER;
    TRY(pop_next(ctx));
    *statement = std::make_unique<CNull>();
    FINALLY_EXIT;
    CATCH_EXIT;
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
static error_t parse_statement(Ctx ctx, return_t(std::unique_ptr<CStatement>) statement) {
    CATCH_ENTER;
    switch (ctx->peek_tok->tok_kind) {
        case TOK_key_return:
            TRY(parse_ret_statement(ctx, statement));
            TEMP_RETURN;
        case TOK_key_if:
            TRY(parse_if_statement(ctx, statement));
            TEMP_RETURN;
        case TOK_key_goto:
            TRY(parse_goto_statement(ctx, statement));
            TEMP_RETURN;
        case TOK_identifier: {
            TRY(peek_next_i(ctx, 1));
            if (ctx->peek_tok_i->tok_kind == TOK_ternary_else) {
                TRY(parse_label_statement(ctx, statement));
                TEMP_RETURN;
            }
            break;
        }
        case TOK_open_brace:
            TRY(parse_compound_statement(ctx, statement));
            TEMP_RETURN;
        case TOK_key_while:
            TRY(parse_while_statement(ctx, statement));
            TEMP_RETURN;
        case TOK_key_do:
            TRY(parse_do_while_statement(ctx, statement));
            TEMP_RETURN;
        case TOK_key_for:
            TRY(parse_for_statement(ctx, statement));
            TEMP_RETURN;
        case TOK_key_switch:
            TRY(parse_switch_statement(ctx, statement));
            TEMP_RETURN;
        case TOK_key_case:
            TRY(parse_case_statement(ctx, statement));
            TEMP_RETURN;
        case TOK_key_default:
            TRY(parse_default_statement(ctx, statement));
            TEMP_RETURN;
        case TOK_key_break:
            TRY(parse_break_statement(ctx, statement));
            TEMP_RETURN;
        case TOK_key_continue:
            TRY(parse_continue_statement(ctx, statement));
            TEMP_RETURN;
        case TOK_semicolon:
            TRY(parse_null_statement(ctx, statement));
            TEMP_RETURN;
        default:
            break;
    }
    TRY(parse_exp_statement(ctx, statement));
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_decltor_decl(Ctx ctx, Declarator& decltor, return_t(std::unique_ptr<CStorageClass>) storage_class);
static error_t parse_var_declaration(Ctx ctx, std::unique_ptr<CStorageClass>&& storage_class, Declarator&& decltor,
    return_t(std::unique_ptr<CVariableDeclaration>) var_decl);

static error_t parse_for_init_decl(Ctx ctx, return_t(std::unique_ptr<CForInit>) for_init) {
    Declarator decltor;
    std::unique_ptr<CStorageClass> storage_class;
    std::unique_ptr<CVariableDeclaration> var_decl;
    CATCH_ENTER;
    TRY(parse_decltor_decl(ctx, decltor, &storage_class));
    if (decltor.derived_type->type() == AST_FunType_t) {
        THROW_AT_LINE_EX(GET_PARSER_MSG(MSG_for_init_decl_as_fun, ctx->identifiers->hash_table[decltor.name].c_str()),
            ctx->next_tok->line);
    }
    TRY(parse_var_declaration(ctx, std::move(storage_class), std::move(decltor), &var_decl));
    *for_init = std::make_unique<CInitDecl>(std::move(var_decl));
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_for_init_exp(Ctx ctx, return_t(std::unique_ptr<CForInit>) for_init) {
    std::unique_ptr<CExp> init;
    CATCH_ENTER;
    TRY(peek_next(ctx));
    if (ctx->peek_tok->tok_kind != TOK_semicolon) {
        TRY(parse_exp(ctx, 0, &init));
    }
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_semicolon));
    *for_init = std::make_unique<CInitExp>(std::move(init));
    FINALLY_EXIT;
    CATCH_EXIT;
}

// <for-init> ::= <variable-declaration> | [ <exp> ] ";"
// for_init = InitDecl(variable_declaration) | InitExp(exp?)
static error_t parse_for_init(Ctx ctx, return_t(std::unique_ptr<CForInit>) for_init) {
    CATCH_ENTER;
    TRY(peek_next(ctx));
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
            TRY(parse_for_init_decl(ctx, for_init));
            break;
        default:
            TRY(parse_for_init_exp(ctx, for_init));
            break;
    }
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_declaration(Ctx ctx, return_t(std::unique_ptr<CDeclaration>) declaration);

static error_t parse_s_block_item(Ctx ctx, return_t(std::unique_ptr<CBlockItem>) block_item) {
    std::unique_ptr<CStatement> statement;
    CATCH_ENTER;
    TRY(parse_statement(ctx, &statement));
    *block_item = std::make_unique<CS>(std::move(statement));
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_d_block_item(Ctx ctx, return_t(std::unique_ptr<CBlockItem>) block_item) {
    std::unique_ptr<CDeclaration> declaration;
    CATCH_ENTER;
    TRY(parse_declaration(ctx, &declaration));
    *block_item = std::make_unique<CD>(std::move(declaration));
    FINALLY_EXIT;
    CATCH_EXIT;
}

// <block-item> ::= <statement> | <declaration>
// block_item = S(statement) | D(declaration)
static error_t parse_block_item(Ctx ctx, return_t(std::unique_ptr<CBlockItem>) block_item) {
    CATCH_ENTER;
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
            TRY(parse_d_block_item(ctx, block_item));
            break;
        default:
            TRY(parse_s_block_item(ctx, block_item));
            break;
    }
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_b_block(Ctx ctx, return_t(std::unique_ptr<CBlock>) block) {
    std::unique_ptr<CBlockItem> block_item;
    std::vector<std::unique_ptr<CBlockItem>> block_items;
    CATCH_ENTER;
    TRY(peek_next(ctx));
    while (ctx->peek_tok->tok_kind != TOK_close_brace) {
        TRY(parse_block_item(ctx, &block_item));
        block_items.push_back(std::move(block_item));
        TRY(peek_next(ctx));
    }
    *block = std::make_unique<CB>(std::move(block_items));
    FINALLY_EXIT;
    CATCH_EXIT;
}

// <block> ::= "{" { <block-item> } "}"
// block = B(block_item*)
static error_t parse_block(Ctx ctx, return_t(std::unique_ptr<CBlock>) block) {
    CATCH_ENTER;
    TRY(pop_next(ctx));
    TRY(parse_b_block(ctx, block));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_close_brace));
    FINALLY_EXIT;
    CATCH_EXIT;
}

// <type-specifier> ::= "int" | "long" | "signed" | "unsigned" | "double" | "char" | "void"
//                    | ("struct" | "union") <identifier>
static error_t parse_type_specifier(Ctx ctx, return_t(std::shared_ptr<Type>) type_specifier) {
    std::string type_tok_kinds_s;
    std::vector<TOKEN_KIND> type_tok_kinds;
    CATCH_ENTER;
    size_t line;
    size_t i = 0;
    TRY(peek_next(ctx));
    line = ctx->peek_tok->line;
    while (true) {
        TRY(peek_next_i(ctx, i));
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
                TRY(pop_next_i(ctx, i));
                type_tok_kinds.push_back(ctx->next_tok_i->tok_kind);
                break;
            case TOK_key_struct:
            case TOK_key_union:
                TRY(pop_next_i(ctx, i));
                type_tok_kinds.push_back(ctx->next_tok_i->tok_kind);
                TRY(peek_next_i(ctx, i));
                TRY(expect_next(ctx, ctx->peek_tok_i, TOK_identifier));
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
                    TRY(peek_next_i(ctx, i));
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
                    TEMP_RETURN;
                }
                case TOK_key_int: {
                    *type_specifier = std::make_shared<Int>();
                    TEMP_RETURN;
                }
                case TOK_key_long: {
                    *type_specifier = std::make_shared<Long>();
                    TEMP_RETURN;
                }
                case TOK_key_double: {
                    *type_specifier = std::make_shared<Double>();
                    TEMP_RETURN;
                }
                case TOK_key_unsigned: {
                    *type_specifier = std::make_shared<UInt>();
                    TEMP_RETURN;
                }
                case TOK_key_signed: {
                    *type_specifier = std::make_shared<Int>();
                    TEMP_RETURN;
                }
                case TOK_key_void: {
                    *type_specifier = std::make_shared<Void>();
                    TEMP_RETURN;
                }
                case TOK_key_struct: {
                    TIdentifier tag;
                    TRY(parse_identifier(ctx, i, &tag));
                    *type_specifier = std::make_shared<Structure>(tag, false);
                    TEMP_RETURN;
                }
                case TOK_key_union: {
                    TIdentifier tag;
                    TRY(parse_identifier(ctx, i, &tag));
                    *type_specifier = std::make_shared<Structure>(tag, true);
                    TEMP_RETURN;
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
                    TEMP_RETURN;
                }
                else if (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_long)
                         != type_tok_kinds.end()) {
                    *type_specifier = std::make_shared<ULong>();
                    TEMP_RETURN;
                }
                else if (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_char)
                         != type_tok_kinds.end()) {
                    *type_specifier = std::make_shared<UChar>();
                    TEMP_RETURN;
                }
            }
            else if (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_signed) != type_tok_kinds.end()) {
                if (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_int) != type_tok_kinds.end()) {
                    *type_specifier = std::make_shared<Int>();
                    TEMP_RETURN;
                }
                else if (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_long)
                         != type_tok_kinds.end()) {
                    *type_specifier = std::make_shared<Long>();
                    TEMP_RETURN;
                }
                else if (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_char)
                         != type_tok_kinds.end()) {
                    *type_specifier = std::make_shared<SChar>();
                    TEMP_RETURN;
                }
            }
            else if ((std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_int) != type_tok_kinds.end())
                     && (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_long)
                         != type_tok_kinds.end())) {
                *type_specifier = std::make_shared<Long>();
                TEMP_RETURN;
            }
            break;
        }
        case 3: {
            if ((std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_int) != type_tok_kinds.end())
                && (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_long) != type_tok_kinds.end())) {
                if (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_unsigned) != type_tok_kinds.end()) {
                    *type_specifier = std::make_shared<ULong>();
                    TEMP_RETURN;
                }
                else if (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_signed)
                         != type_tok_kinds.end()) {
                    *type_specifier = std::make_shared<Long>();
                    TEMP_RETURN;
                }
            }
            break;
        }
        default:
            break;
    }
    type_tok_kinds_s = "(";
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
    FINALLY_EXIT;
    CATCH_EXIT;
}

// <specifier> ::= <type-specifier> | "static" | "extern"
// storage_class = Static | Extern
static error_t parse_storage_class(Ctx ctx, return_t(std::unique_ptr<CStorageClass>) storage_class) {
    CATCH_ENTER;
    TRY(pop_next(ctx));
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
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_initializer(Ctx ctx, return_t(std::unique_ptr<CInitializer>) initializer);

static error_t parse_single_init(Ctx ctx, return_t(std::unique_ptr<CInitializer>) initializer) {
    std::unique_ptr<CExp> exp;
    CATCH_ENTER;
    TRY(parse_exp(ctx, 0, &exp));
    *initializer = std::make_unique<CSingleInit>(std::move(exp));
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_compound_init(Ctx ctx, return_t(std::unique_ptr<CInitializer>) initializer) {
    std::vector<std::unique_ptr<CInitializer>> initializers;
    CATCH_ENTER;
    TRY(pop_next(ctx));
    while (true) {
        TRY(peek_next(ctx));
        if (ctx->peek_tok->tok_kind == TOK_close_brace) {
            break;
        }
        TRY(parse_initializer(ctx, initializer));
        initializers.push_back(std::move(*initializer));
        TRY(peek_next(ctx));
        if (ctx->peek_tok->tok_kind == TOK_close_brace) {
            break;
        }
        TRY(pop_next(ctx));
        TRY(expect_next(ctx, ctx->next_tok, TOK_comma_separator));
    }
    if (initializers.empty()) {
        THROW_AT_LINE_EX(GET_PARSER_MSG_0(MSG_empty_compound_init), ctx->peek_tok->line);
    }
    TRY(pop_next(ctx));
    *initializer = std::make_unique<CCompoundInit>(std::move(initializers));
    FINALLY_EXIT;
    CATCH_EXIT;
}

// <initializer> ::= <exp> | "{" <initializer> { "," <initializer> } [","] "}"
// initializer = SingleInit(exp) | CompoundInit(initializer*)
static error_t parse_initializer(Ctx ctx, return_t(std::unique_ptr<CInitializer>) initializer) {
    CATCH_ENTER;
    TRY(peek_next(ctx));
    if (ctx->peek_tok->tok_kind == TOK_open_brace) {
        TRY(parse_compound_init(ctx, initializer));
    }
    else {
        TRY(parse_single_init(ctx, initializer));
    }
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_decltor(Ctx ctx, return_t(std::unique_ptr<CDeclarator>) decltor);
static error_t proc_decltor(Ctx ctx, CDeclarator* node, std::shared_ptr<Type>&& base_type, Declarator& decltor);

static void proc_ident_decltor(CIdent* node, std::shared_ptr<Type>&& base_type, Declarator& decltor) {
    decltor.name = node->name;
    decltor.derived_type = std::move(base_type);
}

static error_t proc_ptr_decltor(
    Ctx ctx, CPointerDeclarator* node, std::shared_ptr<Type>&& base_type, Declarator& decltor) {
    std::shared_ptr<Type> derived_type;
    CATCH_ENTER;
    derived_type = std::make_shared<Pointer>(std::move(base_type));
    TRY(proc_decltor(ctx, node->decltor.get(), std::move(derived_type), decltor));
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t proc_arr_decltor(
    Ctx ctx, CArrayDeclarator* node, std::shared_ptr<Type>&& base_type, Declarator& decltor) {
    std::shared_ptr<Type> derived_type;
    CATCH_ENTER;
    TLong size = node->size;
    derived_type = std::make_shared<Array>(size, std::move(base_type));
    TRY(proc_decltor(ctx, node->decltor.get(), std::move(derived_type), decltor));
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t proc_param_decltor(
    Ctx ctx, CParam* node, std::vector<TIdentifier>& params, std::vector<std::shared_ptr<Type>>& param_types) {
    Declarator decltor;
    std::shared_ptr<Type> param_type;
    CATCH_ENTER;
    param_type = node->param_type;
    TRY(proc_decltor(ctx, node->decltor.get(), std::move(param_type), decltor));
    THROW_ABORT_IF(decltor.derived_type->type() == AST_FunType_t);
    params.push_back(decltor.name);
    param_types.push_back(std::move(decltor.derived_type));
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t proc_fun_decltor(Ctx ctx, CFunDeclarator* node, std::shared_ptr<Type>&& base_type, Declarator& decltor) {
    std::shared_ptr<Type> derived_type;
    std::vector<TIdentifier> params;
    std::vector<std::shared_ptr<Type>> param_types;
    CATCH_ENTER;
    if (node->decltor->type() != AST_CIdent_t) {
        THROW_AT_LINE_EX(GET_PARSER_MSG_0(MSG_derived_fun_decl), ctx->next_tok->line);
    }

    params.reserve(node->param_list.size());
    param_types.reserve(node->param_list.size());
    for (const auto& param : node->param_list) {
        proc_param_decltor(ctx, param.get(), params, param_types);
    }
    TIdentifier name = static_cast<CIdent*>(node->decltor.get())->name;
    derived_type = std::make_shared<FunType>(std::move(param_types), std::move(base_type));
    decltor.name = name;
    decltor.derived_type = std::move(derived_type);
    decltor.params = std::move(params);
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t proc_decltor(Ctx ctx, CDeclarator* node, std::shared_ptr<Type>&& base_type, Declarator& decltor) {
    CATCH_ENTER;
    switch (node->type()) {
        case AST_CIdent_t:
            proc_ident_decltor(static_cast<CIdent*>(node), std::move(base_type), decltor);
            break;
        case AST_CPointerDeclarator_t:
            TRY(proc_ptr_decltor(ctx, static_cast<CPointerDeclarator*>(node), std::move(base_type), decltor));
            break;
        case AST_CArrayDeclarator_t:
            TRY(proc_arr_decltor(ctx, static_cast<CArrayDeclarator*>(node), std::move(base_type), decltor));
            break;
        case AST_CFunDeclarator_t:
            TRY(proc_fun_decltor(ctx, static_cast<CFunDeclarator*>(node), std::move(base_type), decltor));
            break;
        default:
            THROW_ABORT;
    }
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_ident_decltor(Ctx ctx, return_t(std::unique_ptr<CDeclarator>) decltor) {
    CATCH_ENTER;
    TIdentifier name;
    TRY(parse_identifier(ctx, 0, &name));
    *decltor = std::make_unique<CIdent>(name);
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_simple_decltor(Ctx ctx, return_t(std::unique_ptr<CDeclarator>) decltor) {
    CATCH_ENTER;
    TRY(pop_next(ctx));
    TRY(parse_decltor(ctx, decltor));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_close_paren));
    FINALLY_EXIT;
    CATCH_EXIT;
}

// <simple-declarator> ::= <identifier> | "(" <declarator> ")"
static error_t parse_simple_decltor_decl(Ctx ctx, return_t(std::unique_ptr<CDeclarator>) decltor) {
    CATCH_ENTER;
    TRY(peek_next(ctx));
    switch (ctx->peek_tok->tok_kind) {
        case TOK_identifier:
            TRY(parse_ident_decltor(ctx, decltor));
            break;
        case TOK_open_paren:
            TRY(parse_simple_decltor(ctx, decltor));
            break;
        default:
            THROW_AT_LINE_EX(GET_PARSER_MSG(MSG_expect_simple_decltor, get_tok_fmt(ctx->identifiers, ctx->peek_tok)),
                ctx->peek_tok->line);
    }
    FINALLY_EXIT;
    CATCH_EXIT;
}

// <param> ::= { <type-specifier> }+ <declarator>
// param_info = Param(type, declarator)
static error_t parse_param(Ctx ctx, return_t(std::unique_ptr<CParam>) param) {
    std::unique_ptr<CDeclarator> decltor;
    std::shared_ptr<Type> param_type;
    CATCH_ENTER;
    TRY(parse_type_specifier(ctx, &param_type));
    TRY(parse_decltor(ctx, &decltor));
    *param = std::make_unique<CParam>(std::move(decltor), std::move(param_type));
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_empty_param_list(Ctx ctx) {
    CATCH_ENTER;
    TRY(pop_next(ctx));
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_non_empty_param_list(Ctx ctx, return_t(std::vector<std::unique_ptr<CParam>>) param_list) {
    std::unique_ptr<CParam> param;
    CATCH_ENTER;
    TRY(parse_param(ctx, &param));
    param_list->push_back(std::move(param));
    TRY(peek_next(ctx));
    while (ctx->peek_tok->tok_kind == TOK_comma_separator) {
        TRY(pop_next(ctx));
        TRY(parse_param(ctx, &param));
        param_list->push_back(std::move(param));
        TRY(peek_next(ctx));
    }
    FINALLY_EXIT;
    CATCH_EXIT;
}

// <param-list> ::= "(" "void" ")" | "(" <param> { "," <param> } ")"
static error_t parse_param_list(Ctx ctx, return_t(std::vector<std::unique_ptr<CParam>>) param_list) {
    CATCH_ENTER;
    TRY(pop_next(ctx));
    TRY(peek_next(ctx));
    switch (ctx->peek_tok->tok_kind) {
        case TOK_key_void: {
            TRY(peek_next_i(ctx, 1));
            if (ctx->peek_tok_i->tok_kind == TOK_close_paren) {
                TRY(parse_empty_param_list(ctx));
            }
            else {
                TRY(parse_non_empty_param_list(ctx, param_list));
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
            TRY(parse_non_empty_param_list(ctx, param_list));
            break;
        default:
            THROW_AT_LINE_EX(GET_PARSER_MSG(MSG_expect_param_list, get_tok_fmt(ctx->identifiers, ctx->peek_tok)),
                ctx->peek_tok->line);
    }
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_close_paren));
    FINALLY_EXIT;
    CATCH_EXIT;
}

// (fun) <declarator-suffix> ::= <param-list>
static error_t parse_fun_decltor_suffix(Ctx ctx, return_t(std::unique_ptr<CDeclarator>) decltor) {
    std::vector<std::unique_ptr<CParam>> param_list;
    CATCH_ENTER;
    TRY(parse_param_list(ctx, &param_list));
    *decltor = std::make_unique<CFunDeclarator>(std::move(param_list), std::move(*decltor));
    FINALLY_EXIT;
    CATCH_EXIT;
}

// (array) <declarator-suffix> ::= { "[" <const> "]" }+
static error_t parse_arr_decltor_suffix(Ctx ctx, return_t(std::unique_ptr<CDeclarator>) decltor) {
    CATCH_ENTER;
    do {
        TLong size;
        TRY(parse_arr_size(ctx, &size));
        *decltor = std::make_unique<CArrayDeclarator>(size, std::move(*decltor));
        TRY(peek_next(ctx));
    }
    while (ctx->peek_tok->tok_kind == TOK_open_bracket);
    FINALLY_EXIT;
    CATCH_EXIT;
}

// <direct-declarator> ::= <simple-declarator> [ <declarator-suffix> ]
static error_t parse_direct_decltor(Ctx ctx, return_t(std::unique_ptr<CDeclarator>) decltor) {
    CATCH_ENTER;
    TRY(parse_simple_decltor_decl(ctx, decltor));
    TRY(peek_next(ctx));
    switch (ctx->peek_tok->tok_kind) {
        case TOK_open_paren:
            TRY(parse_fun_decltor_suffix(ctx, decltor));
            break;
        case TOK_open_bracket:
            TRY(parse_arr_decltor_suffix(ctx, decltor));
            break;
        default:
            break;
    }
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_ptr_decltor(Ctx ctx, return_t(std::unique_ptr<CDeclarator>) decltor) {
    CATCH_ENTER;
    TRY(pop_next(ctx));
    TRY(parse_decltor(ctx, decltor));
    *decltor = std::make_unique<CPointerDeclarator>(std::move(*decltor));
    FINALLY_EXIT;
    CATCH_EXIT;
}

// <declarator> ::= "*" <declarator> | <direct-declarator>
// declarator = Ident(identifier) | PointerDeclarator(declarator) | ArrayDeclarator(int, declarator)
//            | FunDeclarator(param_info*, declarator)
static error_t parse_decltor(Ctx ctx, return_t(std::unique_ptr<CDeclarator>) decltor) {
    CATCH_ENTER;
    TRY(peek_next(ctx));
    if (ctx->peek_tok->tok_kind == TOK_binop_multiply) {
        TRY(parse_ptr_decltor(ctx, decltor));
    }
    else {
        TRY(parse_direct_decltor(ctx, decltor));
    }
    FINALLY_EXIT;
    CATCH_EXIT;
}

// <function-declaration> ::= { <specifier> }+ <declarator> ( <block> | ";")
// function_declaration = FunctionDeclaration(identifier, identifier*, block?, type, storage_class?)
static error_t parse_fun_declaration(Ctx ctx, std::unique_ptr<CStorageClass>&& storage_class, Declarator&& decltor,
    return_t(std::unique_ptr<CFunctionDeclaration>) fun_decl) {
    std::unique_ptr<CBlock> body;
    CATCH_ENTER;
    size_t line = ctx->next_tok->line;
    TRY(peek_next(ctx));
    if (ctx->peek_tok->tok_kind == TOK_semicolon) {
        TRY(pop_next(ctx));
    }
    else {
        TRY(expect_next(ctx, ctx->peek_tok, TOK_open_brace));
        TRY(parse_block(ctx, &body));
    }
    *fun_decl = std::make_unique<CFunctionDeclaration>(decltor.name, std::move(decltor.params), std::move(body),
        std::move(decltor.derived_type), std::move(storage_class), line);
    FINALLY_EXIT;
    CATCH_EXIT;
}

// <variable-declaration> ::= { <specifier> }+ <declarator> [ "=" <initializer> ] ";"
// variable_declaration = VariableDeclaration(identifier, initializer?, type, storage_class?)
static error_t parse_var_declaration(Ctx ctx, std::unique_ptr<CStorageClass>&& storage_class, Declarator&& decltor,
    return_t(std::unique_ptr<CVariableDeclaration>) var_decl) {
    std::unique_ptr<CInitializer> initializer;
    CATCH_ENTER;
    size_t line = ctx->next_tok->line;
    TRY(peek_next(ctx));
    if (ctx->peek_tok->tok_kind == TOK_assign) {
        TRY(pop_next(ctx));
        TRY(parse_initializer(ctx, &initializer));
    }
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_semicolon));
    *var_decl = std::make_unique<CVariableDeclaration>(
        decltor.name, std::move(initializer), std::move(decltor.derived_type), std::move(storage_class), line);
    FINALLY_EXIT;
    CATCH_EXIT;
}

// <member-declaration> ::= { <type-specifier> }+ <declarator> ";"
// member_declaration = MemberDeclaration(identifier, type)
static error_t parse_member_declaration(Ctx ctx, return_t(std::unique_ptr<CMemberDeclaration>) member_decl) {
    Declarator decltor;
    std::unique_ptr<CStorageClass> storage_class;
    CATCH_ENTER;
    size_t line;
    TRY(parse_decltor_decl(ctx, decltor, &storage_class));
    if (storage_class) {
        THROW_AT_LINE_EX(GET_PARSER_MSG(MSG_member_decl_not_auto, ctx->identifiers->hash_table[decltor.name].c_str(),
                             get_storage_class_fmt(storage_class.get())),
            ctx->next_tok->line);
    }
    if (decltor.derived_type->type() == AST_FunType_t) {
        THROW_AT_LINE_EX(GET_PARSER_MSG(MSG_member_decl_as_fun, ctx->identifiers->hash_table[decltor.name].c_str()),
            ctx->next_tok->line);
    }
    line = ctx->next_tok->line;
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_semicolon));
    *member_decl = std::make_unique<CMemberDeclaration>(decltor.name, std::move(decltor.derived_type), line);
    FINALLY_EXIT;
    CATCH_EXIT;
}

// <struct-declaration> ::= ("struct" | "union") <identifier> [ "{" { <member-declaration> }+ "}" ] ";"
// struct_declaration = StructDeclaration(identifier, bool, member_declaration*)
static error_t parse_struct_declaration(Ctx ctx, return_t(std::unique_ptr<CStructDeclaration>) struct_decl) {
    std::unique_ptr<CMemberDeclaration> member;
    std::vector<std::unique_ptr<CMemberDeclaration>> members;
    CATCH_ENTER;
    bool is_union;
    size_t line = ctx->peek_tok->line;
    TRY(pop_next(ctx));
    is_union = ctx->next_tok->tok_kind == TOK_key_union;
    TRY(peek_next(ctx));
    TRY(expect_next(ctx, ctx->peek_tok, TOK_identifier));
    TIdentifier tag;
    TRY(parse_identifier(ctx, 0, &tag));
    TRY(pop_next(ctx));
    if (ctx->next_tok->tok_kind == TOK_open_brace) {
        do {
            TRY(parse_member_declaration(ctx, &member));
            members.push_back(std::move(member));
            TRY(peek_next(ctx));
        }
        while (ctx->peek_tok->tok_kind != TOK_close_brace);
        TRY(pop_next(ctx));
        TRY(pop_next(ctx));
    }
    TRY(expect_next(ctx, ctx->next_tok, TOK_semicolon));
    *struct_decl = std::make_unique<CStructDeclaration>(tag, is_union, std::move(members), line);
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_fun_decl(Ctx ctx, std::unique_ptr<CStorageClass>&& storage_class, Declarator&& decltor,
    return_t(std::unique_ptr<CDeclaration>) declaration) {
    std::unique_ptr<CFunctionDeclaration> fun_decl;
    CATCH_ENTER;
    TRY(parse_fun_declaration(ctx, std::move(storage_class), std::move(decltor), &fun_decl));
    *declaration = std::make_unique<CFunDecl>(std::move(fun_decl));
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_var_decl(Ctx ctx, std::unique_ptr<CStorageClass>&& storage_class, Declarator&& decltor,
    return_t(std::unique_ptr<CDeclaration>) declaration) {
    std::unique_ptr<CVariableDeclaration> var_decl;
    CATCH_ENTER;
    TRY(parse_var_declaration(ctx, std::move(storage_class), std::move(decltor), &var_decl));
    *declaration = std::make_unique<CVarDecl>(std::move(var_decl));
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_struct_decl(Ctx ctx, return_t(std::unique_ptr<CDeclaration>) declaration) {
    std::unique_ptr<CStructDeclaration> struct_decl;
    CATCH_ENTER;
    TRY(parse_struct_declaration(ctx, &struct_decl));
    *declaration = std::make_unique<CStructDecl>(std::move(struct_decl));
    FINALLY_EXIT;
    CATCH_EXIT;
}

static error_t parse_decltor_decl(
    Ctx ctx, Declarator& decltor, return_t(std::unique_ptr<CStorageClass>) storage_class) {
    std::unique_ptr<CDeclarator> decltor_1;
    std::shared_ptr<Type> type_specifier;
    CATCH_ENTER;
    TRY(parse_type_specifier(ctx, &type_specifier));
    TRY(peek_next(ctx));
    switch (ctx->peek_tok->tok_kind) {
        case TOK_identifier:
        case TOK_binop_multiply:
        case TOK_open_paren:
            break;
        default:
            TRY(parse_storage_class(ctx, storage_class));
            break;
    }
    TRY(parse_decltor(ctx, &decltor_1));
    TRY(proc_decltor(ctx, decltor_1.get(), std::move(type_specifier), decltor));
    FINALLY_EXIT;
    CATCH_EXIT;
}

// <declaration> ::= <variable-declaration> | <function-declaration> | <struct-declaration>
// declaration = FunDecl(function_declaration) | VarDecl(variable_declaration) | StructDecl(struct_declaration)
static error_t parse_declaration(Ctx ctx, return_t(std::unique_ptr<CDeclaration>) declaration) {
    Declarator decltor;
    std::unique_ptr<CStorageClass> storage_class;
    CATCH_ENTER;
    TRY(peek_next(ctx));
    switch (ctx->peek_tok->tok_kind) {
        case TOK_key_struct:
        case TOK_key_union: {
            TRY(peek_next_i(ctx, 2));
            switch (ctx->peek_tok_i->tok_kind) {
                case TOK_open_brace:
                case TOK_semicolon:
                    TRY(parse_struct_decl(ctx, declaration));
                    TEMP_RETURN;
                default:
                    break;
            }
        }
        default:
            break;
    }
    TRY(parse_decltor_decl(ctx, decltor, &storage_class));
    if (decltor.derived_type->type() == AST_FunType_t) {
        TRY(parse_fun_decl(ctx, std::move(storage_class), std::move(decltor), declaration));
    }
    else {
        TRY(parse_var_decl(ctx, std::move(storage_class), std::move(decltor), declaration));
    }
    FINALLY_EXIT;
    CATCH_EXIT;
}

// TODO HERE going up^

// <program> ::= { <declaration> }
// AST = Program(declaration*)
static error_t parse_program(Ctx ctx, return_t(std::unique_ptr<CProgram>) c_ast) {
    std::unique_ptr<CDeclaration> declaration;
    std::vector<std::unique_ptr<CDeclaration>> declarations;
    CATCH_ENTER;
    while (ctx->pop_idx < ctx->p_toks->size()) {
        TRY(parse_declaration(ctx, &declaration));
        declarations.push_back(std::move(declaration));
    }
    *c_ast = std::make_unique<CProgram>(std::move(declarations));
    FINALLY_EXIT;
    CATCH_EXIT;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

error_t parse_tokens(std::vector<Token>&& tokens, ErrorsContext* errors, IdentifierContext* identifiers,
    return_t(std::unique_ptr<CProgram>) c_ast) {
    ParserContext ctx;
    {
        ctx.errors = errors;
        ctx.identifiers = identifiers;
        ctx.pop_idx = 0;
        ctx.p_toks = &tokens;
    }
    CATCH_ENTER;
    TRY(parse_program(&ctx, c_ast));
    THROW_ABORT_IF(ctx.pop_idx != tokens.size());

    THROW_ABORT_IF(!*c_ast);
    FINALLY_EXIT;
    std::vector<Token>().swap(tokens);
    CATCH_EXIT;
}
