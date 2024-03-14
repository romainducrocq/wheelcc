#include "frontend/parser/parser.hpp"
#include "util/error.hpp"
#include "util/str2t.hpp"
#include "ast/ast.hpp"
#include "ast/front_symt.hpp"
#include "ast/front_ast.hpp"
#include "frontend/parser/lexer.hpp"
#include "frontend/parser/preced.hpp"

#include <inttypes.h>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include <unordered_map>

static std::unordered_map<TOKEN_KIND, std::string> TOKEN_HUMAN_READABLE = {
    {TOKEN_KIND::assignment_bitshiftleft, "<<="},
    {TOKEN_KIND::assignment_bitshiftright, ">>="},

    {TOKEN_KIND::unop_decrement, "--"},
    {TOKEN_KIND::binop_bitshiftleft, "<<"},
    {TOKEN_KIND::binop_bitshiftright, ">>"},
    {TOKEN_KIND::binop_and, "&&"},
    {TOKEN_KIND::binop_or, "||"},
    {TOKEN_KIND::binop_equalto, "=="},
    {TOKEN_KIND::binop_notequal, "!="},
    {TOKEN_KIND::binop_lessthanorequal, "<="},
    {TOKEN_KIND::binop_greaterthanorequal, ">="},
    {TOKEN_KIND::assignment_plus, "+="},
    {TOKEN_KIND::assignment_difference, "-="},
    {TOKEN_KIND::assignment_product, "*="},
    {TOKEN_KIND::assignment_quotient, "/="},
    {TOKEN_KIND::assignment_remainder, "%="},
    {TOKEN_KIND::assignment_bitand, "&="},
    {TOKEN_KIND::assignment_bitor, "|="},
    {TOKEN_KIND::assignment_bitxor, "^="},

    {TOKEN_KIND::parenthesis_open, "("},
    {TOKEN_KIND::parenthesis_close, ")"},
    {TOKEN_KIND::brace_open, "{"},
    {TOKEN_KIND::brace_close, "}"},
    {TOKEN_KIND::brackets_open, "["},
    {TOKEN_KIND::brackets_close, "]"},
    {TOKEN_KIND::semicolon, ";"},
    {TOKEN_KIND::unop_complement, "~"},
    {TOKEN_KIND::unop_negation, "-"},
    {TOKEN_KIND::unop_not, "!"},
    {TOKEN_KIND::binop_addition, "+"},
    {TOKEN_KIND::binop_multiplication, "*"},
    {TOKEN_KIND::binop_division, "/"},
    {TOKEN_KIND::binop_remainder, "%"},
    {TOKEN_KIND::binop_bitand, "&"},
    {TOKEN_KIND::binop_bitor, "|"},
    {TOKEN_KIND::binop_bitxor, "^"},
    {TOKEN_KIND::binop_lessthan, "<"},
    {TOKEN_KIND::binop_greaterthan, ">"},
    {TOKEN_KIND::assignment_simple, "="},
    {TOKEN_KIND::ternary_if, "?"},
    {TOKEN_KIND::ternary_else, ":"},
    {TOKEN_KIND::separator_comma, ","},

    {TOKEN_KIND::key_char, "char"},
    {TOKEN_KIND::key_int, "int"},
    {TOKEN_KIND::key_long, "long"},
    {TOKEN_KIND::key_double, "double"},
    {TOKEN_KIND::key_signed, "signed"},
    {TOKEN_KIND::key_unsigned, "unsigned"},
    {TOKEN_KIND::key_void, "void"},
    {TOKEN_KIND::key_return, "return"},
    {TOKEN_KIND::key_if, "if"},
    {TOKEN_KIND::key_else, "else"},
    {TOKEN_KIND::key_goto, "goto"},
    {TOKEN_KIND::key_do, "do"},
    {TOKEN_KIND::key_while, "while"},
    {TOKEN_KIND::key_for, "for"},
    {TOKEN_KIND::key_break, "break"},
    {TOKEN_KIND::key_continue, "continue"},
    {TOKEN_KIND::key_static, "static"},
    {TOKEN_KIND::key_extern, "extern"},
    
    {TOKEN_KIND::identifier, "identifier"},
    {TOKEN_KIND::string_literal, "string literal"},
    {TOKEN_KIND::char_constant, "const char8"},
    {TOKEN_KIND::float_constant, "const float64"},
    {TOKEN_KIND::unsigned_long_constant, "const uint64"},
    {TOKEN_KIND::unsigned_constant, "const uint32"},
    {TOKEN_KIND::long_constant, "const int64"},
    {TOKEN_KIND::constant, "const int32"}
};

static std::vector<Token>* p_tokens;
static Token* next_token;
static Token* peek_token;
static size_t pop_index = 0;

static void expect_next_is(const Token& next_token_is, TOKEN_KIND expected_token) {
    if(next_token_is.token_kind != expected_token) {
        raise_runtime_error_at_line("Expected token kind " + em(TOKEN_HUMAN_READABLE[expected_token]) +
                                    " but found token " + em(next_token_is.token), next_token_is.line);
    }
}

static const Token& pop_next() {
    if(pop_index >= p_tokens->size()) {
        raise_runtime_error_at_line("All Tokens were consumed before end of program",
                                    p_tokens->back().line);
    }

    next_token = &(*p_tokens)[pop_index];
    pop_index++;
    return *next_token;
}

static const Token& pop_next_i(size_t i) {
    if(i == 0) {
        return pop_next();
    }
    if(pop_index + i >= p_tokens->size()) {
        raise_runtime_error_at_line("All Tokens were consumed before end of program",
                                    p_tokens->back().line);
    }

    if(i == 1) {
        std::swap((*p_tokens)[pop_index], (*p_tokens)[pop_index+1]);
    }
    else {
        Token swap_token_i = std::move((*p_tokens)[pop_index+i]);
        for(size_t j = pop_index + i; j-- > pop_index;) {
            (*p_tokens)[j+1] = std::move((*p_tokens)[j]);
        }
        (*p_tokens)[pop_index] = std::move(swap_token_i);
    }
    pop_next();
    return (*p_tokens)[pop_index-1];
}

static const Token& peek_next() {
    if(pop_index >= p_tokens->size()) {
        raise_runtime_error_at_line("All Tokens were consumed before end of program",
                                    p_tokens->back().line);
    }

    peek_token = &(*p_tokens)[pop_index];
    return *peek_token;
}

static const Token& peek_next_i(size_t i) {
    if(i == 0) {
        return peek_next();
    }
    if(pop_index + i >= p_tokens->size()) {
        raise_runtime_error_at_line("All Tokens were consumed before end of program",
                                    p_tokens->back().line);
    }

    return (*p_tokens)[pop_index + i];
}

// <identifier> ::= ? An identifier token ?
static void parse_identifier(TIdentifier& identifier) {
    expect_next_is(pop_next(), TOKEN_KIND::identifier);
    identifier = std::move(next_token->token);
}

// string = StringLiteral(int*)
// <string> ::= ? A string token ?
static std::shared_ptr<CStringLiteral> parse_string_literal() {
    std::vector<TInt> value;
    string_to_string_literal(next_token->token, value);
    while(peek_next().token_kind == TOKEN_KIND::string_literal) {
        pop_next();
        string_to_string_literal(next_token->token, value);
    }
    return std::make_shared<CStringLiteral>(std::move(value));
}

// <int> ::= ? An int constant token ?
static std::shared_ptr<CConstInt> parse_int_constant(intmax_t intmax) {
    TInt value = intmax_to_int32(intmax);
    return std::make_shared<CConstInt>(std::move(value));
}

// <char> ::= ? A char token ?
static std::shared_ptr<CConstInt> parse_char_constant() {
    TInt value = string_to_char_ascii(next_token->token);
    return std::make_shared<CConstInt>(std::move(value));
}

