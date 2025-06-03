	.section	__TEXT,__text,regular,pure_instructions
	.globl	_validate                       ## -- Begin function validate
	.p2align	4, 0x90
_validate:                              ## @validate
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$96, %rsp
	movl	64(%rbp), %eax
	movl	56(%rbp), %eax
	movl	48(%rbp), %eax
	movl	40(%rbp), %eax
	movl	32(%rbp), %eax
	movl	24(%rbp), %eax
	movl	16(%rbp), %eax
	movq	___stack_chk_guard@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movq	%rax, -8(%rbp)
	movl	%edi, -68(%rbp)
	movl	%esi, -72(%rbp)
	movl	%edx, -76(%rbp)
	movl	%ecx, -80(%rbp)
	movl	%r8d, -84(%rbp)
	movl	%r9d, -88(%rbp)
	movl	-68(%rbp), %eax
	movl	%eax, -64(%rbp)
	movl	-72(%rbp), %eax
	movl	%eax, -60(%rbp)
	movl	-76(%rbp), %eax
	movl	%eax, -56(%rbp)
	movl	-80(%rbp), %eax
	movl	%eax, -52(%rbp)
	movl	-84(%rbp), %eax
	movl	%eax, -48(%rbp)
	movl	-88(%rbp), %eax
	movl	%eax, -44(%rbp)
	movl	16(%rbp), %eax
	movl	%eax, -40(%rbp)
	movl	24(%rbp), %eax
	movl	%eax, -36(%rbp)
	movl	32(%rbp), %eax
	movl	%eax, -32(%rbp)
	movl	40(%rbp), %eax
	movl	%eax, -28(%rbp)
	movl	48(%rbp), %eax
	movl	%eax, -24(%rbp)
	movl	56(%rbp), %eax
	movl	%eax, -20(%rbp)
	movl	64(%rbp), %eax
	movl	%eax, -16(%rbp)
	movl	$0, -92(%rbp)
LBB0_1:                                 ## =>This Inner Loop Header: Depth=1
	cmpl	$13, -92(%rbp)
	jge	LBB0_7
## %bb.2:                               ##   in Loop: Header=BB0_1 Depth=1
	movslq	-92(%rbp), %rax
	cmpl	$10, -64(%rbp,%rax,4)
	je	LBB0_5
## %bb.3:
	movl	-92(%rbp), %esi
	movslq	-92(%rbp), %rax
	movl	-64(%rbp,%rax,4), %edx
	leaq	L_.str(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movq	___stack_chk_guard@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movq	-8(%rbp), %rcx
	cmpq	%rcx, %rax
	jne	LBB0_9
## %bb.4:
	movl	$-1, %edi
	callq	_exit
LBB0_5:                                 ##   in Loop: Header=BB0_1 Depth=1
	jmp	LBB0_6
LBB0_6:                                 ##   in Loop: Header=BB0_1 Depth=1
	movl	-92(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -92(%rbp)
	jmp	LBB0_1
LBB0_7:
	movq	___stack_chk_guard@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movq	-8(%rbp), %rcx
	cmpq	%rcx, %rax
	jne	LBB0_9
## %bb.8:
	xorl	%eax, %eax
	addq	$96, %rsp
	popq	%rbp
	retq
LBB0_9:
	callq	___stack_chk_fail
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"Expected argument %d to have value 10, actual value was %d\n"

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


