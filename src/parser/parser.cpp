#include "parser/parser.hpp"

#include "util/error.hpp"
#include "util/ctypes.hpp"
#include "ast/symbol_table.hpp"
#include "ast/c_ast.hpp"
#include "parser/lexer.hpp"
#include "parser/precedence.hpp"

#include <inttypes.h>
#include <string>
#include <memory>
#include <vector>

/** TODO
cdef list[Token] tokens = []
*/

/** TODO
cdef Token next_token = Token('', TOKEN_KIND.get('error'))
*/

/** TODO
cdef Token peek_token = Token('', TOKEN_KIND.get('error'))
*/

static std::vector<Token>* p_tokens;
static Token* next_token;
static Token* peek_token;
static size_t pop_index = 0;

/** TODO
cdef void expect_next_is(Token next_token_is, int32 expected_token):
    if next_token_is.token_kind != expected_token:
        raise RuntimeError(
            f"""Expected token {
                list(TOKEN_KIND.iter().keys())[
                       list(TOKEN_KIND.iter().values()).index(expected_token)
            ]} but found \"{next_token_is.token}\"""")
*/
static void expect_next_is(const Token& next_token_is, TOKEN_KIND expected_token) {
    if(next_token_is.token_kind != expected_token) { // TODO print token
        raise_runtime_error("Expected token kind \"" + std::to_string(expected_token) + "\" but found \""
                            + next_token_is.token + "\"");
    }
}

/** TODO
cdef Token pop_next():
    global next_token

    if tokens:
        next_token = tokens.pop(0)
        return next_token
    else:

        raise RuntimeError(
            "An error occurred in parser, all Tokens were consumed before end of program")
*/
static const Token& pop_next() {
    if(pop_index >= p_tokens->size()) {
        raise_runtime_error("An error occurred in parser, all Tokens were consumed before end of program");
    }

    next_token = &(*p_tokens)[pop_index];
    pop_index++;
    return *next_token;
}

/** TODO
cdef Token pop_next_i(Py_ssize_t i):
    if i < len(tokens):
        return tokens.pop(i)
    else:

        raise RuntimeError(
            "An error occurred in parser, all Tokens were consumed before end of program")
*/
// TODO does not work if i = 0
static const Token& pop_next_i(size_t i) {
    if(pop_index + i >= p_tokens->size() || i == 0) {
        raise_runtime_error("An error occurred in parser, all Tokens were consumed before end of program");
    }

    static Token next_token_i = std::move((*p_tokens)[pop_index + i]);
    for(size_t j = pop_index + i; j-- >= pop_index;) {
        (*p_tokens)[j+1] = std::move((*p_tokens)[j]);
    }
    (*p_tokens)[pop_index-1] = std::move(next_token_i);
    pop_next();
    return (*p_tokens)[pop_index-2];
}

/** TODO
cdef Token peek_next():
    global peek_token

    if tokens:
        peek_token = tokens[0]
        return peek_token
    else:

        raise RuntimeError(
            "An error occurred in parser, all Tokens were consumed before end of program")
*/
static const Token& peek_next() {
    if(pop_index >= p_tokens->size()) {
        raise_runtime_error("An error occurred in parser, all Tokens were consumed before end of program");
    }

    peek_token = &(*p_tokens)[pop_index];
    return *peek_token;
}

/** TODO
cdef Token peek_next_i(Py_ssize_t i):
    if i < len(tokens):
        return tokens[i]
    else:

        raise RuntimeError(
            "An error occurred in parser, all Tokens were consumed before end of program")
*/
static const Token& peek_next_i(size_t i) {
    if(pop_index + i >= p_tokens->size()) {
        raise_runtime_error("An error occurred in parser, all Tokens were consumed before end of program");
    }

    return (*p_tokens)[pop_index + i];
}

/** TODO
cdef TIdentifier parse_identifier():
    # <identifier> ::= ? An identifier token ?
    expect_next_is(pop_next(), TOKEN_KIND.get('identifier'))
    return TIdentifier(next_token.token)
*/
// <identifier> ::= ? An identifier token ?
static void parse_identifier(TIdentifier& r) {
    expect_next_is(pop_next(), TOKEN_KIND::identifier);
    r = std::move(next_token->token);
}

/** TODO
cdef CConstInt parse_int_constant():
    cdef TInt value = parse_int()
    return CConstInt(value)
*/
// <int> ::= ? A constant token ?
static std::unique_ptr<CConstInt> parse_int_constant(intmax_t intmax) {
    return std::make_unique<CConstInt>(intmax_to_int32(intmax));
}

