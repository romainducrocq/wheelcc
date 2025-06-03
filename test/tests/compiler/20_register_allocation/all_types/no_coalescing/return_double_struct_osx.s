	.section	__TEXT,__text,regular,pure_instructions
	.section	__TEXT,__literal8,8byte_literals
	.p2align	3, 0x0                          ## -- Begin function target
LCPI0_0:
	.quad	0x4069000000000000              ## double 200
	.section	__TEXT,__text,regular,pure_instructions
	.globl	_target
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
	movsd	%xmm0, -16(%rbp)
	movsd	%xmm1, -8(%rbp)
	movsd	-16(%rbp), %xmm0                ## xmm0 = mem[0],zero
	xorps	%xmm1, %xmm1
	ucomisd	%xmm1, %xmm0
	jne	LBB0_1
	jp	LBB0_1
	jmp	LBB0_2
LBB0_1:
	movsd	-16(%rbp), %xmm0                ## xmm0 = mem[0],zero
	leaq	L_.str(%rip), %rdi
	movb	$1, %al
	callq	_printf
	movl	$-1, %edi
	callq	_exit
LBB0_2:
	movsd	-8(%rbp), %xmm0                 ## xmm0 = mem[0],zero
	movsd	LCPI0_0(%rip), %xmm1            ## xmm1 = [2.0E+2,0.0E+0]
	ucomisd	%xmm1, %xmm0
	jne	LBB0_3
	jp	LBB0_3
	jmp	LBB0_4
LBB0_3:
	movsd	-8(%rbp), %xmm0                 ## xmm0 = mem[0],zero
	leaq	L_.str.1(%rip), %rdi
	movb	$1, %al
	callq	_printf
	movl	$-1, %edi
	callq	_exit
LBB0_4:
	xorl	%eax, %eax
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"Expected retval.d1 to have value 0.0, actual value was %f\n"

L_.str.1:                               ## @.str.1
	.asciz	"Expected retval.d2 to have value 200.0, actual value was %f\n"

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


