	.section	__TEXT,__text,regular,pure_instructions
	.globl	_exit_wrapper                   ## -- Begin function exit_wrapper
	.p2align	4, 0x90
_exit_wrapper:                          ## @exit_wrapper
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %edi
	callq	_exit
	.cfi_endproc
                                        ## -- End function
.subsections_via_symbols

