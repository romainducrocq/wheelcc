#include <inttypes.h>

#include "util/c_std.h"
#include "util/str2t.h"
#include "util/throw.h"

#include "ast/ast.h"
#include "ast/front_ast.h"
#include "ast/front_symt.h"

#include "frontend/parser/errors.h"
#include "frontend/parser/lexer.h"
#include "frontend/parser/parser.h"

#include "frontend/intermediate/idents.h"

typedef struct AbstractDeclarator {
    shared_ptr_t(Type) derived_type;
} AbstractDeclarator;

typedef struct Declarator {
    TIdentifier name;
    shared_ptr_t(Type) derived_type;
    vector_t(TIdentifier) params;
} Declarator;

typedef struct ParserContext {
    ErrorsContext* errors;
    IdentifierContext* identifiers;
    // Parser
    size_t pop_idx;
    Token* next_tok;
    Token* peek_tok;
    Token* next_tok_i;
    Token* peek_tok_i;
    vector_t(Token) * p_toks;
} ParserContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Parser

typedef ParserContext* Ctx;

static error_t expect_next(Ctx ctx, Token* next_tok, TOKEN_KIND expect_tok) {
    CATCH_ENTER;
    if (next_tok->tok_kind != expect_tok) {
        THROW_AT_LINE(next_tok->line,
            GET_PARSER_MSG(MSG_unexpected_next_tok, str_fmt_tok(next_tok), get_tok_kind_fmt(expect_tok)));
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t pop_next(Ctx ctx) {
    CATCH_ENTER;
    if (ctx->pop_idx >= vec_size(*ctx->p_toks)) {
        THROW_AT_LINE(vec_back(*ctx->p_toks).line, GET_PARSER_MSG_0(MSG_reached_eof));
    }

    ctx->next_tok = &(*ctx->p_toks)[ctx->pop_idx];
    ctx->pop_idx++;
    FINALLY;
    CATCH_EXIT;
}

static error_t pop_next_i(Ctx ctx, size_t i) {
    CATCH_ENTER;
    if (i == 0) {
        TRY(pop_next(ctx));
        ctx->next_tok_i = ctx->next_tok;
        EARLY_EXIT;
    }
    if (ctx->pop_idx + i >= vec_size(*ctx->p_toks)) {
        THROW_AT_LINE(vec_back(*ctx->p_toks).line, GET_PARSER_MSG_0(MSG_reached_eof));
    }
    {
        Token swap_token_i = (*ctx->p_toks)[ctx->pop_idx + i];
        for (size_t j = ctx->pop_idx + i; j-- > ctx->pop_idx;) {
            (*ctx->p_toks)[j + 1] = (*ctx->p_toks)[j];
        }
        (*ctx->p_toks)[ctx->pop_idx] = swap_token_i;
    }
    TRY(pop_next(ctx));
    ctx->next_tok_i = &(*ctx->p_toks)[ctx->pop_idx - 1];
    FINALLY;
    CATCH_EXIT;
}

static error_t peek_next(Ctx ctx) {
    CATCH_ENTER;
    if (ctx->pop_idx >= vec_size(*ctx->p_toks)) {
        THROW_AT_LINE(vec_back(*ctx->p_toks).line, GET_PARSER_MSG_0(MSG_reached_eof));
    }

    ctx->peek_tok = &(*ctx->p_toks)[ctx->pop_idx];
    FINALLY;
    CATCH_EXIT;
}

static error_t peek_next_i(Ctx ctx, size_t i) {
    CATCH_ENTER;
    if (i == 0) {
        TRY(peek_next(ctx));
        ctx->peek_tok_i = ctx->peek_tok;
        EARLY_EXIT;
    }
    if (ctx->pop_idx + i >= vec_size(*ctx->p_toks)) {
        THROW_AT_LINE(vec_back(*ctx->p_toks).line, GET_PARSER_MSG_0(MSG_reached_eof));
    }

    ctx->peek_tok_i = &(*ctx->p_toks)[ctx->pop_idx + i];
    FINALLY;
    CATCH_EXIT;
}

// <identifier> ::= ? An identifier token ?
static error_t parse_identifier(Ctx ctx, size_t i, TIdentifier* identifier) {
    CATCH_ENTER;
    TRY(pop_next_i(ctx, i));
    *identifier = ctx->next_tok_i->tok;
    FINALLY;
    CATCH_EXIT;
}

// string = StringLiteral(int*)
// <string> ::= ? A string token ?
static error_t parse_string_literal(Ctx ctx, shared_ptr_t(CStringLiteral) * literal) {
    vector_t(TChar) value = vec_new();
    CATCH_ENTER;
    string_to_literal(map_get(ctx->identifiers->hash_table, ctx->next_tok->tok), &value);
    TRY(peek_next(ctx));
    while (ctx->peek_tok->tok_kind == TOK_string_literal) {
        TRY(pop_next(ctx));
        string_to_literal(map_get(ctx->identifiers->hash_table, ctx->next_tok->tok), &value);
        TRY(peek_next(ctx));
    }
    *literal = make_CStringLiteral(&value);
    FINALLY;
    vec_delete(value);
    CATCH_EXIT;
}

// <int> ::= ? An int constant token ?
static shared_ptr_t(CConst) parse_int_const(intmax_t intmax) {
    TInt value = intmax_to_int32(intmax);
    return make_CConstInt(value);
}

// <char> ::= ? A char token ?
static shared_ptr_t(CConst) parse_char_const(Ctx ctx) {
    TInt value = string_to_char_ascii(map_get(ctx->identifiers->hash_table, ctx->next_tok->tok));
    return make_CConstInt(value);
}

// <long> ::= ? An int or long constant token ?
static shared_ptr_t(CConst) parse_long_const(intmax_t intmax) {
    TLong value = intmax_to_int64(intmax);
    return make_CConstLong(value);
}

// <double> ::= ? A floating-point constant token ?
static error_t parse_dbl_const(Ctx ctx, shared_ptr_t(CConst) * constant) {
    CATCH_ENTER;
    TDouble value;
    TRY(string_to_dbl(
        ctx->errors, map_get(ctx->identifiers->hash_table, ctx->next_tok->tok), ctx->next_tok->line, &value));
    *constant = make_CConstDouble(value);
    FINALLY;
    CATCH_EXIT;
}

// <unsigned int> ::= ? An unsigned int constant token ?
static shared_ptr_t(CConst) parse_uint_const(uintmax_t uintmax) {
    TUInt value = uintmax_to_uint32(uintmax);
    return make_CConstUInt(value);
}

// <unsigned long> ::= ? An unsigned int or unsigned long constant token ?
static shared_ptr_t(CConst) parse_ulong_const(uintmax_t uintmax) {
    TULong value = uintmax_to_uint64(uintmax);
    return make_CConstULong(value);
}

// <const> ::= <int> | <long> | <double> | <char>
// (signed) const = ConstInt(int) | ConstLong(long) | ConstDouble(double) | ConstChar(int)
static error_t parse_const(Ctx ctx, shared_ptr_t(CConst) * constant) {
    CATCH_ENTER;
    TRY(pop_next(ctx));
    switch (ctx->next_tok->tok_kind) {
        case TOK_char_const: {
            *constant = parse_char_const(ctx);
            EARLY_EXIT;
        }
        case TOK_dbl_const:
            TRY(parse_dbl_const(ctx, constant));
            EARLY_EXIT;
        default:
            break;
    }

    intmax_t value;
    TRY(string_to_intmax(
        ctx->errors, map_get(ctx->identifiers->hash_table, ctx->next_tok->tok), ctx->next_tok->line, &value));
    if (value > 9223372036854775807ll) {
        THROW_AT_LINE(ctx->next_tok->line,
            GET_PARSER_MSG(MSG_overflow_long_const, map_get(ctx->identifiers->hash_table, ctx->next_tok->tok)));
    }
    if (ctx->next_tok->tok_kind == TOK_int_const && value <= 2147483647l) {
        *constant = parse_int_const(value);
    }
    else {
        *constant = parse_long_const(value);
    }
    FINALLY;
    CATCH_EXIT;
}

// <const> ::= <unsigned int> | <unsigned long>
// (unsigned) const = ConstUInt(uint) | ConstULong(ulong) | ConstUChar(int)
static error_t parse_unsigned_const(Ctx ctx, shared_ptr_t(CConst) * constant) {
    CATCH_ENTER;
    TRY(pop_next(ctx));

    uintmax_t value;
    TRY(string_to_uintmax(
        ctx->errors, map_get(ctx->identifiers->hash_table, ctx->next_tok->tok), ctx->next_tok->line, &value));
    if (value > 18446744073709551615ull) {
        THROW_AT_LINE(ctx->next_tok->line,
            GET_PARSER_MSG(MSG_overflow_ulong_const, map_get(ctx->identifiers->hash_table, ctx->next_tok->tok)));
    }
    if (ctx->next_tok->tok_kind == TOK_uint_const && value <= 4294967295ul) {
        *constant = parse_uint_const(value);
    }
    else {
        *constant = parse_ulong_const(value);
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t parse_arr_size(Ctx ctx, TLong* size) {
    shared_ptr_t(CConst) constant = sptr_new();
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
            THROW_AT_LINE(ctx->peek_tok->line, GET_PARSER_MSG(MSG_arr_size_not_int_const, str_fmt_tok(ctx->peek_tok)));
    }
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_close_bracket));
    switch (constant->type) {
        case AST_CConstInt_t: {
            *size = (TLong)constant->get._CConstInt.value;
            break;
        }
        case AST_CConstLong_t: {
            *size = constant->get._CConstLong.value;
            break;
        }
        case AST_CConstUInt_t: {
            *size = (TLong)constant->get._CConstUInt.value;
            break;
        }
        case AST_CConstULong_t: {
            *size = (TLong)constant->get._CConstULong.value;
            break;
        }
        default:
            THROW_ABORT;
    }
    FINALLY;
    free_CConst(&constant);
    CATCH_EXIT;
}

// <unop> ::= "-" | "~" | "!" | "*" | "&" | "++" | "--"
// unary_operator = Complement | Negate | Not | Prefix | Postfix
static error_t parse_unop(Ctx ctx, unique_ptr_t(CUnaryOp) * unop) {
    CATCH_ENTER;
    TRY(pop_next(ctx));
    switch (ctx->next_tok->tok_kind) {
        case TOK_unop_complement: {
            *unop = make_CComplement();
            break;
        }
        case TOK_unop_neg: {
            *unop = make_CNegate();
            break;
        }
        case TOK_unop_not: {
            *unop = make_CNot();
            break;
        }
        default:
            THROW_AT_LINE(ctx->next_tok->line, GET_PARSER_MSG(MSG_expect_unop, str_fmt_tok(ctx->next_tok)));
    }
    FINALLY;
    CATCH_EXIT;
}

// <binop> ::= "-" | "+" | "*" | "/" | "%" | "&" | "|" | "^" | "<<" | ">>" | "&&" | "||" | "==" | "!=" | "<"
//           | "<=" | ">" | ">=" | "=" | "-=" | "+=" | "*=" | "/=" | "%=" | "&=" | "|=" | "^=" | "<<=" | ">>="
// binary_operator = Add | Subtract | Multiply | Divide | Remainder | BitAnd | BitOr | BitXor | BitShiftLeft
//                 | BitShiftRight | BitShrArithmetic | And | Or | Equal | NotEqual | LessThan | LessOrEqual
//                 | GreaterThan | GreaterOrEqual
static error_t parse_binop(Ctx ctx, unique_ptr_t(CBinaryOp) * binop) {
    CATCH_ENTER;
    TRY(pop_next(ctx));
    switch (ctx->next_tok->tok_kind) {
        case TOK_binop_add:
        case TOK_assign_add:
        case TOK_unop_incr: {
            *binop = make_CAdd();
            break;
        }
        case TOK_unop_neg:
        case TOK_assign_subtract:
        case TOK_unop_decr: {
            *binop = make_CSubtract();
            break;
        }
        case TOK_binop_multiply:
        case TOK_assign_multiply: {
            *binop = make_CMultiply();
            break;
        }
        case TOK_binop_divide:
        case TOK_assign_divide: {
            *binop = make_CDivide();
            break;
        }
        case TOK_binop_remainder:
        case TOK_assign_remainder: {
            *binop = make_CRemainder();
            break;
        }
        case TOK_binop_bitand:
        case TOK_assign_bitand: {
            *binop = make_CBitAnd();
            break;
        }
        case TOK_binop_bitor:
        case TOK_assign_bitor: {
            *binop = make_CBitOr();
            break;
        }
        case TOK_binop_xor:
        case TOK_assign_xor: {
            *binop = make_CBitXor();
            break;
        }
        case TOK_binop_shiftleft:
        case TOK_assign_shiftleft: {
            *binop = make_CBitShiftLeft();
            break;
        }
        case TOK_binop_shiftright:
        case TOK_assign_shiftright: {
            *binop = make_CBitShiftRight();
            break;
        }
        case TOK_binop_and: {
            *binop = make_CAnd();
            break;
        }
        case TOK_binop_or: {
            *binop = make_COr();
            break;
        }
        case TOK_binop_eq: {
            *binop = make_CEqual();
            break;
        }
        case TOK_binop_ne: {
            *binop = make_CNotEqual();
            break;
        }
        case TOK_binop_lt: {
            *binop = make_CLessThan();
            break;
        }
        case TOK_binop_le: {
            *binop = make_CLessOrEqual();
            break;
        }
        case TOK_binop_gt: {
            *binop = make_CGreaterThan();
            break;
        }
        case TOK_binop_ge: {
            *binop = make_CGreaterOrEqual();
            break;
        }
        default:
            THROW_AT_LINE(ctx->next_tok->line, GET_PARSER_MSG(MSG_expect_binop, str_fmt_tok(ctx->next_tok)));
    }
    FINALLY;
    CATCH_EXIT;
}

static void proc_abstract_decltor(
    CAbstractDeclarator* node, shared_ptr_t(Type) * base_type, AbstractDeclarator* abstract_decltor);

static void proc_ptr_abstract_decltor(
    CAbstractPointer* node, shared_ptr_t(Type) * base_type, AbstractDeclarator* abstract_decltor) {
    shared_ptr_t(Type) derived_type = make_Pointer(base_type);
    proc_abstract_decltor(node->abstract_decltor, &derived_type, abstract_decltor);
}

static void proc_arr_abstract_decltor(
    CAbstractArray* node, shared_ptr_t(Type) * base_type, AbstractDeclarator* abstract_decltor) {
    TLong size = node->size;
    shared_ptr_t(Type) derived_type = make_Array(size, base_type);
    proc_abstract_decltor(node->abstract_decltor, &derived_type, abstract_decltor);
}

static void proc_base_abstract_decltor(shared_ptr_t(Type) * base_type, AbstractDeclarator* abstract_decltor) {
    uptr_move(Type, *base_type, abstract_decltor->derived_type);
}

static void proc_abstract_decltor(
    CAbstractDeclarator* node, shared_ptr_t(Type) * base_type, AbstractDeclarator* abstract_decltor) {
    switch (node->type) {
        case AST_CAbstractPointer_t:
            proc_ptr_abstract_decltor(&node->get._CAbstractPointer, base_type, abstract_decltor);
            break;
        case AST_CAbstractArray_t:
            proc_arr_abstract_decltor(&node->get._CAbstractArray, base_type, abstract_decltor);
            break;
        case AST_CAbstractBase_t:
            proc_base_abstract_decltor(base_type, abstract_decltor);
            break;
        default:
            THROW_ABORT;
    }
}

static error_t parse_abstract_decltor(Ctx ctx, unique_ptr_t(CAbstractDeclarator) * abstract_decltor);

// (array) <direct-abstract-declarator> ::= { "[" <const> "]" }+
static error_t parse_arr_abstract_decltor(Ctx ctx, unique_ptr_t(CAbstractDeclarator) * abstract_decltor) {
    CATCH_ENTER;
    *abstract_decltor = make_CAbstractBase();
    do {
        TLong size;
        TRY(parse_arr_size(ctx, &size));
        *abstract_decltor = make_CAbstractArray(size, abstract_decltor);
        TRY(peek_next(ctx));
    }
    while (ctx->peek_tok->tok_kind == TOK_open_bracket);
    FINALLY;
    CATCH_EXIT;
}

// (direct) <direct-abstract-declarator> ::= "(" <abstract-declarator> ")" { "[" <const> "]" }
static error_t parse_direct_abstract_decltor(Ctx ctx, unique_ptr_t(CAbstractDeclarator) * abstract_decltor) {
    CATCH_ENTER;
    TRY(pop_next(ctx));
    TRY(parse_abstract_decltor(ctx, abstract_decltor));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_close_paren));
    TRY(peek_next(ctx));
    while (ctx->peek_tok->tok_kind == TOK_open_bracket) {
        TLong size;
        TRY(parse_arr_size(ctx, &size));
        *abstract_decltor = make_CAbstractArray(size, abstract_decltor);
        TRY(peek_next(ctx));
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t parse_ptr_abstract_decltor(Ctx ctx, unique_ptr_t(CAbstractDeclarator) * abstract_decltor) {
    CATCH_ENTER;
    TRY(pop_next(ctx));
    TRY(peek_next(ctx));
    if (ctx->peek_tok->tok_kind == TOK_close_paren) {
        *abstract_decltor = make_CAbstractBase();
    }
    else {
        TRY(parse_abstract_decltor(ctx, abstract_decltor));
    }
    *abstract_decltor = make_CAbstractPointer(abstract_decltor);
    FINALLY;
    CATCH_EXIT;
}

// <abstract-declarator> ::= "*" [ <abstract-declarator> ] | <direct-abstract-declarator>
// abstract_declarator = AbstractPointer(abstract_declarator) | AbstractArray(int, abstract_declarator) | AbstractBase
static error_t parse_abstract_decltor(Ctx ctx, unique_ptr_t(CAbstractDeclarator) * abstract_decltor) {
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
            THROW_AT_LINE(ctx->peek_tok->line, GET_PARSER_MSG(MSG_expect_abstract_decltor, str_fmt_tok(ctx->peek_tok)));
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t parse_type_specifier(Ctx ctx, shared_ptr_t(Type) * type_specifier);

static error_t parse_unary_exp_factor(Ctx ctx, unique_ptr_t(CExp) * exp);
static error_t parse_cast_exp_factor(Ctx ctx, unique_ptr_t(CExp) * exp);
static error_t parse_exp(Ctx ctx, int32_t min_precedence, unique_ptr_t(CExp) * exp);

static error_t parse_decltor_cast_factor(Ctx ctx, shared_ptr_t(Type) * target_type) {
    AbstractDeclarator abstract_decltor = {sptr_new()};
    unique_ptr_t(CAbstractDeclarator) abstract_decltor_1 = uptr_new();
    CATCH_ENTER;
    TRY(parse_abstract_decltor(ctx, &abstract_decltor_1));
    proc_abstract_decltor(abstract_decltor_1, target_type, &abstract_decltor);
    sptr_move(Type, abstract_decltor.derived_type, *target_type);
    FINALLY;
    free_Type(&abstract_decltor.derived_type);
    free_CAbstractDeclarator(&abstract_decltor_1);
    CATCH_EXIT;
}

// <type-name> ::= { <type-specifier> }+ [ <abstract-declarator> ]
static error_t parse_type_name(Ctx ctx, shared_ptr_t(Type) * target_type) {
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
    FINALLY;
    CATCH_EXIT;
}

// <argument-list> ::= <exp> { "," <exp> }
static error_t parse_arg_list(Ctx ctx, vector_t(unique_ptr_t(CExp)) * args) {
    unique_ptr_t(CExp) arg = uptr_new();
    CATCH_ENTER;
    TRY(parse_exp(ctx, 0, &arg));
    vec_move_back(*args, arg);
    TRY(peek_next(ctx));
    while (ctx->peek_tok->tok_kind == TOK_comma_separator) {
        TRY(pop_next(ctx));
        TRY(parse_exp(ctx, 0, &arg));
        vec_move_back(*args, arg);
        TRY(peek_next(ctx));
    }
    FINALLY;
    free_CExp(&arg);
    CATCH_EXIT;
}

static error_t parse_const_factor(Ctx ctx, unique_ptr_t(CExp) * exp) {
    shared_ptr_t(CConst) constant = sptr_new();
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(parse_const(ctx, &constant));
    *exp = make_CConstant(&constant, line);
    FINALLY;
    free_CConst(&constant);
    CATCH_EXIT;
}

static error_t parse_unsigned_const_factor(Ctx ctx, unique_ptr_t(CExp) * exp) {
    shared_ptr_t(CConst) constant = sptr_new();
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(parse_unsigned_const(ctx, &constant));
    *exp = make_CConstant(&constant, line);
    FINALLY;
    free_CConst(&constant);
    CATCH_EXIT;
}

static error_t parse_string_literal_factor(Ctx ctx, unique_ptr_t(CExp) * exp) {
    shared_ptr_t(CStringLiteral) literal = sptr_new();
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(pop_next(ctx));
    TRY(parse_string_literal(ctx, &literal));
    *exp = make_CString(&literal, line);
    FINALLY;
    free_CStringLiteral(&literal);
    CATCH_EXIT;
}

static error_t parse_var_factor(Ctx ctx, unique_ptr_t(CExp) * exp) {
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TIdentifier name;
    TRY(parse_identifier(ctx, 0, &name));
    *exp = make_CVar(name, line);
    FINALLY;
    CATCH_EXIT;
}

static error_t parse_call_factor(Ctx ctx, unique_ptr_t(CExp) * exp) {
    vector_t(unique_ptr_t(CExp)) args = vec_new();
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
    *exp = make_CFunctionCall(name, &args, line);
    FINALLY;
    for (size_t i = 0; i < vec_size(args); ++i) {
        free_CExp(&args[i]);
    }
    vec_delete(args);
    CATCH_EXIT;
}

static error_t parse_inner_exp_factor(Ctx ctx, unique_ptr_t(CExp) * exp) {
    CATCH_ENTER;
    TRY(pop_next(ctx));
    TRY(parse_exp(ctx, 0, exp));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_close_paren));
    FINALLY;
    CATCH_EXIT;
}