/** TODO
cdef CConstLong parse_long_constant():
    cdef TLong value = parse_long()
    return CConstLong(value)
*/
// <long> ::= ? A constant token ?
static std::unique_ptr<CConstLong> parse_long_constant(intmax_t intmax) {
    return std::make_unique<CConstLong>(intmax_to_int64(intmax));
}

/** TODO
cdef CConstDouble parse_double_constant():
    cdef TDouble value = parse_double()
    return CConstDouble(value)
*/
// <double> ::= ? A constant token ?
static std::unique_ptr<CConstDouble> parse_double_constant() {
    return std::make_unique<CConstDouble>(string_to_double(next_token->token));
}

/** TODO
cdef CConstUInt parse_uint_constant():
    cdef TUInt value = parse_uint()
    return CConstUInt(value)
*/
// <uint> ::= ? A constant token ?
static std::unique_ptr<CConstUInt> parse_uint_constant(uintmax_t uintmax) {
    return std::make_unique<CConstUInt>(uintmax_to_uint32(uintmax));
}

/** TODO
cdef CConstULong parse_ulong_constant():
    cdef TULong value = parse_ulong()
    return CConstULong(value)
*/
// <ulong> ::= ? A constant token ?
static std::unique_ptr<CConstULong> parse_ulong_constant(uintmax_t uintmax) {
    return std::make_unique<CConstULong>(uintmax_to_uint64(uintmax));
}

/** TODO
cdef CConst parse_constant():
    # <const> ::= <int> | <long> | <double>
    if pop_next().token_kind == TOKEN_KIND.get('float_constant'):
        return parse_double_constant()

    elif next_token.token_kind == TOKEN_KIND.get('long_constant'):
        next_token.token = next_token.token[:-1]

    cdef object value = int(next_token.token)

    if value > 9223372036854775807:

        raise RuntimeError(
            f"Constant {next_token.token} is too large to be represented as an int or a long")

    if next_token.token_kind == TOKEN_KIND.get('constant') and \
       value <= 2147483647:
        return parse_int_constant()

    return parse_long_constant()
*/
// TODO see if can return unique ptr
// <const> ::= <int> | <long> | <double>
static std::unique_ptr<CConst> parse_constant() {
    if(pop_next().token_kind == TOKEN_KIND::float_constant) {
        return parse_double_constant();
    } else if (next_token->token_kind == TOKEN_KIND::long_constant) {
        next_token->token.pop_back();
    }

    intmax_t value = string_to_intmax(next_token->token);
    if(value > 9223372036854775807) {
        raise_runtime_error("Constant \"" + next_token->token + "\" is too large to be represented " +
                            "as an int or a long");
    }
    if(next_token->token_kind == TOKEN_KIND::constant && value <= 2147483647) {
        return parse_int_constant(value);
    }
    return parse_long_constant(value);
}

/** TODO
cdef CConst parse_unsigned_constant():
    # <const> ::= <uint> | <ulong>
    if pop_next().token_kind == TOKEN_KIND.get('unsigned_long_constant'):
        next_token.token = next_token.token[:-1]
    next_token.token = next_token.token[:-1]

    cdef object value = int(next_token.token)

    if value > 18446744073709551615:

        raise RuntimeError(
            f"Constant {next_token.token} is too large to be represented as an unsigned int or a unsigned long")

    if next_token.token_kind == TOKEN_KIND.get('unsigned_constant') and \
       value <= 4294967295:
        return parse_uint_constant()

    return parse_ulong_constant()
*/

