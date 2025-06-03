	.text
	.globl	exit_wrapper
	.type	exit_wrapper, @function
exit_wrapper:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	exit@PLT
	.cfi_endproc
.LFE0:
	.size	exit_wrapper, .-exit_wrapper
    .section        ".note.GNU-stack","",@progbits

