#!/bin/bash

CC="clang -arch x86_64 -pedantic-errors -std=c17"

ROOT="${PWD}/../.."
TEST_SUITE="${ROOT}/../writing-a-c-compiler-tests"

TEST_DIR="${PWD}/../tests/compiler"
TEST_DIR_MACOS="${TEST_DIR}/macos"
TEST_SRCS=()
for i in $(seq 1 20); do
    TEST_SRCS+=("$(basename $(find ${TEST_DIR} -maxdepth 1 -name "${i}_*" -type d))")
done

# Get all tests
if [ -d "${TEST_DIR_MACOS}" ]; then
    rm -r ${TEST_DIR_MACOS}/
fi
i=0
for SRC in ${TEST_SRCS[@]}; do
    i=$((i+1))
    mkdir -p ${TEST_DIR_MACOS}/${SRC}/
    cp -rv ${TEST_SUITE}/tests/chapter_${i}/* ${TEST_DIR_MACOS}/${SRC}/
done

# Add newline for POSIX compliance
for i in $(find ${TEST_DIR_MACOS}/ -type f); do
    echo "" >> ${i}
done

# Merge multiple helpers into one
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c >> ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/track_arg_registers_lib.c
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c >> ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/track_dbl_arg_registers_lib.c
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c >> ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/mixed_type_funcall_generates_args_lib.c
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c >> ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/funcall_generates_args_lib.c
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c >> ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/mixed_type_arg_registers_lib.c
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c >> ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/target_shim.c

# Add assembly tests
mv -v ${TEST_DIR_MACOS}/${TEST_SRCS[8]}/valid/stack_arguments/stack_alignment_check_osx.s ${TEST_DIR}/${TEST_SRCS[8]}/valid/stack_arguments/stack_alignment_osx.s
mv -v ${TEST_DIR_MACOS}/${TEST_SRCS[9]}/valid/data_on_page_boundary_osx.s ${TEST_DIR}/${TEST_SRCS[9]}/valid/push_arg_on_page_boundary_osx.s
mv -v ${TEST_DIR_MACOS}/${TEST_SRCS[15]}/valid/chars/data_on_page_boundary_osx.s ${TEST_DIR}/${TEST_SRCS[15]}/valid/chars/push_arg_on_page_boundary_osx.s
mv -v ${TEST_DIR_MACOS}/${TEST_SRCS[17]}/valid/parameters/data_on_page_boundary_osx.s ${TEST_DIR}/${TEST_SRCS[17]}/valid/parameters/pass_args_on_page_boundary_osx.s
mv -v ${TEST_DIR_MACOS}/${TEST_SRCS[17]}/valid/params_and_returns/data_on_page_boundary_osx.s ${TEST_DIR}/${TEST_SRCS[17]}/valid/params_and_returns/return_struct_on_page_boundary_osx.s
mv -v ${TEST_DIR_MACOS}/${TEST_SRCS[17]}/valid/params_and_returns/return_space_address_overlap_osx.s ${TEST_DIR}/${TEST_SRCS[17]}/valid/params_and_returns/return_space_overlap_osx.s
mv -v ${TEST_DIR_MACOS}/${TEST_SRCS[17]}/valid/params_and_returns/big_data_on_page_boundary_osx.s ${TEST_DIR}/${TEST_SRCS[17]}/valid/params_and_returns/return_big_struct_on_page_boundary_osx.s
mv -v ${TEST_DIR_MACOS}/${TEST_SRCS[17]}/valid/params_and_returns/validate_return_pointer_osx.s ${TEST_DIR}/${TEST_SRCS[17]}/valid/params_and_returns/return_pointer_in_rax_osx.s
mv -v ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/clobber_xmm_regs_osx.s ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/dbl_fun_call_osx.s
cp -v ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/aliasing_optimized_away_osx.s
cp -v ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/dbl_trivially_colorable_osx.s
cp -v ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/fourteen_pseudos_interfere_osx.s
cp -v ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/cmp_no_updates_osx.s

# Generate assembly for helpers that use libc
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[12]}/helper_libs/nan.c -o ${TEST_DIR}/${TEST_SRCS[12]}/valid/extra_credit/nan__+lm_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[12]}/helper_libs/nan.c -o ${TEST_DIR}/${TEST_SRCS[12]}/valid/extra_credit/nan_compound_assign__+lm_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[12]}/helper_libs/nan.c -o ${TEST_DIR}/${TEST_SRCS[12]}/valid/extra_credit/nan_incr_and_decr__+lm_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[12]}/helper_libs/nan.c -o ${TEST_DIR}/${TEST_SRCS[18]}/constant_folding/all_types/extra_credit/fold_nan__+lm_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[12]}/helper_libs/nan.c -o ${TEST_DIR}/${TEST_SRCS[18]}/constant_folding/all_types/extra_credit/return_nan__+lm_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[12]}/helper_libs/nan.c -o ${TEST_DIR}/${TEST_SRCS[18]}/copy_propagation/all_types/extra_credit/redundant_nan_copy__+lm_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[18]}/helper_libs/exit.c -o ${TEST_DIR}/${TEST_SRCS[18]}/unreachable_code_elimination/infinite_loop_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[18]}/helper_libs/exit.c -o ${TEST_DIR}/${TEST_SRCS[18]}/dead_store_elimination/int_only/static_not_always_live_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/coalesce_prevents_spill_lib.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/coalesce_prevents_spill_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/funcall_generates_args_lib.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/dbl_funcall_generates_args_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/funcall_generates_args_lib.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/funcall_generates_args_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/mixed_type_arg_registers_lib.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/mixed_type_arg_registers_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/mixed_type_funcall_generates_args_lib.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/mixed_type_funcall_generates_args_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/return_all_int_struct_lib.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/return_all_int_struct_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/return_double_lib.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/return_double_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/return_double_struct_lib.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/return_double_struct_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/target_shim.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/george_coalesce_xmm_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/track_arg_registers_lib.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/track_arg_registers_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/track_dbl_arg_registers_lib.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/track_dbl_arg_registers_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/use_all_hardregs_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/eax_live_at_exit_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/cmp_generates_operands_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/many_pseudos_fewer_conflicts_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/copy_no_interference_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/idiv_interference_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/optimistic_coloring_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/preserve_across_fun_call_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/same_instr_interference_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/same_instr_no_interference_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/unary_interference_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/unary_uses_operand_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/bin_uses_operands_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/division_uses_ax_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/force_spill_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/loop_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/rewrite_regression_test_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/test_spill_metric_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/trivially_colorable_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/callee_saved_stack_alignment_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/mixed_type_stack_alignment_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/indexed_operand_reads_regs_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/test_spill_metric_2_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/force_spill_doubles_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/force_spill_mixed_ints_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/div_uses_ax_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/store_pointer_in_register_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/div_interference_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/dbl_bin_uses_operands_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/type_conversion_interference_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/gp_xmm_mixed_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/one_aliased_var_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/xmm0_live_at_exit_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/ptr_rax_live_at_exit_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/george_coalesce_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/briggs_coalesce_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/briggs_coalesce_hardreg_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/briggs_dont_coalesce_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/george_dont_coalesce_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/george_dont_coalesce_2_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/no_george_test_for_pseudos_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/george_off_by_one_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/briggs_coalesce_xmm_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/george_off_by_one_xmm_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/briggs_xmm_k_value_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/george_xmm_k_value_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/briggs_coalesce_long_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/dont_coalesce_movzx_osx.s 2>&1 | grep ".s -x c "
${CC} -v -S ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/util.c -o ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/coalesce_char_osx.s 2>&1 | grep ".s -x c "

# Merge multiple assembly helpers into one
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/alignment_check_wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/callee_saved_stack_alignment_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/alignment_check_wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/mixed_type_stack_alignment_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/briggs_coalesce_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/briggs_coalesce_hardreg_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/briggs_coalesce_long_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/briggs_coalesce_xmm_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/briggs_dont_coalesce_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/briggs_xmm_k_value_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/coalesce_char_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/coalesce_prevents_spill_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/copy_no_interference_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/force_spill_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/force_spill_doubles_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/force_spill_mixed_ints_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/george_coalesce_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/george_coalesce_xmm_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/george_dont_coalesce_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/george_dont_coalesce_2_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/george_off_by_one_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/george_off_by_one_xmm_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/with_coalescing/george_xmm_k_value_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/gp_xmm_mixed_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/loop_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/many_pseudos_fewer_conflicts_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/mixed_type_arg_registers_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/with_coalescing/no_george_test_for_pseudos_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/one_aliased_var_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/optimistic_coloring_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/preserve_across_fun_call_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/return_all_int_struct_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/return_double_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/return_double_struct_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/rewrite_regression_test_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/same_instr_no_interference_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/store_pointer_in_register_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/test_spill_metric_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/test_spill_metric_2_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/track_arg_registers_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/all_types/no_coalescing/track_dbl_arg_registers_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/trivially_colorable_osx.s
cat ${TEST_DIR_MACOS}/${TEST_SRCS[19]}/helper_libs/wrapper_osx.s >> ${TEST_DIR}/${TEST_SRCS[19]}/int_only/no_coalescing/use_all_hardregs_osx.s

# Cleanup assembly tests
for i in $(find ${TEST_DIR}/ -name "*_osx.s" -type f); do
    sed -i '' '/\.build_version/d' ${i}
    echo "" >> ${i}
done

# Cleanup macos
rm -r ${TEST_DIR_MACOS}/

exit 0
