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
    std::vector<Token>* p_toks;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Parser

typedef ParserContext* Ctx;

static void expect_next(Ctx ctx, Token& next_tok, TOKEN_KIND expect_tok) {
    if (next_tok.tok_kind != expect_tok) {
        THROW_AT_LINE(GET_PARSER_MSG(MSG_unexpected_next_tok, get_tok_fmt(ctx->identifiers, &next_tok),
                          get_tok_kind_fmt(expect_tok)),
            next_tok.line);
    }
}

static Token& pop_next(Ctx ctx) {
    if (ctx->pop_idx >= ctx->p_toks->size()) {
        THROW_AT_LINE(GET_PARSER_MSG_0(MSG_reached_eof), ctx->p_toks->back().line);
    }

    ctx->next_tok = &(*ctx->p_toks)[ctx->pop_idx];
    ctx->pop_idx++;
    return *ctx->next_tok;
}

static Token& pop_next_i(Ctx ctx, size_t i) {
    if (i == 0) {
        return pop_next(ctx);
    }
    if (ctx->pop_idx + i >= ctx->p_toks->size()) {
        THROW_AT_LINE(GET_PARSER_MSG_0(MSG_reached_eof), ctx->p_toks->back().line);
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
    pop_next(ctx);
    return (*ctx->p_toks)[ctx->pop_idx - 1];
}

static Token& peek_next(Ctx ctx) {
    if (ctx->pop_idx >= ctx->p_toks->size()) {
        THROW_AT_LINE(GET_PARSER_MSG_0(MSG_reached_eof), ctx->p_toks->back().line);
    }

    ctx->peek_tok = &(*ctx->p_toks)[ctx->pop_idx];
    return *ctx->peek_tok;
}

static Token& peek_next_i(Ctx ctx, size_t i) {
    if (i == 0) {
        return peek_next(ctx);
    }
    if (ctx->pop_idx + i >= ctx->p_toks->size()) {
        THROW_AT_LINE(GET_PARSER_MSG_0(MSG_reached_eof), ctx->p_toks->back().line);
    }

    return (*ctx->p_toks)[ctx->pop_idx + i];
}

// <identifier> ::= ? An identifier token ?
static TIdentifier parse_identifier(Ctx ctx, size_t i) { return pop_next_i(ctx, i).tok_key; }

// string = StringLiteral(int*)
// <string> ::= ? A string token ?
static std::shared_ptr<CStringLiteral> parse_string_literal(Ctx ctx) {
    std::vector<TChar> value;
    {
        string_to_literal(ctx->identifiers->hash_table[ctx->next_tok->tok_key], value);
        while (peek_next(ctx).tok_kind == TOK_string_literal) {
            pop_next(ctx);
            string_to_literal(ctx->identifiers->hash_table[ctx->next_tok->tok_key], value);
        }
    }
    return std::make_shared<CStringLiteral>(std::move(value));
}

// <int> ::= ? An int constant token ?
static std::shared_ptr<CConstInt> parse_int_const(intmax_t intmax) {
    TInt value = intmax_to_int32(intmax);
    return std::make_shared<CConstInt>(value);
}

// <char> ::= ? A char token ?
static std::shared_ptr<CConstInt> parse_char_const(Ctx ctx) {
    TInt value = string_to_char_ascii(ctx->identifiers->hash_table[ctx->next_tok->tok_key]);
    return std::make_shared<CConstInt>(value);
}

// <long> ::= ? An int or long constant token ?
static std::shared_ptr<CConstLong> parse_long_const(intmax_t intmax) {
    TLong value = intmax_to_int64(intmax);
    return std::make_shared<CConstLong>(value);
}

// <double> ::= ? A floating-point constant token ?
static std::shared_ptr<CConstDouble> parse_dbl_const(Ctx ctx) {
    TDouble value =
        string_to_dbl(ctx->errors, ctx->identifiers->hash_table[ctx->next_tok->tok_key].c_str(), ctx->next_tok->line);
    return std::make_shared<CConstDouble>(value);
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
static std::shared_ptr<CConst> parse_const(Ctx ctx) {
    switch (pop_next(ctx).tok_kind) {
        case TOK_char_const:
            return parse_char_const(ctx);
        case TOK_dbl_const:
            return parse_dbl_const(ctx);
        default:
            break;
    }

    intmax_t value = string_to_intmax(
        ctx->errors, ctx->identifiers->hash_table[ctx->next_tok->tok_key].c_str(), ctx->next_tok->line);
    if (value > 9223372036854775807ll) {
        THROW_AT_LINE(
            GET_PARSER_MSG(MSG_overflow_long_const, ctx->identifiers->hash_table[ctx->next_tok->tok_key].c_str()),
            ctx->next_tok->line);
    }
    if (ctx->next_tok->tok_kind == TOK_int_const && value <= 2147483647l) {
        return parse_int_const(value);
    }
    return parse_long_const(value);
}

// <const> ::= <unsigned int> | <unsigned long>
// (unsigned) const = ConstUInt(uint) | ConstULong(ulong) | ConstUChar(int)
static std::shared_ptr<CConst> parse_unsigned_const(Ctx ctx) {
    pop_next(ctx);

    uintmax_t value = string_to_uintmax(
        ctx->errors, ctx->identifiers->hash_table[ctx->next_tok->tok_key].c_str(), ctx->next_tok->line);
    if (value > 18446744073709551615ull) {
        THROW_AT_LINE(
            GET_PARSER_MSG(MSG_overflow_ulong_const, ctx->identifiers->hash_table[ctx->next_tok->tok_key].c_str()),
            ctx->next_tok->line);
    }
    if (ctx->next_tok->tok_kind == TOK_uint_const && value <= 4294967295ul) {
        return parse_uint_const(value);
    }
    return parse_ulong_const(value);
}

static TLong parse_arr_size(Ctx ctx) {
    pop_next(ctx);
    std::shared_ptr<CConst> constant;
    switch (peek_next(ctx).tok_kind) {
        case TOK_int_const:
        case TOK_long_const:
        case TOK_char_const:
            constant = parse_const(ctx);
            break;
        case TOK_uint_const:
        case TOK_ulong_const:
            constant = parse_unsigned_const(ctx);
            break;
        default:
            THROW_AT_LINE(GET_PARSER_MSG(MSG_arr_size_not_int_const, get_tok_fmt(ctx->identifiers, ctx->peek_tok)),
                ctx->peek_tok->line);
    }
    expect_next(ctx, pop_next(ctx), TOK_close_bracket);
    switch (constant->type()) {
        case AST_CConstInt_t:
            return (TLong)(static_cast<CConstInt*>(constant.get())->value);
        case AST_CConstLong_t:
            return static_cast<CConstLong*>(constant.get())->value;
        case AST_CConstUInt_t:
            return (TLong)(static_cast<CConstUInt*>(constant.get())->value);
        case AST_CConstULong_t:
            return (TLong)(static_cast<CConstULong*>(constant.get())->value);
        default:
            THROW_ABORT;
    }
}

// <unop> ::= "-" | "~" | "!" | "*" | "&" | "++" | "--"
// unary_operator = Complement | Negate | Not | Prefix | Postfix
static std::unique_ptr<CUnaryOp> parse_unop(Ctx ctx) {
    switch (pop_next(ctx).tok_kind) {
        case TOK_unop_complement:
            return std::make_unique<CComplement>();
        case TOK_unop_neg:
            return std::make_unique<CNegate>();
        case TOK_unop_not:
            return std::make_unique<CNot>();
        default:
            THROW_AT_LINE(
                GET_PARSER_MSG(MSG_expect_unop, get_tok_fmt(ctx->identifiers, ctx->next_tok)), ctx->next_tok->line);
    }
}

// <binop> ::= "-" | "+" | "*" | "/" | "%" | "&" | "|" | "^" | "<<" | ">>" | "&&" | "||" | "==" | "!=" | "<"
//           | "<=" | ">" | ">=" | "=" | "-=" | "+=" | "*=" | "/=" | "%=" | "&=" | "|=" | "^=" | "<<=" | ">>="
// binary_operator = Add | Subtract | Multiply | Divide | Remainder | BitAnd | BitOr | BitXor | BitShiftLeft
//                 | BitShiftRight | BitShrArithmetic | And | Or | Equal | NotEqual | LessThan | LessOrEqual
//                 | GreaterThan | GreaterOrEqual
static std::unique_ptr<CBinaryOp> parse_binop(Ctx ctx) {
    switch (pop_next(ctx).tok_kind) {
        case TOK_binop_add:
        case TOK_assign_add:
        case TOK_unop_incr:
            return std::make_unique<CAdd>();
        case TOK_unop_neg:
        case TOK_assign_subtract:
        case TOK_unop_decr:
            return std::make_unique<CSubtract>();
        case TOK_binop_multiply:
        case TOK_assign_multiply:
            return std::make_unique<CMultiply>();
        case TOK_binop_divide:
        case TOK_assign_divide:
            return std::make_unique<CDivide>();
        case TOK_binop_remainder:
        case TOK_assign_remainder:
            return std::make_unique<CRemainder>();
        case TOK_binop_bitand:
        case TOK_assign_bitand:
            return std::make_unique<CBitAnd>();
        case TOK_binop_bitor:
        case TOK_assign_bitor:
            return std::make_unique<CBitOr>();
        case TOK_binop_xor:
        case TOK_assign_xor:
            return std::make_unique<CBitXor>();
        case TOK_binop_shiftleft:
        case TOK_assign_shiftleft:
            return std::make_unique<CBitShiftLeft>();
        case TOK_binop_shiftright:
        case TOK_assign_shiftright:
            return std::make_unique<CBitShiftRight>();
        case TOK_binop_and:
            return std::make_unique<CAnd>();
        case TOK_binop_or:
            return std::make_unique<COr>();
        case TOK_binop_eq:
            return std::make_unique<CEqual>();
        case TOK_binop_ne:
            return std::make_unique<CNotEqual>();
        case TOK_binop_lt:
            return std::make_unique<CLessThan>();
        case TOK_binop_le:
            return std::make_unique<CLessOrEqual>();
        case TOK_binop_gt:
            return std::make_unique<CGreaterThan>();
        case TOK_binop_ge:
            return std::make_unique<CGreaterOrEqual>();
        default:
            THROW_AT_LINE(
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

static std::unique_ptr<CAbstractDeclarator> parse_abstract_decltor(Ctx ctx);

// (array) <direct-abstract-declarator> ::= { "[" <const> "]" }+
static std::unique_ptr<CAbstractDeclarator> parse_arr_abstract_decltor(Ctx ctx) {
    std::unique_ptr<CAbstractDeclarator> abstract_decltor = std::make_unique<CAbstractBase>();
    do {
        TLong size = parse_arr_size(ctx);
        abstract_decltor = std::make_unique<CAbstractArray>(size, std::move(abstract_decltor));
    }
    while (peek_next(ctx).tok_kind == TOK_open_bracket);
    return abstract_decltor;
}

// (direct) <direct-abstract-declarator> ::= "(" <abstract-declarator> ")" { "[" <const> "]" }
static std::unique_ptr<CAbstractDeclarator> parse_direct_abstract_decltor(Ctx ctx) {
    pop_next(ctx);
    std::unique_ptr<CAbstractDeclarator> abstract_decltor = parse_abstract_decltor(ctx);
    expect_next(ctx, pop_next(ctx), TOK_close_paren);
    while (peek_next(ctx).tok_kind == TOK_open_bracket) {
        TLong size = parse_arr_size(ctx);
        abstract_decltor = std::make_unique<CAbstractArray>(size, std::move(abstract_decltor));
    }
    return abstract_decltor;
}

static std::unique_ptr<CAbstractPointer> parse_ptr_abstract_decltor(Ctx ctx) {
    pop_next(ctx);
    std::unique_ptr<CAbstractDeclarator> abstract_decltor;
    if (peek_next(ctx).tok_kind == TOK_close_paren) {
        abstract_decltor = std::make_unique<CAbstractBase>();
    }
    else {
        abstract_decltor = parse_abstract_decltor(ctx);
    }
    return std::make_unique<CAbstractPointer>(std::move(abstract_decltor));
}

// <abstract-declarator> ::= "*" [ <abstract-declarator> ] | <direct-abstract-declarator>
// abstract_declarator = AbstractPointer(abstract_declarator) | AbstractArray(int, abstract_declarator) | AbstractBase
static std::unique_ptr<CAbstractDeclarator> parse_abstract_decltor(Ctx ctx) {
    switch (peek_next(ctx).tok_kind) {
        case TOK_binop_multiply:
            return parse_ptr_abstract_decltor(ctx);
        case TOK_open_paren:
            return parse_direct_abstract_decltor(ctx);
        case TOK_open_bracket:
            return parse_arr_abstract_decltor(ctx);
        default:
            THROW_AT_LINE(GET_PARSER_MSG(MSG_expect_abstract_decltor, get_tok_fmt(ctx->identifiers, ctx->peek_tok)),
                ctx->peek_tok->line);
    }
}

static std::shared_ptr<Type> parse_type_specifier(Ctx ctx);

static std::unique_ptr<CExp> parse_unary_exp_factor(Ctx ctx);
static std::unique_ptr<CExp> parse_cast_exp_factor(Ctx ctx);
static std::unique_ptr<CExp> parse_exp(Ctx ctx, int32_t min_precedence);

static void parse_decltor_cast_factor(Ctx ctx, std::shared_ptr<Type>& target_type) {
    AbstractDeclarator abstract_decltor;
    proc_abstract_decltor(parse_abstract_decltor(ctx).get(), std::move(target_type), abstract_decltor);
    target_type = std::move(abstract_decltor.derived_type);
}

// <type-name> ::= { <type-specifier> }+ [ <abstract-declarator> ]
static std::shared_ptr<Type> parse_type_name(Ctx ctx) {
    std::shared_ptr<Type> type_name = parse_type_specifier(ctx);
    switch (peek_next(ctx).tok_kind) {
        case TOK_binop_multiply:
        case TOK_open_paren:
        case TOK_open_bracket:
            parse_decltor_cast_factor(ctx, type_name);
        default:
            break;
    }
    return type_name;
}

// <argument-list> ::= <exp> { "," <exp> }
static std::vector<std::unique_ptr<CExp>> parse_arg_list(Ctx ctx) {
    std::vector<std::unique_ptr<CExp>> args;
    {
        std::unique_ptr<CExp> arg = parse_exp(ctx, 0);
        args.push_back(std::move(arg));
    }
    while (peek_next(ctx).tok_kind == TOK_comma_separator) {
        pop_next(ctx);
        std::unique_ptr<CExp> arg = parse_exp(ctx, 0);
        args.push_back(std::move(arg));
    }
    return args;
}

static std::unique_ptr<CConstant> parse_const_factor(Ctx ctx) {
    size_t line = ctx->peek_tok->line;
    std::shared_ptr<CConst> constant = parse_const(ctx);
    return std::make_unique<CConstant>(std::move(constant), line);
}

static std::unique_ptr<CConstant> parse_unsigned_const_factor(Ctx ctx) {
    size_t line = ctx->peek_tok->line;
    std::shared_ptr<CConst> constant = parse_unsigned_const(ctx);
    return std::make_unique<CConstant>(std::move(constant), line);
}

static std::unique_ptr<CString> parse_string_literal_factor(Ctx ctx) {
    size_t line = ctx->peek_tok->line;
    pop_next(ctx);
    std::shared_ptr<CStringLiteral> literal = parse_string_literal(ctx);
    return std::make_unique<CString>(std::move(literal), line);
}

static std::unique_ptr<CVar> parse_var_factor(Ctx ctx) {
    size_t line = ctx->peek_tok->line;
    TIdentifier name = parse_identifier(ctx, 0);
    return std::make_unique<CVar>(name, line);
}

static std::unique_ptr<CFunctionCall> parse_call_factor(Ctx ctx) {
    size_t line = ctx->peek_tok->line;
    TIdentifier name = parse_identifier(ctx, 0);
    pop_next(ctx);
    std::vector<std::unique_ptr<CExp>> args;
    if (peek_next(ctx).tok_kind != TOK_close_paren) {
        args = parse_arg_list(ctx);
    }
    expect_next(ctx, pop_next(ctx), TOK_close_paren);
    return std::make_unique<CFunctionCall>(name, std::move(args), line);
}

static std::unique_ptr<CExp> parse_inner_exp_factor(Ctx ctx) {
    pop_next(ctx);
    std::unique_ptr<CExp> inner_exp = parse_exp(ctx, 0);
    expect_next(ctx, pop_next(ctx), TOK_close_paren);
    return inner_exp;
}

static std::unique_ptr<CSubscript> parse_subscript_factor(Ctx ctx, std::unique_ptr<CExp>&& primary_exp) {
    size_t line = ctx->peek_tok->line;
    pop_next(ctx);
    std::unique_ptr<CExp> subscript_exp = parse_exp(ctx, 0);
    expect_next(ctx, pop_next(ctx), TOK_close_bracket);
    return std::make_unique<CSubscript>(std::move(primary_exp), std::move(subscript_exp), line);
}

static std::unique_ptr<CDot> parse_dot_factor(Ctx ctx, std::unique_ptr<CExp>&& structure) {
    size_t line = ctx->peek_tok->line;
    pop_next(ctx);
    expect_next(ctx, peek_next(ctx), TOK_identifier);
    TIdentifier member = parse_identifier(ctx, 0);
    return std::make_unique<CDot>(member, std::move(structure), line);
}

static std::unique_ptr<CArrow> parse_arrow_factor(Ctx ctx, std::unique_ptr<CExp>&& pointer) {
    size_t line = ctx->peek_tok->line;
    pop_next(ctx);
    expect_next(ctx, peek_next(ctx), TOK_identifier);
    TIdentifier member = parse_identifier(ctx, 0);
    return std::make_unique<CArrow>(member, std::move(pointer), line);
}

static std::unique_ptr<CAssignment> parse_postfix_incr_factor(Ctx ctx, std::unique_ptr<CExp>&& exp_left) {
    size_t line = ctx->peek_tok->line;
    std::unique_ptr<CUnaryOp> unop = std::make_unique<CPostfix>();
    std::unique_ptr<CExp> exp_left_1;
    std::unique_ptr<CExp> exp_right_1;
    {
        std::unique_ptr<CBinaryOp> binop = parse_binop(ctx);
        std::unique_ptr<CExp> exp_right;
        {
            std::shared_ptr<CConst> constant = std::make_shared<CConstInt>(1);
            exp_right = std::make_unique<CConstant>(std::move(constant), line);
        }
        exp_right_1 = std::make_unique<CBinary>(std::move(binop), std::move(exp_left), std::move(exp_right), line);
    }
    return std::make_unique<CAssignment>(std::move(unop), std::move(exp_left_1), std::move(exp_right_1), line);
}

static std::unique_ptr<CUnary> parse_unary_factor(Ctx ctx) {
    size_t line = ctx->peek_tok->line;
    std::unique_ptr<CUnaryOp> unop = parse_unop(ctx);
    std::unique_ptr<CExp> exp = parse_cast_exp_factor(ctx);
    return std::make_unique<CUnary>(std::move(unop), std::move(exp), line);
}

static std::unique_ptr<CAssignment> parse_incr_unary_factor(Ctx ctx) {
    size_t line = ctx->peek_tok->line;
    std::unique_ptr<CUnaryOp> unop = std::make_unique<CPrefix>();
    std::unique_ptr<CExp> exp_left_1;
    std::unique_ptr<CExp> exp_right_1;
    {
        std::unique_ptr<CBinaryOp> binop = parse_binop(ctx);
        std::unique_ptr<CExp> exp_left = parse_cast_exp_factor(ctx);
        std::unique_ptr<CExp> exp_right;
        {
            std::shared_ptr<CConst> constant = std::make_shared<CConstInt>(1);
            exp_right = std::make_unique<CConstant>(std::move(constant), line);
        }
        exp_right_1 = std::make_unique<CBinary>(std::move(binop), std::move(exp_left), std::move(exp_right), line);
    }
    return std::make_unique<CAssignment>(std::move(unop), std::move(exp_left_1), std::move(exp_right_1), line);
}

static std::unique_ptr<CDereference> parse_deref_factor(Ctx ctx) {
    size_t line = ctx->next_tok->line;
    std::unique_ptr<CExp> exp = parse_cast_exp_factor(ctx);
    return std::make_unique<CDereference>(std::move(exp), line);
}

static std::unique_ptr<CAddrOf> parse_addrof_factor(Ctx ctx) {
    size_t line = ctx->next_tok->line;
    std::unique_ptr<CExp> exp = parse_cast_exp_factor(ctx);
    return std::make_unique<CAddrOf>(std::move(exp), line);
}

static std::unique_ptr<CExp> parse_ptr_unary_factor(Ctx ctx) {
    switch (pop_next(ctx).tok_kind) {
        case TOK_binop_multiply:
            return parse_deref_factor(ctx);
        case TOK_binop_bitand:
            return parse_addrof_factor(ctx);
        default:
            THROW_AT_LINE(GET_PARSER_MSG(MSG_expect_ptr_unary_factor, get_tok_fmt(ctx->identifiers, ctx->next_tok)),
                ctx->next_tok->line);
    }
}

static std::unique_ptr<CSizeOfT> parse_sizeoft_factor(Ctx ctx) {
    size_t line = ctx->peek_tok->line;
    pop_next(ctx);
    std::shared_ptr<Type> target_type = parse_type_name(ctx);
    expect_next(ctx, pop_next(ctx), TOK_close_paren);
    return std::make_unique<CSizeOfT>(std::move(target_type), line);
}

static std::unique_ptr<CSizeOf> parse_sizeof_factor(Ctx ctx) {
    size_t line = ctx->peek_tok->line;
    std::unique_ptr<CExp> exp = parse_unary_exp_factor(ctx);
    return std::make_unique<CSizeOf>(std::move(exp), line);
}

static std::unique_ptr<CExp> parse_sizeof_unary_factor(Ctx ctx) {
    pop_next(ctx);
    if (peek_next(ctx).tok_kind == TOK_open_paren) {
        switch (peek_next_i(ctx, 1).tok_kind) {
            case TOK_key_char:
            case TOK_key_int:
            case TOK_key_long:
            case TOK_key_double:
            case TOK_key_unsigned:
            case TOK_key_signed:
            case TOK_key_void:
            case TOK_key_struct:
            case TOK_key_union:
                return parse_sizeoft_factor(ctx);
            default:
                break;
        }
    }
    return parse_sizeof_factor(ctx);
}

static std::unique_ptr<CCast> parse_cast_factor(Ctx ctx) {
    size_t line = ctx->peek_tok->line;
    pop_next(ctx);
    std::shared_ptr<Type> target_type = parse_type_name(ctx);
    expect_next(ctx, pop_next(ctx), TOK_close_paren);
    std::unique_ptr<CExp> exp = parse_cast_exp_factor(ctx);
    return std::make_unique<CCast>(std::move(exp), std::move(target_type), line);
}

// <primary-exp> ::= <const> | <identifier> | "(" <exp> ")" | { <string> }+ | <identifier> "(" [ <argument-list> ] ")"
static std::unique_ptr<CExp> parse_primary_exp_factor(Ctx ctx) {
    switch (peek_next(ctx).tok_kind) {
        case TOK_int_const:
        case TOK_long_const:
        case TOK_char_const:
        case TOK_dbl_const:
            return parse_const_factor(ctx);
        case TOK_uint_const:
        case TOK_ulong_const:
            return parse_unsigned_const_factor(ctx);
        case TOK_identifier: {
            if (peek_next_i(ctx, 1).tok_kind == TOK_open_paren) {
                return parse_call_factor(ctx);
            }
            return parse_var_factor(ctx);
        }
        case TOK_string_literal:
            return parse_string_literal_factor(ctx);
        case TOK_open_paren:
            return parse_inner_exp_factor(ctx);
        default:
            THROW_AT_LINE(GET_PARSER_MSG(MSG_expect_primary_exp_factor, get_tok_fmt(ctx->identifiers, ctx->peek_tok)),
                ctx->peek_tok->line);
    }
}

// <postfix-op> ::= "[" <exp> "]" | "." <identifier> | "->" <identifier>
static std::unique_ptr<CExp> parse_postfix_op_exp_factor(Ctx ctx, std::unique_ptr<CExp>&& primary_exp) {
    std::unique_ptr<CExp> postfix_exp = std::move(primary_exp);
    switch (peek_next(ctx).tok_kind) {
        case TOK_open_bracket: {
            postfix_exp = parse_subscript_factor(ctx, std::move(postfix_exp));
            break;
        }
        case TOK_structop_member: {
            postfix_exp = parse_dot_factor(ctx, std::move(postfix_exp));
            break;
        }
        case TOK_structop_ptr: {
            postfix_exp = parse_arrow_factor(ctx, std::move(postfix_exp));
            break;
        }
        case TOK_unop_incr:
        case TOK_unop_decr: {
            postfix_exp = parse_postfix_incr_factor(ctx, std::move(postfix_exp));
            break;
        }
        default:
            return postfix_exp;
    }
    return parse_postfix_op_exp_factor(ctx, std::move(postfix_exp));
}

// <postfix-exp> ::= <primary-exp> { <postfix-op> }
static std::unique_ptr<CExp> parse_postfix_exp_factor(Ctx ctx) {
    std::unique_ptr<CExp> primary_exp = parse_primary_exp_factor(ctx);
    switch (peek_next(ctx).tok_kind) {
        case TOK_open_bracket:
        case TOK_structop_member:
        case TOK_structop_ptr:
        case TOK_unop_incr:
        case TOK_unop_decr:
            return parse_postfix_op_exp_factor(ctx, std::move(primary_exp));
        default:
            return primary_exp;
    }
}

//<unary-exp> ::= <unop> <cast-exp> | "sizeof" <unary-exp> | "sizeof" "(" <type-name> ")" | <postfix-exp>
static std::unique_ptr<CExp> parse_unary_exp_factor(Ctx ctx) {
    switch (peek_next(ctx).tok_kind) {
        case TOK_unop_complement:
        case TOK_unop_neg:
        case TOK_unop_not:
            return parse_unary_factor(ctx);
        case TOK_unop_incr:
        case TOK_unop_decr:
            return parse_incr_unary_factor(ctx);
        case TOK_binop_multiply:
        case TOK_binop_bitand:
            return parse_ptr_unary_factor(ctx);
        case TOK_key_sizeof:
            return parse_sizeof_unary_factor(ctx);
        default:
            return parse_postfix_exp_factor(ctx);
    }
}

// <cast-exp> ::= "(" <type-name> ")" <cast-exp> | <unary-exp>
static std::unique_ptr<CExp> parse_cast_exp_factor(Ctx ctx) {
    if (peek_next(ctx).tok_kind == TOK_open_paren) {
        switch (peek_next_i(ctx, 1).tok_kind) {
            case TOK_key_char:
            case TOK_key_int:
            case TOK_key_long:
            case TOK_key_double:
            case TOK_key_unsigned:
            case TOK_key_signed:
            case TOK_key_void:
            case TOK_key_struct:
            case TOK_key_union:
                return parse_cast_factor(ctx);
            default:
                break;
        }
    }
    return parse_unary_exp_factor(ctx);
}

static std::unique_ptr<CAssignment> parse_assign_exp(Ctx ctx, std::unique_ptr<CExp>&& exp_left, int32_t precedence) {
    size_t line = ctx->peek_tok->line;
    pop_next(ctx);
    std::unique_ptr<CUnaryOp> unop;
    std::unique_ptr<CExp> exp_right = parse_exp(ctx, precedence);
    return std::make_unique<CAssignment>(std::move(unop), std::move(exp_left), std::move(exp_right), line);
}

static std::unique_ptr<CAssignment> parse_assign_compound_exp(
    Ctx ctx, std::unique_ptr<CExp>&& exp_left, int32_t precedence) {
    size_t line = ctx->peek_tok->line;
    std::unique_ptr<CUnaryOp> unop;
    std::unique_ptr<CExp> exp_left_1;
    std::unique_ptr<CExp> exp_right_1;
    {
        std::unique_ptr<CBinaryOp> binop = parse_binop(ctx);
        std::unique_ptr<CExp> exp_right = parse_exp(ctx, precedence);
        exp_right_1 = std::make_unique<CBinary>(std::move(binop), std::move(exp_left), std::move(exp_right), line);
    }
    return std::make_unique<CAssignment>(std::move(unop), std::move(exp_left_1), std::move(exp_right_1), line);
}

static std::unique_ptr<CBinary> parse_binary_exp(Ctx ctx, std::unique_ptr<CExp>&& exp_left, int32_t precedence) {
    size_t line = ctx->peek_tok->line;
    std::unique_ptr<CBinaryOp> binop = parse_binop(ctx);
    std::unique_ptr<CExp> exp_right = parse_exp(ctx, precedence + 1);
    return std::make_unique<CBinary>(std::move(binop), std::move(exp_left), std::move(exp_right), line);
}

static std::unique_ptr<CConditional> parse_ternary_exp(Ctx ctx, std::unique_ptr<CExp>&& exp_left, int32_t precedence) {
    size_t line = ctx->peek_tok->line;
    pop_next(ctx);
    std::unique_ptr<CExp> exp_middle = parse_exp(ctx, 0);
    expect_next(ctx, pop_next(ctx), TOK_ternary_else);
    std::unique_ptr<CExp> exp_right = parse_exp(ctx, precedence);
    return std::make_unique<CConditional>(std::move(exp_left), std::move(exp_middle), std::move(exp_right), line);
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
static std::unique_ptr<CExp> parse_exp(Ctx ctx, int32_t min_precedence) {
    std::unique_ptr<CExp> exp_left = parse_cast_exp_factor(ctx);
    while (true) {
        int32_t precedence = get_tok_precedence(peek_next(ctx).tok_kind);
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
                exp_left = parse_binary_exp(ctx, std::move(exp_left), precedence);
                break;
            case TOK_assign:
                exp_left = parse_assign_exp(ctx, std::move(exp_left), precedence);
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
                exp_left = parse_assign_compound_exp(ctx, std::move(exp_left), precedence);
                break;
            case TOK_ternary_if:
                exp_left = parse_ternary_exp(ctx, std::move(exp_left), precedence);
                break;
            default:
                THROW_AT_LINE(
                    GET_PARSER_MSG(MSG_expect_exp, get_tok_fmt(ctx->identifiers, ctx->peek_tok)), ctx->peek_tok->line);
        }
    }
    return exp_left;
}

static std::unique_ptr<CForInit> parse_for_init(Ctx ctx);
static std::unique_ptr<CBlock> parse_block(Ctx ctx);
static std::unique_ptr<CStatement> parse_statement(Ctx ctx);

static std::unique_ptr<CReturn> parse_ret_statement(Ctx ctx) {
    size_t line = ctx->peek_tok->line;
    pop_next(ctx);
    std::unique_ptr<CExp> exp;
    if (peek_next(ctx).tok_kind != TOK_semicolon) {
        exp = parse_exp(ctx, 0);
    }
    expect_next(ctx, pop_next(ctx), TOK_semicolon);
    return std::make_unique<CReturn>(std::move(exp), line);
}

static std::unique_ptr<CExpression> parse_exp_statement(Ctx ctx) {
    std::unique_ptr<CExp> exp = parse_exp(ctx, 0);
    expect_next(ctx, pop_next(ctx), TOK_semicolon);
    return std::make_unique<CExpression>(std::move(exp));
}

static std::unique_ptr<CIf> parse_if_statement(Ctx ctx) {
    pop_next(ctx);
    expect_next(ctx, pop_next(ctx), TOK_open_paren);
    std::unique_ptr<CExp> condition = parse_exp(ctx, 0);
    expect_next(ctx, pop_next(ctx), TOK_close_paren);
    peek_next(ctx);
    std::unique_ptr<CStatement> then = parse_statement(ctx);
    std::unique_ptr<CStatement> else_fi;
    if (peek_next(ctx).tok_kind == TOK_key_else) {
        pop_next(ctx);
        peek_next(ctx);
        else_fi = parse_statement(ctx);
    }
    return std::make_unique<CIf>(std::move(condition), std::move(then), std::move(else_fi));
}

static std::unique_ptr<CGoto> parse_goto_statement(Ctx ctx) {
    size_t line = ctx->peek_tok->line;
    pop_next(ctx);
    expect_next(ctx, peek_next(ctx), TOK_identifier);
    TIdentifier target = parse_identifier(ctx, 0);
    expect_next(ctx, pop_next(ctx), TOK_semicolon);
    return std::make_unique<CGoto>(target, line);
}

static std::unique_ptr<CLabel> parse_label_statement(Ctx ctx) {
    size_t line = ctx->peek_tok->line;
    TIdentifier target = parse_identifier(ctx, 0);
    pop_next(ctx);
    peek_next(ctx);
    std::unique_ptr<CStatement> jump_to = parse_statement(ctx);
    return std::make_unique<CLabel>(target, std::move(jump_to), line);
}

static std::unique_ptr<CCompound> parse_compound_statement(Ctx ctx) {
    std::unique_ptr<CBlock> block = parse_block(ctx);
    return std::make_unique<CCompound>(std::move(block));
}

static std::unique_ptr<CWhile> parse_while_statement(Ctx ctx) {
    pop_next(ctx);
    expect_next(ctx, pop_next(ctx), TOK_open_paren);
    std::unique_ptr<CExp> condition = parse_exp(ctx, 0);
    expect_next(ctx, pop_next(ctx), TOK_close_paren);
    peek_next(ctx);
    std::unique_ptr<CStatement> body = parse_statement(ctx);
    return std::make_unique<CWhile>(std::move(condition), std::move(body));
}

static std::unique_ptr<CDoWhile> parse_do_while_statement(Ctx ctx) {
    pop_next(ctx);
    peek_next(ctx);
    std::unique_ptr<CStatement> body = parse_statement(ctx);
    expect_next(ctx, pop_next(ctx), TOK_key_while);
    expect_next(ctx, pop_next(ctx), TOK_open_paren);
    std::unique_ptr<CExp> condition = parse_exp(ctx, 0);
    expect_next(ctx, pop_next(ctx), TOK_close_paren);
    expect_next(ctx, pop_next(ctx), TOK_semicolon);
    return std::make_unique<CDoWhile>(std::move(condition), std::move(body));
}

static std::unique_ptr<CFor> parse_for_statement(Ctx ctx) {
    pop_next(ctx);
    expect_next(ctx, pop_next(ctx), TOK_open_paren);
    std::unique_ptr<CForInit> init = parse_for_init(ctx);
    std::unique_ptr<CExp> condition;
    if (peek_next(ctx).tok_kind != TOK_semicolon) {
        condition = parse_exp(ctx, 0);
    }
    expect_next(ctx, pop_next(ctx), TOK_semicolon);
    std::unique_ptr<CExp> post;
    if (peek_next(ctx).tok_kind != TOK_close_paren) {
        post = parse_exp(ctx, 0);
    }
    expect_next(ctx, pop_next(ctx), TOK_close_paren);
    peek_next(ctx);
    std::unique_ptr<CStatement> body = parse_statement(ctx);
    return std::make_unique<CFor>(std::move(init), std::move(condition), std::move(post), std::move(body));
}

static std::unique_ptr<CSwitch> parse_switch_statement(Ctx ctx) {
    pop_next(ctx);
    expect_next(ctx, pop_next(ctx), TOK_open_paren);
    std::unique_ptr<CExp> match = parse_exp(ctx, 0);
    expect_next(ctx, pop_next(ctx), TOK_close_paren);
    peek_next(ctx);
    std::unique_ptr<CStatement> body = parse_statement(ctx);
    return std::make_unique<CSwitch>(std::move(match), std::move(body));
}

static std::unique_ptr<CCase> parse_case_statement(Ctx ctx) {
    pop_next(ctx);
    std::unique_ptr<CExp> value;
    {
        size_t line = ctx->peek_tok->line;
        std::shared_ptr<CConst> constant;
        switch (peek_next(ctx).tok_kind) {
            case TOK_int_const:
            case TOK_long_const:
            case TOK_char_const:
                constant = parse_const(ctx);
                break;
            case TOK_uint_const:
            case TOK_ulong_const:
                constant = parse_unsigned_const(ctx);
                break;
            default:
                THROW_AT_LINE(
                    GET_PARSER_MSG(MSG_case_value_not_int_const, get_tok_fmt(ctx->identifiers, ctx->peek_tok)),
                    ctx->peek_tok->line);
        }
        value = std::make_unique<CConstant>(std::move(constant), line);
    }
    expect_next(ctx, pop_next(ctx), TOK_ternary_else);
    peek_next(ctx);
    std::unique_ptr<CStatement> jump_to = parse_statement(ctx);
    return std::make_unique<CCase>(std::move(value), std::move(jump_to));
}

static std::unique_ptr<CDefault> parse_default_statement(Ctx ctx) {
    size_t line = ctx->peek_tok->line;
    pop_next(ctx);
    expect_next(ctx, pop_next(ctx), TOK_ternary_else);
    peek_next(ctx);
    std::unique_ptr<CStatement> jump_to = parse_statement(ctx);
    return std::make_unique<CDefault>(std::move(jump_to), line);
}

static std::unique_ptr<CBreak> parse_break_statement(Ctx ctx) {
    size_t line = ctx->peek_tok->line;
    pop_next(ctx);
    expect_next(ctx, pop_next(ctx), TOK_semicolon);
    return std::make_unique<CBreak>(line);
}

static std::unique_ptr<CContinue> parse_continue_statement(Ctx ctx) {
    size_t line = ctx->peek_tok->line;
    pop_next(ctx);
    expect_next(ctx, pop_next(ctx), TOK_semicolon);
    return std::make_unique<CContinue>(line);
}

static std::unique_ptr<CNull> parse_null_statement(Ctx ctx) {
    pop_next(ctx);
    return std::make_unique<CNull>();
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
static std::unique_ptr<CStatement> parse_statement(Ctx ctx) {
    switch (ctx->peek_tok->tok_kind) {
        case TOK_key_return:
            return parse_ret_statement(ctx);
        case TOK_key_if:
            return parse_if_statement(ctx);
        case TOK_key_goto:
            return parse_goto_statement(ctx);
        case TOK_identifier: {
            if (peek_next_i(ctx, 1).tok_kind == TOK_ternary_else) {
                return parse_label_statement(ctx);
            }
            break;
        }
        case TOK_open_brace:
            return parse_compound_statement(ctx);
        case TOK_key_while:
            return parse_while_statement(ctx);
        case TOK_key_do:
            return parse_do_while_statement(ctx);
        case TOK_key_for:
            return parse_for_statement(ctx);
        case TOK_key_switch:
            return parse_switch_statement(ctx);
        case TOK_key_case:
            return parse_case_statement(ctx);
        case TOK_key_default:
            return parse_default_statement(ctx);
        case TOK_key_break:
            return parse_break_statement(ctx);
        case TOK_key_continue:
            return parse_continue_statement(ctx);
        case TOK_semicolon:
            return parse_null_statement(ctx);
        default:
            break;
    }
    return parse_exp_statement(ctx);
}

static std::unique_ptr<CStorageClass> parse_decltor_decl(Ctx ctx, Declarator& decltor);
static std::unique_ptr<CVariableDeclaration> parse_var_declaration(
    Ctx ctx, std::unique_ptr<CStorageClass>&& storage_class, Declarator&& decltor);

static std::unique_ptr<CInitDecl> parse_for_init_decl(Ctx ctx) {
    Declarator decltor;
    std::unique_ptr<CStorageClass> storage_class = parse_decltor_decl(ctx, decltor);
    if (decltor.derived_type->type() == AST_FunType_t) {
        THROW_AT_LINE(GET_PARSER_MSG(MSG_for_init_decl_as_fun, ctx->identifiers->hash_table[decltor.name].c_str()),
            ctx->next_tok->line);
    }
    std::unique_ptr<CVariableDeclaration> init =
        parse_var_declaration(ctx, std::move(storage_class), std::move(decltor));
    return std::make_unique<CInitDecl>(std::move(init));
}

static std::unique_ptr<CInitExp> parse_for_init_exp(Ctx ctx) {
    std::unique_ptr<CExp> init;
    if (peek_next(ctx).tok_kind != TOK_semicolon) {
        init = parse_exp(ctx, 0);
    }
    expect_next(ctx, pop_next(ctx), TOK_semicolon);
    return std::make_unique<CInitExp>(std::move(init));
}

// <for-init> ::= <variable-declaration> | [ <exp> ] ";"
// for_init = InitDecl(variable_declaration) | InitExp(exp?)
static std::unique_ptr<CForInit> parse_for_init(Ctx ctx) {
    switch (peek_next(ctx).tok_kind) {
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
            return parse_for_init_decl(ctx);
        default:
            return parse_for_init_exp(ctx);
    }
}

static std::unique_ptr<CDeclaration> parse_declaration(Ctx ctx);

static std::unique_ptr<CS> parse_s_block_item(Ctx ctx) {
    std::unique_ptr<CStatement> statement = parse_statement(ctx);
    return std::make_unique<CS>(std::move(statement));
}

static std::unique_ptr<CD> parse_d_block_item(Ctx ctx) {
    std::unique_ptr<CDeclaration> declaration = parse_declaration(ctx);
    return std::make_unique<CD>(std::move(declaration));
}

// <block-item> ::= <statement> | <declaration>
// block_item = S(statement) | D(declaration)
static std::unique_ptr<CBlockItem> parse_block_item(Ctx ctx) {
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
            return parse_d_block_item(ctx);
        default:
            return parse_s_block_item(ctx);
    }
}

static std::unique_ptr<CB> parse_b_block(Ctx ctx) {
    std::vector<std::unique_ptr<CBlockItem>> block_items;
    while (peek_next(ctx).tok_kind != TOK_close_brace) {
        std::unique_ptr<CBlockItem> block_item = parse_block_item(ctx);
        block_items.push_back(std::move(block_item));
    }
    return std::make_unique<CB>(std::move(block_items));
}

// <block> ::= "{" { <block-item> } "}"
// block = B(block_item*)
static std::unique_ptr<CBlock> parse_block(Ctx ctx) {
    pop_next(ctx);
    std::unique_ptr<CBlock> block = parse_b_block(ctx);
    expect_next(ctx, pop_next(ctx), TOK_close_brace);
    return block;
}

// <type-specifier> ::= "int" | "long" | "signed" | "unsigned" | "double" | "char" | "void"
//                    | ("struct" | "union") <identifier>
static std::shared_ptr<Type> parse_type_specifier(Ctx ctx) {
    size_t i = 0;
    size_t line = peek_next(ctx).line;
    std::vector<TOKEN_KIND> type_tok_kinds;
    while (true) {
        switch (peek_next_i(ctx, i).tok_kind) {
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
                type_tok_kinds.push_back(pop_next_i(ctx, i).tok_kind);
                break;
            case TOK_key_struct:
            case TOK_key_union: {
                type_tok_kinds.push_back(pop_next_i(ctx, i).tok_kind);
                expect_next(ctx, peek_next_i(ctx, i), TOK_identifier);
                break;
            }
            case TOK_key_static:
            case TOK_key_extern:
            case TOK_binop_multiply:
            case TOK_open_paren:
                i++;
                break;
            case TOK_open_bracket: {
                i++;
                while (peek_next_i(ctx, i).tok_kind != TOK_close_bracket) {
                    i++;
                }
                i++;
                break;
            }
            default:
                THROW_AT_LINE(GET_PARSER_MSG(MSG_expect_specifier, get_tok_fmt(ctx->identifiers, &peek_next_i(ctx, i))),
                    peek_next_i(ctx, i).line);
        }
    }
Lbreak:
    switch (type_tok_kinds.size()) {
        case 1: {
            switch (type_tok_kinds[0]) {
                case TOK_key_char:
                    return std::make_shared<Char>();
                case TOK_key_int:
                    return std::make_shared<Int>();
                case TOK_key_long:
                    return std::make_shared<Long>();
                case TOK_key_double:
                    return std::make_shared<Double>();
                case TOK_key_unsigned:
                    return std::make_shared<UInt>();
                case TOK_key_signed:
                    return std::make_shared<Int>();
                case TOK_key_void:
                    return std::make_shared<Void>();
                case TOK_key_struct: {
                    TIdentifier tag = parse_identifier(ctx, i);
                    return std::make_shared<Structure>(tag, false);
                }
                case TOK_key_union: {
                    TIdentifier tag = parse_identifier(ctx, i);
                    return std::make_shared<Structure>(tag, true);
                }
                default:
                    break;
            }
            break;
        }
        case 2: {
            if (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_unsigned) != type_tok_kinds.end()) {
                if (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_int) != type_tok_kinds.end()) {
                    return std::make_shared<UInt>();
                }
                else if (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_long)
                         != type_tok_kinds.end()) {
                    return std::make_shared<ULong>();
                }
                else if (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_char)
                         != type_tok_kinds.end()) {
                    return std::make_shared<UChar>();
                }
            }
            else if (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_signed) != type_tok_kinds.end()) {
                if (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_int) != type_tok_kinds.end()) {
                    return std::make_shared<Int>();
                }
                else if (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_long)
                         != type_tok_kinds.end()) {
                    return std::make_shared<Long>();
                }
                else if (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_char)
                         != type_tok_kinds.end()) {
                    return std::make_shared<SChar>();
                }
            }
            else if ((std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_int) != type_tok_kinds.end())
                     && (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_long)
                         != type_tok_kinds.end())) {
                return std::make_shared<Long>();
            }
            break;
        }
        case 3: {
            if ((std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_int) != type_tok_kinds.end())
                && (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_long) != type_tok_kinds.end())) {
                if (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_unsigned) != type_tok_kinds.end()) {
                    return std::make_shared<ULong>();
                }
                else if (std::find(type_tok_kinds.begin(), type_tok_kinds.end(), TOK_key_signed)
                         != type_tok_kinds.end()) {
                    return std::make_shared<Long>();
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
    THROW_AT_LINE(GET_PARSER_MSG(MSG_expect_specifier_list, type_tok_kinds_s.c_str()), line);
}

