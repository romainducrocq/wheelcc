#!/bin/bash

rm -rv */*
for i in $(seq 1 18); do cp -rv ../../../writing-a-c-compiler-tests/tests/chapter_${i}/* ${i}_*/; done
for i in $(find . -name "*.c" -type f); do gcc -E -P ${i} -o ${i%.*}.i; done
for i in $(grep -rnw $(find . -name "*.i" -type f) -e "++" -e "--" -e "switch" -e "case" -e "default" -e "union" | cut -d":" -f1 | sort --uniq); do rm -rv "${i%.*}.c"; done
rm $(find 18_structures/ -name "*.h" -type f | grep union | grep extra_credit)
rm -rv $(find . -name "*.i" -type f)
rm -rv $(find . -name "*.s" -type f | grep --invert-match linux)

mv -v 13_floating-point_numbers/valid/function_calls/standard_library_call.c 13_floating-point_numbers/valid/function_calls/standard_library_call__+lm.c
mv -v 13_floating-point_numbers/valid/extra_credit/nan.c 13_floating-point_numbers/valid/extra_credit/nan__+lm.c
mv -v 13_floating-point_numbers/valid/libraries/double_params_and_result.c 13_floating-point_numbers/valid/libraries/double_params_and_result__+lm.c
mv -v 13_floating-point_numbers/valid/libraries/double_params_and_result_client.c 13_floating-point_numbers/valid/libraries/double_params_and_result__+lm_client.c
mv -v 13_floating-point_numbers/valid/special_values/negative_zero.c 13_floating-point_numbers/valid/special_values/negative_zero__+lm.c

mv -v 9_functions/valid/stack_arguments/stack_alignment_check_linux.s 9_functions/valid/stack_arguments/stack_alignment_data.s
mv -v 10_file-scope_variables_and_storage-class_specifiers/valid/data_on_page_boundary_linux.s 10_file-scope_variables_and_storage-class_specifiers/valid/push_arg_on_page_boundary_data.s
mv -v 16_characters_and_strings/valid/chars/data_on_page_boundary_linux.s 16_characters_and_strings/valid/chars/push_arg_on_page_boundary_data.s
mv -v 18_structures/valid/params_and_returns/data_on_page_boundary_linux.s 18_structures/valid/params_and_returns/return_struct_on_page_boundary_data.s
mv -v 18_structures/valid/params_and_returns/return_space_address_overlap_linux.s 18_structures/valid/params_and_returns/return_space_overlap_data.s
mv -v 18_structures/valid/params_and_returns/big_data_on_page_boundary_linux.s 18_structures/valid/params_and_returns/return_big_struct_on_page_boundary_data.s
mv -v 18_structures/valid/parameters/data_on_page_boundary_linux.s 18_structures/valid/parameters/pass_args_on_page_boundary_data.s

gcc -S 13_floating-point_numbers/helper_libs/nan.c -o 13_floating-point_numbers/valid/extra_credit/nan__+lm_data.s
rm -rv 13_floating-point_numbers/helper_libs/

mv -v 14_pointers/valid/extra_credit/eval_compound_lhs_once.c 14_pointers/valid/extra_credit/eval_compound_lhs_once.c.fail
mv -v 15_arrays_and_pointer_arithmetic/valid/extra_credit/compound_assign_to_subscripted_val.c 15_arrays_and_pointer_arithmetic/valid/extra_credit/compound_assign_to_subscripted_val.c.fail