static error_t parse_subscript_factor(Ctx ctx, unique_ptr_t(CExp) * exp) {
    unique_ptr_t(CExp) subscript_exp = uptr_new();
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(pop_next(ctx));
    TRY(parse_exp(ctx, 0, &subscript_exp));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_close_bracket));
    *exp = make_CSubscript(exp, &subscript_exp, line);
    FINALLY;
    free_CExp(&subscript_exp);
    CATCH_EXIT;
}

static error_t parse_dot_factor(Ctx ctx, unique_ptr_t(CExp) * exp) {
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(pop_next(ctx));
    TRY(peek_next(ctx));
    TRY(expect_next(ctx, ctx->peek_tok, TOK_identifier));
    TIdentifier member;
    TRY(parse_identifier(ctx, 0, &member));
    *exp = make_CDot(member, exp, line);
    FINALLY;
    CATCH_EXIT;
}

static error_t parse_arrow_factor(Ctx ctx, unique_ptr_t(CExp) * exp) {
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(pop_next(ctx));
    TRY(peek_next(ctx));
    TRY(expect_next(ctx, ctx->peek_tok, TOK_identifier));
    TIdentifier member;
    TRY(parse_identifier(ctx, 0, &member));
    *exp = make_CArrow(member, exp, line);
    FINALLY;
    CATCH_EXIT;
}