// <specifier> ::= <type-specifier> | "static" | "extern"
// storage_class = Static | Extern
static std::unique_ptr<CStorageClass> parse_storage_class(Ctx ctx) {
    switch (pop_next(ctx).tok_kind) {
        case TOK_key_static:
            return std::make_unique<CStatic>();
        case TOK_key_extern:
            return std::make_unique<CExtern>();
        default:
            THROW_AT_LINE(GET_PARSER_MSG(MSG_expect_storage_class, get_tok_fmt(ctx->identifiers, ctx->next_tok)),
                ctx->next_tok->line);
    }
}

static std::unique_ptr<CInitializer> parse_initializer(Ctx ctx);

static std::unique_ptr<CSingleInit> parse_single_init(Ctx ctx) {
    std::unique_ptr<CExp> exp = parse_exp(ctx, 0);
    return std::make_unique<CSingleInit>(std::move(exp));
}

static std::unique_ptr<CCompoundInit> parse_compound_init(Ctx ctx) {
    pop_next(ctx);
    std::vector<std::unique_ptr<CInitializer>> initializers;
    while (true) {
        if (peek_next(ctx).tok_kind == TOK_close_brace) {
            break;
        }
        std::unique_ptr<CInitializer> initializer = parse_initializer(ctx);
        initializers.push_back(std::move(initializer));
        if (peek_next(ctx).tok_kind == TOK_close_brace) {
            break;
        }
        expect_next(ctx, pop_next(ctx), TOK_comma_separator);
    }
    if (initializers.empty()) {
        THROW_AT_LINE(GET_PARSER_MSG_0(MSG_empty_compound_init), ctx->peek_tok->line);
    }
    pop_next(ctx);
    return std::make_unique<CCompoundInit>(std::move(initializers));
}

