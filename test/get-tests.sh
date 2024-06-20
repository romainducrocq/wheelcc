#!/bin/bash

ROOT="${PWD}/.."
TEST_SUITE="${ROOT}/../writing-a-c-compiler-tests"

TEST_DIR="${PWD}/tests"
TEST_SRCS=(
"1_int_constants"
"2_unary_operators"
"3_binary_operators"
"4_logical_and_relational_operators"
"5_local_variables"
"6_statements_and_conditional_expressions"
"7_compound_statements"
"8_loops"
"9_functions"
"10_file-scope_variables_and_storage-class_specifiers"
"11_long_integers"
"12_unsigned_integers"
"13_floating-point_numbers"
"14_pointers"
"15_arrays_and_pointer_arithmetic"
"16_characters_and_strings"
"17_supporting_dynamic_memory_allocation"
"18_structures"
)

# Get all tests
i=0
for SRC in ${TEST_SRCS[@]}; do
    i=$((i+1))
    rm -rv ${TEST_DIR}/${SRC}/*
    cp -rv ${TEST_SUITE}/tests/chapter_${i}/* ${TEST_DIR}/${SRC}/
done

# Remove unsupported extra credit
for FILE in $(find ${TEST_DIR} -name "*.c" -type f); do
    gcc -E -P ${FILE} -o ${FILE%.*}.i
done
for FILE in $(\
    grep -rnw $(find ${TEST_DIR} -name "*.i" -type f) \
        -e "++" -e "--" -e "switch" -e "case" -e "default" -e "union" | \
        cut -d":" -f1 | sort --uniq); do
    rm -rv "${FILE%.*}.c"
done
rm -rv $(find ${TEST_DIR}/${TEST_SRCS[17]}/ -name "*.h" -type f | grep union | grep extra_credit)
rm -rv $(find ${TEST_DIR} -name "*.i" -type f)

# Remove unused assembly files
rm -rv $(find ${TEST_DIR} -name "*.s" -type f | grep --invert-match linux)

# Process special files
mv -v ${TEST_DIR}/${TEST_SRCS[12]}/valid/function_calls/standard_library_call.c ${TEST_DIR}/${TEST_SRCS[12]}/valid/function_calls/standard_library_call__+lm.c
mv -v ${TEST_DIR}/${TEST_SRCS[12]}/valid/extra_credit/nan.c ${TEST_DIR}/${TEST_SRCS[12]}/valid/extra_credit/nan__+lm.c
mv -v ${TEST_DIR}/${TEST_SRCS[12]}/valid/libraries/double_params_and_result.c ${TEST_DIR}/${TEST_SRCS[12]}/valid/libraries/double_params_and_result__+lm.c
mv -v ${TEST_DIR}/${TEST_SRCS[12]}/valid/libraries/double_params_and_result_client.c ${TEST_DIR}/${TEST_SRCS[12]}/valid/libraries/double_params_and_result__+lm_client.c
mv -v ${TEST_DIR}/${TEST_SRCS[12]}/valid/special_values/negative_zero.c ${TEST_DIR}/${TEST_SRCS[12]}/valid/special_values/negative_zero__+lm.c

mv -v ${TEST_DIR}/${TEST_SRCS[8]}/valid/stack_arguments/stack_alignment_check_linux.s ${TEST_DIR}/${TEST_SRCS[8]}/valid/stack_arguments/stack_alignment_data.s
mv -v ${TEST_DIR}/${TEST_SRCS[9]}/valid/data_on_page_boundary_linux.s ${TEST_DIR}/${TEST_SRCS[9]}/valid/push_arg_on_page_boundary_data.s
mv -v ${TEST_DIR}/${TEST_SRCS[15]}/valid/chars/data_on_page_boundary_linux.s ${TEST_DIR}/${TEST_SRCS[15]}/valid/chars/push_arg_on_page_boundary_data.s
mv -v ${TEST_DIR}/${TEST_SRCS[17]}/valid/params_and_returns/data_on_page_boundary_linux.s ${TEST_DIR}/${TEST_SRCS[17]}/valid/params_and_returns/return_struct_on_page_boundary_data.s
mv -v ${TEST_DIR}/${TEST_SRCS[17]}/valid/params_and_returns/return_space_address_overlap_linux.s ${TEST_DIR}/${TEST_SRCS[17]}/valid/params_and_returns/return_space_overlap_data.s
mv -v ${TEST_DIR}/${TEST_SRCS[17]}/valid/params_and_returns/big_data_on_page_boundary_linux.s ${TEST_DIR}/${TEST_SRCS[17]}/valid/params_and_returns/return_big_struct_on_page_boundary_data.s
mv -v ${TEST_DIR}/${TEST_SRCS[17]}/valid/parameters/data_on_page_boundary_linux.s ${TEST_DIR}/${TEST_SRCS[17]}/valid/parameters/pass_args_on_page_boundary_data.s

gcc -v -S ${TEST_DIR}/${TEST_SRCS[12]}/helper_libs/nan.c -o ${TEST_DIR}/${TEST_SRCS[12]}/valid/extra_credit/nan__+lm_data.s 2>&1 | grep COLLECT_GCC_OPTIONS
rm -rv ${TEST_DIR}/${TEST_SRCS[12]}/helper_libs/

mv -v ${TEST_DIR}/${TEST_SRCS[13]}/valid/extra_credit/eval_compound_lhs_once.c ${TEST_DIR}/${TEST_SRCS[13]}/valid/extra_credit/eval_compound_lhs_once.c.fail
mv -v ${TEST_DIR}/${TEST_SRCS[14]}/valid/extra_credit/compound_assign_to_subscripted_val.c ${TEST_DIR}/${TEST_SRCS[14]}/valid/extra_credit/compound_assign_to_subscripted_val.c.fail

exit 0