static error_t parse_postfix_incr_factor(Ctx ctx, unique_ptr_t(CExp) * exp) {
    unique_ptr_t(CExp) exp_right = uptr_new();
    unique_ptr_t(CExp) exp_right_1 = uptr_new();
    unique_ptr_t(CUnaryOp) unop = uptr_new();
    unique_ptr_t(CBinaryOp) binop = uptr_new();
    shared_ptr_t(CConst) constant = sptr_new();
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    unop = make_CPostfix();
    TRY(parse_binop(ctx, &binop));
    constant = make_CConstInt(1);
    exp_right = make_CConstant(&constant, line);
    exp_right_1 = make_CBinary(&binop, exp, &exp_right, line);
    *exp = make_CAssignment(&unop, NULL, &exp_right_1, line);
    FINALLY;
    free_CExp(&exp_right);
    free_CExp(&exp_right_1);
    free_CUnaryOp(&unop);
    free_CBinaryOp(&binop);
    free_CConst(&constant);
    CATCH_EXIT;
}

static error_t parse_unary_factor(Ctx ctx, unique_ptr_t(CExp) * exp) {
    unique_ptr_t(CExp) cast_exp = uptr_new();
    unique_ptr_t(CUnaryOp) unop = uptr_new();
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(parse_unop(ctx, &unop));
    TRY(parse_cast_exp_factor(ctx, &cast_exp));
    *exp = make_CUnary(&unop, &cast_exp, line);
    FINALLY;
    free_CExp(&cast_exp);
    free_CUnaryOp(&unop);
    CATCH_EXIT;
}

static error_t parse_incr_unary_factor(Ctx ctx, unique_ptr_t(CExp) * exp) {
    unique_ptr_t(CExp) exp_left = uptr_new();
    unique_ptr_t(CExp) exp_right = uptr_new();
    unique_ptr_t(CExp) exp_left_1 = uptr_new();
    unique_ptr_t(CExp) exp_right_1 = uptr_new();
    unique_ptr_t(CUnaryOp) unop = uptr_new();
    unique_ptr_t(CBinaryOp) binop = uptr_new();
    shared_ptr_t(CConst) constant = sptr_new();
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    unop = make_CPrefix();
    TRY(parse_binop(ctx, &binop));
    TRY(parse_cast_exp_factor(ctx, &exp_left));
    constant = make_CConstInt(1);
    exp_right = make_CConstant(&constant, line);
    exp_right_1 = make_CBinary(&binop, &exp_left, &exp_right, line);
    *exp = make_CAssignment(&unop, &exp_left_1, &exp_right_1, line);
    FINALLY;
    free_CExp(&exp_left);
    free_CExp(&exp_right);
    free_CExp(&exp_left_1);
    free_CExp(&exp_right_1);
    free_CUnaryOp(&unop);
    free_CBinaryOp(&binop);
    free_CConst(&constant);
    CATCH_EXIT;
}

static error_t parse_deref_factor(Ctx ctx, unique_ptr_t(CExp) * exp) {
    unique_ptr_t(CExp) cast_exp = uptr_new();
    CATCH_ENTER;
    size_t line = ctx->next_tok->line;
    TRY(parse_cast_exp_factor(ctx, &cast_exp));
    *exp = make_CDereference(&cast_exp, line);
    FINALLY;
    free_CExp(&cast_exp);
    CATCH_EXIT;
}

static error_t parse_addrof_factor(Ctx ctx, unique_ptr_t(CExp) * exp) {
    unique_ptr_t(CExp) cast_exp = uptr_new();
    CATCH_ENTER;
    size_t line = ctx->next_tok->line;
    TRY(parse_cast_exp_factor(ctx, &cast_exp));
    *exp = make_CAddrOf(&cast_exp, line);
    FINALLY;
    free_CExp(&cast_exp);
    CATCH_EXIT;
}

static error_t parse_ptr_unary_factor(Ctx ctx, unique_ptr_t(CExp) * exp) {
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
            THROW_AT_LINE(ctx->next_tok->line, GET_PARSER_MSG(MSG_expect_ptr_unary_factor, str_fmt_tok(ctx->next_tok)));
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t parse_sizeoft_factor(Ctx ctx, unique_ptr_t(CExp) * exp) {
    shared_ptr_t(Type) target_type = sptr_new();
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(pop_next(ctx));
    TRY(parse_type_name(ctx, &target_type));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_close_paren));
    *exp = make_CSizeOfT(&target_type, line);
    FINALLY;
    free_Type(&target_type);
    CATCH_EXIT;
}