// <initializer> ::= <exp> | "{" <initializer> { "," <initializer> } [","] "}"
// initializer = SingleInit(exp) | CompoundInit(initializer*)
static std::unique_ptr<CInitializer> parse_initializer(Ctx ctx) {
    if (peek_next(ctx).tok_kind == TOK_open_brace) {
        return parse_compound_init(ctx);
    }
    else {
        return parse_single_init(ctx);
    }
}

static std::unique_ptr<CDeclarator> parse_decltor(Ctx ctx);
static void proc_decltor(Ctx ctx, CDeclarator* node, std::shared_ptr<Type>&& base_type, Declarator& decltor);

static void proc_ident_decltor(CIdent* node, std::shared_ptr<Type>&& base_type, Declarator& decltor) {
    decltor.name = node->name;
    decltor.derived_type = std::move(base_type);
}

static void proc_ptr_decltor(
    Ctx ctx, CPointerDeclarator* node, std::shared_ptr<Type>&& base_type, Declarator& decltor) {
    std::shared_ptr<Type> derived_type = std::make_shared<Pointer>(std::move(base_type));
    proc_decltor(ctx, node->decltor.get(), std::move(derived_type), decltor);
}

static void proc_arr_decltor(Ctx ctx, CArrayDeclarator* node, std::shared_ptr<Type>&& base_type, Declarator& decltor) {
    TLong size = node->size;
    std::shared_ptr<Type> derived_type = std::make_shared<Array>(size, std::move(base_type));
    proc_decltor(ctx, node->decltor.get(), std::move(derived_type), decltor);
}