// <long> ::= ? An int or long constant token ?
static std::shared_ptr<CConstLong> parse_long_constant(intmax_t intmax) {
    TLong value = intmax_to_int64(intmax);
    return std::make_shared<CConstLong>(std::move(value));
}

// <double> ::= ? A floating-point constant token ?
static std::shared_ptr<CConstDouble> parse_double_constant() {
    TDouble value = string_to_double(next_token->token, next_token->line);
    return std::make_shared<CConstDouble>(std::move(value));
}

// <unsigned int> ::= ? An unsigned int constant token ?
static std::shared_ptr<CConstUInt> parse_uint_constant(uintmax_t uintmax) {
    TUInt value = uintmax_to_uint32(uintmax);
    return std::make_shared<CConstUInt>(std::move(value));
}

// <unsigned long> ::= ? An unsigned int or unsigned long constant token ?
static std::shared_ptr<CConstULong> parse_ulong_constant(uintmax_t uintmax) {
    TULong value = uintmax_to_uint64(uintmax);
    return std::make_shared<CConstULong>(std::move(value));
}

// <const> ::= <int> | <long> | <double> | <char>
// (signed) const = ConstInt(int) | ConstLong(long) | ConstDouble(double) | ConstChar(int)
static std::shared_ptr<CConst> parse_constant() {
    switch(pop_next().token_kind) {
        case TOKEN_KIND::long_constant:
            next_token->token.pop_back();
            break;
        case TOKEN_KIND::char_constant:
            return parse_char_constant();
        case TOKEN_KIND::float_constant:
            return parse_double_constant();
        default:
            break;
    }

    intmax_t value = string_to_intmax(next_token->token, next_token->line);
    if(value > 9223372036854775807ll) {
        raise_runtime_error_at_line("Constant " + em(next_token->token) +
                                    " is too large to be represent as an int or a long", next_token->line);
    }
    if(next_token->token_kind == TOKEN_KIND::constant && value <= 2147483647l) {
        return parse_int_constant(std::move(value));
    }
    return parse_long_constant(std::move(value));
}

// <const> ::= <unsigned int> | <unsigned long>
// (unsigned) const = ConstUInt(uint) | ConstULong(ulong) | ConstUChar(int)
static std::shared_ptr<CConst> parse_unsigned_constant() {
    if(pop_next().token_kind == TOKEN_KIND::unsigned_long_constant) {
        next_token->token.pop_back();
    }
    next_token->token.pop_back();

    uintmax_t value = string_to_uintmax(next_token->token, next_token->line);
    if(value > 18446744073709551615ull) {
        raise_runtime_error_at_line("Constant " + em(next_token->token) +
                                    " is too large to be represented as an unsigned int or a unsigned long",
                                    next_token->line);
    }
    if(next_token->token_kind == TOKEN_KIND::unsigned_constant && value <= 4294967295ul) {
        return parse_uint_constant(std::move(value));
    }
    return parse_ulong_constant(std::move(value));
}

static TLong parse_array_size_t() {
    pop_next();
    std::shared_ptr<CConst> size;
    switch(peek_next().token_kind) {
        case TOKEN_KIND::constant:
        case TOKEN_KIND::long_constant:
        case TOKEN_KIND::char_constant:
            size = parse_constant();
            break;
        case TOKEN_KIND::unsigned_constant:
        case TOKEN_KIND::unsigned_long_constant:
            size = parse_unsigned_constant();
            break;
        default:
            raise_runtime_error_at_line("Size of array declarator suffix must be a constant integer",
                                        peek_token->line);
    }
    expect_next_is(pop_next(), TOKEN_KIND::brackets_close);
    switch(size->type()) {
        case AST_T::CConstInt_t:
            return static_cast<TLong>(static_cast<CConstInt*>(size.get())->value);
        case AST_T::CConstLong_t:
            return static_cast<CConstLong*>(size.get())->value;
        case AST_T::CConstUInt_t:
            return static_cast<TLong>(static_cast<CConstUInt*>(size.get())->value);
        case AST_T::CConstULong_t:
            return static_cast<TLong>(static_cast<CConstULong*>(size.get())->value);
        default:
            RAISE_INTERNAL_ERROR;
    }
}

// <unop> ::= "-" | "~" | "!" | "*" | "&"
// unary_operator = Complement | Negate | Not
static std::unique_ptr<CUnaryOp> parse_unary_op() {
    switch(pop_next().token_kind) {
        case TOKEN_KIND::unop_complement:
            return std::make_unique<CComplement>();
        case TOKEN_KIND::unop_negation:
            return std::make_unique<CNegate>();
        case TOKEN_KIND::unop_not:
            return std::make_unique<CNot>();
        default:
            raise_runtime_error_at_line("Expected token type " + em("unary operator") +
                                        " but found token " + em(next_token->token), next_token->line);
    }
}

// binop> ::= "-" | "+" | "*" | "/" | "%" | "&" | "|" | "^" | "<<" | ">>" | "&&" | "||" | "==" | "!="
//          | "<" | "<=" | ">" | ">="
// binary_operator = Add | Subtract | Multiply | Divide | Remainder | BitAnd | BitOr | BitXor | BitShiftLeft
//                 | BitShiftRight | And | Or | Equal | NotEqual | LessThan | LessOrEqual | GreaterThan | GreaterOrEqual
static std::unique_ptr<CBinaryOp> parse_binary_op() {
    switch(pop_next().token_kind) {
        case TOKEN_KIND::binop_addition:
        case TOKEN_KIND::assignment_plus:
            return std::make_unique<CAdd>();
        case TOKEN_KIND::unop_negation:
        case TOKEN_KIND::assignment_difference:
            return std::make_unique<CSubtract>();
        case TOKEN_KIND::binop_multiplication:
        case TOKEN_KIND::assignment_product:
            return std::make_unique<CMultiply>();
        case TOKEN_KIND::binop_division:
        case TOKEN_KIND::assignment_quotient:
            return std::make_unique<CDivide>();
        case TOKEN_KIND::binop_remainder:
        case TOKEN_KIND::assignment_remainder:
            return std::make_unique<CRemainder>();
        case TOKEN_KIND::binop_bitand:
        case TOKEN_KIND::assignment_bitand:
            return std::make_unique<CBitAnd>();
        case TOKEN_KIND::binop_bitor:
        case TOKEN_KIND::assignment_bitor:
            return std::make_unique<CBitOr>();
        case TOKEN_KIND::binop_bitxor:
        case TOKEN_KIND::assignment_bitxor:
            return std::make_unique<CBitXor>();
        case TOKEN_KIND::binop_bitshiftleft:
        case TOKEN_KIND::assignment_bitshiftleft:
            return std::make_unique<CBitShiftLeft>();
        case TOKEN_KIND::binop_bitshiftright:
        case TOKEN_KIND::assignment_bitshiftright:
            return std::make_unique<CBitShiftRight>();
        case TOKEN_KIND::binop_and:
            return std::make_unique<CAnd>();
        case TOKEN_KIND::binop_or:
            return std::make_unique<COr>();
        case TOKEN_KIND::binop_equalto:
            return std::make_unique<CEqual>();
        case TOKEN_KIND::binop_notequal:
            return std::make_unique<CNotEqual>();
        case TOKEN_KIND::binop_lessthan:
            return std::make_unique<CLessThan>();
        case TOKEN_KIND::binop_lessthanorequal:
            return std::make_unique<CLessOrEqual>();
        case TOKEN_KIND::binop_greaterthan:
            return std::make_unique<CGreaterThan>();
        case TOKEN_KIND::binop_greaterthanorequal:
            return std::make_unique<CGreaterOrEqual>();
        default:
            raise_runtime_error_at_line("Expected token type " + em("binary operator") +
                                        " but found token " + em(next_token->token), next_token->line);
    }
}

