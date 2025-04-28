	.text
	.section	.rodata
	.align 8
.LC1:
	.string	"Expected retval.d1 to have value 0.0, actual value was %f\n"
	.align 8
.LC3:
	.string	"Expected retval.d2 to have value 200.0, actual value was %f\n"
	.text
	.globl	target
	.type	target, @function
target:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	call	return_struct@PLT
	movq	%xmm0, %rax
	movapd	%xmm1, %xmm0
	movq	%rax, -16(%rbp)
	movsd	%xmm0, -8(%rbp)
	movsd	-16(%rbp), %xmm0
	pxor	%xmm1, %xmm1
	ucomisd	%xmm1, %xmm0
	jp	.L7
	pxor	%xmm1, %xmm1
	ucomisd	%xmm1, %xmm0
	je	.L2
.L7:
	movq	-16(%rbp), %rax
	movq	%rax, %xmm0
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	movl	$1, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L2:
	movsd	-8(%rbp), %xmm0
	ucomisd	.LC2(%rip), %xmm0
	jp	.L8
	ucomisd	.LC2(%rip), %xmm0
	je	.L4
.L8:
	movq	-8(%rbp), %rax
	movq	%rax, %xmm0
	leaq	.LC3(%rip), %rax
	movq	%rax, %rdi
	movl	$1, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L4:
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	target, .-target
	.section	.rodata
	.align 8
.LC2:
	.long	0
	.long	1080623104
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