static error_t parse_sizeof_factor(Ctx ctx, unique_ptr_t(CExp) * exp) {
    unique_ptr_t(CExp) unary_exp = uptr_new();
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(parse_unary_exp_factor(ctx, &unary_exp));
    *exp = make_CSizeOf(&unary_exp, line);
    FINALLY;
    free_CExp(&unary_exp);
    CATCH_EXIT;
}

static error_t parse_sizeof_unary_factor(Ctx ctx, unique_ptr_t(CExp) * exp) {
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
                EARLY_EXIT;
            default:
                break;
        }
    }
    TRY(parse_sizeof_factor(ctx, exp));
    FINALLY;
    CATCH_EXIT;
}

static error_t parse_cast_factor(Ctx ctx, unique_ptr_t(CExp) * exp) {
    unique_ptr_t(CExp) cast_exp = uptr_new();
    shared_ptr_t(Type) target_type = sptr_new();
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(pop_next(ctx));
    TRY(parse_type_name(ctx, &target_type));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_close_paren));
    TRY(parse_cast_exp_factor(ctx, &cast_exp));
    *exp = make_CCast(&cast_exp, &target_type, line);
    FINALLY;
    free_CExp(&cast_exp);
    free_Type(&target_type);
    CATCH_EXIT;
}

// <primary-exp> ::= <const> | <identifier> | "(" <exp> ")" | { <string> }+ | <identifier> "(" [ <argument-list> ] ")"
static error_t parse_primary_exp_factor(Ctx ctx, unique_ptr_t(CExp) * exp) {
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
            THROW_AT_LINE(
                ctx->peek_tok->line, GET_PARSER_MSG(MSG_expect_primary_exp_factor, str_fmt_tok(ctx->peek_tok)));
    }
    FINALLY;
    CATCH_EXIT;
}

// <postfix-op> ::= "[" <exp> "]" | "." <identifier> | "->" <identifier>
static error_t parse_postfix_op_exp_factor(Ctx ctx, unique_ptr_t(CExp) * exp) {
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
            EARLY_EXIT;
    }
    TRY(parse_postfix_op_exp_factor(ctx, exp));
    FINALLY;
    CATCH_EXIT;
}

// <postfix-exp> ::= <primary-exp> { <postfix-op> }
static error_t parse_postfix_exp_factor(Ctx ctx, unique_ptr_t(CExp) * exp) {
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
    FINALLY;
    CATCH_EXIT;
}

// <unary-exp> ::= <unop> <cast-exp> | "sizeof" <unary-exp> | "sizeof" "(" <type-name> ")" | <postfix-exp>
static error_t parse_unary_exp_factor(Ctx ctx, unique_ptr_t(CExp) * exp) {
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
    FINALLY;
    CATCH_EXIT;
}

// <cast-exp> ::= "(" <type-name> ")" <cast-exp> | <unary-exp>
static error_t parse_cast_exp_factor(Ctx ctx, unique_ptr_t(CExp) * exp) {
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
                EARLY_EXIT;
            default:
                break;
        }
    }
    TRY(parse_unary_exp_factor(ctx, exp));
    FINALLY;
    CATCH_EXIT;
}

static error_t parse_assign_exp(Ctx ctx, int32_t precedence, unique_ptr_t(CExp) * exp_left) {
    unique_ptr_t(CExp) exp_right = uptr_new();
    unique_ptr_t(CUnaryOp) unop = uptr_new();
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(pop_next(ctx));
    TRY(parse_exp(ctx, precedence, &exp_right));
    *exp_left = make_CAssignment(&unop, exp_left, &exp_right, line);
    FINALLY;
    free_CExp(&exp_right);
    free_CUnaryOp(&unop);
    CATCH_EXIT;
}

static error_t parse_assign_compound_exp(Ctx ctx, int32_t precedence, unique_ptr_t(CExp) * exp_left) {
    unique_ptr_t(CExp) exp_right = uptr_new();
    unique_ptr_t(CExp) exp_right_1 = uptr_new();
    unique_ptr_t(CBinaryOp) binop = uptr_new();
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(parse_binop(ctx, &binop));
    TRY(parse_exp(ctx, precedence, &exp_right));
    exp_right_1 = make_CBinary(&binop, exp_left, &exp_right, line);
    *exp_left = make_CAssignment(NULL, NULL, &exp_right_1, line);
    FINALLY;
    free_CExp(&exp_right);
    free_CExp(&exp_right_1);
    free_CBinaryOp(&binop);
    CATCH_EXIT;
}

static error_t parse_binary_exp(Ctx ctx, int32_t precedence, unique_ptr_t(CExp) * exp_left) {
    unique_ptr_t(CExp) exp_right = uptr_new();
    unique_ptr_t(CBinaryOp) binop = uptr_new();
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(parse_binop(ctx, &binop));
    TRY(parse_exp(ctx, precedence + 1, &exp_right));
    *exp_left = make_CBinary(&binop, exp_left, &exp_right, line);
    FINALLY;
    free_CExp(&exp_right);
    free_CBinaryOp(&binop);
    CATCH_EXIT;
}

static error_t parse_ternary_exp(Ctx ctx, int32_t precedence, unique_ptr_t(CExp) * exp_left) {
    unique_ptr_t(CExp) exp_middle = uptr_new();
    unique_ptr_t(CExp) exp_right = uptr_new();
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(pop_next(ctx));
    TRY(parse_exp(ctx, 0, &exp_middle));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_ternary_else));
    TRY(parse_exp(ctx, precedence, &exp_right));
    *exp_left = make_CConditional(exp_left, &exp_middle, &exp_right, line);
    FINALLY;
    free_CExp(&exp_middle);
    free_CExp(&exp_right);
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
static error_t parse_exp(Ctx ctx, int32_t min_precedence, unique_ptr_t(CExp) * exp) {
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
                THROW_AT_LINE(ctx->peek_tok->line, GET_PARSER_MSG(MSG_expect_exp, str_fmt_tok(ctx->peek_tok)));
        }
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t parse_for_init(Ctx ctx, unique_ptr_t(CForInit) * for_init);
static error_t parse_block(Ctx ctx, unique_ptr_t(CBlock) * block);
static error_t parse_statement(Ctx ctx, unique_ptr_t(CStatement) * statement);

static error_t parse_ret_statement(Ctx ctx, unique_ptr_t(CStatement) * statement) {
    unique_ptr_t(CExp) exp = uptr_new();
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(pop_next(ctx));
    TRY(peek_next(ctx));
    if (ctx->peek_tok->tok_kind != TOK_semicolon) {
        TRY(parse_exp(ctx, 0, &exp));
    }
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_semicolon));
    *statement = make_CReturn(&exp, line);
    FINALLY;
    free_CExp(&exp);
    CATCH_EXIT;
}

static error_t parse_exp_statement(Ctx ctx, unique_ptr_t(CStatement) * statement) {
    unique_ptr_t(CExp) exp = uptr_new();
    CATCH_ENTER;
    TRY(parse_exp(ctx, 0, &exp));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_semicolon));
    *statement = make_CExpression(&exp);
    FINALLY;
    free_CExp(&exp);
    CATCH_EXIT;
}

static error_t parse_if_statement(Ctx ctx, unique_ptr_t(CStatement) * statement) {
    unique_ptr_t(CExp) condition = uptr_new();
    unique_ptr_t(CStatement) then = uptr_new();
    unique_ptr_t(CStatement) else_fi = uptr_new();
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
    *statement = make_CIf(&condition, &then, &else_fi);
    FINALLY;
    free_CExp(&condition);
    free_CStatement(&then);
    free_CStatement(&else_fi);
    CATCH_EXIT;
}

static error_t parse_goto_statement(Ctx ctx, unique_ptr_t(CStatement) * statement) {
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(pop_next(ctx));
    TRY(peek_next(ctx));
    TRY(expect_next(ctx, ctx->peek_tok, TOK_identifier));
    TIdentifier target;
    TRY(parse_identifier(ctx, 0, &target));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_semicolon));
    *statement = make_CGoto(target, line);
    FINALLY;
    CATCH_EXIT;
}

static error_t parse_label_statement(Ctx ctx, unique_ptr_t(CStatement) * statement) {
    unique_ptr_t(CStatement) jump_to = uptr_new();
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TIdentifier target;
    TRY(parse_identifier(ctx, 0, &target));
    TRY(pop_next(ctx));
    TRY(peek_next(ctx));
    TRY(parse_statement(ctx, &jump_to));
    *statement = make_CLabel(target, &jump_to, line);
    FINALLY;
    free_CStatement(&jump_to);
    CATCH_EXIT;
}

static error_t parse_compound_statement(Ctx ctx, unique_ptr_t(CStatement) * statement) {
    unique_ptr_t(CBlock) block = uptr_new();
    CATCH_ENTER;
    TRY(parse_block(ctx, &block));
    *statement = make_CCompound(&block);
    FINALLY;
    free_CBlock(&block);
    CATCH_EXIT;
}

static error_t parse_while_statement(Ctx ctx, unique_ptr_t(CStatement) * statement) {
    unique_ptr_t(CExp) condition = uptr_new();
    unique_ptr_t(CStatement) body = uptr_new();
    CATCH_ENTER;
    TRY(pop_next(ctx));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_open_paren));
    TRY(parse_exp(ctx, 0, &condition));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_close_paren));
    TRY(peek_next(ctx));
    TRY(parse_statement(ctx, &body));
    *statement = make_CWhile(&condition, &body);
    FINALLY;
    free_CExp(&condition);
    free_CStatement(&body);
    CATCH_EXIT;
}