static void parse_process_abstract_declarator(CAbstractDeclarator* node, std::shared_ptr<Type> base_type,
                                              AbstractDeclarator& abstract_declarator);

static void parse_process_pointer_abstract_declarator(CAbstractPointer* node, std::shared_ptr<Type> base_type,
                                                      AbstractDeclarator& abstract_declarator) {
    std::shared_ptr<Type> derived_type = std::make_shared<Pointer>(std::move(base_type));
    parse_process_abstract_declarator(node->abstract_declarator.get(), std::move(derived_type),
                                      abstract_declarator);
}

static void parse_process_array_abstract_declarator(CAbstractArray* node, std::shared_ptr<Type> base_type,
                                                    AbstractDeclarator& abstract_declarator) {
    TLong size = node->size;
    std::shared_ptr<Type> derived_type = std::make_shared<Array>(std::move(size), std::move(base_type));
    parse_process_abstract_declarator(node->abstract_declarator.get(), std::move(derived_type),
                                      abstract_declarator);
}

static void parse_process_base_abstract_declarator(std::shared_ptr<Type> base_type,
                                                   AbstractDeclarator& abstract_declarator) {
    abstract_declarator.derived_type = std::move(base_type);
}

static void parse_process_abstract_declarator(CAbstractDeclarator* node, std::shared_ptr<Type> base_type,
                                              AbstractDeclarator& abstract_declarator) {
    switch(node->type()) {
        case AST_T::CAbstractPointer_t:
            parse_process_pointer_abstract_declarator(static_cast<CAbstractPointer*>(node), std::move(base_type),
                                                      abstract_declarator);
            break;
        case AST_T::CAbstractArray_t:
            parse_process_array_abstract_declarator(static_cast<CAbstractArray*>(node), std::move(base_type),
                                                    abstract_declarator);
            break;
        case AST_T::CAbstractBase_t:
            parse_process_base_abstract_declarator(std::move(base_type), abstract_declarator);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::unique_ptr<CAbstractDeclarator> parse_abstract_declarator();

// (array) <direct-abstract-declarator> ::= { "[" <const> "]" }+
static std::unique_ptr<CAbstractDeclarator> parse_array_direct_abstract_declarator() {
    std::unique_ptr<CAbstractDeclarator> abstract_declarator = std::make_unique<CAbstractBase>();
    do {
        TLong size = parse_array_size_t();
        abstract_declarator = std::make_unique<CAbstractArray>(std::move(size), std::move(abstract_declarator));
    } while(peek_next().token_kind == TOKEN_KIND::brackets_open);
    return abstract_declarator;
}

// (direct) <direct-abstract-declarator> ::= "(" <abstract-declarator> ")" { "[" <const> "]" }
static std::unique_ptr<CAbstractDeclarator> parse_direct_abstract_declarator() {
    pop_next();
    std::unique_ptr<CAbstractDeclarator> abstract_declarator = parse_abstract_declarator();
    expect_next_is(pop_next(), TOKEN_KIND::parenthesis_close);
    while(peek_next().token_kind == TOKEN_KIND::brackets_open) {
        TLong size = parse_array_size_t();
        abstract_declarator = std::make_unique<CAbstractArray>(std::move(size), std::move(abstract_declarator));
    }
    return abstract_declarator;
}

static std::unique_ptr<CAbstractPointer> parse_pointer_abstract_declarator() {
    pop_next();
    std::unique_ptr<CAbstractDeclarator> abstract_declarator;
    if(peek_next().token_kind == TOKEN_KIND::parenthesis_close) {
        abstract_declarator = std::make_unique<CAbstractBase>();
    }
    else {
        abstract_declarator = parse_abstract_declarator();
    }
    return std::make_unique<CAbstractPointer>(std::move(abstract_declarator));
}

// <abstract-declarator> ::= "*" [ <abstract-declarator> ] | <direct-abstract-declarator>
// abstract_declarator = AbstractPointer(abstract_declarator) | AbstractArray(int, abstract_declarator) | AbstractBase
static std::unique_ptr<CAbstractDeclarator> parse_abstract_declarator() {
    switch(peek_next().token_kind) {
        case TOKEN_KIND::binop_multiplication:
            return parse_pointer_abstract_declarator();
        case TOKEN_KIND::parenthesis_open:
            return parse_direct_abstract_declarator();
        case TOKEN_KIND::brackets_open:
            return parse_array_direct_abstract_declarator();
        default:
            raise_runtime_error_at_line("Expected token type " + em("abstract declarator") +
                                        " but found token " + em(peek_token->token),
                                        peek_token->line);
    }
}

static std::shared_ptr<Type> parse_type_specifier();

static std::unique_ptr<CExp> parse_unary_exp_factor();
static std::unique_ptr<CExp> parse_exp(int32_t min_precedence);

// <argument-list> ::= <exp> { "," <exp> }
static std::vector<std::unique_ptr<CExp>> parse_argument_list() {
    std::vector<std::unique_ptr<CExp>> args;
    {
        std::unique_ptr<CExp> arg = parse_exp(0);
        args.push_back(std::move(arg));
    }
    while(peek_next().token_kind == TOKEN_KIND::separator_comma) {
        pop_next();
        std::unique_ptr<CExp> arg = parse_exp(0);
        args.push_back(std::move(arg));
    }
    return args;
}

static std::unique_ptr<CVar> parse_var_factor() {
    TIdentifier name; parse_identifier(name);
    return std::make_unique<CVar>(std::move(name));
}

static std::unique_ptr<CString> parse_string_literal_factor() {
    std::shared_ptr<CStringLiteral> literal = parse_string_literal();
    return std::make_unique<CString>(std::move(literal));
}

static void parse_abstract_declarator_cast_factor(std::shared_ptr<Type>& target_type) {
    AbstractDeclarator abstract_declarator;
    parse_process_abstract_declarator(parse_abstract_declarator().get(), std::move(target_type),
                                      abstract_declarator);
    target_type = std::move(abstract_declarator.derived_type);
}

// "(" { <type-specifier> }+ [ <abstract-declarator> ] ")" <factor>
static std::unique_ptr<CCast> parse_cast_factor() {
    pop_next();
    std::shared_ptr<Type> target_type = parse_type_specifier();
    switch(peek_next().token_kind) {
        case TOKEN_KIND::binop_multiplication:
        case TOKEN_KIND::parenthesis_open:
        case TOKEN_KIND::brackets_open:
            parse_abstract_declarator_cast_factor(target_type);
        default:
            break;
    }
    expect_next_is(pop_next(), TOKEN_KIND::parenthesis_close);
    std::unique_ptr<CExp> exp = parse_unary_exp_factor();
    return std::make_unique<CCast>(std::move(exp), std::move(target_type));
}

static std::unique_ptr<CConstant> parse_constant_factor() {
    std::shared_ptr<CConst> constant = parse_constant();
    return std::make_unique<CConstant>(std::move(constant));
}

static std::unique_ptr<CConstant> parse_unsigned_constant_factor() {
    std::shared_ptr<CConst> constant = parse_unsigned_constant();
    return std::make_unique<CConstant>(std::move(constant));
}

static std::unique_ptr<CUnary> parse_unary_factor() {
    std::unique_ptr<CUnaryOp> unary_op = parse_unary_op();
    std::unique_ptr<CExp> exp = parse_unary_exp_factor();
    return std::make_unique<CUnary>(std::move(unary_op), std::move(exp));
}

static std::unique_ptr<CExp> parse_inner_exp_factor() {
    pop_next();
    std::unique_ptr<CExp> inner_exp = parse_exp(0);
    expect_next_is(pop_next(), TOKEN_KIND::parenthesis_close);
    return inner_exp;
}

static std::unique_ptr<CFunctionCall> parse_function_call_factor() {
    TIdentifier name; parse_identifier(name);
    expect_next_is(pop_next(), TOKEN_KIND::parenthesis_open);
    std::vector<std::unique_ptr<CExp>> args;
    if(peek_next().token_kind != TOKEN_KIND::parenthesis_close) {
        args = parse_argument_list();
    }
    expect_next_is(pop_next(), TOKEN_KIND::parenthesis_close);
    return std::make_unique<CFunctionCall>(std::move(name), std::move(args));
}

static std::unique_ptr<CExp> parse_pointer_factor() {
    switch(pop_next().token_kind) {
        case TOKEN_KIND::binop_multiplication: {
            std::unique_ptr<CExp> exp = parse_unary_exp_factor();
            return std::make_unique<CDereference>(std::move(exp));
        }
        case TOKEN_KIND::binop_bitand: {
            std::unique_ptr<CExp> exp = parse_unary_exp_factor();
            return std::make_unique<CAddrOf>(std::move(exp));
        }
        default:
            raise_runtime_error_at_line("Expected token type " + em("unary operator") +
                                        " but found token " + em(next_token->token), next_token->line);
    }
}

// <primary-exp> ::= <const> | <identifier> | "(" <exp> ")" | { <string> }+ | <identifier> "(" [ <argument-list> ] ")"
static std::unique_ptr<CExp> parse_primary_exp_factor() {
    switch(peek_next().token_kind) {
        case TOKEN_KIND::constant:
        case TOKEN_KIND::long_constant:
        case TOKEN_KIND::char_constant:
        case TOKEN_KIND::float_constant:
            return parse_constant_factor();
        case TOKEN_KIND::unsigned_constant:
        case TOKEN_KIND::unsigned_long_constant:
            return parse_unsigned_constant_factor();
        case TOKEN_KIND::identifier: {
            if(peek_next_i(1).token_kind == TOKEN_KIND::parenthesis_open) {
                return parse_function_call_factor();
            }
            return parse_var_factor();
        }
        case TOKEN_KIND::string_literal:
            return parse_string_literal_factor();
        case TOKEN_KIND::parenthesis_open:
            return parse_inner_exp_factor();
        default:
            break;
    }
    raise_runtime_error_at_line("Expected token type " + em("factor") +
                                " but found token " + em(next_token->token), next_token->line);
}

// <postfix-exp> ::= <primary-exp> { "[" <exp> "]" }
static std::unique_ptr<CExp> parse_postfix_exp_factor() {
    std::unique_ptr<CExp> primary_exp = parse_primary_exp_factor();
    while(peek_next().token_kind == TOKEN_KIND::brackets_open) {
        pop_next();
        std::unique_ptr<CExp> subscript_exp = parse_exp(0);
        expect_next_is(pop_next(), TOKEN_KIND::brackets_close);
        primary_exp = std::make_unique<CSubscript>(std::move(primary_exp), std::move(subscript_exp));
    }
    return primary_exp;
}

// <unary-exp> ::= <unop> <unary-exp> | "(" { <type-specifier> }+ [ <abstract-declarator> ] ")" <unary-exp>
//               | <postfix-exp>
static std::unique_ptr<CExp> parse_unary_exp_factor() {
    switch(peek_next().token_kind) {
        case TOKEN_KIND::unop_complement:
        case TOKEN_KIND::unop_negation:
        case TOKEN_KIND::unop_not:
            return parse_unary_factor();
        case TOKEN_KIND::binop_multiplication:
        case TOKEN_KIND::binop_bitand:
            return parse_pointer_factor();
        case TOKEN_KIND::parenthesis_open: {
            switch(peek_next_i(1).token_kind) {
                case TOKEN_KIND::key_char:
                case TOKEN_KIND::key_int:
                case TOKEN_KIND::key_long:
                case TOKEN_KIND::key_double:
                case TOKEN_KIND::key_unsigned:
                case TOKEN_KIND::key_signed:
                    return parse_cast_factor();
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
    return parse_postfix_exp_factor();
}

static std::unique_ptr<CAssignment> parse_assigment_exp(std::unique_ptr<CExp> exp_left, int32_t precedence) {
    pop_next();
    std::unique_ptr<CExp> exp_right = parse_exp(precedence);
    return std::make_unique<CAssignment>(std::move(exp_left), std::move(exp_right));
}

static std::unique_ptr<CAssignment> parse_assigment_compound_exp(std::unique_ptr<CExp> exp_left, int32_t precedence) {
    std::unique_ptr<CExp> exp_left_2;
    std::unique_ptr<CExp> exp_right_2;
    {
        std::unique_ptr<CBinaryOp> binary_op = parse_binary_op();
        std::unique_ptr<CExp> exp_right = parse_exp(precedence);
        exp_right_2 = std::make_unique<CBinary>(std::move(binary_op), std::move(exp_left), std::move(exp_right));
    }
    return std::make_unique<CAssignment>(std::move(exp_left_2), std::move(exp_right_2));
}

static std::unique_ptr<CBinary> parse_binary_exp(std::unique_ptr<CExp> exp_left, int32_t precedence) {
    std::unique_ptr<CBinaryOp> binary_op = parse_binary_op();
    std::unique_ptr<CExp> exp_right = parse_exp(precedence + 1);
    return std::make_unique<CBinary>(std::move(binary_op), std::move(exp_left), std::move(exp_right));
}

static std::unique_ptr<CConditional> parse_ternary_exp(std::unique_ptr<CExp> exp_left, int32_t precedence) {
    pop_next();
    std::unique_ptr<CExp> exp_middle = parse_exp(0);
    expect_next_is(pop_next(), TOKEN_KIND::ternary_else);
    std::unique_ptr<CExp> exp_right = parse_exp(precedence);
    return std::make_unique<CConditional>(std::move(exp_left), std::move(exp_middle), std::move(exp_right));
}

// <exp> ::= <unary-exp> | <exp> <binop> <exp> | <exp> "?" <exp> ":" <exp>
// exp = Constant(const, type) | String(string) | Var(identifier, type) | Cast(type, exp, type)
//     | Unary(unary_operator, exp, type) | Binary(binary_operator, exp, exp, type) | Assignment(exp, exp, type)
//     | Conditional(exp, exp, exp, type) | FunctionCall(identifier, exp*, type) | Dereference(exp, type)
//     | AddrOf(exp, type) | Subscript(exp, exp)
static std::unique_ptr<CExp> parse_exp(int32_t min_precedence) {
    int32_t precedence;
    std::unique_ptr<CExp> exp_left = parse_unary_exp_factor();
    while(true) {
        precedence = parse_token_precedence(peek_next().token_kind);
        if(precedence < min_precedence) {
            break;
        }
        switch(peek_token->token_kind) {
            case TOKEN_KIND::binop_addition:
            case TOKEN_KIND::unop_negation:
            case TOKEN_KIND::binop_multiplication:
            case TOKEN_KIND::binop_division:
            case TOKEN_KIND::binop_remainder:
            case TOKEN_KIND::binop_bitand:
            case TOKEN_KIND::binop_bitor:
            case TOKEN_KIND::binop_bitxor:
            case TOKEN_KIND::binop_bitshiftleft:
            case TOKEN_KIND::binop_bitshiftright:
            case TOKEN_KIND::binop_lessthan:
            case TOKEN_KIND::binop_lessthanorequal:
            case TOKEN_KIND::binop_greaterthan:
            case TOKEN_KIND::binop_greaterthanorequal:
            case TOKEN_KIND::binop_equalto:
            case TOKEN_KIND::binop_notequal:
            case TOKEN_KIND::binop_and:
            case TOKEN_KIND::binop_or:
                exp_left = parse_binary_exp(std::move(exp_left), precedence);
                break;
            case TOKEN_KIND::assignment_simple:
                exp_left = parse_assigment_exp(std::move(exp_left), precedence);
                break;
            case TOKEN_KIND::assignment_plus:
            case TOKEN_KIND::assignment_difference:
            case TOKEN_KIND::assignment_product:
            case TOKEN_KIND::assignment_quotient:
            case TOKEN_KIND::assignment_remainder:
            case TOKEN_KIND::assignment_bitand:
            case TOKEN_KIND::assignment_bitor:
            case TOKEN_KIND::assignment_bitxor:
            case TOKEN_KIND::assignment_bitshiftleft:
            case TOKEN_KIND::assignment_bitshiftright:
                exp_left = parse_assigment_compound_exp(std::move(exp_left), precedence);
                break;
            case TOKEN_KIND::ternary_if:
                exp_left = parse_ternary_exp(std::move(exp_left), precedence);
                break;
            default:
                raise_runtime_error_at_line("Expected token type " + em("expression") +
                                            " but found token " + em(peek_token->token),
                                            peek_token->line);
        }
    }
    return exp_left;
}

static std::unique_ptr<CForInit> parse_for_init();
static std::unique_ptr<CBlock> parse_block();
static std::unique_ptr<CStatement> parse_statement();

static std::unique_ptr<CReturn> parse_return_statement() {
    pop_next();
    std::unique_ptr<CExp> exp = parse_exp(0);
    expect_next_is(pop_next(), TOKEN_KIND::semicolon);
    return std::make_unique<CReturn>(std::move(exp));
}

static std::unique_ptr<CExpression> parse_expression_statement() {
    std::unique_ptr<CExp> exp = parse_exp(0);
    expect_next_is(pop_next(), TOKEN_KIND::semicolon);
    return std::make_unique<CExpression>(std::move(exp));
}

static std::unique_ptr<CIf> parse_if_statement() {
    pop_next();
    expect_next_is(pop_next(), TOKEN_KIND::parenthesis_open);
    std::unique_ptr<CExp> condition = parse_exp(0);
    expect_next_is(pop_next(), TOKEN_KIND::parenthesis_close);
    peek_next();
    std::unique_ptr<CStatement> then = parse_statement();
    std::unique_ptr<CStatement> else_fi;
    if(peek_next().token_kind == TOKEN_KIND::key_else) {
        pop_next();
        peek_next();
        else_fi = parse_statement();
    }
    return std::make_unique<CIf>(std::move(condition), std::move(then), std::move(else_fi));
}

static std::unique_ptr<CGoto> parse_goto_statement() {
    pop_next();
    TIdentifier target; parse_identifier(target);
    expect_next_is(pop_next(), TOKEN_KIND::semicolon);
    return std::make_unique<CGoto>(std::move(target));
}

static std::unique_ptr<CLabel> parse_label_statement() {
    TIdentifier target; parse_identifier(target);
    expect_next_is(pop_next(), TOKEN_KIND::ternary_else);
    peek_next();
    std::unique_ptr<CStatement> jump_to = parse_statement();
    return std::make_unique<CLabel>(std::move(target), std::move(jump_to));
}

static std::unique_ptr<CCompound> parse_compound_statement() {
    std::unique_ptr<CBlock> block = parse_block();
    return std::make_unique<CCompound>(std::move(block));
}

static std::unique_ptr<CWhile> parse_while_statement() {
    pop_next();
    expect_next_is(pop_next(), TOKEN_KIND::parenthesis_open);
    std::unique_ptr<CExp> condition = parse_exp(0);
    expect_next_is(pop_next(), TOKEN_KIND::parenthesis_close);
    peek_next();
    std::unique_ptr<CStatement> body = parse_statement();
    return std::make_unique<CWhile>(std::move(condition), std::move(body));
}

static std::unique_ptr<CDoWhile> parse_do_while_statement() {
    pop_next();
    peek_next();
    std::unique_ptr<CStatement> body = parse_statement();
    expect_next_is(pop_next(), TOKEN_KIND::key_while);
    expect_next_is(pop_next(), TOKEN_KIND::parenthesis_open);
    std::unique_ptr<CExp> condition = parse_exp(0);
    expect_next_is(pop_next(), TOKEN_KIND::parenthesis_close);
    expect_next_is(pop_next(), TOKEN_KIND::semicolon);
    return std::make_unique<CDoWhile>(std::move(condition), std::move(body));
}

static std::unique_ptr<CFor> parse_for_statement() {
    pop_next();
    expect_next_is(pop_next(), TOKEN_KIND::parenthesis_open);
    std::unique_ptr<CForInit> init = parse_for_init();
    std::unique_ptr<CExp> condition;
    if(peek_next().token_kind != TOKEN_KIND::semicolon) {
        condition = parse_exp(0);
    }
    expect_next_is(pop_next(), TOKEN_KIND::semicolon);
    std::unique_ptr<CExp> post;
    if(peek_next().token_kind != TOKEN_KIND::parenthesis_close) {
        post = parse_exp(0);
    }
    expect_next_is(pop_next(), TOKEN_KIND::parenthesis_close);
    peek_next();
    std::unique_ptr<CStatement> body = parse_statement();
    return std::make_unique<CFor>(std::move(init), std::move(condition), std::move(post), std::move(body));
}

static std::unique_ptr<CBreak> parse_break_statement() {
    pop_next();
    expect_next_is(pop_next(), TOKEN_KIND::semicolon);
    return std::make_unique<CBreak>();
}

static std::unique_ptr<CContinue> parse_continue_statement() {
    pop_next();
    expect_next_is(pop_next(), TOKEN_KIND::semicolon);
    return std::make_unique<CContinue>();
}

static std::unique_ptr<CNull> parse_null_statement() {
    pop_next();
    return std::make_unique<CNull>();
}

// <statement> ::= ";" | "return" <exp> ";" | "if" "(" <exp> ")" <statement> [ "else" <statement> ]
//               | "goto" <identifier> ";" | <identifier> ":" | <block> | "do" <statement> "while" "(" <exp> ")" ";"
//               | "while" "(" <exp> ")" <statement> | "for" "(" <for-init> [ <exp> ] ";" [ <exp> ] ")" <statement>
//               | "break" ";" | "continue" ";" | <exp> ";"
// statement = Return(exp) | Expression(exp) | If(exp, statement, statement?) | Goto(identifier)
//           | Label(identifier, target) | Compound(block) | While(exp, statement, identifier)
//           | DoWhile(statement, exp, identifier) | For(for_init, exp?, exp?, statement, identifier)
//           | Break(identifier) | Continue(identifier) | Null
static std::unique_ptr<CStatement> parse_statement() {
    switch(peek_token->token_kind) {
        case TOKEN_KIND::key_return:
            return parse_return_statement();
        case TOKEN_KIND::key_if:
            return parse_if_statement();
        case TOKEN_KIND::key_goto:
            return parse_goto_statement();
        case TOKEN_KIND::identifier: {
            if(peek_next_i(1).token_kind == TOKEN_KIND::ternary_else) {
                return parse_label_statement();
            }
            break;
        }
        case TOKEN_KIND::brace_open:
            return parse_compound_statement();
        case TOKEN_KIND::key_while:
            return parse_while_statement();
        case TOKEN_KIND::key_do:
            return parse_do_while_statement();
        case TOKEN_KIND::key_for:
            return parse_for_statement();
        case TOKEN_KIND::key_break:
            return parse_break_statement();
        case TOKEN_KIND::key_continue:
            return parse_continue_statement();
        case TOKEN_KIND::semicolon:
            return parse_null_statement();
        default:
            break;
    }
    return parse_expression_statement();
}

static std::unique_ptr<CStorageClass> parse_declarator_declaration(Declarator& declarator);
static std::unique_ptr<CVariableDeclaration> parse_variable_declaration(std::unique_ptr<CStorageClass> storage_class,
                                                                        Declarator&& declarator);

static std::unique_ptr<CInitDecl> parse_decl_for_init() {
    Declarator declarator;
    std::unique_ptr<CStorageClass> storage_class = parse_declarator_declaration(declarator);
    if(declarator.derived_type->type() == AST_T::FunType_t) {
        raise_runtime_error_at_line("Declaration in for loop initialization must be a variable type",
                                    next_token->line);
    }
    std::unique_ptr<CVariableDeclaration> init = parse_variable_declaration(std::move(storage_class),
                                                                            std::move(declarator));
    return std::make_unique<CInitDecl>(std::move(init));
}

static std::unique_ptr<CInitExp> parse_exp_for_init() {
    std::unique_ptr<CExp> init;
    if(peek_next().token_kind != TOKEN_KIND::semicolon) {
        init = parse_exp(0);
    }
    expect_next_is(pop_next(), TOKEN_KIND::semicolon);
    return std::make_unique<CInitExp>(std::move(init));
}

// <for-init> ::= <variable-declaration> | [ <exp> ] ";"
// for_init = InitDecl(variable_declaration) | InitExp(exp?)
static std::unique_ptr<CForInit> parse_for_init() {
    switch(peek_next().token_kind) {
        case TOKEN_KIND::key_char:
        case TOKEN_KIND::key_int:
        case TOKEN_KIND::key_long:
        case TOKEN_KIND::key_double:
        case TOKEN_KIND::key_unsigned:
        case TOKEN_KIND::key_signed:
        case TOKEN_KIND::key_static:
        case TOKEN_KIND::key_extern:
            return parse_decl_for_init();
        default:
            return parse_exp_for_init();
    }
}

static std::unique_ptr<CDeclaration> parse_declaration();

static std::unique_ptr<CS> parse_s_block_item() {
    std::unique_ptr<CStatement> statement = parse_statement();
    return std::make_unique<CS>(std::move(statement));
}

static std::unique_ptr<CD> parse_d_block_item() {
    std::unique_ptr<CDeclaration> declaration = parse_declaration();
    return std::make_unique<CD>(std::move(declaration));
}

// <block-item> ::= <statement> | <declaration>
// block_item = S(statement) | D(declaration)
static std::unique_ptr<CBlockItem> parse_block_item() {
    switch(peek_token->token_kind) {
        case TOKEN_KIND::key_char:
        case TOKEN_KIND::key_int:
        case TOKEN_KIND::key_long:
        case TOKEN_KIND::key_double:
        case TOKEN_KIND::key_unsigned:
        case TOKEN_KIND::key_signed:
        case TOKEN_KIND::key_static:
        case TOKEN_KIND::key_extern:
            return parse_d_block_item();
        default:
            return parse_s_block_item();
    }
}

static std::unique_ptr<CB> parse_b_block() {
    std::vector<std::unique_ptr<CBlockItem>> block_items;
    while(peek_next().token_kind != TOKEN_KIND::brace_close) {
        std::unique_ptr<CBlockItem> block_item = parse_block_item();
        block_items.push_back(std::move(block_item));
    }
    return std::make_unique<CB>(std::move(block_items));
}

// <block> ::= "{" { <block-item> } "}"
// block = B(block_item*)
static std::unique_ptr<CBlock> parse_block() {
    expect_next_is(pop_next(), TOKEN_KIND::brace_open);
    std::unique_ptr<CBlock> block = parse_b_block();
    expect_next_is(pop_next(), TOKEN_KIND::brace_close);
    return block;
}

// <type-specifier> ::= "int" | "long" | "signed" | "unsigned" | "double" | "char"
static std::shared_ptr<Type> parse_type_specifier() {
    size_t specifier = 0;
    size_t line = peek_next().line;
    std::vector<TOKEN_KIND> type_token_kinds;
    while(true) {
        switch(peek_next_i(specifier).token_kind) {
            case TOKEN_KIND::identifier:
            case TOKEN_KIND::parenthesis_close:
                goto Lbreak;
            case TOKEN_KIND::key_char:
            case TOKEN_KIND::key_int:
            case TOKEN_KIND::key_long:
            case TOKEN_KIND::key_double:
            case TOKEN_KIND::key_unsigned:
            case TOKEN_KIND::key_signed:
                type_token_kinds.push_back(pop_next_i(specifier).token_kind);
                break;
            case TOKEN_KIND::key_static:
            case TOKEN_KIND::key_extern:
            case TOKEN_KIND::binop_multiplication:
            case TOKEN_KIND::parenthesis_open:
                specifier += 1;
                break;
            case TOKEN_KIND::brackets_open: {
                specifier += 1;
                while(peek_next_i(specifier).token_kind != TOKEN_KIND::brackets_close) {
                    specifier += 1;
                }
                specifier += 1;
                break;
            }
            default:
                raise_runtime_error_at_line("Expected token type " + em("specifier") +
                                            " but found token " + peek_next_i(specifier).token,
                                            peek_next_i(specifier).line);
        }
    }
    Lbreak:
    switch(type_token_kinds.size()) {
        case 1: {
            switch(type_token_kinds[0]) {
                case TOKEN_KIND::key_char:
                    return std::make_unique<Char>();
                case TOKEN_KIND::key_int:
                    return std::make_unique<Int>();
                case TOKEN_KIND::key_long:
                    return std::make_unique<Long>();
                case TOKEN_KIND::key_double:
                    return std::make_unique<Double>();
                case TOKEN_KIND::key_unsigned:
                    return std::make_unique<UInt>();
                case TOKEN_KIND::key_signed:
                    return std::make_unique<Int>();
                default:
                    break;
            }
            break;
        }
        case 2: {
            if(std::find(type_token_kinds.begin(), type_token_kinds.end(),
                         TOKEN_KIND::key_unsigned) != type_token_kinds.end()) {
                if(std::find(type_token_kinds.begin(), type_token_kinds.end(),
                             TOKEN_KIND::key_int) != type_token_kinds.end()) {
                    return std::make_unique<UInt>();
                }
                else if(std::find(type_token_kinds.begin(), type_token_kinds.end(),
                                  TOKEN_KIND::key_long) != type_token_kinds.end()) {
                    return std::make_unique<ULong>();
                }
                else if(std::find(type_token_kinds.begin(), type_token_kinds.end(),
                                  TOKEN_KIND::key_char) != type_token_kinds.end()) {
                    return std::make_unique<UChar>();
                }
            }
            else if(std::find(type_token_kinds.begin(), type_token_kinds.end(),
                              TOKEN_KIND::key_signed) != type_token_kinds.end()) {
                if(std::find(type_token_kinds.begin(), type_token_kinds.end(),
                             TOKEN_KIND::key_int) != type_token_kinds.end()) {
                    return std::make_unique<Int>();
                }
                else if(std::find(type_token_kinds.begin(), type_token_kinds.end(),
                                  TOKEN_KIND::key_long) != type_token_kinds.end()) {
                    return std::make_unique<Long>();
                }
                else if(std::find(type_token_kinds.begin(), type_token_kinds.end(),
                                  TOKEN_KIND::key_char) != type_token_kinds.end()) {
                    return std::make_unique<SChar>();
                }
            }
            else if((std::find(type_token_kinds.begin(), type_token_kinds.end(),
                                TOKEN_KIND::key_int) != type_token_kinds.end()) &&
                    (std::find(type_token_kinds.begin(), type_token_kinds.end(),
                               TOKEN_KIND::key_long) != type_token_kinds.end())) {
                return std::make_unique<Long>();
            }
            break;
        }
        case 3: {
            if((std::find(type_token_kinds.begin(), type_token_kinds.end(),
                          TOKEN_KIND::key_int) != type_token_kinds.end()) &&
               (std::find(type_token_kinds.begin(), type_token_kinds.end(),
                          TOKEN_KIND::key_long) != type_token_kinds.end())) {
                if(std::find(type_token_kinds.begin(), type_token_kinds.end(),
                             TOKEN_KIND::key_unsigned) != type_token_kinds.end()) {
                    return std::make_unique<ULong>();
                }
                else if(std::find(type_token_kinds.begin(), type_token_kinds.end(),
                                   TOKEN_KIND::key_signed) != type_token_kinds.end()) {
                    return std::make_unique<Long>();
                }
            }
            break;
        }
        default:
            break;
    }
    std::string type_token_kinds_string = "";
    for(const auto& type_token_kind: type_token_kinds) {
        type_token_kinds_string +=  TOKEN_HUMAN_READABLE[type_token_kind] + ",";
    }
    raise_runtime_error_at_line("Expected list of unique token types " + em("(type specifier,)") +
                                " but found token kinds " + em("(" + type_token_kinds_string + ")"),
                                line);
}

// <specifier> ::= <type-specifier> | "static" | "extern"
// storage_class = Static | Extern
static std::unique_ptr<CStorageClass> parse_storage_class() {
    switch(pop_next().token_kind) {
        case TOKEN_KIND::key_static:
            return std::make_unique<CStatic>();
        case TOKEN_KIND::key_extern:
            return std::make_unique<CExtern>();
        default:
            raise_runtime_error_at_line("Expected token type " + em("storage class") +
                                        " but found token " + em(next_token->token),
                                        next_token->line);
    }
}

static std::unique_ptr<CInitializer> parse_initializer();

static std::unique_ptr<CSingleInit> parse_single_initializer() {
    std::unique_ptr<CExp> exp = parse_exp(0);
    return std::make_unique<CSingleInit>(std::move(exp));
}

static std::unique_ptr<CCompoundInit> parse_compound_initializer() {
    pop_next();
    std::vector<std::unique_ptr<CInitializer>> initializers;
    while(true) {
        if(peek_next().token_kind == TOKEN_KIND::brace_close) {
            break;
        }
        std::unique_ptr<CInitializer> initializer = parse_initializer();
        initializers.push_back(std::move(initializer));
        if(peek_next().token_kind == TOKEN_KIND::brace_close) {
            break;
        }
        expect_next_is(pop_next(), TOKEN_KIND::separator_comma);
    }
    pop_next();
    return std::make_unique<CCompoundInit>(std::move(initializers));
}

// <initializer> ::= <exp> | "{" <initializer> { "," <initializer> } [","] "}"
// initializer = SingleInit(exp) | CompoundInit(initializer*)
static std::unique_ptr<CInitializer> parse_initializer() {
    if(peek_next().token_kind == TOKEN_KIND::brace_open) {
        return parse_compound_initializer();
    }
    else {
        return parse_single_initializer();
    }
}

static std::unique_ptr<CDeclarator> parse_declarator();
static void parse_process_declarator(CDeclarator* node, std::shared_ptr<Type> base_type, Declarator& declarator);

static void parse_process_ident_declarator(CIdent* node, std::shared_ptr<Type> base_type, Declarator& declarator) {
    declarator.name = std::move(node->name);
    declarator.derived_type = std::move(base_type);
}

static void parse_process_pointer_declarator(CPointerDeclarator* node, std::shared_ptr<Type> base_type,
                                             Declarator& declarator) {
    std::shared_ptr<Type> derived_type = std::make_shared<Pointer>(std::move(base_type));
    parse_process_declarator(node->declarator.get(), std::move(derived_type), declarator);
}

static void parse_process_array_declarator(CArrayDeclarator* node, std::shared_ptr<Type> base_type,
                                           Declarator& declarator) {
    TLong size = node->size;
    std::shared_ptr<Type> derived_type = std::make_shared<Array>(std::move(size), std::move(base_type));
    parse_process_declarator(node->declarator.get(), std::move(derived_type), declarator);
}

static void parse_process_fun_declarator(CFunDeclarator* node, std::shared_ptr<Type> base_type,
                                         Declarator& declarator) {
    if(node->declarator->type() != AST_T::CIdent_t) {
        raise_runtime_error_at_line("Additional type derivations can not be applied to function types",
                                    next_token->line);
    }

    std::vector<TIdentifier> params;
    std::vector<std::shared_ptr<Type>> param_types;
    for(size_t param = 0; param < node->param_list.size(); param++) {
        Declarator param_declarator;
        parse_process_declarator(node->param_list[param]->declarator.get(),
                                 node->param_list[param]->param_type, param_declarator);
        if(param_declarator.derived_type->type() == AST_T::FunType_t) {
            raise_runtime_error_at_line("Function pointer parameters can not be applied in type derivations",
                                        next_token->line);
        }
        params.push_back(std::move(param_declarator.name));
        param_types.push_back(std::move(param_declarator.derived_type));
    }
    TIdentifier name = static_cast<CIdent*>(node->declarator.get())->name;
    std::shared_ptr<Type> derived_type = std::make_shared<FunType>(std::move(param_types), std::move(base_type));
    declarator.name = std::move(name);
    declarator.derived_type = std::move(derived_type);
    declarator.params = std::move(params);
}

static void parse_process_declarator(CDeclarator* node, std::shared_ptr<Type> base_type, Declarator& declarator) {
    switch(node->type()) {
        case AST_T::CIdent_t:
            parse_process_ident_declarator(static_cast<CIdent*>(node), std::move(base_type), declarator);
            break;
        case AST_T::CPointerDeclarator_t:
            parse_process_pointer_declarator(static_cast<CPointerDeclarator*>(node), std::move(base_type),
                                             declarator);
            break;
        case AST_T::CArrayDeclarator_t:
            parse_process_array_declarator(static_cast<CArrayDeclarator*>(node), std::move(base_type),
                                           declarator);
            break;
        case AST_T::CFunDeclarator_t:
            parse_process_fun_declarator(static_cast<CFunDeclarator*>(node), std::move(base_type), declarator);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::unique_ptr<CIdent> parse_ident_simple_declarator() {
    TIdentifier name; parse_identifier(name);
    return std::make_unique<CIdent>(std::move(name));
}

static std::unique_ptr<CDeclarator> parse_declarator_simple_declarator() {
    pop_next();
    std::unique_ptr<CDeclarator> declarator = parse_declarator();
    expect_next_is(pop_next(), TOKEN_KIND::parenthesis_close);
    return declarator;
}

// <simple-declarator> ::= <identifier> | "(" <declarator> ")"
static std::unique_ptr<CDeclarator> parse_simple_declarator() {
    switch(peek_next().token_kind) {
        case TOKEN_KIND::identifier:
            return parse_ident_simple_declarator();
        case TOKEN_KIND::parenthesis_open:
            return parse_declarator_simple_declarator();
        default:
            raise_runtime_error_at_line("Expected token type " + em("declarator") +
                                        " but found token " + em(peek_token->token),
                                        peek_token->line);
    }
}

// <param> ::= { <type-specifier> }+ <declarator>
// param_info = Param(type, declarator)
static std::unique_ptr<CParam> parse_param() {
    std::shared_ptr<Type> param_type = parse_type_specifier();
    std::unique_ptr<CDeclarator> declarator = parse_declarator();
    return std::make_unique<CParam>(std::move(declarator), std::move(param_type));
}

static void parse_empty_param_list() {
    pop_next();
}

static std::vector<std::unique_ptr<CParam>> parse_non_empty_param_list() {
    std::vector<std::unique_ptr<CParam>> param_list;
    {
        std::unique_ptr<CParam> param = parse_param();
        param_list.push_back(std::move(param));
    }
    while(peek_next().token_kind == TOKEN_KIND::separator_comma) {
        pop_next();
        std::unique_ptr<CParam> param = parse_param();
        param_list.push_back(std::move(param));
    }
    return param_list;
}

// <param-list> ::= "(" "void" ")" | "(" <param> { "," <param> } ")"
static std::vector<std::unique_ptr<CParam>> parse_param_list() {
    pop_next();
    std::vector<std::unique_ptr<CParam>> param_list;
    switch(peek_next().token_kind) {
        case TOKEN_KIND::key_void: {
            parse_empty_param_list();
            break;
        }
        case TOKEN_KIND::key_char:
        case TOKEN_KIND::key_int:
        case TOKEN_KIND::key_long:
        case TOKEN_KIND::key_double:
        case TOKEN_KIND::key_unsigned:
        case TOKEN_KIND::key_signed: {
            param_list = parse_non_empty_param_list();
            break;
        }
        default:
            raise_runtime_error_at_line("Expected token type " + em("type specifier") +
                                        " but found token " + em(peek_token->token),
                                        peek_token->line);
    }
    expect_next_is(pop_next(), TOKEN_KIND::parenthesis_close);
    return param_list;
}

// (fun) <declarator-suffix> ::= <param-list>
static std::unique_ptr<CFunDeclarator> parse_fun_declarator_suffix(std::unique_ptr<CDeclarator> declarator) {
    std::vector<std::unique_ptr<CParam>> param_list = parse_param_list();
    return std::make_unique<CFunDeclarator>(std::move(param_list), std::move(declarator));
}

// (array) <declarator-suffix> ::= { "[" <const> "]" }+
static std::unique_ptr<CDeclarator> parse_array_declarator_suffix(std::unique_ptr<CDeclarator> declarator) {
    do {
        TLong size = parse_array_size_t();
        declarator = std::make_unique<CArrayDeclarator>(std::move(size), std::move(declarator));
    } while(peek_next().token_kind == TOKEN_KIND::brackets_open);
    return declarator;
}

// <direct-declarator> ::= <simple-declarator> [ <declarator-suffix> ]
static std::unique_ptr<CDeclarator> parse_direct_declarator() {
    std::unique_ptr<CDeclarator> declarator = parse_simple_declarator();
    switch(peek_next().token_kind) {
        case TOKEN_KIND::parenthesis_open: {
            return parse_fun_declarator_suffix(std::move(declarator));
        }
        case TOKEN_KIND::brackets_open: {
            return parse_array_declarator_suffix(std::move(declarator));
        }
        default:
            return declarator;
    }
}

static std::unique_ptr<CPointerDeclarator> parse_pointer_declarator() {
    pop_next();
    std::unique_ptr<CDeclarator> declarator = parse_declarator();
    return std::make_unique<CPointerDeclarator>(std::move(declarator));
}

// <declarator> ::= "*" <declarator> | <direct-declarator>
// declarator = Ident(identifier) | PointerDeclarator(declarator) | ArrayDeclarator(int, declarator)
//            | FunDeclarator(param_info* params, declarator)
static std::unique_ptr<CDeclarator> parse_declarator() {
    switch(peek_next().token_kind) {
        case TOKEN_KIND::binop_multiplication:
            return parse_pointer_declarator();
        default:
            return parse_direct_declarator();
    }
}

// <function-declaration> ::= { <specifier> }+ <declarator> ( <block> | ";")
// function_declaration = FunctionDeclaration(identifier name, identifier* params, block? body, type fun_type,
//                                            storage_class?)
static std::unique_ptr<CFunctionDeclaration> parse_function_declaration(std::unique_ptr<CStorageClass> storage_class,
                                                                        Declarator&& declarator) {
    std::unique_ptr<CBlock> body;
    if(peek_next().token_kind == TOKEN_KIND::semicolon) {
        pop_next();
    }
    else {
        body = parse_block();
    }
    return std::make_unique<CFunctionDeclaration>(std::move(declarator.name), std::move(declarator.params),
                                                  std::move(body),std::move(declarator.derived_type),
                                                  std::move(storage_class));
}

// <variable-declaration> ::= { <specifier> }+ <declarator> [ "=" <initializer> ] ";"
// variable_declaration = VariableDeclaration(identifier name, initializer? init, type var_type, storage_class?)
static std::unique_ptr<CVariableDeclaration> parse_variable_declaration(std::unique_ptr<CStorageClass> storage_class,
                                                                        Declarator&& declarator) {
    std::unique_ptr<CInitializer> init;
    if(peek_next().token_kind == TOKEN_KIND::assignment_simple) {
        pop_next();
        init = parse_initializer();
    }
    expect_next_is(pop_next(), TOKEN_KIND::semicolon);
    return std::make_unique<CVariableDeclaration>(std::move(declarator.name), std::move(init),
                                                  std::move(declarator.derived_type), std::move(storage_class));
}

static std::unique_ptr<CFunDecl> parse_fun_decl_declaration(std::unique_ptr<CStorageClass> storage_class,
                                                            Declarator&& declarator) {
    std::unique_ptr<CFunctionDeclaration> function_decl = parse_function_declaration(std::move(storage_class),
                                                                                     std::move(declarator));
    return std::make_unique<CFunDecl>(std::move(function_decl));
}

static std::unique_ptr<CVarDecl> parse_var_decl_declaration(std::unique_ptr<CStorageClass> storage_class,
                                                            Declarator&& declarator) {
    std::unique_ptr<CVariableDeclaration> variable_decl = parse_variable_declaration(std::move(storage_class),
                                                                                     std::move(declarator));
    return std::make_unique<CVarDecl>(std::move(variable_decl));
}

static std::unique_ptr<CStorageClass> parse_declarator_declaration(Declarator& declarator) {
    std::shared_ptr<Type> type_specifier = parse_type_specifier();
    std::unique_ptr<CStorageClass> storage_class;
    switch(peek_next().token_kind) {
        case TOKEN_KIND::identifier:
        case TOKEN_KIND::binop_multiplication:
        case TOKEN_KIND::parenthesis_open:
            break;
        default:
            storage_class = parse_storage_class();
    }
    parse_process_declarator(parse_declarator().get(), std::move(type_specifier), declarator);
    return storage_class;
}

// <declaration> ::= <variable-declaration> | <function-declaration>
// declaration = FunDecl(function_declaration) | VarDecl(variable_declaration)
static std::unique_ptr<CDeclaration> parse_declaration() {
    Declarator declarator;
    std::unique_ptr<CStorageClass> storage_class = parse_declarator_declaration(declarator);
    if(declarator.derived_type->type() == AST_T::FunType_t) {
        return parse_fun_decl_declaration(std::move(storage_class), std::move(declarator));
    }
    else {
        return parse_var_decl_declaration(std::move(storage_class), std::move(declarator));
    }
}

// <program> ::= { <declaration> }
// AST = Program(declaration*)
static std::unique_ptr<CProgram> parse_program() {
    std::vector<std::unique_ptr<CDeclaration>> declarations;
    while(pop_index < p_tokens->size()) {
        std::unique_ptr<CDeclaration> declaration = parse_declaration();
        declarations.push_back(std::move(declaration));
    }
    return std::make_unique<CProgram>(std::move(declarations));
}

std::unique_ptr<CProgram> parsing(std::vector<Token>&& tokens) {
    p_tokens = &tokens;
    std::unique_ptr<CProgram> c_ast = parse_program();
    p_tokens = nullptr;
    if(pop_index != tokens.size()) {
        RAISE_INTERNAL_ERROR;
    }
    tokens.clear();
    if(!c_ast) {
        RAISE_INTERNAL_ERROR;
    }
    return c_ast;
}