static void proc_fun_decltor(Ctx ctx, CFunDeclarator* node, std::shared_ptr<Type>&& base_type, Declarator& decltor) {
    if (node->decltor->type() != AST_CIdent_t) {
        THROW_AT_LINE(GET_PARSER_MSG_0(MSG_derived_fun_decl), ctx->next_tok->line);
    }

    std::vector<TIdentifier> params;
    std::vector<std::shared_ptr<Type>> param_types;
    params.reserve(node->param_list.size());
    param_types.reserve(node->param_list.size());
    for (const auto& param : node->param_list) {
        Declarator param_decltor;
        std::shared_ptr<Type> param_type = param->param_type;
        proc_decltor(ctx, param->decltor.get(), std::move(param_type), param_decltor);
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

static void proc_decltor(Ctx ctx, CDeclarator* node, std::shared_ptr<Type>&& base_type, Declarator& decltor) {
    switch (node->type()) {
        case AST_CIdent_t:
            proc_ident_decltor(static_cast<CIdent*>(node), std::move(base_type), decltor);
            break;
        case AST_CPointerDeclarator_t:
            proc_ptr_decltor(ctx, static_cast<CPointerDeclarator*>(node), std::move(base_type), decltor);
            break;
        case AST_CArrayDeclarator_t:
            proc_arr_decltor(ctx, static_cast<CArrayDeclarator*>(node), std::move(base_type), decltor);
            break;
        case AST_CFunDeclarator_t:
            proc_fun_decltor(ctx, static_cast<CFunDeclarator*>(node), std::move(base_type), decltor);
            break;
        default:
            THROW_ABORT;
    }
}

static std::unique_ptr<CIdent> parse_ident_decltor(Ctx ctx) {
    TIdentifier name = parse_identifier(ctx, 0);
    return std::make_unique<CIdent>(name);
}

static std::unique_ptr<CDeclarator> parse_simple_decltor(Ctx ctx) {
    pop_next(ctx);
    std::unique_ptr<CDeclarator> decltor = parse_decltor(ctx);
    expect_next(ctx, pop_next(ctx), TOK_close_paren);
    return decltor;
}

// <simple-declarator> ::= <identifier> | "(" <declarator> ")"
static std::unique_ptr<CDeclarator> parse_simple_decltor_decl(Ctx ctx) {
    switch (peek_next(ctx).tok_kind) {
        case TOK_identifier:
            return parse_ident_decltor(ctx);
        case TOK_open_paren:
            return parse_simple_decltor(ctx);
        default:
            THROW_AT_LINE(GET_PARSER_MSG(MSG_expect_simple_decltor, get_tok_fmt(ctx->identifiers, ctx->peek_tok)),
                ctx->peek_tok->line);
    }
}

// <param> ::= { <type-specifier> }+ <declarator>
// param_info = Param(type, declarator)
static std::unique_ptr<CParam> parse_param(Ctx ctx) {
    std::shared_ptr<Type> param_type = parse_type_specifier(ctx);
    std::unique_ptr<CDeclarator> decltor = parse_decltor(ctx);
    return std::make_unique<CParam>(std::move(decltor), std::move(param_type));
}

static void parse_empty_param_list(Ctx ctx) { pop_next(ctx); }

static std::vector<std::unique_ptr<CParam>> parse_non_empty_param_list(Ctx ctx) {
    std::vector<std::unique_ptr<CParam>> param_list;
    {
        std::unique_ptr<CParam> param = parse_param(ctx);
        param_list.push_back(std::move(param));
    }
    while (peek_next(ctx).tok_kind == TOK_comma_separator) {
        pop_next(ctx);
        std::unique_ptr<CParam> param = parse_param(ctx);
        param_list.push_back(std::move(param));
    }
    return param_list;
}

// <param-list> ::= "(" "void" ")" | "(" <param> { "," <param> } ")"
static std::vector<std::unique_ptr<CParam>> parse_param_list(Ctx ctx) {
    pop_next(ctx);
    std::vector<std::unique_ptr<CParam>> param_list;
    switch (peek_next(ctx).tok_kind) {
        case TOK_key_void: {
            if (peek_next_i(ctx, 1).tok_kind == TOK_close_paren) {
                parse_empty_param_list(ctx);
            }
            else {
                param_list = parse_non_empty_param_list(ctx);
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
        case TOK_key_union: {
            param_list = parse_non_empty_param_list(ctx);
            break;
        }
        default:
            THROW_AT_LINE(GET_PARSER_MSG(MSG_expect_param_list, get_tok_fmt(ctx->identifiers, ctx->peek_tok)),
                ctx->peek_tok->line);
    }
    expect_next(ctx, pop_next(ctx), TOK_close_paren);
    return param_list;
}

// (fun) <declarator-suffix> ::= <param-list>
static std::unique_ptr<CFunDeclarator> parse_fun_decltor_suffix(Ctx ctx, std::unique_ptr<CDeclarator>&& decltor) {
    std::vector<std::unique_ptr<CParam>> param_list = parse_param_list(ctx);
    return std::make_unique<CFunDeclarator>(std::move(param_list), std::move(decltor));
}

// (array) <declarator-suffix> ::= { "[" <const> "]" }+
static std::unique_ptr<CDeclarator> parse_arr_decltor_suffix(Ctx ctx, std::unique_ptr<CDeclarator>&& decltor) {
    do {
        TLong size = parse_arr_size(ctx);
        decltor = std::make_unique<CArrayDeclarator>(size, std::move(decltor));
    }
    while (peek_next(ctx).tok_kind == TOK_open_bracket);
    return std::move(decltor);
}

// <direct-declarator> ::= <simple-declarator> [ <declarator-suffix> ]
static std::unique_ptr<CDeclarator> parse_direct_decltor(Ctx ctx) {
    std::unique_ptr<CDeclarator> decltor = parse_simple_decltor_decl(ctx);
    switch (peek_next(ctx).tok_kind) {
        case TOK_open_paren: {
            return parse_fun_decltor_suffix(ctx, std::move(decltor));
        }
        case TOK_open_bracket: {
            return parse_arr_decltor_suffix(ctx, std::move(decltor));
        }
        default:
            return decltor;
    }
}

static std::unique_ptr<CPointerDeclarator> parse_ptr_decltor(Ctx ctx) {
    pop_next(ctx);
    std::unique_ptr<CDeclarator> decltor = parse_decltor(ctx);
    return std::make_unique<CPointerDeclarator>(std::move(decltor));
}

// <declarator> ::= "*" <declarator> | <direct-declarator>
// declarator = Ident(identifier) | PointerDeclarator(declarator) | ArrayDeclarator(int, declarator)
//            | FunDeclarator(param_info*, declarator)
static std::unique_ptr<CDeclarator> parse_decltor(Ctx ctx) {
    if (peek_next(ctx).tok_kind == TOK_binop_multiply) {
        return parse_ptr_decltor(ctx);
    }
    else {
        return parse_direct_decltor(ctx);
    }
}

// <function-declaration> ::= { <specifier> }+ <declarator> ( <block> | ";")
// function_declaration = FunctionDeclaration(identifier, identifier*, block?, type, storage_class?)
static std::unique_ptr<CFunctionDeclaration> parse_fun_declaration(
    Ctx ctx, std::unique_ptr<CStorageClass>&& storage_class, Declarator&& decltor) {
    size_t line = ctx->next_tok->line;
    std::unique_ptr<CBlock> body;
    if (peek_next(ctx).tok_kind == TOK_semicolon) {
        pop_next(ctx);
    }
    else {
        expect_next(ctx, peek_next(ctx), TOK_open_brace);
        body = parse_block(ctx);
    }
    return std::make_unique<CFunctionDeclaration>(decltor.name, std::move(decltor.params), std::move(body),
        std::move(decltor.derived_type), std::move(storage_class), line);
}

// <variable-declaration> ::= { <specifier> }+ <declarator> [ "=" <initializer> ] ";"
// variable_declaration = VariableDeclaration(identifier, initializer?, type, storage_class?)
static std::unique_ptr<CVariableDeclaration> parse_var_declaration(
    Ctx ctx, std::unique_ptr<CStorageClass>&& storage_class, Declarator&& decltor) {
    size_t line = ctx->next_tok->line;
    std::unique_ptr<CInitializer> init;
    if (peek_next(ctx).tok_kind == TOK_assign) {
        pop_next(ctx);
        init = parse_initializer(ctx);
    }
    expect_next(ctx, pop_next(ctx), TOK_semicolon);
    return std::make_unique<CVariableDeclaration>(
        decltor.name, std::move(init), std::move(decltor.derived_type), std::move(storage_class), line);
}

// <member-declaration> ::= { <type-specifier> }+ <declarator> ";"
// member_declaration = MemberDeclaration(identifier, type)
static std::unique_ptr<CMemberDeclaration> parse_member_decl(Ctx ctx) {
    Declarator decltor;
    {
        std::unique_ptr<CStorageClass> storage_class = parse_decltor_decl(ctx, decltor);
        if (storage_class) {
            THROW_AT_LINE(GET_PARSER_MSG(MSG_member_decl_not_auto, ctx->identifiers->hash_table[decltor.name].c_str(),
                              get_storage_class_fmt(storage_class.get())),
                ctx->next_tok->line);
        }
    }
    if (decltor.derived_type->type() == AST_FunType_t) {
        THROW_AT_LINE(GET_PARSER_MSG(MSG_member_decl_as_fun, ctx->identifiers->hash_table[decltor.name].c_str()),
            ctx->next_tok->line);
    }
    size_t line = ctx->next_tok->line;
    expect_next(ctx, pop_next(ctx), TOK_semicolon);
    return std::make_unique<CMemberDeclaration>(decltor.name, std::move(decltor.derived_type), line);
}

// <struct-declaration> ::= ("struct" | "union") <identifier> [ "{" { <member-declaration> }+ "}" ] ";"
// struct_declaration = StructDeclaration(identifier, bool, member_declaration*)
static std::unique_ptr<CStructDeclaration> parse_struct_decl(Ctx ctx) {
    size_t line = ctx->peek_tok->line;
    bool is_union = pop_next(ctx).tok_kind == TOK_key_union;
    expect_next(ctx, peek_next(ctx), TOK_identifier);
    TIdentifier tag = parse_identifier(ctx, 0);
    std::vector<std::unique_ptr<CMemberDeclaration>> members;
    if (pop_next(ctx).tok_kind == TOK_open_brace) {
        do {
            std::unique_ptr<CMemberDeclaration> member = parse_member_decl(ctx);
            members.push_back(std::move(member));
        }
        while (peek_next(ctx).tok_kind != TOK_close_brace);
        pop_next(ctx);
        pop_next(ctx);
    }
    expect_next(ctx, *ctx->next_tok, TOK_semicolon);
    return std::make_unique<CStructDeclaration>(tag, is_union, std::move(members), line);
}

static std::unique_ptr<CFunDecl> parse_fun_decl(
    Ctx ctx, std::unique_ptr<CStorageClass>&& storage_class, Declarator&& decltor) {
    std::unique_ptr<CFunctionDeclaration> fun_decl =
        parse_fun_declaration(ctx, std::move(storage_class), std::move(decltor));
    return std::make_unique<CFunDecl>(std::move(fun_decl));
}

static std::unique_ptr<CVarDecl> parse_var_decl(
    Ctx ctx, std::unique_ptr<CStorageClass>&& storage_class, Declarator&& decltor) {
    std::unique_ptr<CVariableDeclaration> var_decl =
        parse_var_declaration(ctx, std::move(storage_class), std::move(decltor));
    return std::make_unique<CVarDecl>(std::move(var_decl));
}

static std::unique_ptr<CStructDecl> parse_struct_declaration(Ctx ctx) {
    std::unique_ptr<CStructDeclaration> struct_decl = parse_struct_decl(ctx);
    return std::make_unique<CStructDecl>(std::move(struct_decl));
}

static std::unique_ptr<CStorageClass> parse_decltor_decl(Ctx ctx, Declarator& decltor) {
    std::shared_ptr<Type> type_specifier = parse_type_specifier(ctx);
    std::unique_ptr<CStorageClass> storage_class;
    switch (peek_next(ctx).tok_kind) {
        case TOK_identifier:
        case TOK_binop_multiply:
        case TOK_open_paren:
            break;
        default:
            storage_class = parse_storage_class(ctx);
    }
    std::unique_ptr<CDeclarator> decltor_1 = parse_decltor(ctx);
    proc_decltor(ctx, decltor_1.get(), std::move(type_specifier), decltor);
    return storage_class;
}

// <declaration> ::= <variable-declaration> | <function-declaration> | <struct-declaration>
// declaration = FunDecl(function_declaration) | VarDecl(variable_declaration) | StructDecl(struct_declaration)
static std::unique_ptr<CDeclaration> parse_declaration(Ctx ctx) {
    switch (peek_next(ctx).tok_kind) {
        case TOK_key_struct:
        case TOK_key_union: {
            switch (peek_next_i(ctx, 2).tok_kind) {
                case TOK_open_brace:
                case TOK_semicolon:
                    return parse_struct_declaration(ctx);
                default:
                    break;
            }
        }
        default:
            break;
    }
    Declarator decltor;
    std::unique_ptr<CStorageClass> storage_class = parse_decltor_decl(ctx, decltor);
    if (decltor.derived_type->type() == AST_FunType_t) {
        return parse_fun_decl(ctx, std::move(storage_class), std::move(decltor));
    }
    else {
        return parse_var_decl(ctx, std::move(storage_class), std::move(decltor));
    }
}

// <program> ::= { <declaration> }
// AST = Program(declaration*)
static std::unique_ptr<CProgram> parse_program(Ctx ctx) {
    std::vector<std::unique_ptr<CDeclaration>> declarations;
    while (ctx->pop_idx < ctx->p_toks->size()) {
        std::unique_ptr<CDeclaration> declaration = parse_declaration(ctx);
        declarations.push_back(std::move(declaration));
    }
    return std::make_unique<CProgram>(std::move(declarations));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<CProgram> parse_tokens(
    std::vector<Token>&& tokens, ErrorsContext* errors, IdentifierContext* identifiers) {
    ParserContext ctx;
    {
        ctx.errors = errors;
        ctx.identifiers = identifiers;
        ctx.pop_idx = 0;
        ctx.p_toks = &tokens;
    }
    std::unique_ptr<CProgram> c_ast = parse_program(&ctx);
    THROW_ABORT_IF(ctx.pop_idx != tokens.size());

    std::vector<Token>().swap(tokens);
    THROW_ABORT_IF(!c_ast);
    return c_ast;
}