static error_t parse_do_while_statement(Ctx ctx, unique_ptr_t(CStatement) * statement) {
    unique_ptr_t(CStatement) body = uptr_new();
    unique_ptr_t(CExp) condition = uptr_new();
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
    *statement = make_CDoWhile(&condition, &body);
    FINALLY;
    free_CStatement(&body);
    free_CExp(&condition);
    CATCH_EXIT;
}

static error_t parse_for_statement(Ctx ctx, unique_ptr_t(CStatement) * statement) {
    unique_ptr_t(CForInit) for_init = uptr_new();
    unique_ptr_t(CExp) condition = uptr_new();
    unique_ptr_t(CExp) post = uptr_new();
    unique_ptr_t(CStatement) body = uptr_new();
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
    *statement = make_CFor(&for_init, &condition, &post, &body);
    FINALLY;
    free_CForInit(&for_init);
    free_CExp(&condition);
    free_CExp(&post);
    free_CStatement(&body);
    CATCH_EXIT;
}

static error_t parse_switch_statement(Ctx ctx, unique_ptr_t(CStatement) * statement) {
    unique_ptr_t(CExp) match = uptr_new();
    unique_ptr_t(CStatement) body = uptr_new();
    CATCH_ENTER;
    TRY(pop_next(ctx));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_open_paren));
    TRY(parse_exp(ctx, 0, &match));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_close_paren));
    TRY(peek_next(ctx));
    TRY(parse_statement(ctx, &body));
    *statement = make_CSwitch(&match, &body);
    FINALLY;
    free_CExp(&match);
    free_CStatement(&body);
    CATCH_EXIT;
}

static error_t parse_case_statement(Ctx ctx, unique_ptr_t(CStatement) * statement) {
    unique_ptr_t(CExp) value = uptr_new();
    unique_ptr_t(CStatement) jump_to = uptr_new();
    shared_ptr_t(CConst) constant = sptr_new();
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
            THROW_AT_LINE(
                ctx->peek_tok->line, GET_PARSER_MSG(MSG_case_value_not_int_const, str_fmt_tok(ctx->peek_tok)));
    }
    value = make_CConstant(&constant, line);
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_ternary_else));
    TRY(peek_next(ctx));
    TRY(parse_statement(ctx, &jump_to));
    *statement = make_CCase(&value, &jump_to);
    FINALLY;
    free_CExp(&value);
    free_CStatement(&jump_to);
    free_CConst(&constant);
    CATCH_EXIT;
}

static error_t parse_default_statement(Ctx ctx, unique_ptr_t(CStatement) * statement) {
    unique_ptr_t(CStatement) jump_to = uptr_new();
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(pop_next(ctx));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_ternary_else));
    TRY(peek_next(ctx));
    TRY(parse_statement(ctx, &jump_to));
    *statement = make_CDefault(&jump_to, line);
    FINALLY;
    free_CStatement(&jump_to);
    CATCH_EXIT;
}

static error_t parse_break_statement(Ctx ctx, unique_ptr_t(CStatement) * statement) {
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(pop_next(ctx));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_semicolon));
    *statement = make_CBreak(line);
    FINALLY;
    CATCH_EXIT;
}

static error_t parse_continue_statement(Ctx ctx, unique_ptr_t(CStatement) * statement) {
    CATCH_ENTER;
    size_t line = ctx->peek_tok->line;
    TRY(pop_next(ctx));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_semicolon));
    *statement = make_CContinue(line);
    FINALLY;
    CATCH_EXIT;
}

static error_t parse_null_statement(Ctx ctx, unique_ptr_t(CStatement) * statement) {
    CATCH_ENTER;
    TRY(pop_next(ctx));
    *statement = make_CNull();
    FINALLY;
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
static error_t parse_statement(Ctx ctx, unique_ptr_t(CStatement) * statement) {
    CATCH_ENTER;
    switch (ctx->peek_tok->tok_kind) {
        case TOK_key_return:
            TRY(parse_ret_statement(ctx, statement));
            EARLY_EXIT;
        case TOK_key_if:
            TRY(parse_if_statement(ctx, statement));
            EARLY_EXIT;
        case TOK_key_goto:
            TRY(parse_goto_statement(ctx, statement));
            EARLY_EXIT;
        case TOK_identifier: {
            TRY(peek_next_i(ctx, 1));
            if (ctx->peek_tok_i->tok_kind == TOK_ternary_else) {
                TRY(parse_label_statement(ctx, statement));
                EARLY_EXIT;
            }
            break;
        }
        case TOK_open_brace:
            TRY(parse_compound_statement(ctx, statement));
            EARLY_EXIT;
        case TOK_key_while:
            TRY(parse_while_statement(ctx, statement));
            EARLY_EXIT;
        case TOK_key_do:
            TRY(parse_do_while_statement(ctx, statement));
            EARLY_EXIT;
        case TOK_key_for:
            TRY(parse_for_statement(ctx, statement));
            EARLY_EXIT;
        case TOK_key_switch:
            TRY(parse_switch_statement(ctx, statement));
            EARLY_EXIT;
        case TOK_key_case:
            TRY(parse_case_statement(ctx, statement));
            EARLY_EXIT;
        case TOK_key_default:
            TRY(parse_default_statement(ctx, statement));
            EARLY_EXIT;
        case TOK_key_break:
            TRY(parse_break_statement(ctx, statement));
            EARLY_EXIT;
        case TOK_key_continue:
            TRY(parse_continue_statement(ctx, statement));
            EARLY_EXIT;
        case TOK_semicolon:
            TRY(parse_null_statement(ctx, statement));
            EARLY_EXIT;
        default:
            break;
    }
    TRY(parse_exp_statement(ctx, statement));
    FINALLY;
    CATCH_EXIT;
}

static error_t parse_decltor_decl(Ctx ctx, Declarator* decltor, unique_ptr_t(CStorageClass) * storage_class);
static error_t parse_var_declaration(Ctx ctx, unique_ptr_t(CStorageClass) * storage_class, Declarator* decltor,
    unique_ptr_t(CVariableDeclaration) * var_decl);

static error_t parse_for_init_decl(Ctx ctx, unique_ptr_t(CForInit) * for_init) {
    Declarator decltor = {0, sptr_new(), vec_new()};
    unique_ptr_t(CStorageClass) storage_class = uptr_new();
    unique_ptr_t(CVariableDeclaration) var_decl = uptr_new();
    CATCH_ENTER;
    TRY(parse_decltor_decl(ctx, &decltor, &storage_class));
    if (decltor.derived_type->type == AST_FunType_t) {
        THROW_AT_LINE(ctx->next_tok->line,
            GET_PARSER_MSG(MSG_for_init_decl_as_fun, map_get(ctx->identifiers->hash_table, decltor.name)));
    }
    TRY(parse_var_declaration(ctx, &storage_class, &decltor, &var_decl));
    *for_init = make_CInitDecl(&var_decl);
    FINALLY;
    free_Type(&decltor.derived_type);
    vec_delete(decltor.params);
    free_CStorageClass(&storage_class);
    free_CVariableDeclaration(&var_decl);
    CATCH_EXIT;
}

static error_t parse_for_init_exp(Ctx ctx, unique_ptr_t(CForInit) * for_init) {
    unique_ptr_t(CExp) init = uptr_new();
    CATCH_ENTER;
    TRY(peek_next(ctx));
    if (ctx->peek_tok->tok_kind != TOK_semicolon) {
        TRY(parse_exp(ctx, 0, &init));
    }
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_semicolon));
    *for_init = make_CInitExp(&init);
    FINALLY;
    free_CExp(&init);
    CATCH_EXIT;
}

// <for-init> ::= <variable-declaration> | [ <exp> ] ";"
// for_init = InitDecl(variable_declaration) | InitExp(exp?)
static error_t parse_for_init(Ctx ctx, unique_ptr_t(CForInit) * for_init) {
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
    FINALLY;
    CATCH_EXIT;
}

static error_t parse_declaration(Ctx ctx, unique_ptr_t(CDeclaration) * declaration);

static error_t parse_s_block_item(Ctx ctx, unique_ptr_t(CBlockItem) * block_item) {
    unique_ptr_t(CStatement) statement = uptr_new();
    CATCH_ENTER;
    TRY(parse_statement(ctx, &statement));
    *block_item = make_CS(&statement);
    FINALLY;
    free_CStatement(&statement);
    CATCH_EXIT;
}

static error_t parse_d_block_item(Ctx ctx, unique_ptr_t(CBlockItem) * block_item) {
    unique_ptr_t(CDeclaration) declaration = uptr_new();
    CATCH_ENTER;
    TRY(parse_declaration(ctx, &declaration));
    *block_item = make_CD(&declaration);
    FINALLY;
    free_CDeclaration(&declaration);
    CATCH_EXIT;
}

// <block-item> ::= <statement> | <declaration>
// block_item = S(statement) | D(declaration)
static error_t parse_block_item(Ctx ctx, unique_ptr_t(CBlockItem) * block_item) {
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
    FINALLY;
    CATCH_EXIT;
}

