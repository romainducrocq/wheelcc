#!/bin/bash

LIGHT_RED='\033[1;31m'
LIGHT_GREEN='\033[1;32m'
NC='\033[0m'

function stage () {
    echo "parse"
}

function invalid () {
    wheelcc --$(stage) ${1} > /dev/null 2>&1
    RET=${?}
    if [ ${RET} -ne 0 ]
    then
        echo -e "${LIGHT_GREEN}[y (${RET})] ${1}${NC}"
    else
        echo -e "${LIGHT_RED}[n (${RET})] ${1}${NC}"
    fi
}

function valid () {
    wheelcc --$(stage) ${1} > /dev/null 2>&1
    RET=${?}
    if [ ${RET} -eq 0 ]
    then
        echo -e "${LIGHT_GREEN}[y (${RET})] ${1}${NC}"
    else
        echo -e "${LIGHT_RED}[n (${RET})] ${1}${NC}"
    fi
}

## parse

# invalid
invalid tests/18_structures/invalid_lex/dot_bad_token.c
invalid tests/18_structures/invalid_lex/dot_bad_token_2.c
invalid tests/18_structures/invalid_parse/arrow_missing_member.c
invalid tests/18_structures/invalid_parse/struct_member_no_type.c
invalid tests/18_structures/invalid_parse/var_decl_two_struct_kws.c
invalid tests/18_structures/invalid_parse/struct_decl_extra_semicolon.c
invalid tests/18_structures/invalid_parse/var_decl_bad_tag_2.c
invalid tests/18_structures/invalid_parse/struct_member_name_kw.c
invalid tests/18_structures/invalid_parse/struct_decl_double_semicolon.c
invalid tests/18_structures/invalid_parse/struct_member_storage_class.c
invalid tests/18_structures/invalid_parse/dot_invalid_member.c
invalid tests/18_structures/invalid_parse/var_decl_bad_type_specifier.c
invalid tests/18_structures/invalid_parse/struct_member_is_function.c
invalid tests/18_structures/invalid_parse/var_decl_missing_struct_kw.c
invalid tests/18_structures/invalid_parse/struct_decl_kw_wrong_order.c
invalid tests/18_structures/invalid_parse/struct_member_no_declarator.c
invalid tests/18_structures/invalid_parse/struct_decl_two_kws.c
invalid tests/18_structures/invalid_parse/struct_decl_tag_kw.c
invalid tests/18_structures/invalid_parse/struct_decl_missing_end_semicolon.c
invalid tests/18_structures/invalid_parse/dot_no_left_expr.c
invalid tests/18_structures/invalid_parse/struct_member_no_semicolon.c
invalid tests/18_structures/invalid_parse/var_decl_two_tags.c
invalid tests/18_structures/invalid_parse/misplaced_storage_class.c
invalid tests/18_structures/invalid_parse/var_decl_bad_tag_1.c
invalid tests/18_structures/invalid_parse/struct_decl_empty_member_list.c
invalid tests/18_structures/invalid_parse/struct_member_initializer.c
invalid tests/18_structures/invalid_parse/dot_operator_in_declarator.c

# valid
valid tests/18_structures/valid/no_structure_parameters/parse_and_lex/trailing_comma.c
valid tests/18_structures/valid/no_structure_parameters/parse_and_lex/space_around_struct_member.c
valid tests/18_structures/valid/no_structure_parameters/parse_and_lex/postfix_precedence.c
valid tests/18_structures/valid/no_structure_parameters/parse_and_lex/struct_member_looks_like_const.c

# ## validate

# # invalid
# invalid tests/18_structures/invalid_struct_tags/cast_undeclared.c
# invalid tests/18_structures/invalid_struct_tags/for_loop_scope.c
# invalid tests/18_structures/invalid_struct_tags/array_of_undeclared.c
# invalid tests/18_structures/invalid_struct_tags/deref_undeclared.c
# invalid tests/18_structures/invalid_struct_tags/member_type_undeclared.c
# invalid tests/18_structures/invalid_struct_tags/file_scope_var_type_undeclared.c
# invalid tests/18_structures/invalid_struct_tags/sizeof_undeclared.c
# invalid tests/18_structures/invalid_struct_tags/param_undeclared.c
# invalid tests/18_structures/invalid_struct_tags/return_type_undeclared.c
# invalid tests/18_structures/invalid_struct_tags/for_loop_scope_2.c
# invalid tests/18_structures/invalid_struct_tags/var_type_undeclared.c
# invalid tests/18_structures/invalid_types/tag_resolution/conflicting_fun_ret_types.c
# invalid tests/18_structures/invalid_types/tag_resolution/distinct_struct_types.c
# invalid tests/18_structures/invalid_types/tag_resolution/incomplete_shadows_complete_cast.c
# invalid tests/18_structures/invalid_types/tag_resolution/conflicting_fun_param_types.c
# invalid tests/18_structures/invalid_types/tag_resolution/member_name_wrong_scope_nested.c
# invalid tests/18_structures/invalid_types/tag_resolution/shadow_struct.c
# invalid tests/18_structures/invalid_types/tag_resolution/mismatched_return_type.c
# invalid tests/18_structures/invalid_types/tag_resolution/member_name_wrong_scope.c
# invalid tests/18_structures/invalid_types/tag_resolution/incomplete_shadows_complete.c
# invalid tests/18_structures/invalid_types/tag_resolution/invalid_shadow_self_reference.c
# invalid tests/18_structures/invalid_types/tag_resolution/address_of_wrong_type.c
# invalid tests/18_structures/invalid_types/tag_resolution/shadowed_tag_branch_mismatch.c

# # valid
# valid tests/18_structures/valid/no_structure_parameters/semantic_analysis/resolve_tags.c
# valid tests/18_structures/valid/no_structure_parameters/semantic_analysis/namespaces.c