/** TODO
cdef CBinaryOp parse_binary_op():
    # <binop> ::= "-" | "+" | "*" | "/" | "%" | "&" | "|" | "^" | "<<" | ">>" | "&&" | "||" | "==" | "!="
    #                 | "<" | "<=" | ">" | ">="
    if pop_next().token_kind in (TOKEN_KIND.get('unop_negation'),
                                 TOKEN_KIND.get('assignment_difference')):
        return CSubtract()
    elif next_token.token_kind in (TOKEN_KIND.get('binop_addition'),
                                   TOKEN_KIND.get('assignment_plus')):
        return CAdd()
    elif next_token.token_kind in (TOKEN_KIND.get('binop_multiplication'),
                                   TOKEN_KIND.get('assignment_product')):
        return CMultiply()
    elif next_token.token_kind in (TOKEN_KIND.get('binop_division'),
                                   TOKEN_KIND.get('assignment_quotient')):
        return CDivide()
    elif next_token.token_kind in (TOKEN_KIND.get('binop_remainder'),
                                   TOKEN_KIND.get('assignment_remainder')):
        return CRemainder()
    elif next_token.token_kind in (TOKEN_KIND.get('binop_bitand'),
                                   TOKEN_KIND.get('assignment_bitand')):
        return CBitAnd()
    elif next_token.token_kind in (TOKEN_KIND.get('binop_bitor'),
                                   TOKEN_KIND.get('assignment_bitor')):
        return CBitOr()
    elif next_token.token_kind in (TOKEN_KIND.get('binop_bitxor'),
                                   TOKEN_KIND.get('assignment_bitxor')):
        return CBitXor()
    elif next_token.token_kind in (TOKEN_KIND.get('binop_bitshiftleft'),
                                   TOKEN_KIND.get('assignment_bitshiftleft')):
        return CBitShiftLeft()
    elif next_token.token_kind in (TOKEN_KIND.get('binop_bitshiftright'),
                                   TOKEN_KIND.get('assignment_bitshiftright')):
        return CBitShiftRight()
    elif next_token.token_kind == TOKEN_KIND.get('binop_and'):
        return CAnd()
    elif next_token.token_kind == TOKEN_KIND.get('binop_or'):
        return COr()
    elif next_token.token_kind == TOKEN_KIND.get('binop_equalto'):
        return CEqual()
    elif next_token.token_kind == TOKEN_KIND.get('binop_notequal'):
        return CNotEqual()
    elif next_token.token_kind == TOKEN_KIND.get('binop_lessthan'):
        return CLessThan()
    elif next_token.token_kind == TOKEN_KIND.get('binop_lessthanorequal'):
        return CLessOrEqual()
    elif next_token.token_kind == TOKEN_KIND.get('binop_greaterthan'):
        return CGreaterThan()
    elif next_token.token_kind == TOKEN_KIND.get('binop_greaterthanorequal'):
        return CGreaterOrEqual()
    else:

        raise RuntimeError(
            f"Expected token type \"binary_op\" but found token \"{next_token.token}\"")
*/

/** TODO
cdef CUnaryOp parse_unary_op():
    # <unop> ::= "-" | "~" | "!"
    if pop_next().token_kind == TOKEN_KIND.get('unop_complement'):
        return CComplement()
    elif next_token.token_kind == TOKEN_KIND.get('unop_negation'):
        return CNegate()
    elif next_token.token_kind == TOKEN_KIND.get('unop_not'):
        return CNot()
    else:

        raise RuntimeError(
            f"Expected token type \"unary_op\" but found token \"{next_token.token}\"")
*/

/** TODO
cdef list[CExp] parse_argument_list():
    # <exp> { "," <exp> }
    cdef list[CExp] args = []
    args.append(parse_exp())
    while peek_next().token_kind == TOKEN_KIND.get('separator_comma'):
        _ = pop_next()
        args.append(parse_exp())
    return args
*/

/** TODO
cdef CVar parse_var_factor():
    cdef TIdentifier name = parse_identifier()
    return CVar(name)
*/

/** TODO
cdef CCast parse_cast_factor():
    cdef Type target_type = parse_type_specifier()
    expect_next_is(pop_next(), TOKEN_KIND.get('parenthesis_close'))
    cdef CExp exp = parse_factor()
    return CCast(exp, target_type)
*/

/** TODO
cdef CConstant parse_constant_factor():
    cdef CConst constant = parse_constant()
    return CConstant(constant)
*/

/** TODO
cdef CConstant parse_unsigned_constant_factor():
    cdef CConst constant = parse_unsigned_constant()
    return CConstant(constant)
*/

/** TODO
cdef CUnary parse_unary_factor():
    cdef CUnaryOp unary_op = parse_unary_op()
    cdef CExp exp = parse_factor()
    return CUnary(unary_op, exp)
*/

/** TODO
cdef CExp parse_inner_exp_factor():
    cdef CExp inner_exp = parse_exp()
    expect_next_is(pop_next(), TOKEN_KIND.get('parenthesis_close'))
    return inner_exp
*/

