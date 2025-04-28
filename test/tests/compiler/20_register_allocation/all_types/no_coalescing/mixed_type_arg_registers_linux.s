	.text
	.section	.rodata
	.align 8
.LC1:
	.string	"Expected a.d to have value 11.0, actual value was %f\n"
	.align 8
.LC2:
	.string	"Expected a.c to have value 8, actual value was %d\n"
	.align 8
.LC3:
	.string	"Expected a.i to have value 9, actual value was %d\n"
	.align 8
.LC4:
	.string	"Expected b.ul to have value 10, actual value was %lu\n"
	.align 8
.LC6:
	.string	"Expected b.d to have value 12.0, actual value was %f\n"
	.align 8
.LC7:
	.string	"Expected c to have value 12, actual value was %d\n"
	.align 8
.LC9:
	.string	"Expected in_mem.d1 to have value 13.0, actual value was %f\n"
	.align 8
.LC11:
	.string	"Expected in_mem.d2 to have value 14.0, actual value was %f\n"
	.align 8
.LC12:
	.string	"Expected in_mem.s to have value 11, actual value was %d\n"
	.text
	.globl	callee
	.type	callee, @function
callee:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$56, %rsp
	.cfi_offset 3, -24
	movq	%xmm0, %r8
	movq	%rdi, %rax
	movl	$0, %ecx
	movl	$0, %ebx
	movq	%r8, %rcx
	movq	%rax, %rbx
	movq	%rcx, -32(%rbp)
	movq	%rbx, -24(%rbp)
	movapd	%xmm1, %xmm0
	movq	%rsi, %rcx
	movq	%rdi, %rbx
	movq	%xmm0, %rbx
	movq	%rcx, -48(%rbp)
	movq	%rbx, -40(%rbp)
	movl	%edx, %eax
	movb	%al, -52(%rbp)
	movsd	-32(%rbp), %xmm0
	ucomisd	.LC0(%rip), %xmm0
	jp	.L16
	ucomisd	.LC0(%rip), %xmm0
	je	.L2
.L16:
	movq	-32(%rbp), %rax
	movq	%rax, %xmm0
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	movl	$1, %eax
	call	printf@PLT
