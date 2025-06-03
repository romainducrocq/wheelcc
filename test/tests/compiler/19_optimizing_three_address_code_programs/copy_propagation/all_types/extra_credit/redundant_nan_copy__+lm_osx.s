	.section	__TEXT,__text,regular,pure_instructions
	.globl	_double_isnan                   ## -- Begin function double_isnan
	.p2align	4, 0x90
_double_isnan:                          ## @double_isnan
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movsd	%xmm0, -16(%rbp)
	movsd	-16(%rbp), %xmm0                ## xmm0 = mem[0],zero
	movsd	%xmm0, -8(%rbp)
	movsd	-8(%rbp), %xmm0                 ## xmm0 = mem[0],zero
	ucomisd	-8(%rbp), %xmm0
	setne	%al
	setp	%cl
	orb	%cl, %al
	andb	$1, %al
	movzbl	%al, %eax
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
.subsections_via_symbols