/** TODO
cdef CExp parse_function_call_factor():
    cdef TIdentifier name = parse_identifier()
    expect_next_is(pop_next(), TOKEN_KIND.get('parenthesis_open'))
    cdef list[CExp] args
    if peek_next().token_kind == TOKEN_KIND.get('parenthesis_close'):
        args = []
    else:
        args = parse_argument_list()
    expect_next_is(pop_next(), TOKEN_KIND.get('parenthesis_close'))
    return CFunctionCall(name, args)
*/

/** TODO
cdef CExp parse_factor():
    # <factor> ::= <const> | <identifier> | "(" { <type-specifier> }+ ")" <factor> | <unop> <factor> | "(" <exp> ")"
    #            | <identifier> "(" [ <argument-list> ] ")"
    if peek_next().token_kind == TOKEN_KIND.get('identifier'):
        if peek_next_i(1).token_kind == TOKEN_KIND.get('parenthesis_open'):
            return parse_function_call_factor()
        else:
            return parse_var_factor()
    elif peek_token.token_kind in (TOKEN_KIND.get('constant'),
                                   TOKEN_KIND.get('long_constant'),
                                   TOKEN_KIND.get('float_constant')):
        return parse_constant_factor()
    elif peek_token.token_kind in (TOKEN_KIND.get('unsigned_constant'),
                                   TOKEN_KIND.get('unsigned_long_constant')):
        return parse_unsigned_constant_factor()
    elif peek_token.token_kind in (TOKEN_KIND.get('unop_complement'),
                                   TOKEN_KIND.get('unop_negation'),
                                   TOKEN_KIND.get('unop_not')):
        return parse_unary_factor()
    elif pop_next().token_kind == TOKEN_KIND.get('parenthesis_open'):
        if peek_next().token_kind in (TOKEN_KIND.get('key_int'),
                                      TOKEN_KIND.get('key_long'),
                                      TOKEN_KIND.get('key_double'),
                                      TOKEN_KIND.get('key_unsigned'),
                                      TOKEN_KIND.get('key_signed')):
            return parse_cast_factor()
        else:
            return parse_inner_exp_factor()
    else:

        raise RuntimeError(
            f"Expected token type \"factor\" but found token \"{next_token.token}\"")
*/

/** TODO
cdef CAssignment parse_assigment_exp(CExp exp_left, int32 precedence):
    _ = pop_next()
    cdef CExp exp_right = parse_exp(precedence)
    return CAssignment(exp_left, exp_right)
*/

/** TODO
cdef CAssignment parse_assigment_compound_exp(CExp exp_left, int32 precedence):
    cdef CBinaryOp binary_op = parse_binary_op()
    cdef CExp exp_right = parse_exp(precedence)
    exp_right = CAssignmentCompound(binary_op, exp_left, exp_right)
    return CAssignment(exp_left, exp_right)
*/

/** TODO
cdef CConditional parse_ternary_exp(CExp exp_left, int32 precedence):
    _ = pop_next()
    cdef CExp exp_middle = parse_exp()
    expect_next_is(pop_next(), TOKEN_KIND.get('ternary_else'))
    cdef CExp exp_right = parse_exp(precedence)
    return CConditional(exp_left, exp_middle, exp_right)
*/

/** TODO
cdef CBinary parse_binary_exp(CExp exp_left, int32 precedence):
    cdef CBinaryOp binary_op = parse_binary_op()
    cdef CExp exp_right = parse_exp(precedence + 1)
    return CBinary(binary_op, exp_left, exp_right)
*/

