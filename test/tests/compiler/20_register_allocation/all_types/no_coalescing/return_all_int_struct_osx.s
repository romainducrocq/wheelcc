	.section	__TEXT,__text,regular,pure_instructions
	.globl	_target                         ## -- Begin function target
	.p2align	4, 0x90
_target:                                ## @target
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	callq	_return_struct
	movq	%rax, -16(%rbp)
	movq	%rdx, -8(%rbp)
	cmpl	$20, -16(%rbp)
	je	LBB0_2
## %bb.1:
	movl	-16(%rbp), %esi
	leaq	L_.str(%rip), %rdi
	xorl	%eax, %eax
                                        ## kill: def $al killed $al killed $eax
	callq	_printf
	movl	$-1, %edi
	callq	_exit
LBB0_2:
	cmpl	$30, -12(%rbp)
	je	LBB0_4
## %bb.3:
	movl	-12(%rbp), %esi
	leaq	L_.str.1(%rip), %rdi
	xorl	%eax, %eax
                                        ## kill: def $al killed $al killed $eax
	callq	_printf
	movl	$-1, %edi
	callq	_exit
LBB0_4:
	cmpq	$40, -8(%rbp)
	je	LBB0_6
## %bb.5:
	movq	-8(%rbp), %rsi
	leaq	L_.str.2(%rip), %rdi
	xorl	%eax, %eax
                                        ## kill: def $al killed $al killed $eax
	callq	_printf
	movl	$-1, %edi
	callq	_exit
LBB0_6:
	xorl	%eax, %eax
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"Expected retval.a to have value 20, actual value was %d\n"

L_.str.1:                               ## @.str.1
	.asciz	"Expected retval.b to have value 30, actual value was %d\n"

L_.str.2:                               ## @.str.2
	.asciz	"Expected retval.l to have value 40, actual value was %ld\n"

.subsections_via_symbols
	## define some floating-point constants
	.literal8
Lone:
	.double 1.0
Ltwo:
	.double 2.0
Lthree:
	.double 3.0
Lfour:
	.double 4.0
Lfive:
	.double 5.0
Lsix:
	.double 6.0
Lseven:
	.double 7.0
Leight:
	.double 8.0
	## define main
	.text
	.globl	_main
_main:
	pushq	%rbp
	movq	%rsp, %rbp
	# save callee-saved regs
	push	%rbx
	pushq	%r12
	pushq	%r13
	pushq	%r14
	pushq	%r15
	pushq 	%rdi # to maintain stack alignment
	# give them arbitrary values
	movq	$-1, %rbx
	movq	$-2, %r12
	movq	$-3, %r13
	movq	$-4, %r14
	movq	$-5, %r15
	# call target
	movl	$1, %edi
	movl	$2, %esi
	movl	$3, %edx
	movl	$4, %ecx
	movl	$5, %r8d
	movl	$6, %r9d
	movsd	Lone(%rip), %xmm0
	movsd	Ltwo(%rip), %xmm1
	movsd	Lthree(%rip), %xmm2
	movsd	Lfour(%rip), %xmm3
	movsd	Lfive(%rip), %xmm4
	movsd	Lsix(%rip), %xmm5
	movsd	Lseven(%rip), %xmm6
	movsd	Leight(%rip), %xmm7
	callq	_target
	# make sure values of callee-saved regs were preserved
	cmpq	$-1, %rbx
	jne		Lfail
	cmpq	$-2, %r12
	jne		Lfail
	cmp		$-3, %r13
	jne		Lfail
	cmpq	$-4, %r14
	jne		Lfail
	cmp		$-5, %r15
	jne		Lfail
	popq 	%rdi
	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%rbx
	popq	%rbp
	retq
Lfail:
	# raise SIGSEGV
	movl	$11, %edi
	call	_raise
	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%rbx
	popq	%rbp
	retq


