#!/bin/bash

CC="gcc -pedantic-errors -std=c17"

ROOT="${PWD}/.."
TEST_SUITE="${ROOT}/../writing-a-c-compiler-tests"

TEST_DIR="${PWD}/tests/compiler"
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
"19_optimizing_three_address_code_programs"
"20_register_allocation"
)

# Get all tests
i=0
for SRC in ${TEST_SRCS[@]}; do
    i=$((i+1))
    mkdir -p ${TEST_DIR}/${SRC}/
    rm -rv $${TEST_DIR}/${SRC}/*
    cp -rv ${TEST_SUITE}/tests/chapter_${i}/* ${TEST_DIR}/${SRC}/
done

# Remove unused markdown files
rm -rv $(find ${TEST_DIR} -name "*.md" -type f)

# Remove unused assembly files
rm -rv $(find ${TEST_DIR} -name "*.s" -type f | grep --invert-match linux)

# Add libs and client dependencies
mv -v ${TEST_DIR}/${TEST_SRCS[12]}/valid/function_calls/standard_library_call.c ${TEST_DIR}/${TEST_SRCS[12]}/valid/function_calls/standard_library_call__+lm.c
mv -v ${TEST_DIR}/${TEST_SRCS[12]}/valid/extra_credit/nan.c ${TEST_DIR}/${TEST_SRCS[12]}/valid/extra_credit/nan__+lm.c
mv -v ${TEST_DIR}/${TEST_SRCS[12]}/valid/extra_credit/nan_compound_assign.c ${TEST_DIR}/${TEST_SRCS[12]}/valid/extra_credit/nan_compound_assign__+lm.c
mv -v ${TEST_DIR}/${TEST_SRCS[12]}/valid/extra_credit/nan_incr_and_decr.c ${TEST_DIR}/${TEST_SRCS[12]}/valid/extra_credit/nan_incr_and_decr__+lm.c
mv -v ${TEST_DIR}/${TEST_SRCS[12]}/valid/libraries/double_params_and_result.c ${TEST_DIR}/${TEST_SRCS[12]}/valid/libraries/double_params_and_result__+lm.c
mv -v ${TEST_DIR}/${TEST_SRCS[12]}/valid/libraries/double_params_and_result_client.c ${TEST_DIR}/${TEST_SRCS[12]}/valid/libraries/double_params_and_result__+lm_client.c
mv -v ${TEST_DIR}/${TEST_SRCS[12]}/valid/special_values/negative_zero.c ${TEST_DIR}/${TEST_SRCS[12]}/valid/special_values/negative_zero__+lm.c
mv -v ${TEST_DIR}/${TEST_SRCS[18]}/constant_folding/all_types/extra_credit/fold_nan.c ${TEST_DIR}/${TEST_SRCS[18]}/constant_folding/all_types/extra_credit/fold_nan__+lm.c 
mv -v ${TEST_DIR}/${TEST_SRCS[18]}/constant_folding/all_types/extra_credit/return_nan.c ${TEST_DIR}/${TEST_SRCS[18]}/constant_folding/all_types/extra_credit/return_nan__+lm.c 
mv -v ${TEST_DIR}/${TEST_SRCS[18]}/copy_propagation/all_types/extra_credit/redundant_nan_copy.c ${TEST_DIR}/${TEST_SRCS[18]}/copy_propagation/all_types/extra_credit/redundant_nan_copy__+lm.c 
mv -v ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/return_double_lib.c ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/return_double_client.c

# Merge multiple helpers into one
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c >> ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/track_arg_registers_lib.c
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c >> ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/track_dbl_arg_registers_lib.c
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c >> ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/mixed_type_funcall_generates_args_lib.c
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c >> ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/funcall_generates_args_lib.c
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c >> ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/mixed_type_arg_registers_lib.c
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c >> ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/target_shim.c

# Add os assembly helpers
mv -v ${TEST_DIR}/${TEST_SRCS[8]}/valid/stack_arguments/stack_alignment_check_linux.s ${TEST_DIR}/${TEST_SRCS[8]}/valid/stack_arguments/stack_alignment_linux.s
mv -v ${TEST_DIR}/${TEST_SRCS[9]}/valid/data_on_page_boundary_linux.s ${TEST_DIR}/${TEST_SRCS[9]}/valid/push_arg_on_page_boundary_linux.s
mv -v ${TEST_DIR}/${TEST_SRCS[15]}/valid/chars/data_on_page_boundary_linux.s ${TEST_DIR}/${TEST_SRCS[15]}/valid/chars/push_arg_on_page_boundary_linux.s
mv -v ${TEST_DIR}/${TEST_SRCS[17]}/valid/parameters/data_on_page_boundary_linux.s ${TEST_DIR}/${TEST_SRCS[17]}/valid/parameters/pass_args_on_page_boundary_linux.s
mv -v ${TEST_DIR}/${TEST_SRCS[17]}/valid/params_and_returns/data_on_page_boundary_linux.s ${TEST_DIR}/${TEST_SRCS[17]}/valid/params_and_returns/return_struct_on_page_boundary_linux.s
mv -v ${TEST_DIR}/${TEST_SRCS[17]}/valid/params_and_returns/return_space_address_overlap_linux.s ${TEST_DIR}/${TEST_SRCS[17]}/valid/params_and_returns/return_space_overlap_linux.s
mv -v ${TEST_DIR}/${TEST_SRCS[17]}/valid/params_and_returns/big_data_on_page_boundary_linux.s ${TEST_DIR}/${TEST_SRCS[17]}/valid/params_and_returns/return_big_struct_on_page_boundary_linux.s
mv -v ${TEST_DIR}/${TEST_SRCS[17]}/valid/params_and_returns/validate_return_pointer_linux.s ${TEST_DIR}/${TEST_SRCS[17]}/valid/params_and_returns/return_pointer_in_rax_linux.s
mv -v ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/clobber_xmm_regs_linux.s ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/dbl_fun_call_linux.s

# Generate assembly for helpers that use libc
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[12]}/helper_libs/nan.c -o ${TEST_DIR}/${TEST_SRCS[12]}/valid/extra_credit/nan__+lm_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[12]}/helper_libs/nan.c -o ${TEST_DIR}/${TEST_SRCS[12]}/valid/extra_credit/nan_compound_assign__+lm_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[12]}/helper_libs/nan.c -o ${TEST_DIR}/${TEST_SRCS[12]}/valid/extra_credit/nan_incr_and_decr__+lm_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[12]}/helper_libs/nan.c -o ${TEST_DIR}/${TEST_SRCS[18]}/constant_folding/all_types/extra_credit/fold_nan__+lm_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[12]}/helper_libs/nan.c -o ${TEST_DIR}/${TEST_SRCS[18]}/constant_folding/all_types/extra_credit/return_nan__+lm_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[12]}/helper_libs/nan.c -o ${TEST_DIR}/${TEST_SRCS[18]}/copy_propagation/all_types/extra_credit/redundant_nan_copy__+lm_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[18]}/helper_libs/exit.c -o ${TEST_DIR}/${TEST_SRCS[18]}/unreachable_code_elimination/infinite_loop_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[18]}/helper_libs/exit.c -o ${TEST_DIR}/${TEST_SRCS[18]}/dead_store_elimination/int_only/static_not_always_live_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/track_arg_registers_lib.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/track_arg_registers_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/track_dbl_arg_registers_lib.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/track_dbl_arg_registers_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/mixed_type_funcall_generates_args_lib.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/mixed_type_funcall_generates_args_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/funcall_generates_args_lib.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/funcall_generates_args_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/funcall_generates_args_lib.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/dbl_funcall_generates_args_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/return_all_int_struct_lib.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/return_all_int_struct_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/return_double_struct_lib.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/return_double_struct_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/mixed_type_arg_registers_lib.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/mixed_type_arg_registers_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/coalesce_prevents_spill_lib.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/coalesce_prevents_spill_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/target_shim.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/george_coalesce_xmm_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/use_all_hardregs_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/eax_live_at_exit_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/cmp_generates_operands_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/many_pseudos_fewer_conflicts_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/copy_no_interference_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/idiv_interference_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/optimistic_coloring_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/preserve_across_fun_call_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/same_instr_interference_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/same_instr_no_interference_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/unary_interference_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/unary_uses_operand_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/bin_uses_operands_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/division_uses_ax_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/force_spill_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/loop_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/rewrite_regression_test_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/test_spill_metric_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/trivially_colorable_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/callee_saved_stack_alignment_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/mixed_type_stack_alignment_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/indexed_operand_reads_regs_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/test_spill_metric_2_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/force_spill_doubles_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/force_spill_mixed_ints_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/div_uses_ax_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/store_pointer_in_register_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/div_interference_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/dbl_bin_uses_operands_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/type_conversion_interference_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/gp_xmm_mixed_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/one_aliased_var_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/xmm0_live_at_exit_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/ptr_rax_live_at_exit_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/george_coalesce_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/briggs_coalesce_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/briggs_coalesce_hardreg_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/briggs_dont_coalesce_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/george_dont_coalesce_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/george_dont_coalesce_2_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/no_george_test_for_pseudos_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/george_off_by_one_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/briggs_coalesce_xmm_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/george_off_by_one_xmm_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/briggs_xmm_k_value_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/george_xmm_k_value_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/briggs_coalesce_long_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/dont_coalesce_movzx_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS
${CC} -v -S ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/coalesce_char_linux.s 2>&1 | grep COLLECT_GCC_OPTIONS

# Merge multiple assembly helpers into one
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/alignment_check_wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/callee_saved_stack_alignment_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/alignment_check_wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/mixed_type_stack_alignment_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/briggs_coalesce_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/briggs_coalesce_hardreg_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/briggs_coalesce_long_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/briggs_coalesce_xmm_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/briggs_dont_coalesce_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/briggs_xmm_k_value_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/coalesce_char_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/coalesce_prevents_spill_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/copy_no_interference_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/force_spill_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/force_spill_doubles_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/force_spill_mixed_ints_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/george_coalesce_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/george_coalesce_xmm_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/george_dont_coalesce_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/george_dont_coalesce_2_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/george_off_by_one_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/george_off_by_one_xmm_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/george_xmm_k_value_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/gp_xmm_mixed_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/loop_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/many_pseudos_fewer_conflicts_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/mixed_type_arg_registers_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/no_george_test_for_pseudos_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/one_aliased_var_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/optimistic_coloring_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/preserve_across_fun_call_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/return_all_int_struct_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/return_double_struct_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/rewrite_regression_test_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/same_instr_no_interference_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/store_pointer_in_register_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/test_spill_metric_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/test_spill_metric_2_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/track_arg_registers_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/track_dbl_arg_registers_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/trivially_colorable_linux.s
cat ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/wrapper_linux.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/use_all_hardregs_linux.s

# Cleanup assembly helpers
for i in $(find ${TEST_DIR}/ -name "*_linux.s" -type f); do
    sed -i '/\.ident/d' ${i}
    sed -i '/\.file/d' ${i}
    sed -i '/\.note\.GNU-stack/d' ${i}
    echo "    .section        \".note.GNU-stack\",\"\",@progbits" >> ${i}
done

# Cleanup helpers
rm -rv ${TEST_DIR}/${TEST_SRCS[12]}/helper_libs/
rm -rv ${TEST_DIR}/${TEST_SRCS[18]}/helper_libs/
rm -rv ${TEST_DIR}/${TEST_SRCS[19]}/helper_libs/

exit 0