/** TODO
cdef CExp parse_exp(int32 min_precedence = 0):
    # <exp> ::= <factor> | <exp> <binop> <exp> | <exp> "?" <exp> ":" <exp>
    cdef int32 precedence
    cdef CExp exp_left = parse_factor()
    while True:
        precedence = parse_token_precedence(peek_next().token_kind)
        if precedence < min_precedence:
            break
        elif peek_token.token_kind == TOKEN_KIND.get('assignment_simple'):
            exp_left = parse_assigment_exp(exp_left, precedence)
        elif peek_token.token_kind in (TOKEN_KIND.get('assignment_plus'),
                                       TOKEN_KIND.get('assignment_difference'),
                                       TOKEN_KIND.get('assignment_product'),
                                       TOKEN_KIND.get('assignment_quotient'),
                                       TOKEN_KIND.get('assignment_remainder'),
                                       TOKEN_KIND.get('assignment_bitand'),
                                       TOKEN_KIND.get('assignment_bitor'),
                                       TOKEN_KIND.get('assignment_bitxor'),
                                       TOKEN_KIND.get('assignment_bitshiftleft'),
                                       TOKEN_KIND.get('assignment_bitshiftright')):
            exp_left = parse_assigment_compound_exp(exp_left, precedence)
        elif peek_token.token_kind == TOKEN_KIND.get('ternary_if'):
            exp_left = parse_ternary_exp(exp_left, precedence)
        elif peek_token.token_kind in (TOKEN_KIND.get('unop_negation'),
                                       TOKEN_KIND.get('binop_addition'),
                                       TOKEN_KIND.get('binop_multiplication'),
                                       TOKEN_KIND.get('binop_division'),
                                       TOKEN_KIND.get('binop_remainder'),
                                       TOKEN_KIND.get('binop_bitand'),
                                       TOKEN_KIND.get('binop_bitor'),
                                       TOKEN_KIND.get('binop_bitxor'),
                                       TOKEN_KIND.get('binop_bitshiftleft'),
                                       TOKEN_KIND.get('binop_bitshiftright'),
                                       TOKEN_KIND.get('binop_lessthan'),
                                       TOKEN_KIND.get('binop_lessthanorequal'),
                                       TOKEN_KIND.get('binop_greaterthan'),
                                       TOKEN_KIND.get('binop_greaterthanorequal'),
                                       TOKEN_KIND.get('binop_equalto'),
                                       TOKEN_KIND.get('binop_notequal'),
                                       TOKEN_KIND.get('binop_and'),
                                       TOKEN_KIND.get('binop_or')):
            exp_left = parse_binary_exp(exp_left, precedence)
        else:

            raise RuntimeError(
                f"Expected token type \"exp\" but found token \"{peek_token.token}\"")

    return exp_left
*/

/** TODO
cdef CNull parse_null_statement():
    _ = pop_next()
    return CNull()
*/

/** TODO
cdef CReturn parse_return_statement():
    _ = pop_next()
    cdef CExp exp = parse_exp()
    expect_next_is(pop_next(), TOKEN_KIND.get('semicolon'))
    return CReturn(exp)
*/

/** TODO
cdef CIf parse_if_statement():
    _ = pop_next()
    expect_next_is(pop_next(), TOKEN_KIND.get('parenthesis_open'))
    cdef CExp condition = parse_exp()
    expect_next_is(pop_next(), TOKEN_KIND.get('parenthesis_close'))
    _ = peek_next()
    cdef CStatement then = parse_statement()
    cdef CStatement else_fi
    if peek_next().token_kind == TOKEN_KIND.get('key_else'):
        _ = pop_next()
        _ = peek_next()
        else_fi = parse_statement()
    else:
        else_fi = None
    return CIf(condition, then, else_fi)
*/

/** TODO
cdef CGoto parse_goto_statement():
    _ = pop_next()
    cdef TIdentifier target = parse_identifier()
    expect_next_is(pop_next(), TOKEN_KIND.get('semicolon'))
    return CGoto(target)
*/

/** TODO
cdef CLabel parse_label_statement():
    cdef TIdentifier target = parse_identifier()
    expect_next_is(pop_next(), TOKEN_KIND.get('ternary_else'))
    _ = peek_next()
    cdef CStatement jump_to = parse_statement()
    return CLabel(target, jump_to)
*/

/** TODO
cdef CCompound parse_compound_statement():
    cdef CBlock block = parse_block()
    return CCompound(block)
*/

/** TODO
cdef CWhile parse_while_statement():
    _ = pop_next()
    expect_next_is(pop_next(), TOKEN_KIND.get('parenthesis_open'))
    cdef CExp condition = parse_exp()
    expect_next_is(pop_next(), TOKEN_KIND.get('parenthesis_close'))
    _ = peek_next()
    body = parse_statement()
    return CWhile(condition, body)
*/

/** TODO
cdef CDoWhile parse_do_while_statement():
    _ = pop_next()
    _ = peek_next()
    body = parse_statement()
    expect_next_is(pop_next(), TOKEN_KIND.get('key_while'))
    expect_next_is(pop_next(), TOKEN_KIND.get('parenthesis_open'))
    cdef CExp condition = parse_exp()
    expect_next_is(pop_next(), TOKEN_KIND.get('parenthesis_close'))
    expect_next_is(pop_next(), TOKEN_KIND.get('semicolon'))
    return CDoWhile(condition, body)
*/

