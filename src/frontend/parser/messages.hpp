#ifndef _FRONTEND_PARSER_MESSAGE_HPP
#define _FRONTEND_PARSER_MESSAGE_HPP

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Errors

enum MESSAGE_LEXER {
    unhandled_lexer_error = 300,
    invalid_token,
    failed_to_include_header_file
};

enum MESSAGE_PARSER {
    unhandled_parser_error = 400,
    unexpected_next_token,
    reached_end_of_file,
    number_too_large_for_long_constant,
    number_too_large_for_unsigned_long_constant,
    array_size_not_a_constant_integer,
    case_value_not_a_constant_integer,
    unexpected_unary_operator,
    unexpected_binary_operator,
    unexpected_abstract_declarator,
    unexpected_pointer_unary_factor,
    unexpected_primary_expression_factor,
    unexpected_expression,
    function_declared_in_for_initial,
    unexpected_type_specifier,
    unexpected_type_specifier_list,
    unexpected_storage_class,
    empty_compound_initializer,
    type_derivation_on_function_declaration,
    unexpected_simple_declarator,
    unexpected_parameter_list,
    member_declared_with_non_automatic_storage,
    member_declared_as_function
};

#endif
