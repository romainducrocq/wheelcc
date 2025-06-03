	.text
	.section	.rodata
	.align 8
.LC0:
	.string	"Expected argument %d to have value 10, actual value was %d\n"
	.text
	.globl	validate
	.type	validate, @function
validate:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$96, %rsp
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
	movl	$0, -4(%rbp)
	jmp	.L2
.L4:
	movl	-4(%rbp), %eax
	cltq
	movl	-64(%rbp,%rax,4), %eax
	cmpl	$10, %eax
	je	.L3
	movl	-4(%rbp), %eax
	cltq
	movl	-64(%rbp,%rax,4), %edx
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L3:
	addl	$1, -4(%rbp)
.L2:
	cmpl	$12, -4(%rbp)
	jle	.L4
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	validate, .-validate
	## define some floating-point constants
	.section .rodata
	.align 8
.Lone:
	.double 1.0
.Ltwo:
	.double 2.0
.Lthree:
	.double 3.0
.Lfour:
	.double 4.0
.Lfive:
	.double 5.0
.Lsix:
	.double 6.0
.Lseven:
	.double 7.0
.Leight:
	.double 8.0
	## define main
	.text
	.globl	main
main:
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
	movsd	.Lone(%rip), %xmm0
	movsd	.Ltwo(%rip), %xmm1
	movsd	.Lthree(%rip), %xmm2
	movsd	.Lfour(%rip), %xmm3
	movsd	.Lfive(%rip), %xmm4
	movsd	.Lsix(%rip), %xmm5
	movsd	.Lseven(%rip), %xmm6
	movsd	.Leight(%rip), %xmm7
	callq	target
	# make sure values of callee-saved regs were preserved
	cmpq	$-1, %rbx
	jne		.Lfail
	cmpq	$-2, %r12
	jne		.Lfail
	cmp		$-3, %r13
	jne		.Lfail
	cmpq	$-4, %r14
	jne		.Lfail
	cmp		$-5, %r15
	jne		.Lfail
	popq 	%rdi
	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%rbx
	popq	%rbp
	retq
.Lfail:
	# raise SIGSEGV
	movl	$11, %edi
	call	raise@PLT
	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%rbx
	popq	%rbp
	retq

    .section        ".note.GNU-stack","",@progbits