/** TODO
cdef CFor parse_for_statement():
    _ = pop_next()
    expect_next_is(pop_next(), TOKEN_KIND.get('parenthesis_open'))
    cdef CForInit init = parse_for_init()
    cdef CExp condition
    if peek_next().token_kind == TOKEN_KIND.get('semicolon'):
        condition = None
    else:
        condition = parse_exp()
    expect_next_is(pop_next(), TOKEN_KIND.get('semicolon'))
    cdef CExp post
    if peek_next().token_kind == TOKEN_KIND.get('parenthesis_close'):
        post = None
    else:
        post = parse_exp()
    expect_next_is(pop_next(), TOKEN_KIND.get('parenthesis_close'))
    _ = peek_next()
    body = parse_statement()
    return CFor(init, condition, post, body)
*/

/** TODO
cdef CBreak parse_break_statement():
    _ = pop_next()
    expect_next_is(pop_next(), TOKEN_KIND.get('semicolon'))
    return CBreak()
*/

/** TODO
cdef CContinue parse_continue_statement():
    _ = pop_next()
    expect_next_is(pop_next(), TOKEN_KIND.get('semicolon'))
    return CContinue()
*/

/** TODO
cdef CExpression parse_expression_statement():
    cdef CExp exp = parse_exp()
    expect_next_is(pop_next(), TOKEN_KIND.get('semicolon'))
    return CExpression(exp)
*/

/** TODO
cdef CStatement parse_statement():
    # <statement> ::= "return" <exp> ";" | <exp> ";" | "if" "(" <exp> ")" <statement> [ "else" <statement> ] | ";"
    #               | <block> | "while" "(" <exp> ")" <statement> | "do" <statement> "while" "(" <exp> ")" ";"
    #               | "for" "(" <for-init> [ <exp> ] ";" [ <exp> ] ")" <statement> | "break" ";" | "continue" ";"
    if peek_token.token_kind == TOKEN_KIND.get('semicolon'):
        return parse_null_statement()
    elif peek_token.token_kind == TOKEN_KIND.get('key_return'):
        return parse_return_statement()
    elif peek_token.token_kind == TOKEN_KIND.get('key_if'):
        return parse_if_statement()
    elif peek_token.token_kind == TOKEN_KIND.get('key_goto'):
        return parse_goto_statement()
    elif peek_token.token_kind == TOKEN_KIND.get('identifier'):
        if peek_next_i(1).token_kind == TOKEN_KIND.get('ternary_else'):
            return parse_label_statement()
    elif peek_token.token_kind == TOKEN_KIND.get('brace_open'):
        return parse_compound_statement()
    elif peek_token.token_kind == TOKEN_KIND.get('key_while'):
        return parse_while_statement()
    elif peek_token.token_kind == TOKEN_KIND.get('key_do'):
        return parse_do_while_statement()
    elif peek_token.token_kind == TOKEN_KIND.get('key_for'):
        return parse_for_statement()
    elif peek_token.token_kind == TOKEN_KIND.get('key_break'):
        return parse_break_statement()
    elif peek_token.token_kind == TOKEN_KIND.get('key_continue'):
        return parse_continue_statement()

    return parse_expression_statement()
*/

/** TODO
cdef CInitDecl parse_decl_for_init():
    cdef Type type_specifier = parse_type_specifier()
    cdef CVariableDeclaration init = parse_variable_declaration(type_specifier)
    return CInitDecl(init)
*/

/** TODO
cdef CInitExp parse_exp_for_init():
    cdef CExp exp
    if peek_next().token_kind == TOKEN_KIND.get('semicolon'):
        init = None
    else:
        init = parse_exp()
    expect_next_is(pop_next(), TOKEN_KIND.get('semicolon'))
    return CInitExp(init)
*/

/** TODO
cdef CForInit parse_for_init():
    # <for-init> ::= <variable-declaration> | [<exp>] ";"
    if peek_next().token_kind in (TOKEN_KIND.get('key_int'),
                                  TOKEN_KIND.get('key_long'),
                                  TOKEN_KIND.get('key_double'),
                                  TOKEN_KIND.get('key_unsigned'),
                                  TOKEN_KIND.get('key_signed'),
                                  TOKEN_KIND.get('key_static'),
                                  TOKEN_KIND.get('key_extern')):
        return parse_decl_for_init()
    else:
        return parse_exp_for_init()
*/

/** TODO
cdef CD parse_d_block_item():
    cdef CDeclaration declaration = parse_declaration()
    return CD(declaration)
*/

/** TODO
cdef CS parse_s_block_item():
    cdef CStatement statement = parse_statement()
    return CS(statement)
*/