static error_t parse_b_block(Ctx ctx, unique_ptr_t(CBlock) * block) {
    unique_ptr_t(CBlockItem) block_item = uptr_new();
    vector_t(unique_ptr_t(CBlockItem)) block_items = vec_new();
    CATCH_ENTER;
    TRY(peek_next(ctx));
    while (ctx->peek_tok->tok_kind != TOK_close_brace) {
        TRY(parse_block_item(ctx, &block_item));
        vec_move_back(block_items, block_item);
        TRY(peek_next(ctx));
    }
    *block = make_CB(&block_items);
    FINALLY;
    free_CBlockItem(&block_item);
    for (size_t i = 0; i < vec_size(block_items); ++i) {
        free_CBlockItem(&block_items[i]);
    }
    vec_delete(block_items);
    CATCH_EXIT;
}

// <block> ::= "{" { <block-item> } "}"
// block = B(block_item*)
static error_t parse_block(Ctx ctx, unique_ptr_t(CBlock) * block) {
    CATCH_ENTER;
    TRY(pop_next(ctx));
    TRY(parse_b_block(ctx, block));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_close_brace));
    FINALLY;
    CATCH_EXIT;
}

// <type-specifier> ::= "int" | "long" | "signed" | "unsigned" | "double" | "char" | "void"
//                    | ("struct" | "union") <identifier>
static error_t parse_type_specifier(Ctx ctx, shared_ptr_t(Type) * type_specifier) {
    size_t type_tok_kinds_size = 0;
    string_t type_tok_kinds_fmt = str_new(NULL);
    TOKEN_KIND type_tok_kinds[4] = {TOK_error, TOK_error, TOK_error, TOK_error};
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
            case TOK_key_void: {
                TRY(pop_next_i(ctx, i));
                type_tok_kinds[type_tok_kinds_size] = ctx->next_tok_i->tok_kind;
                type_tok_kinds_size++;
                break;
            }
            case TOK_key_struct:
            case TOK_key_union: {
                TRY(pop_next_i(ctx, i));
                type_tok_kinds[type_tok_kinds_size] = ctx->next_tok_i->tok_kind;
                type_tok_kinds_size++;
                TRY(peek_next_i(ctx, i));
                TRY(expect_next(ctx, ctx->peek_tok_i, TOK_identifier));
                break;
            }
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
                THROW_AT_LINE(
                    ctx->peek_tok_i->line, GET_PARSER_MSG(MSG_expect_specifier, str_fmt_tok(ctx->peek_tok_i)));
        }
    }
Lbreak:
    switch (type_tok_kinds_size) {
        case 1: {
            switch (type_tok_kinds[0]) {
                case TOK_key_char: {
                    *type_specifier = make_Char();
                    EARLY_EXIT;
                }
                case TOK_key_int: {
                    *type_specifier = make_Int();
                    EARLY_EXIT;
                }
                case TOK_key_long: {
                    *type_specifier = make_Long();
                    EARLY_EXIT;
                }
                case TOK_key_double: {
                    *type_specifier = make_Double();
                    EARLY_EXIT;
                }
                case TOK_key_unsigned: {
                    *type_specifier = make_UInt();
                    EARLY_EXIT;
                }
                case TOK_key_signed: {
                    *type_specifier = make_Int();
                    EARLY_EXIT;
                }
                case TOK_key_void: {
                    *type_specifier = make_Void();
                    EARLY_EXIT;
                }
                case TOK_key_struct: {
                    TIdentifier tag;
                    TRY(parse_identifier(ctx, i, &tag));
                    *type_specifier = make_Structure(tag, false);
                    EARLY_EXIT;
                }
                case TOK_key_union: {
                    TIdentifier tag;
                    TRY(parse_identifier(ctx, i, &tag));
                    *type_specifier = make_Structure(tag, true);
                    EARLY_EXIT;
                }
                default:
                    break;
            }
            break;
        }
        case 2: {
            if (type_tok_kinds[0] == TOK_key_unsigned || type_tok_kinds[1] == TOK_key_unsigned) {
                if (type_tok_kinds[0] == TOK_key_int || type_tok_kinds[1] == TOK_key_int) {
                    *type_specifier = make_UInt();
                    EARLY_EXIT;
                }
                else if (type_tok_kinds[0] == TOK_key_long || type_tok_kinds[1] == TOK_key_long) {
                    *type_specifier = make_ULong();
                    EARLY_EXIT;
                }
                else if (type_tok_kinds[0] == TOK_key_char || type_tok_kinds[1] == TOK_key_char) {
                    *type_specifier = make_UChar();
                    EARLY_EXIT;
                }
            }
            else if (type_tok_kinds[0] == TOK_key_signed || type_tok_kinds[1] == TOK_key_signed) {
                if (type_tok_kinds[0] == TOK_key_int || type_tok_kinds[1] == TOK_key_int) {
                    *type_specifier = make_Int();
                    EARLY_EXIT;
                }
                else if (type_tok_kinds[0] == TOK_key_long || type_tok_kinds[1] == TOK_key_long) {
                    *type_specifier = make_Long();
                    EARLY_EXIT;
                }
                else if (type_tok_kinds[0] == TOK_key_char || type_tok_kinds[1] == TOK_key_char) {
                    *type_specifier = make_SChar();
                    EARLY_EXIT;
                }
            }
            else if ((type_tok_kinds[0] == TOK_key_int || type_tok_kinds[1] == TOK_key_int)
                     && (type_tok_kinds[0] == TOK_key_long || type_tok_kinds[1] == TOK_key_long)) {
                *type_specifier = make_Long();
                EARLY_EXIT;
            }
            break;
        }
        case 3: {
            if ((type_tok_kinds[0] == TOK_key_int || type_tok_kinds[1] == TOK_key_int
                    || type_tok_kinds[2] == TOK_key_int)
                && (type_tok_kinds[0] == TOK_key_long || type_tok_kinds[1] == TOK_key_long
                    || type_tok_kinds[2] == TOK_key_long)) {
                if (type_tok_kinds[0] == TOK_key_unsigned || type_tok_kinds[1] == TOK_key_unsigned
                    || type_tok_kinds[2] == TOK_key_unsigned) {
                    *type_specifier = make_ULong();
                    EARLY_EXIT;
                }
                else if (type_tok_kinds[0] == TOK_key_signed || type_tok_kinds[1] == TOK_key_signed
                         || type_tok_kinds[2] == TOK_key_signed) {
                    *type_specifier = make_Long();
                    EARLY_EXIT;
                }
            }
            break;
        }
        default:
            break;
    }
    type_tok_kinds_fmt = str_new("(");
    for (i = 0; i < type_tok_kinds_size; ++i) {
        str_append(type_tok_kinds_fmt, get_tok_kind_fmt(type_tok_kinds[i]));
        str_append(type_tok_kinds_fmt, ", ");
    }
    if (type_tok_kinds_size > 0) {
        str_pop_back(type_tok_kinds_fmt);
        str_pop_back(type_tok_kinds_fmt);
    }
    str_append(type_tok_kinds_fmt, ")");
    THROW_AT_LINE(line, GET_PARSER_MSG(MSG_expect_specifier_list, type_tok_kinds_fmt));
    FINALLY;
    str_delete(type_tok_kinds_fmt);
    CATCH_EXIT;
}

