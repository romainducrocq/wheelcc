#ifndef _FRONT_PARSER_MESSAGES_H
#define _FRONT_PARSER_MESSAGES_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Errors

enum MESSAGE_LEXER {
    MSG_unhandled_lexer_error = 300,
    MSG_invalid_token,
    MSG_failed_to_include_header_file
};

enum MESSAGE_PARSER {
    MSG_unhandled_parser_error = 400,
    MSG_unexpected_next_token,
    MSG_reached_end_of_file,
    MSG_number_too_large_for_long_constant,
    MSG_number_too_large_for_unsigned_long_constant,
    MSG_array_size_not_a_constant_integer,
    MSG_case_value_not_a_constant_integer,
    MSG_unexpected_unary_operator,
    MSG_unexpected_binary_operator,
    MSG_unexpected_abstract_declarator,
    MSG_unexpected_pointer_unary_factor,
    MSG_unexpected_primary_expression_factor,
    MSG_unexpected_expression,
    MSG_function_declared_in_for_initial,
    MSG_unexpected_type_specifier,
    MSG_unexpected_type_specifier_list,
    MSG_unexpected_storage_class,
    MSG_empty_compound_initializer,
    MSG_type_derivation_on_function_declaration,
    MSG_unexpected_simple_declarator,
    MSG_unexpected_parameter_list,
    MSG_member_declared_with_non_automatic_storage,
    MSG_member_declared_as_function
};

#endif