/** TODO
cdef CBlockItem parse_block_item():
    # <block-item> ::= <statement> | <declaration>
    if peek_token.token_kind in (TOKEN_KIND.get('key_int'),
                                 TOKEN_KIND.get('key_long'),
                                 TOKEN_KIND.get('key_double'),
                                 TOKEN_KIND.get('key_unsigned'),
                                 TOKEN_KIND.get('key_signed'),
                                 TOKEN_KIND.get('key_static'),
                                 TOKEN_KIND.get('key_extern')):
        return parse_d_block_item()
    else:
        return parse_s_block_item()
*/

/** TODO
cdef CB parse_b_block():
    cdef CBlockItem block_item
    cdef list[CBlockItem] block_items = []
    while peek_next().token_kind != TOKEN_KIND.get('brace_close'):
        block_item = parse_block_item()
        block_items.append(block_item)
    return CB(block_items)
*/

/** TODO
cdef CBlock parse_block():
    # <block> ::= "{" { <block-item> } "}
    expect_next_is(pop_next(), TOKEN_KIND.get('brace_open'))
    cdef CBlock block = parse_b_block()
    expect_next_is(pop_next(), TOKEN_KIND.get('brace_close'))
    return block
*/

/** TODO
cdef Type parse_type_specifier():
    # <type-specifier> ::= "int" | "long"
    cdef Py_ssize_t specifier = 0
    cdef list[int32] type_token_kinds = []
    while True:
        if peek_next_i(specifier).token_kind in (TOKEN_KIND.get('identifier'),
                                                 TOKEN_KIND.get('parenthesis_close')):
            break
        elif peek_next_i(specifier).token_kind in (TOKEN_KIND.get('key_int'),
                                                   TOKEN_KIND.get('key_long'),
                                                   TOKEN_KIND.get('key_double'),
                                                   TOKEN_KIND.get('key_unsigned'),
                                                   TOKEN_KIND.get('key_signed')):
            type_token_kinds.append(pop_next_i(specifier).token_kind)
        elif peek_next_i(specifier).token_kind in (TOKEN_KIND.get('key_static'),
                                                   TOKEN_KIND.get('key_extern')):
            specifier += 1
        else:

            raise RuntimeError(
                f"Expected token type \"specifier\" but found token \"{peek_next_i(specifier).token}\"")

    if len(type_token_kinds) == 1:
        if type_token_kinds[0] == TOKEN_KIND.get('key_int'):
            return Int()
        elif type_token_kinds[0] == TOKEN_KIND.get('key_long'):
            return Long()
        elif type_token_kinds[0] == TOKEN_KIND.get('key_double'):
            return Double()
        elif type_token_kinds[0] == TOKEN_KIND.get('key_unsigned'):
            return UInt()
        elif type_token_kinds[0] == TOKEN_KIND.get('key_signed'):
            return Int()

    elif len(type_token_kinds) == 2:
        if TOKEN_KIND.get('key_unsigned') in type_token_kinds:
            if TOKEN_KIND.get('key_int') in type_token_kinds:
                return UInt()
            elif TOKEN_KIND.get('key_long') in type_token_kinds:
                return ULong()

        elif TOKEN_KIND.get('key_signed') in type_token_kinds:
            if TOKEN_KIND.get('key_int') in type_token_kinds:
                return Int()
            elif TOKEN_KIND.get('key_long') in type_token_kinds:
                return Long()

        elif TOKEN_KIND.get('key_int') in type_token_kinds and \
             TOKEN_KIND.get('key_long') in type_token_kinds:
            return Long()

    elif len(type_token_kinds) == 3:
        if TOKEN_KIND.get('key_int') in type_token_kinds and \
           TOKEN_KIND.get('key_long') in type_token_kinds:
            if TOKEN_KIND.get('key_unsigned') in type_token_kinds:
                return ULong()
            elif TOKEN_KIND.get('key_signed') in type_token_kinds:
                return Long()

    raise RuntimeError(
            f"Expected token type \"type specifier\" but found token \"{str(type_token_kinds)}\"")
*/

/** TODO
cdef CStorageClass parse_storage_class():
    # <storage_class> ::= "static" | "extern"
    if pop_next().token_kind == TOKEN_KIND.get('key_static'):
        return CStatic()
    elif next_token.token_kind == TOKEN_KIND.get('key_extern'):
        return CExtern()
    else:

        raise RuntimeError(
            f"Expected token type \"storage class\" but found token \"{next_token.token}\"")
*/

