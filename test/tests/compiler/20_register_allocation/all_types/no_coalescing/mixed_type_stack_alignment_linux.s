	.text
	.section	.rodata
.LC0:
	.string	"Expected %d but found %d\n"
	.text
	.globl	check_one_int
	.type	check_one_int, @function
check_one_int:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	cmpl	-8(%rbp), %eax
	je	.L2
	movl	-4(%rbp), %edx
	movl	-8(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L2:
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	check_one_int, .-check_one_int
	.section	.rodata
	.align 8
.LC1:
	.string	"Expected argument %d to have value %d, actual value was %d\n"
	.text
	.globl	check_5_ints
	.type	check_5_ints, @function
check_5_ints:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$64, %rsp
	movl	%edi, -36(%rbp)
	movl	%esi, -40(%rbp)
	movl	%edx, -44(%rbp)
	movl	%ecx, -48(%rbp)
	movl	%r8d, -52(%rbp)
	movl	%r9d, -56(%rbp)
	movl	-36(%rbp), %eax
	movl	%eax, -32(%rbp)
	movl	-40(%rbp), %eax
	movl	%eax, -28(%rbp)
	movl	-44(%rbp), %eax
	movl	%eax, -24(%rbp)
	movl	-48(%rbp), %eax
	movl	%eax, -20(%rbp)
	movl	-52(%rbp), %eax
	movl	%eax, -16(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L5
.L7:
	movl	-56(%rbp), %edx
	movl	-4(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, -8(%rbp)
	movl	-4(%rbp), %eax
	cltq
	movl	-32(%rbp,%rax,4), %eax
	cmpl	%eax, -8(%rbp)
	je	.L6
	movl	-4(%rbp), %eax
	cltq
	movl	-32(%rbp,%rax,4), %edx
	movl	-56(%rbp), %ecx
	movl	-4(%rbp), %eax
	leal	(%rcx,%rax), %esi
	movl	-4(%rbp), %eax
	movl	%edx, %ecx
	movl	%esi, %edx
	movl	%eax, %esi
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L6:
	addl	$1, -4(%rbp)
.L5:
	cmpl	$4, -4(%rbp)
	jle	.L7
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	check_5_ints, .-check_5_ints
	.globl	check_12_ints
	.type	check_12_ints, @function
check_12_ints:
.LFB2:
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
	movl	$0, -4(%rbp)
	jmp	.L10
.L12:
	movl	64(%rbp), %edx
	movl	-4(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, -8(%rbp)
	movl	-4(%rbp), %eax
	cltq
	movl	-64(%rbp,%rax,4), %eax
	cmpl	%eax, -8(%rbp)
	je	.L11
	movl	-4(%rbp), %eax
	cltq
	movl	-64(%rbp,%rax,4), %edx
	movl	64(%rbp), %ecx
	movl	-4(%rbp), %eax
	leal	(%rcx,%rax), %esi
	movl	-4(%rbp), %eax
	movl	%edx, %ecx
	movl	%esi, %edx
	movl	%eax, %esi
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L11:
	addl	$1, -4(%rbp)
.L10:
	cmpl	$11, -4(%rbp)
	jle	.L12
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	check_12_ints, .-check_12_ints
	.section	.rodata
.LC2:
	.string	"Expected %c but found %c\n"
	.text
	.globl	check_one_uchar
	.type	check_one_uchar, @function
check_one_uchar:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, %edx
	movl	%esi, %eax
	movb	%dl, -4(%rbp)
	movb	%al, -8(%rbp)
	movzbl	-4(%rbp), %eax
	cmpb	-8(%rbp), %al
	je	.L15
	movzbl	-4(%rbp), %edx
	movzbl	-8(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC2(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L15:
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	check_one_uchar, .-check_one_uchar
	.section	.rodata
.LC3:
	.string	"Expected %u but found %u\n"
	.text
	.globl	check_one_uint
	.type	check_one_uint, @function
check_one_uint:
.LFB4:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	cmpl	-8(%rbp), %eax
	je	.L18
	movl	-4(%rbp), %edx
	movl	-8(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC3(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L18:
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	check_one_uint, .-check_one_uint
	.section	.rodata
.LC4:
	.string	"Expected %ld but found %ld\n"
	.text
	.globl	check_one_long
	.type	check_one_long, @function
check_one_long:
.LFB5:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	cmpq	-16(%rbp), %rax
	je	.L21
	movq	-8(%rbp), %rdx
	movq	-16(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC4(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L21:
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	check_one_long, .-check_one_long
	.section	.rodata
.LC5:
	.string	"Expected %lu but found %lu\n"
	.text
	.globl	check_one_ulong
	.type	check_one_ulong, @function
check_one_ulong:
.LFB6:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	cmpq	-16(%rbp), %rax
	je	.L24
	movq	-8(%rbp), %rdx
	movq	-16(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L24:
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	check_one_ulong, .-check_one_ulong
	.section	.rodata
.LC6:
	.string	"Expected %f but found %f\n"
	.text
	.globl	check_one_double
	.type	check_one_double, @function
check_one_double:
.LFB7:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movsd	%xmm0, -8(%rbp)
	movsd	%xmm1, -16(%rbp)
	movsd	-8(%rbp), %xmm0
	ucomisd	-16(%rbp), %xmm0
	jp	.L30
	movsd	-8(%rbp), %xmm0
	ucomisd	-16(%rbp), %xmm0
	je	.L27
.L30:
	movsd	-8(%rbp), %xmm0
	movq	-16(%rbp), %rax
	movapd	%xmm0, %xmm1
	movq	%rax, %xmm0
	leaq	.LC6(%rip), %rax
	movq	%rax, %rdi
	movl	$2, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L27:
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	check_one_double, .-check_one_double
	.section	.rodata
	.align 8
.LC7:
	.string	"Expected argument %d to have value %ld, actual value was %ld\n"
	.text
	.globl	check_12_longs
	.type	check_12_longs, @function
check_12_longs:
.LFB8:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$160, %rsp
	movq	%rdi, -120(%rbp)
	movq	%rsi, -128(%rbp)
	movq	%rdx, -136(%rbp)
	movq	%rcx, -144(%rbp)
	movq	%r8, -152(%rbp)
	movq	%r9, -160(%rbp)
	movq	-120(%rbp), %rax
	movq	%rax, -112(%rbp)
	movq	-128(%rbp), %rax
	movq	%rax, -104(%rbp)
	movq	-136(%rbp), %rax
	movq	%rax, -96(%rbp)
	movq	-144(%rbp), %rax
	movq	%rax, -88(%rbp)
	movq	-152(%rbp), %rax
	movq	%rax, -80(%rbp)
	movq	-160(%rbp), %rax
	movq	%rax, -72(%rbp)
	movq	16(%rbp), %rax
	movq	%rax, -64(%rbp)
	movq	24(%rbp), %rax
	movq	%rax, -56(%rbp)
	movq	32(%rbp), %rax
	movq	%rax, -48(%rbp)
	movq	40(%rbp), %rax
	movq	%rax, -40(%rbp)
	movq	48(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	56(%rbp), %rax
	movq	%rax, -24(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L32
.L34:
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	64(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, -16(%rbp)
	movl	-4(%rbp), %eax
	cltq
	movq	-112(%rbp,%rax,8), %rax
	cmpq	%rax, -16(%rbp)
	je	.L33
	movl	-4(%rbp), %eax
	cltq
	movq	-112(%rbp,%rax,8), %rdx
	movl	-4(%rbp), %eax
	movslq	%eax, %rcx
	movq	64(%rbp), %rax
	leaq	(%rcx,%rax), %rsi
	movl	-4(%rbp), %eax
	movq	%rdx, %rcx
	movq	%rsi, %rdx
	movl	%eax, %esi
	leaq	.LC7(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L33:
	addl	$1, -4(%rbp)
.L32:
	cmpl	$11, -4(%rbp)
	jle	.L34
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	check_12_longs, .-check_12_longs
	.globl	check_six_chars
	.type	check_six_chars, @function
check_six_chars:
.LFB9:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movl	%ecx, %eax
	movl	%r8d, %r10d
	movl	%r9d, %r8d
	movl	%edi, %ecx
	movb	%cl, -20(%rbp)
	movl	%esi, %ecx
	movb	%cl, -24(%rbp)
	movb	%dl, -28(%rbp)
	movb	%al, -32(%rbp)
	movl	%r10d, %eax
	movb	%al, -36(%rbp)
	movl	%r8d, %eax
	movb	%al, -40(%rbp)
	movzbl	-20(%rbp), %eax
	movb	%al, -14(%rbp)
	movzbl	-24(%rbp), %eax
	movb	%al, -13(%rbp)
	movzbl	-28(%rbp), %eax
	movb	%al, -12(%rbp)
	movzbl	-32(%rbp), %eax
	movb	%al, -11(%rbp)
	movzbl	-36(%rbp), %eax
	movb	%al, -10(%rbp)
	movzbl	-40(%rbp), %eax
	movb	%al, -9(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L37
.L39:
	movl	16(%rbp), %edx
	movl	-4(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, -8(%rbp)
	movl	-4(%rbp), %eax
	cltq
	movzbl	-14(%rbp,%rax), %eax
	movsbl	%al, %eax
	cmpl	%eax, -8(%rbp)
	je	.L38
	movl	-4(%rbp), %eax
	cltq
	movzbl	-14(%rbp,%rax), %eax
	movsbl	%al, %edx
	movl	16(%rbp), %ecx
	movl	-4(%rbp), %eax
	leal	(%rcx,%rax), %esi
	movl	-4(%rbp), %eax
	movl	%edx, %ecx
	movl	%esi, %edx
	movl	%eax, %esi
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L38:
	addl	$1, -4(%rbp)
.L37:
	cmpl	$5, -4(%rbp)
	jle	.L39
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE9:
	.size	check_six_chars, .-check_six_chars
	.section	.rodata
	.align 8
.LC8:
	.string	"Expected argument %d to have value %f, actual value was %f\n"
	.text
	.globl	check_14_doubles
	.type	check_14_doubles, @function
check_14_doubles:
.LFB10:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$192, %rsp
	movsd	%xmm0, -136(%rbp)
	movsd	%xmm1, -144(%rbp)
	movsd	%xmm2, -152(%rbp)
	movsd	%xmm3, -160(%rbp)
	movsd	%xmm4, -168(%rbp)
	movsd	%xmm5, -176(%rbp)
	movsd	%xmm6, -184(%rbp)
	movsd	%xmm7, -192(%rbp)
	movsd	-136(%rbp), %xmm0
	movsd	%xmm0, -128(%rbp)
	movsd	-144(%rbp), %xmm0
	movsd	%xmm0, -120(%rbp)
	movsd	-152(%rbp), %xmm0
	movsd	%xmm0, -112(%rbp)
	movsd	-160(%rbp), %xmm0
	movsd	%xmm0, -104(%rbp)
	movsd	-168(%rbp), %xmm0
	movsd	%xmm0, -96(%rbp)
	movsd	-176(%rbp), %xmm0
	movsd	%xmm0, -88(%rbp)
	movsd	-184(%rbp), %xmm0
	movsd	%xmm0, -80(%rbp)
	movsd	-192(%rbp), %xmm0
	movsd	%xmm0, -72(%rbp)
	movsd	16(%rbp), %xmm0
	movsd	%xmm0, -64(%rbp)
	movsd	24(%rbp), %xmm0
	movsd	%xmm0, -56(%rbp)
	movsd	32(%rbp), %xmm0
	movsd	%xmm0, -48(%rbp)
	movsd	40(%rbp), %xmm0
	movsd	%xmm0, -40(%rbp)
	movsd	48(%rbp), %xmm0
	movsd	%xmm0, -32(%rbp)
	movsd	56(%rbp), %xmm0
	movsd	%xmm0, -24(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L42
.L45:
	pxor	%xmm0, %xmm0
	cvtsi2sdl	-4(%rbp), %xmm0
	movsd	64(%rbp), %xmm1
	addsd	%xmm1, %xmm0
	movsd	%xmm0, -16(%rbp)
	movl	-4(%rbp), %eax
	cltq
	movsd	-128(%rbp,%rax,8), %xmm0
	ucomisd	-16(%rbp), %xmm0
	jp	.L47
	ucomisd	-16(%rbp), %xmm0
	je	.L43
.L47:
	movl	-4(%rbp), %eax
	cltq
	movsd	-128(%rbp,%rax,8), %xmm1
	pxor	%xmm0, %xmm0
	cvtsi2sdl	-4(%rbp), %xmm0
	addsd	64(%rbp), %xmm0
	movq	%xmm0, %rdx
	movl	-4(%rbp), %eax
	movq	%rdx, %xmm0
	movl	%eax, %esi
	leaq	.LC8(%rip), %rax
	movq	%rax, %rdi
	movl	$2, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L43:
	addl	$1, -4(%rbp)
.L42:
	cmpl	$13, -4(%rbp)
	jle	.L45
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE10:
	.size	check_14_doubles, .-check_14_doubles
	.section	.rodata
	.align 8
.LC9:
	.string	"Expected *k to point to have value %d, actual value was %ld\n"
	.align 8
.LC10:
	.string	"Expected *l to point to have value %d, actual value was %f\n"
	.text
	.globl	check_12_vals
	.type	check_12_vals, @function
check_12_vals:
.LFB11:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$80, %rsp
	movl	%edi, -52(%rbp)
	movl	%esi, -56(%rbp)
	movl	%edx, -60(%rbp)
	movl	%ecx, -64(%rbp)
	movl	%r8d, -68(%rbp)
	movl	%r9d, -72(%rbp)
	movl	-52(%rbp), %eax
	movl	%eax, -48(%rbp)
	movl	-56(%rbp), %eax
	movl	%eax, -44(%rbp)
	movl	-60(%rbp), %eax
	movl	%eax, -40(%rbp)
	movl	-64(%rbp), %eax
	movl	%eax, -36(%rbp)
	movl	-68(%rbp), %eax
	movl	%eax, -32(%rbp)
	movl	-72(%rbp), %eax
	movl	%eax, -28(%rbp)
	movl	16(%rbp), %eax
	movl	%eax, -24(%rbp)
	movl	24(%rbp), %eax
	movl	%eax, -20(%rbp)
	movl	32(%rbp), %eax
	movl	%eax, -16(%rbp)
	movl	40(%rbp), %eax
	movl	%eax, -12(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L49
.L51:
	movl	64(%rbp), %edx
	movl	-4(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, -8(%rbp)
	movl	-4(%rbp), %eax
	cltq
	movl	-48(%rbp,%rax,4), %eax
	cmpl	%eax, -8(%rbp)
	je	.L50
	movl	-4(%rbp), %eax
	cltq
	movl	-48(%rbp,%rax,4), %ecx
	movl	-8(%rbp), %edx
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L50:
	addl	$1, -4(%rbp)
.L49:
	cmpl	$9, -4(%rbp)
	jle	.L51
	movq	48(%rbp), %rax
	movq	(%rax), %rdx
	movl	64(%rbp), %eax
	addl	$10, %eax
	cltq
	cmpq	%rax, %rdx
	je	.L52
	movq	48(%rbp), %rax
	movq	(%rax), %rax
	movl	64(%rbp), %edx
	leal	10(%rdx), %ecx
	movq	%rax, %rdx
	movl	%ecx, %esi
	leaq	.LC9(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L52:
	movq	56(%rbp), %rax
	movsd	(%rax), %xmm0
	movl	64(%rbp), %eax
	addl	$11, %eax
	pxor	%xmm1, %xmm1
	cvtsi2sdl	%eax, %xmm1
	ucomisd	%xmm1, %xmm0
	jp	.L56
	ucomisd	%xmm1, %xmm0
	je	.L53
.L56:
	movq	56(%rbp), %rax
	movq	(%rax), %rax
	movl	64(%rbp), %edx
	addl	$11, %edx
	movq	%rax, %xmm0
	movl	%edx, %esi
	leaq	.LC10(%rip), %rax
	movq	%rax, %rdi
	movl	$1, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L53:
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE11:
	.size	check_12_vals, .-check_12_vals
	.globl	id
	.type	id, @function
id:
.LFB12:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE12:
	.size	id, .-id
	.globl	dbl_id
	.type	dbl_id, @function
dbl_id:
.LFB13:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movsd	%xmm0, -8(%rbp)
	movsd	-8(%rbp), %xmm0
	movq	%xmm0, %rax
	movq	%rax, %xmm0
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE13:
	.size	dbl_id, .-dbl_id
	.globl	long_id
	.type	long_id, @function
long_id:
.LFB14:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE14:
	.size	long_id, .-long_id
	.globl	unsigned_id
	.type	unsigned_id, @function
unsigned_id:
.LFB15:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE15:
	.size	unsigned_id, .-unsigned_id
	.globl	uchar_id
	.type	uchar_id, @function
uchar_id:
.LFB16:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, %eax
	movb	%al, -4(%rbp)
	movzbl	-4(%rbp), %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE16:
	.size	uchar_id, .-uchar_id
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
	# call test functions, all of which exit early on failure
	callq	test1
	callq	test2
	callq	test3
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
    .text
    .globl check_alignment
check_alignment:
    pushq   %rbp
    movq    %rsp, %rbp
    # calculate rsp % 16
    movq    %rsp, %rax
    movq    $0, %rdx
    movq    $16, %rcx
    div     %rcx
    # compare result (in rdx) to 0
    cmpq    $0, %rdx
    je      .L_OK
    # it's not zero; exit
    # using exit code already in EDI
    call    exit@PLT
.L_OK:
    # success; rsp is aligned correctly
    movl    $0, %eax
    popq    %rbp
    retq

    .section        ".note.GNU-stack","",@progbits