// <specifier> ::= <type-specifier> | "static" | "extern"
// storage_class = Static | Extern
static error_t parse_storage_class(Ctx ctx, unique_ptr_t(CStorageClass) * storage_class) {
    CATCH_ENTER;
    TRY(pop_next(ctx));
    switch (ctx->next_tok->tok_kind) {
        case TOK_key_static: {
            *storage_class = make_CStatic();
            break;
        }
        case TOK_key_extern: {
            *storage_class = make_CExtern();
            break;
        }
        default:
            THROW_AT_LINE(ctx->next_tok->line, GET_PARSER_MSG(MSG_expect_storage_class, str_fmt_tok(ctx->next_tok)));
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t parse_initializer(Ctx ctx, unique_ptr_t(CInitializer) * initializer);

static error_t parse_single_init(Ctx ctx, unique_ptr_t(CInitializer) * initializer) {
    unique_ptr_t(CExp) exp = uptr_new();
    CATCH_ENTER;
    TRY(parse_exp(ctx, 0, &exp));
    *initializer = make_CSingleInit(&exp);
    FINALLY;
    free_CExp(&exp);
    CATCH_EXIT;
}

static error_t parse_compound_init(Ctx ctx, unique_ptr_t(CInitializer) * initializer) {
    vector_t(unique_ptr_t(CInitializer)) initializers = vec_new();
    CATCH_ENTER;
    TRY(pop_next(ctx));
    while (true) {
        TRY(peek_next(ctx));
        if (ctx->peek_tok->tok_kind == TOK_close_brace) {
            break;
        }
        TRY(parse_initializer(ctx, initializer));
        vec_move_back(initializers, *initializer);
        TRY(peek_next(ctx));
        if (ctx->peek_tok->tok_kind == TOK_close_brace) {
            break;
        }
        TRY(pop_next(ctx));
        TRY(expect_next(ctx, ctx->next_tok, TOK_comma_separator));
    }
    if (vec_empty(initializers)) {
        THROW_AT_LINE(ctx->peek_tok->line, GET_PARSER_MSG_0(MSG_empty_compound_init));
    }
    TRY(pop_next(ctx));
    *initializer = make_CCompoundInit(&initializers);
    FINALLY;
    for (size_t i = 0; i < vec_size(initializers); ++i) {
        free_CInitializer(&initializers[i]);
    }
    vec_delete(initializers);
    CATCH_EXIT;
}

// <initializer> ::= <exp> | "{" <initializer> { "," <initializer> } [","] "}"
// initializer = SingleInit(exp) | CompoundInit(initializer*)
static error_t parse_initializer(Ctx ctx, unique_ptr_t(CInitializer) * initializer) {
    CATCH_ENTER;
    TRY(peek_next(ctx));
    if (ctx->peek_tok->tok_kind == TOK_open_brace) {
        TRY(parse_compound_init(ctx, initializer));
    }
    else {
        TRY(parse_single_init(ctx, initializer));
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t parse_decltor(Ctx ctx, unique_ptr_t(CDeclarator) * decltor);
static error_t proc_decltor(Ctx ctx, CDeclarator* node, shared_ptr_t(Type) * base_type, Declarator* decltor);

static void proc_ident_decltor(CIdent* node, shared_ptr_t(Type) * base_type, Declarator* decltor) {
    decltor->name = node->name;
    sptr_move(Type, *base_type, decltor->derived_type);
}

static error_t proc_ptr_decltor(
    Ctx ctx, CPointerDeclarator* node, shared_ptr_t(Type) * base_type, Declarator* decltor) {
    shared_ptr_t(Type) derived_type = sptr_new();
    CATCH_ENTER;
    derived_type = make_Pointer(base_type);
    TRY(proc_decltor(ctx, node->decltor, &derived_type, decltor));
    FINALLY;
    free_Type(&derived_type);
    CATCH_EXIT;
}

static error_t proc_arr_decltor(Ctx ctx, CArrayDeclarator* node, shared_ptr_t(Type) * base_type, Declarator* decltor) {
    shared_ptr_t(Type) derived_type = sptr_new();
    CATCH_ENTER;
    TLong size = node->size;
    derived_type = make_Array(size, base_type);
    TRY(proc_decltor(ctx, node->decltor, &derived_type, decltor));
    FINALLY;
    free_Type(&derived_type);
    CATCH_EXIT;
}

static error_t proc_param_decltor(
    Ctx ctx, CParam* node, vector_t(TIdentifier) * params, vector_t(shared_ptr_t(Type)) * param_types) {
    Declarator decltor = {0, sptr_new(), vec_new()};
    shared_ptr_t(Type) param_type = sptr_new();
    CATCH_ENTER;
    param_type = node->param_type;
    TRY(proc_decltor(ctx, node->decltor, &param_type, &decltor));
    THROW_ABORT_IF(decltor.derived_type->type == AST_FunType_t);
    vec_push_back(*params, decltor.name);
    vec_move_back(*param_types, decltor.derived_type);
    FINALLY;
    free_Type(&decltor.derived_type);
    vec_delete(decltor.params);
    free_Type(&param_type);
    CATCH_EXIT;
}

static error_t proc_fun_decltor(Ctx ctx, CFunDeclarator* node, shared_ptr_t(Type) * base_type, Declarator* decltor) {
    shared_ptr_t(Type) derived_type = sptr_new();
    vector_t(TIdentifier) params = vec_new();
    vector_t(shared_ptr_t(Type)) param_types = vec_new();
    CATCH_ENTER;
    if (node->decltor->type != AST_CIdent_t) {
        THROW_AT_LINE(ctx->next_tok->line, GET_PARSER_MSG_0(MSG_derived_fun_decl));
    }

    TIdentifier name;
    vec_reserve(params, vec_size(node->param_list));
    vec_reserve(param_types, vec_size(node->param_list));
    for (size_t i = 0; i < vec_size(node->param_list); ++i) {
        TRY(proc_param_decltor(ctx, node->param_list[i], &params, &param_types));
    }
    name = node->decltor->get._CIdent.name;
    derived_type = make_FunType(&param_types, base_type);
    decltor->name = name;
    sptr_move(Type, derived_type, decltor->derived_type);
    vec_move(params, decltor->params);
    FINALLY;
    vec_delete(params);
    free_Type(&derived_type);
    for (size_t i = 0; i < vec_size(param_types); ++i) {
        free_Type(&param_types[i]);
    }
    vec_delete(param_types);
    CATCH_EXIT;
}

static error_t proc_decltor(Ctx ctx, CDeclarator* node, shared_ptr_t(Type) * base_type, Declarator* decltor) {
    CATCH_ENTER;
    switch (node->type) {
        case AST_CIdent_t:
            proc_ident_decltor(&node->get._CIdent, base_type, decltor);
            break;
        case AST_CPointerDeclarator_t:
            TRY(proc_ptr_decltor(ctx, &node->get._CPointerDeclarator, base_type, decltor));
            break;
        case AST_CArrayDeclarator_t:
            TRY(proc_arr_decltor(ctx, &node->get._CArrayDeclarator, base_type, decltor));
            break;
        case AST_CFunDeclarator_t:
            TRY(proc_fun_decltor(ctx, &node->get._CFunDeclarator, base_type, decltor));
            break;
        default:
            THROW_ABORT;
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t parse_ident_decltor(Ctx ctx, unique_ptr_t(CDeclarator) * decltor) {
    CATCH_ENTER;
    TIdentifier name;
    TRY(parse_identifier(ctx, 0, &name));
    *decltor = make_CIdent(name);
    FINALLY;
    CATCH_EXIT;
}

static error_t parse_simple_decltor(Ctx ctx, unique_ptr_t(CDeclarator) * decltor) {
    CATCH_ENTER;
    TRY(pop_next(ctx));
    TRY(parse_decltor(ctx, decltor));
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_close_paren));
    FINALLY;
    CATCH_EXIT;
}

// <simple-declarator> ::= <identifier> | "(" <declarator> ")"
static error_t parse_simple_decltor_decl(Ctx ctx, unique_ptr_t(CDeclarator) * decltor) {
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
            THROW_AT_LINE(ctx->peek_tok->line, GET_PARSER_MSG(MSG_expect_simple_decltor, str_fmt_tok(ctx->peek_tok)));
    }
    FINALLY;
    CATCH_EXIT;
}

// <param> ::= { <type-specifier> }+ <declarator>
// param_info = Param(type, declarator)
static error_t parse_param(Ctx ctx, unique_ptr_t(CParam) * param) {
    unique_ptr_t(CDeclarator) decltor = uptr_new();
    shared_ptr_t(Type) param_type = sptr_new();
    CATCH_ENTER;
    TRY(parse_type_specifier(ctx, &param_type));
    TRY(parse_decltor(ctx, &decltor));
    *param = make_CParam(&decltor, &param_type);
    FINALLY;
    free_CDeclarator(&decltor);
    free_Type(&param_type);
    CATCH_EXIT;
}

static error_t parse_empty_param_list(Ctx ctx) {
    CATCH_ENTER;
    TRY(pop_next(ctx));
    FINALLY;
    CATCH_EXIT;
}

static error_t parse_non_empty_param_list(Ctx ctx, vector_t(unique_ptr_t(CParam)) * param_list) {
    unique_ptr_t(CParam) param = uptr_new();
    CATCH_ENTER;
    TRY(parse_param(ctx, &param));
    vec_move_back(*param_list, param);
    TRY(peek_next(ctx));
    while (ctx->peek_tok->tok_kind == TOK_comma_separator) {
        TRY(pop_next(ctx));
        TRY(parse_param(ctx, &param));
        vec_move_back(*param_list, param);
        TRY(peek_next(ctx));
    }
    FINALLY;
    free_CParam(&param);
    CATCH_EXIT;
}

// <param-list> ::= "(" "void" ")" | "(" <param> { "," <param> } ")"
static error_t parse_param_list(Ctx ctx, vector_t(unique_ptr_t(CParam)) * param_list) {
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
            THROW_AT_LINE(ctx->peek_tok->line, GET_PARSER_MSG(MSG_expect_param_list, str_fmt_tok(ctx->peek_tok)));
    }
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_close_paren));
    FINALLY;
    CATCH_EXIT;
}

// (fun) <declarator-suffix> ::= <param-list>
static error_t parse_fun_decltor_suffix(Ctx ctx, unique_ptr_t(CDeclarator) * decltor) {
    vector_t(unique_ptr_t(CParam)) param_list = vec_new();
    CATCH_ENTER;
    TRY(parse_param_list(ctx, &param_list));
    *decltor = make_CFunDeclarator(&param_list, decltor);
    FINALLY;
    for (size_t i = 0; i < vec_size(param_list); ++i) {
        free_CParam(&param_list[i]);
    }
    vec_delete(param_list);
    CATCH_EXIT;
}

// (array) <declarator-suffix> ::= { "[" <const> "]" }+
static error_t parse_arr_decltor_suffix(Ctx ctx, unique_ptr_t(CDeclarator) * decltor) {
    CATCH_ENTER;
    do {
        TLong size;
        TRY(parse_arr_size(ctx, &size));
        *decltor = make_CArrayDeclarator(size, decltor);
        TRY(peek_next(ctx));
    }
    while (ctx->peek_tok->tok_kind == TOK_open_bracket);
    FINALLY;
    CATCH_EXIT;
}

// <direct-declarator> ::= <simple-declarator> [ <declarator-suffix> ]
static error_t parse_direct_decltor(Ctx ctx, unique_ptr_t(CDeclarator) * decltor) {
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
    FINALLY;
    CATCH_EXIT;
}

static error_t parse_ptr_decltor(Ctx ctx, unique_ptr_t(CDeclarator) * decltor) {
    CATCH_ENTER;
    TRY(pop_next(ctx));
    TRY(parse_decltor(ctx, decltor));
    *decltor = make_CPointerDeclarator(decltor);
    FINALLY;
    CATCH_EXIT;
}