/** TODO
cdef CFunctionDeclaration parse_function_declaration(Type ret_type):
    # <function-declaration> ::= [ <storage-class> ] <identifier> "(" <param-list> ")" ( <block> | ";")
    # <param-list> ::= "void" | { <type-specifier> }+ <identifier> { "," { <type-specifier> }+ <identifier> }
    cdef storage_class
    if peek_next().token_kind == TOKEN_KIND.get('identifier'):
        storage_class = None
    else:
        storage_class = parse_storage_class()
    cdef TIdentifier name = parse_identifier()
    expect_next_is(pop_next(), TOKEN_KIND.get('parenthesis_open'))
    cdef list[Type] param_types = []
    cdef list[TIdentifier] params = []
    if peek_next().token_kind == TOKEN_KIND.get('key_void'):
        _ = pop_next()
    elif peek_token.token_kind in (TOKEN_KIND.get('key_int'),
                                   TOKEN_KIND.get('key_long'),
                                   TOKEN_KIND.get('key_double'),
                                   TOKEN_KIND.get('key_unsigned'),
                                   TOKEN_KIND.get('key_signed')):
        param_types.append(parse_type_specifier())
        params.append(parse_identifier())
        while peek_next().token_kind == TOKEN_KIND.get('separator_comma'):
            _ = pop_next()
            param_types.append(parse_type_specifier())
            params.append(parse_identifier())
    expect_next_is(pop_next(), TOKEN_KIND.get('parenthesis_close'))
    cdef CBlock body
    if peek_next().token_kind == TOKEN_KIND.get('semicolon'):
        _ = pop_next()
        body = None
    else:
        body = parse_block()
    cdef Type fun_type = FunType(param_types, ret_type)
    return CFunctionDeclaration(name, params, body, fun_type, storage_class)
*/

/** TODO
cdef CVariableDeclaration parse_variable_declaration(Type var_type):
    # <variable-declaration> ::= [ <storage-class> ] <identifier> [ "=" <exp> ] ";"
    cdef storage_class
    if peek_next().token_kind == TOKEN_KIND.get('identifier'):
        storage_class = None
    else:
        storage_class = parse_storage_class()
    cdef TIdentifier name = parse_identifier()
    cdef CExp init
    if peek_next().token_kind == TOKEN_KIND.get('assignment_simple'):
        _ = pop_next()
        init = parse_exp()
    else:
        init = None
    expect_next_is(pop_next(), TOKEN_KIND.get('semicolon'))
    return CVariableDeclaration(name, init, var_type, storage_class)
*/

/** TODO
cdef CFunDecl parse_fun_decl_declaration(Type ret_type):
    cdef CFunctionDeclaration function_decl = parse_function_declaration(ret_type)
    return CFunDecl(function_decl)
*/

/** TODO
cdef CVarDecl parse_var_decl_declaration(Type var_type):
    cdef CVariableDeclaration variable_decl = parse_variable_declaration(var_type)
    return CVarDecl(variable_decl)
*/

/** TODO
cdef CDeclaration parse_declaration():
    # <declaration> ::= { <specifier> }+ (<variable-declaration> | <function-declaration>)
    cdef Type type_specifier = parse_type_specifier()
    cdef Py_ssize_t i = 2
    if peek_next().token_kind == TOKEN_KIND.get("identifier"):
        i = 1
    if peek_next_i(i).token_kind == TOKEN_KIND.get('parenthesis_open'):
        return parse_fun_decl_declaration(type_specifier)
    else:
        return parse_var_decl_declaration(type_specifier)
*/

/** TODO
cdef CProgram parse_program():
    # <program> ::= { <declaration> }
    cdef list[CDeclaration] declarations = []
    while tokens:
        declarations.append(parse_declaration())

    return CProgram(declarations)
*/

/** TODO
cdef CProgram parsing(list[Token] lex_tokens):
    global tokens
    global next_token
    global peek_token
    tokens = lex_tokens
    next_token = Token('', TOKEN_KIND.get('error'))
    peek_token = Token('', TOKEN_KIND.get('error'))

    cdef CProgram c_ast = parse_program()

    if tokens:
        raise RuntimeError(
            "An error occurred in parser, not all Tokens were consumed")

    if not c_ast:
        raise RuntimeError(
            "An error occurred in parser, Ast was not parsed")

    return c_ast
*/
void parsing(std::vector<Token>& tokens, CProgram& /*c_ast*/) {
    p_tokens = &tokens;
}
