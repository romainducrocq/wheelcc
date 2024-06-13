#!/bin/bash

ROOT="${PWD}/.."
TEST_SUITE="${ROOT}/../writing-a-c-compiler-tests"

function test () {
    echo ""
    echo "----------------------------------------------------------------------"
    echo "${@}"
    ./test_compiler ${ROOT}/bin/driver.sh ${@}
    if [ ${?} -ne 0 ]; then exit 1; fi
}

cd ${TEST_SUITE}

if [ ${#} -ne 0 ]; then
    test ${@}
else
    for i in $(seq 1 18); do
        # test --chapter ${i} --latest-only --bitwise --compound --goto --nan
        # --bitwise
        #     chapter 3 : test_valid/extra_credit/bitwise_shiftr_negative
        #     chapter 10: test_valid/extra_credit/bitwise_ops_file_scope_vars
        #     chapter 11: test_valid/extra_credit/bitshift
        #     chapter 16: test_valid/extra_credit/bitshift_chars
        # --compound
        #     chapter 14: test_valid/extra_credit/compound_assign_through_pointer
        #                 test_valid/extra_credit/eval_compound_lhs_once
        #     chapter 15: test_valid/extra_credit/compound_assign_to_nested_subscript
        #                 test_valid/extra_credit/compound_assign_to_subscripted_val
        # --bitwise --compound
        #     chapter 11: test_valid/extra_credit/compound_bitshift.c
        #     chapter 12: test_valid/extra_credit/compound_bitshift.c
        #     chapter 14: test_valid/extra_credit/compound_bitwise_dereferenced_ptrs.c
        # --goto
        # --nan
        test --chapter ${i} --latest-only
    done
fi

exit 0