// <declarator> ::= "*" <declarator> | <direct-declarator>
// declarator = Ident(identifier) | PointerDeclarator(declarator) | ArrayDeclarator(int, declarator)
//            | FunDeclarator(param_info*, declarator)
static error_t parse_decltor(Ctx ctx, unique_ptr_t(CDeclarator) * decltor) {
    CATCH_ENTER;
    TRY(peek_next(ctx));
    if (ctx->peek_tok->tok_kind == TOK_binop_multiply) {
        TRY(parse_ptr_decltor(ctx, decltor));
    }
    else {
        TRY(parse_direct_decltor(ctx, decltor));
    }
    FINALLY;
    CATCH_EXIT;
}

// <function-declaration> ::= { <specifier> }+ <declarator> ( <block> | ";")
// function_declaration = FunctionDeclaration(identifier, identifier*, block?, type, storage_class?)
static error_t parse_fun_declaration(Ctx ctx, unique_ptr_t(CStorageClass) * storage_class, Declarator* decltor,
    unique_ptr_t(CFunctionDeclaration) * fun_decl) {
    unique_ptr_t(CBlock) body = uptr_new();
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
    *fun_decl =
        make_CFunctionDeclaration(decltor->name, &decltor->params, &body, &decltor->derived_type, storage_class, line);
    FINALLY;
    free_CBlock(&body);
    CATCH_EXIT;
}

// <variable-declaration> ::= { <specifier> }+ <declarator> [ "=" <initializer> ] ";"
// variable_declaration = VariableDeclaration(identifier, initializer?, type, storage_class?)
static error_t parse_var_declaration(Ctx ctx, unique_ptr_t(CStorageClass) * storage_class, Declarator* decltor,
    unique_ptr_t(CVariableDeclaration) * var_decl) {
    unique_ptr_t(CInitializer) initializer = uptr_new();
    CATCH_ENTER;
    size_t line = ctx->next_tok->line;
    TRY(peek_next(ctx));
    if (ctx->peek_tok->tok_kind == TOK_assign) {
        TRY(pop_next(ctx));
        TRY(parse_initializer(ctx, &initializer));
    }
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_semicolon));
    *var_decl = make_CVariableDeclaration(decltor->name, &initializer, &decltor->derived_type, storage_class, line);
    FINALLY;
    free_CInitializer(&initializer);
    CATCH_EXIT;
}

// <member-declaration> ::= { <type-specifier> }+ <declarator> ";"
// member_declaration = MemberDeclaration(identifier, type)
static error_t parse_member_declaration(Ctx ctx, unique_ptr_t(CMemberDeclaration) * member_decl) {
    Declarator decltor = {0, sptr_new(), vec_new()};
    unique_ptr_t(CStorageClass) storage_class = uptr_new();
    CATCH_ENTER;
    size_t line;
    TRY(parse_decltor_decl(ctx, &decltor, &storage_class));
    if (storage_class) {
        THROW_AT_LINE(ctx->next_tok->line,
            GET_PARSER_MSG(MSG_member_decl_not_auto, map_get(ctx->identifiers->hash_table, decltor.name),
                get_storage_class_fmt(storage_class)));
    }
    if (decltor.derived_type->type == AST_FunType_t) {
        THROW_AT_LINE(ctx->next_tok->line,
            GET_PARSER_MSG(MSG_member_decl_as_fun, map_get(ctx->identifiers->hash_table, decltor.name)));
    }
    line = ctx->next_tok->line;
    TRY(pop_next(ctx));
    TRY(expect_next(ctx, ctx->next_tok, TOK_semicolon));
    *member_decl = make_CMemberDeclaration(decltor.name, &decltor.derived_type, line);
    FINALLY;
    free_Type(&decltor.derived_type);
    vec_delete(decltor.params);
    free_CStorageClass(&storage_class);
    CATCH_EXIT;
}

// <struct-declaration> ::= ("struct" | "union") <identifier> [ "{" { <member-declaration> }+ "}" ] ";"
// struct_declaration = StructDeclaration(identifier, bool, member_declaration*)
static error_t parse_struct_declaration(Ctx ctx, unique_ptr_t(CStructDeclaration) * struct_decl) {
    unique_ptr_t(CMemberDeclaration) member = uptr_new();
    vector_t(unique_ptr_t(CMemberDeclaration)) members = vec_new();
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
            vec_move_back(members, member);
            TRY(peek_next(ctx));
        }
        while (ctx->peek_tok->tok_kind != TOK_close_brace);
        TRY(pop_next(ctx));
        TRY(pop_next(ctx));
    }
    TRY(expect_next(ctx, ctx->next_tok, TOK_semicolon));
    *struct_decl = make_CStructDeclaration(tag, is_union, &members, line);
    FINALLY;
    free_CMemberDeclaration(&member);
    for (size_t i = 0; i < vec_size(members); ++i) {
        free_CMemberDeclaration(&members[i]);
    }
    vec_delete(members);
    CATCH_EXIT;
}

static error_t parse_fun_decl(Ctx ctx, unique_ptr_t(CStorageClass) * storage_class, Declarator* decltor,
    unique_ptr_t(CDeclaration) * declaration) {
    unique_ptr_t(CFunctionDeclaration) fun_decl = uptr_new();
    CATCH_ENTER;
    TRY(parse_fun_declaration(ctx, storage_class, decltor, &fun_decl));
    *declaration = make_CFunDecl(&fun_decl);
    FINALLY;
    free_CFunctionDeclaration(&fun_decl);
    CATCH_EXIT;
}

static error_t parse_var_decl(Ctx ctx, unique_ptr_t(CStorageClass) * storage_class, Declarator* decltor,
    unique_ptr_t(CDeclaration) * declaration) {
    unique_ptr_t(CVariableDeclaration) var_decl = uptr_new();
    CATCH_ENTER;
    TRY(parse_var_declaration(ctx, storage_class, decltor, &var_decl));
    *declaration = make_CVarDecl(&var_decl);
    FINALLY;
    free_CVariableDeclaration(&var_decl);
    CATCH_EXIT;
}

static error_t parse_struct_decl(Ctx ctx, unique_ptr_t(CDeclaration) * declaration) {
    unique_ptr_t(CStructDeclaration) struct_decl = uptr_new();
    CATCH_ENTER;
    TRY(parse_struct_declaration(ctx, &struct_decl));
    *declaration = make_CStructDecl(&struct_decl);
    FINALLY;
    free_CStructDeclaration(&struct_decl);
    CATCH_EXIT;
}

static error_t parse_decltor_decl(Ctx ctx, Declarator* decltor, unique_ptr_t(CStorageClass) * storage_class) {
    unique_ptr_t(CDeclarator) decltor_1 = uptr_new();
    shared_ptr_t(Type) type_specifier = sptr_new();
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
    TRY(proc_decltor(ctx, decltor_1, &type_specifier, decltor));
    FINALLY;
    free_CDeclarator(&decltor_1);
    free_Type(&type_specifier);
    CATCH_EXIT;
}

// <declaration> ::= <variable-declaration> | <function-declaration> | <struct-declaration>
// declaration = FunDecl(function_declaration) | VarDecl(variable_declaration) | StructDecl(struct_declaration)
static error_t parse_declaration(Ctx ctx, unique_ptr_t(CDeclaration) * declaration) {
    Declarator decltor = {0, sptr_new(), vec_new()};
    unique_ptr_t(CStorageClass) storage_class = uptr_new();
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
                    EARLY_EXIT;
                default:
                    break;
            }
        }
        default:
            break;
    }
    TRY(parse_decltor_decl(ctx, &decltor, &storage_class));
    if (decltor.derived_type->type == AST_FunType_t) {
        TRY(parse_fun_decl(ctx, &storage_class, &decltor, declaration));
    }
    else {
        TRY(parse_var_decl(ctx, &storage_class, &decltor, declaration));
    }
    FINALLY;
    free_Type(&decltor.derived_type);
    vec_delete(decltor.params);
    free_CStorageClass(&storage_class);
    CATCH_EXIT;
}

// <program> ::= { <declaration> }
// AST = Program(declaration*)
static error_t parse_program(Ctx ctx, unique_ptr_t(CProgram) * c_ast) {
    unique_ptr_t(CDeclaration) declaration = uptr_new();
    vector_t(unique_ptr_t(CDeclaration)) declarations = vec_new();
    CATCH_ENTER;
    while (ctx->pop_idx < vec_size(*ctx->p_toks)) {
        TRY(parse_declaration(ctx, &declaration));
        vec_move_back(declarations, declaration);
    }
    *c_ast = make_CProgram(&declarations);
    FINALLY;
    free_CDeclaration(&declaration);
    for (size_t i = 0; i < vec_size(declarations); ++i) {
        free_CDeclaration(&declarations[i]);
    }
    vec_delete(declarations);
    CATCH_EXIT;
}

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

error_t parse_tokens(
    vector_t(Token) * tokens, ErrorsContext* errors, IdentifierContext* identifiers, unique_ptr_t(CProgram) * c_ast) {
    ParserContext ctx;
    {
        ctx.errors = errors;
        ctx.identifiers = identifiers;
        ctx.pop_idx = 0;
        ctx.p_toks = tokens;
    }
    CATCH_ENTER;
    TRY(parse_program(&ctx, c_ast));
    THROW_ABORT_IF(ctx.pop_idx != vec_size(*tokens));

    THROW_ABORT_IF(!*c_ast);
    FINALLY;
    vec_delete(*tokens);
    CATCH_EXIT;
}