.L2:
	movzbl	-24(%rbp), %eax
	cmpb	$8, %al
	je	.L4
	movzbl	-24(%rbp), %eax
	movsbl	%al, %eax
	movl	%eax, %esi
	leaq	.LC2(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
.L4:
	movl	-20(%rbp), %eax
	cmpl	$9, %eax
	je	.L5
	movl	-20(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC3(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
.L5:
	movq	-48(%rbp), %rax
	cmpq	$10, %rax
	je	.L6
	movq	-48(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC4(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
.L6:
	movsd	-40(%rbp), %xmm0
	ucomisd	.LC5(%rip), %xmm0
	jp	.L17
	ucomisd	.LC5(%rip), %xmm0
	je	.L7
.L17:
	movq	-40(%rbp), %rax
	movq	%rax, %xmm0
	leaq	.LC6(%rip), %rax
	movq	%rax, %rdi
	movl	$1, %eax
	call	printf@PLT
.L7:
	cmpb	$12, -52(%rbp)
	je	.L9
	movsbl	-52(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC7(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
.L9:
	movsd	16(%rbp), %xmm0
	ucomisd	.LC8(%rip), %xmm0
	jp	.L18
	ucomisd	.LC8(%rip), %xmm0
	je	.L10
.L18:
	movq	16(%rbp), %rax
	movq	%rax, %xmm0
	leaq	.LC9(%rip), %rax
	movq	%rax, %rdi
	movl	$1, %eax
	call	printf@PLT
.L10:
	movsd	24(%rbp), %xmm0
	ucomisd	.LC10(%rip), %xmm0
	jp	.L19
	ucomisd	.LC10(%rip), %xmm0
	je	.L12
.L19:
	movq	24(%rbp), %rax
	movq	%rax, %xmm0
	leaq	.LC11(%rip), %rax
	movq	%rax, %rdi
	movl	$1, %eax
	call	printf@PLT
.L12:
	movzbl	32(%rbp), %eax
	cmpb	$11, %al
	je	.L14
	movzbl	32(%rbp), %eax
	movsbl	%al, %eax
	movl	%eax, %esi
	leaq	.LC12(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
.L14:
	movl	$0, %eax
	movq	-8(%rbp), %rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	callee, .-callee
	.section	.rodata
	.align 8
.LC13:
	.string	"Expected one to have value 1, actual value was %d\n"
	.align 8
.LC14:
	.string	"Expected two to have value 2, actual value was %ld\n"
	.align 8
.LC15:
	.string	"Expected three to have value 3, actual value was %u\n"
	.align 8
.LC16:
	.string	"Expected four to have value 4, actual value was %lu\n"
	.align 8
.LC17:
	.string	"Expected five to have value 5, actual value was %d\n"
	.align 8
.LC18:
	.string	"Expected six to have value 6, actual value was %d\n"
	.align 8
.LC19:
	.string	"Expected seven to have value 7, actual value was %d\n"
	.text
	.globl	check_some_args
	.type	check_some_args, @function
check_some_args:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -8(%rbp)
	movq	%rcx, -24(%rbp)
	movl	%r8d, %edx
	movl	%r9d, %ecx
	movl	16(%rbp), %eax
	movb	%dl, -28(%rbp)
	movl	%ecx, %edx
	movb	%dl, -32(%rbp)
	movb	%al, -36(%rbp)
	cmpl	$1, -4(%rbp)
	je	.L21
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC13(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L21:
	cmpq	$2, -16(%rbp)
	je	.L22
	movq	-16(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC14(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L22:
	cmpl	$3, -8(%rbp)
	je	.L23
	movl	-8(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC15(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L23:
	cmpq	$4, -24(%rbp)
	je	.L24
	movq	-24(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC16(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L24:
	cmpb	$5, -28(%rbp)
	je	.L25
	movsbl	-28(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC17(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L25:
	cmpb	$6, -32(%rbp)
	je	.L26
	movzbl	-32(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC18(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L26:
	cmpb	$7, -36(%rbp)
	je	.L27
	movsbl	-36(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC19(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L27:
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	check_some_args, .-check_some_args
	.section	.rodata
.LC20:
	.string	"Expected %d but found %d\n"
	.text
	.globl	check_one_int
	.type	check_one_int, @function
check_one_int:
.LFB2:
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
	je	.L30
	movl	-4(%rbp), %edx
	movl	-8(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC20(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L30:
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	check_one_int, .-check_one_int
	.section	.rodata
	.align 8
.LC21:
	.string	"Expected argument %d to have value %d, actual value was %d\n"
	.text
	.globl	check_5_ints
	.type	check_5_ints, @function
check_5_ints:
.LFB3:
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
	jmp	.L33
.L35:
	movl	-56(%rbp), %edx
	movl	-4(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, -8(%rbp)
	movl	-4(%rbp), %eax
	cltq
	movl	-32(%rbp,%rax,4), %eax
	cmpl	%eax, -8(%rbp)
	je	.L34
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
	leaq	.LC21(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L34:
	addl	$1, -4(%rbp)
.L33:
	cmpl	$4, -4(%rbp)
	jle	.L35
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	check_5_ints, .-check_5_ints
	.globl	check_12_ints
	.type	check_12_ints, @function
check_12_ints:
.LFB4:
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
	jmp	.L38
.L40:
	movl	64(%rbp), %edx
	movl	-4(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, -8(%rbp)
	movl	-4(%rbp), %eax
	cltq
	movl	-64(%rbp,%rax,4), %eax
	cmpl	%eax, -8(%rbp)
	je	.L39
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
	leaq	.LC21(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L39:
	addl	$1, -4(%rbp)
.L38:
	cmpl	$11, -4(%rbp)
	jle	.L40
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	check_12_ints, .-check_12_ints
	.section	.rodata
.LC22:
	.string	"Expected %c but found %c\n"
	.text
	.globl	check_one_uchar
	.type	check_one_uchar, @function
check_one_uchar:
.LFB5:
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
	je	.L43
	movzbl	-4(%rbp), %edx
	movzbl	-8(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC22(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L43:
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	check_one_uchar, .-check_one_uchar
	.section	.rodata
.LC23:
	.string	"Expected %u but found %u\n"
	.text
	.globl	check_one_uint
	.type	check_one_uint, @function
check_one_uint:
.LFB6:
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
	je	.L46
	movl	-4(%rbp), %edx
	movl	-8(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC23(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L46:
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	check_one_uint, .-check_one_uint
	.section	.rodata
.LC24:
	.string	"Expected %ld but found %ld\n"
	.text
	.globl	check_one_long
	.type	check_one_long, @function
check_one_long:
.LFB7:
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
	je	.L49
	movq	-8(%rbp), %rdx
	movq	-16(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC24(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L49:
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	check_one_long, .-check_one_long
	.section	.rodata
.LC25:
	.string	"Expected %lu but found %lu\n"
	.text
	.globl	check_one_ulong
	.type	check_one_ulong, @function
check_one_ulong:
.LFB8:
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
	je	.L52
	movq	-8(%rbp), %rdx
	movq	-16(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC25(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L52:
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	check_one_ulong, .-check_one_ulong
	.section	.rodata
.LC26:
	.string	"Expected %f but found %f\n"
	.text
	.globl	check_one_double
	.type	check_one_double, @function
check_one_double:
.LFB9:
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
	jp	.L58
	movsd	-8(%rbp), %xmm0
	ucomisd	-16(%rbp), %xmm0
	je	.L55
.L58:
	movsd	-8(%rbp), %xmm0
	movq	-16(%rbp), %rax
	movapd	%xmm0, %xmm1
	movq	%rax, %xmm0
	leaq	.LC26(%rip), %rax
	movq	%rax, %rdi
	movl	$2, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L55:
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE9:
	.size	check_one_double, .-check_one_double
	.section	.rodata
	.align 8
.LC27:
	.string	"Expected argument %d to have value %ld, actual value was %ld\n"
	.text
	.globl	check_12_longs
	.type	check_12_longs, @function
check_12_longs:
.LFB10:
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
	jmp	.L60
.L62:
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	64(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, -16(%rbp)
	movl	-4(%rbp), %eax
	cltq
	movq	-112(%rbp,%rax,8), %rax
	cmpq	%rax, -16(%rbp)
	je	.L61
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
	leaq	.LC27(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L61:
	addl	$1, -4(%rbp)
.L60:
	cmpl	$11, -4(%rbp)
	jle	.L62
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE10:
	.size	check_12_longs, .-check_12_longs
	.globl	check_six_chars
	.type	check_six_chars, @function
check_six_chars:
.LFB11:
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
	jmp	.L65
.L67:
	movl	16(%rbp), %edx
	movl	-4(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, -8(%rbp)
	movl	-4(%rbp), %eax
	cltq
	movzbl	-14(%rbp,%rax), %eax
	movsbl	%al, %eax
	cmpl	%eax, -8(%rbp)
	je	.L66
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
	leaq	.LC21(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L66:
	addl	$1, -4(%rbp)
.L65:
	cmpl	$5, -4(%rbp)
	jle	.L67
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE11:
	.size	check_six_chars, .-check_six_chars
	.section	.rodata
	.align 8
.LC28:
	.string	"Expected argument %d to have value %f, actual value was %f\n"
	.text
	.globl	check_14_doubles
	.type	check_14_doubles, @function
check_14_doubles:
.LFB12:
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
	jmp	.L70
.L73:
	pxor	%xmm0, %xmm0
	cvtsi2sdl	-4(%rbp), %xmm0
	movsd	64(%rbp), %xmm1
	addsd	%xmm1, %xmm0
	movsd	%xmm0, -16(%rbp)
	movl	-4(%rbp), %eax
	cltq
	movsd	-128(%rbp,%rax,8), %xmm0
	ucomisd	-16(%rbp), %xmm0
	jp	.L75
	ucomisd	-16(%rbp), %xmm0
	je	.L71
.L75:
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
	leaq	.LC28(%rip), %rax
	movq	%rax, %rdi
	movl	$2, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L71:
	addl	$1, -4(%rbp)
.L70:
	cmpl	$13, -4(%rbp)
	jle	.L73
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE12:
	.size	check_14_doubles, .-check_14_doubles
	.section	.rodata
	.align 8
.LC29:
	.string	"Expected *k to point to have value %d, actual value was %ld\n"
	.align 8
.LC30:
	.string	"Expected *l to point to have value %d, actual value was %f\n"
	.text
	.globl	check_12_vals
	.type	check_12_vals, @function
check_12_vals:
.LFB13:
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
	jmp	.L77
.L79:
	movl	64(%rbp), %edx
	movl	-4(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, -8(%rbp)
	movl	-4(%rbp), %eax
	cltq
	movl	-48(%rbp,%rax,4), %eax
	cmpl	%eax, -8(%rbp)
	je	.L78
	movl	-4(%rbp), %eax
	cltq
	movl	-48(%rbp,%rax,4), %ecx
	movl	-8(%rbp), %edx
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC21(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L78:
	addl	$1, -4(%rbp)
.L77:
	cmpl	$9, -4(%rbp)
	jle	.L79
	movq	48(%rbp), %rax
	movq	(%rax), %rdx
	movl	64(%rbp), %eax
	addl	$10, %eax
	cltq
	cmpq	%rax, %rdx
	je	.L80
	movq	48(%rbp), %rax
	movq	(%rax), %rax
	movl	64(%rbp), %edx
	leal	10(%rdx), %ecx
	movq	%rax, %rdx
	movl	%ecx, %esi
	leaq	.LC29(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L80:
	movq	56(%rbp), %rax
	movsd	(%rax), %xmm0
	movl	64(%rbp), %eax
	addl	$11, %eax
	pxor	%xmm1, %xmm1
	cvtsi2sdl	%eax, %xmm1
	ucomisd	%xmm1, %xmm0
	jp	.L84
	ucomisd	%xmm1, %xmm0
	je	.L81
.L84:
	movq	56(%rbp), %rax
	movq	(%rax), %rax
	movl	64(%rbp), %edx
	addl	$11, %edx
	movq	%rax, %xmm0
	movl	%edx, %esi
	leaq	.LC30(%rip), %rax
	movq	%rax, %rdi
	movl	$1, %eax
	call	printf@PLT
	movl	$-1, %edi
	call	exit@PLT
.L81:
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE13:
	.size	check_12_vals, .-check_12_vals
	.globl	id
	.type	id, @function
id:
.LFB14:
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
.LFE14:
	.size	id, .-id
	.globl	dbl_id
	.type	dbl_id, @function
dbl_id:
.LFB15:
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
.LFE15:
	.size	dbl_id, .-dbl_id
	.globl	long_id
	.type	long_id, @function
long_id:
.LFB16:
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
.LFE16:
	.size	long_id, .-long_id
	.globl	unsigned_id
	.type	unsigned_id, @function
unsigned_id:
.LFB17:
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
.LFE17:
	.size	unsigned_id, .-unsigned_id
	.globl	uchar_id
	.type	uchar_id, @function
uchar_id:
.LFB18:
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
.LFE18:
	.size	uchar_id, .-uchar_id
	.section	.rodata
	.align 8
.LC0:
	.long	0
	.long	1076232192
	.align 8
.LC5:
	.long	0
	.long	1076363264
	.align 8
.LC8:
	.long	0
	.long	1076494336
	.align 8
.LC10:
	.long	0
	.long	1076625408
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
