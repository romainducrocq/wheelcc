	.section	__TEXT,__text,regular,pure_instructions
	.globl	_check_one_int                  ## -- Begin function check_one_int
	.p2align	4, 0x90
_check_one_int:                         ## @check_one_int
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	cmpl	-8(%rbp), %eax
	je	LBB0_2
## %bb.1:
	movl	-8(%rbp), %esi
	movl	-4(%rbp), %edx
	leaq	L_.str(%rip), %rdi
	xorl	%eax, %eax
                                        ## kill: def $al killed $al killed $eax
	callq	_printf
	movl	$-1, %edi
	callq	_exit
LBB0_2:
	xorl	%eax, %eax
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_check_5_ints                   ## -- Begin function check_5_ints
	.p2align	4, 0x90
_check_5_ints:                          ## @check_5_ints
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$64, %rsp
	movq	___stack_chk_guard@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movq	%rax, -8(%rbp)
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
	movl	$0, -60(%rbp)
LBB1_1:                                 ## =>This Inner Loop Header: Depth=1
	cmpl	$5, -60(%rbp)
	jge	LBB1_7
## %bb.2:                               ##   in Loop: Header=BB1_1 Depth=1
	movl	-56(%rbp), %eax
	addl	-60(%rbp), %eax
	movl	%eax, -64(%rbp)
	movslq	-60(%rbp), %rax
	movl	-32(%rbp,%rax,4), %eax
	cmpl	-64(%rbp), %eax
	je	LBB1_5
## %bb.3:
	movl	-60(%rbp), %esi
	movl	-56(%rbp), %edx
	addl	-60(%rbp), %edx
	movslq	-60(%rbp), %rax
	movl	-32(%rbp,%rax,4), %ecx
	leaq	L_.str.1(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movq	___stack_chk_guard@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movq	-8(%rbp), %rcx
	cmpq	%rcx, %rax
	jne	LBB1_9
## %bb.4:
	movl	$-1, %edi
	callq	_exit
LBB1_5:                                 ##   in Loop: Header=BB1_1 Depth=1
	jmp	LBB1_6
LBB1_6:                                 ##   in Loop: Header=BB1_1 Depth=1
	movl	-60(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -60(%rbp)
	jmp	LBB1_1
LBB1_7:
	movq	___stack_chk_guard@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movq	-8(%rbp), %rcx
	cmpq	%rcx, %rax
	jne	LBB1_9
## %bb.8:
	xorl	%eax, %eax
	addq	$64, %rsp
	popq	%rbp
	retq
LBB1_9:
	callq	___stack_chk_fail
	.cfi_endproc
                                        ## -- End function
	.globl	_check_12_ints                  ## -- Begin function check_12_ints
	.p2align	4, 0x90
_check_12_ints:                         ## @check_12_ints
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
	movl	$0, -92(%rbp)
LBB2_1:                                 ## =>This Inner Loop Header: Depth=1
	cmpl	$12, -92(%rbp)
	jge	LBB2_7
## %bb.2:                               ##   in Loop: Header=BB2_1 Depth=1
	movl	64(%rbp), %eax
	addl	-92(%rbp), %eax
	movl	%eax, -96(%rbp)
	movslq	-92(%rbp), %rax
	movl	-64(%rbp,%rax,4), %eax
	cmpl	-96(%rbp), %eax
	je	LBB2_5
## %bb.3:
	movl	-92(%rbp), %esi
	movl	64(%rbp), %edx
	addl	-92(%rbp), %edx
	movslq	-92(%rbp), %rax
	movl	-64(%rbp,%rax,4), %ecx
	leaq	L_.str.1(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movq	___stack_chk_guard@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movq	-8(%rbp), %rcx
	cmpq	%rcx, %rax
	jne	LBB2_9
## %bb.4:
	movl	$-1, %edi
	callq	_exit
LBB2_5:                                 ##   in Loop: Header=BB2_1 Depth=1
	jmp	LBB2_6
LBB2_6:                                 ##   in Loop: Header=BB2_1 Depth=1
	movl	-92(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -92(%rbp)
	jmp	LBB2_1
LBB2_7:
	movq	___stack_chk_guard@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movq	-8(%rbp), %rcx
	cmpq	%rcx, %rax
	jne	LBB2_9
## %bb.8:
	xorl	%eax, %eax
	addq	$96, %rsp
	popq	%rbp
	retq
LBB2_9:
	callq	___stack_chk_fail
	.cfi_endproc
                                        ## -- End function
	.globl	_check_one_uchar                ## -- Begin function check_one_uchar
	.p2align	4, 0x90
_check_one_uchar:                       ## @check_one_uchar
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movb	%sil, %al
	movb	%dil, %cl
	movb	%cl, -1(%rbp)
	movb	%al, -2(%rbp)
	movzbl	-1(%rbp), %eax
	movzbl	-2(%rbp), %ecx
	cmpl	%ecx, %eax
	je	LBB3_2
## %bb.1:
	movzbl	-2(%rbp), %esi
	movzbl	-1(%rbp), %edx
	leaq	L_.str.2(%rip), %rdi
	xorl	%eax, %eax
                                        ## kill: def $al killed $al killed $eax
	callq	_printf
	movl	$-1, %edi
	callq	_exit
LBB3_2:
	xorl	%eax, %eax
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_check_one_uint                 ## -- Begin function check_one_uint
	.p2align	4, 0x90
_check_one_uint:                        ## @check_one_uint
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	cmpl	-8(%rbp), %eax
	je	LBB4_2
## %bb.1:
	movl	-8(%rbp), %esi
	movl	-4(%rbp), %edx
	leaq	L_.str.3(%rip), %rdi
	xorl	%eax, %eax
                                        ## kill: def $al killed $al killed $eax
	callq	_printf
	movl	$-1, %edi
	callq	_exit
LBB4_2:
	xorl	%eax, %eax
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_check_one_long                 ## -- Begin function check_one_long
	.p2align	4, 0x90
_check_one_long:                        ## @check_one_long
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	cmpq	-16(%rbp), %rax
	je	LBB5_2
## %bb.1:
	movq	-16(%rbp), %rsi
	movq	-8(%rbp), %rdx
	leaq	L_.str.4(%rip), %rdi
	xorl	%eax, %eax
                                        ## kill: def $al killed $al killed $eax
	callq	_printf
	movl	$-1, %edi
	callq	_exit
LBB5_2:
	xorl	%eax, %eax
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_check_one_ulong                ## -- Begin function check_one_ulong
	.p2align	4, 0x90
_check_one_ulong:                       ## @check_one_ulong
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	cmpq	-16(%rbp), %rax
	je	LBB6_2
## %bb.1:
	movq	-16(%rbp), %rsi
	movq	-8(%rbp), %rdx
	leaq	L_.str.5(%rip), %rdi
	xorl	%eax, %eax
                                        ## kill: def $al killed $al killed $eax
	callq	_printf
	movl	$-1, %edi
	callq	_exit
LBB6_2:
	xorl	%eax, %eax
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_check_one_double               ## -- Begin function check_one_double
	.p2align	4, 0x90
_check_one_double:                      ## @check_one_double
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movsd	%xmm0, -8(%rbp)
	movsd	%xmm1, -16(%rbp)
	movsd	-8(%rbp), %xmm0                 ## xmm0 = mem[0],zero
	ucomisd	-16(%rbp), %xmm0
	jne	LBB7_1
	jp	LBB7_1
	jmp	LBB7_2
LBB7_1:
	movsd	-16(%rbp), %xmm0                ## xmm0 = mem[0],zero
	movsd	-8(%rbp), %xmm1                 ## xmm1 = mem[0],zero
	leaq	L_.str.6(%rip), %rdi
	movb	$2, %al
	callq	_printf
	movl	$-1, %edi
	callq	_exit
LBB7_2:
	xorl	%eax, %eax
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_check_12_longs                 ## -- Begin function check_12_longs
	.p2align	4, 0x90
_check_12_longs:                        ## @check_12_longs
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$176, %rsp
	movq	64(%rbp), %rax
	movq	56(%rbp), %rax
	movq	48(%rbp), %rax
	movq	40(%rbp), %rax
	movq	32(%rbp), %rax
	movq	24(%rbp), %rax
	movq	16(%rbp), %rax
	movq	___stack_chk_guard@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movq	%rax, -8(%rbp)
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
	movl	$0, -164(%rbp)
LBB8_1:                                 ## =>This Inner Loop Header: Depth=1
	cmpl	$12, -164(%rbp)
	jge	LBB8_7
## %bb.2:                               ##   in Loop: Header=BB8_1 Depth=1
	movq	64(%rbp), %rax
	movslq	-164(%rbp), %rcx
	addq	%rcx, %rax
	movq	%rax, -176(%rbp)
	movslq	-164(%rbp), %rax
	movq	-112(%rbp,%rax,8), %rax
	cmpq	-176(%rbp), %rax
	je	LBB8_5
## %bb.3:
	movl	-164(%rbp), %esi
	movq	64(%rbp), %rdx
	movslq	-164(%rbp), %rax
	addq	%rax, %rdx
	movslq	-164(%rbp), %rax
	movq	-112(%rbp,%rax,8), %rcx
	leaq	L_.str.7(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movq	___stack_chk_guard@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movq	-8(%rbp), %rcx
	cmpq	%rcx, %rax
	jne	LBB8_9
## %bb.4:
	movl	$-1, %edi
	callq	_exit
LBB8_5:                                 ##   in Loop: Header=BB8_1 Depth=1
	jmp	LBB8_6
LBB8_6:                                 ##   in Loop: Header=BB8_1 Depth=1
	movl	-164(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -164(%rbp)
	jmp	LBB8_1
LBB8_7:
	movq	___stack_chk_guard@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movq	-8(%rbp), %rcx
	cmpq	%rcx, %rax
	jne	LBB8_9
## %bb.8:
	xorl	%eax, %eax
	addq	$176, %rsp
	popq	%rbp
	retq
LBB8_9:
	callq	___stack_chk_fail
	.cfi_endproc
                                        ## -- End function
	.globl	_check_six_chars                ## -- Begin function check_six_chars
	.p2align	4, 0x90
_check_six_chars:                       ## @check_six_chars
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	movl	%r8d, -36(%rbp)                 ## 4-byte Spill
	movl	%ecx, %eax
	movl	-36(%rbp), %ecx                 ## 4-byte Reload
	movl	%eax, -32(%rbp)                 ## 4-byte Spill
	movl	%edx, %eax
	movl	-32(%rbp), %edx                 ## 4-byte Reload
	movl	%eax, -28(%rbp)                 ## 4-byte Spill
	movl	%esi, %eax
	movl	-28(%rbp), %esi                 ## 4-byte Reload
	movl	%eax, -24(%rbp)                 ## 4-byte Spill
	movl	%edi, %r8d
	movl	-24(%rbp), %edi                 ## 4-byte Reload
	movb	%r9b, %al
                                        ## kill: def $cl killed $cl killed $ecx
                                        ## kill: def $dl killed $dl killed $edx
                                        ## kill: def $sil killed $sil killed $esi
                                        ## kill: def $dil killed $dil killed $edi
                                        ## kill: def $r8b killed $r8b killed $r8d
	movl	16(%rbp), %r9d
	movb	%r8b, -1(%rbp)
	movb	%dil, -2(%rbp)
	movb	%sil, -3(%rbp)
	movb	%dl, -4(%rbp)
	movb	%cl, -5(%rbp)
	movb	%al, -6(%rbp)
	movb	-1(%rbp), %al
	movb	%al, -12(%rbp)
	movb	-2(%rbp), %al
	movb	%al, -11(%rbp)
	movb	-3(%rbp), %al
	movb	%al, -10(%rbp)
	movb	-4(%rbp), %al
	movb	%al, -9(%rbp)
	movb	-5(%rbp), %al
	movb	%al, -8(%rbp)
	movb	-6(%rbp), %al
	movb	%al, -7(%rbp)
	movl	$0, -16(%rbp)
LBB9_1:                                 ## =>This Inner Loop Header: Depth=1
	cmpl	$6, -16(%rbp)
	jge	LBB9_6
## %bb.2:                               ##   in Loop: Header=BB9_1 Depth=1
	movl	16(%rbp), %eax
	addl	-16(%rbp), %eax
	movl	%eax, -20(%rbp)
	movslq	-16(%rbp), %rax
	movsbl	-12(%rbp,%rax), %eax
	cmpl	-20(%rbp), %eax
	je	LBB9_4
## %bb.3:
	movslq	-16(%rbp), %rax
	movl	%eax, %esi
	movl	16(%rbp), %edx
	addl	%esi, %edx
	movsbl	-12(%rbp,%rax), %ecx
	leaq	L_.str.1(%rip), %rdi
	xorl	%eax, %eax
                                        ## kill: def $al killed $al killed $eax
	callq	_printf
	movl	$-1, %edi
	callq	_exit
LBB9_4:                                 ##   in Loop: Header=BB9_1 Depth=1
	jmp	LBB9_5
LBB9_5:                                 ##   in Loop: Header=BB9_1 Depth=1
	movl	-16(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -16(%rbp)
	jmp	LBB9_1
LBB9_6:
	xorl	%eax, %eax
	addq	$48, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_check_14_doubles               ## -- Begin function check_14_doubles
	.p2align	4, 0x90
_check_14_doubles:                      ## @check_14_doubles
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$208, %rsp
	movsd	64(%rbp), %xmm8                 ## xmm8 = mem[0],zero
	movsd	56(%rbp), %xmm8                 ## xmm8 = mem[0],zero
	movsd	48(%rbp), %xmm8                 ## xmm8 = mem[0],zero
	movsd	40(%rbp), %xmm8                 ## xmm8 = mem[0],zero
	movsd	32(%rbp), %xmm8                 ## xmm8 = mem[0],zero
	movsd	24(%rbp), %xmm8                 ## xmm8 = mem[0],zero
	movsd	16(%rbp), %xmm8                 ## xmm8 = mem[0],zero
	movq	___stack_chk_guard@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movq	%rax, -8(%rbp)
	movsd	%xmm0, -136(%rbp)
	movsd	%xmm1, -144(%rbp)
	movsd	%xmm2, -152(%rbp)
	movsd	%xmm3, -160(%rbp)
	movsd	%xmm4, -168(%rbp)
	movsd	%xmm5, -176(%rbp)
	movsd	%xmm6, -184(%rbp)
	movsd	%xmm7, -192(%rbp)
	movsd	-136(%rbp), %xmm0               ## xmm0 = mem[0],zero
	movsd	%xmm0, -128(%rbp)
	movsd	-144(%rbp), %xmm0               ## xmm0 = mem[0],zero
	movsd	%xmm0, -120(%rbp)
	movsd	-152(%rbp), %xmm0               ## xmm0 = mem[0],zero
	movsd	%xmm0, -112(%rbp)
	movsd	-160(%rbp), %xmm0               ## xmm0 = mem[0],zero
	movsd	%xmm0, -104(%rbp)
	movsd	-168(%rbp), %xmm0               ## xmm0 = mem[0],zero
	movsd	%xmm0, -96(%rbp)
	movsd	-176(%rbp), %xmm0               ## xmm0 = mem[0],zero
	movsd	%xmm0, -88(%rbp)
	movsd	-184(%rbp), %xmm0               ## xmm0 = mem[0],zero
	movsd	%xmm0, -80(%rbp)
	movsd	-192(%rbp), %xmm0               ## xmm0 = mem[0],zero
	movsd	%xmm0, -72(%rbp)
	movsd	16(%rbp), %xmm0                 ## xmm0 = mem[0],zero
	movsd	%xmm0, -64(%rbp)
	movsd	24(%rbp), %xmm0                 ## xmm0 = mem[0],zero
	movsd	%xmm0, -56(%rbp)
	movsd	32(%rbp), %xmm0                 ## xmm0 = mem[0],zero
	movsd	%xmm0, -48(%rbp)
	movsd	40(%rbp), %xmm0                 ## xmm0 = mem[0],zero
	movsd	%xmm0, -40(%rbp)
	movsd	48(%rbp), %xmm0                 ## xmm0 = mem[0],zero
	movsd	%xmm0, -32(%rbp)
	movsd	56(%rbp), %xmm0                 ## xmm0 = mem[0],zero
	movsd	%xmm0, -24(%rbp)
	movl	$0, -196(%rbp)
LBB10_1:                                ## =>This Inner Loop Header: Depth=1
	cmpl	$14, -196(%rbp)
	jge	LBB10_7
## %bb.2:                               ##   in Loop: Header=BB10_1 Depth=1
	movsd	64(%rbp), %xmm0                 ## xmm0 = mem[0],zero
	cvtsi2sdl	-196(%rbp), %xmm1
	addsd	%xmm1, %xmm0
	movsd	%xmm0, -208(%rbp)
	movslq	-196(%rbp), %rax
	movsd	-128(%rbp,%rax,8), %xmm0        ## xmm0 = mem[0],zero
	ucomisd	-208(%rbp), %xmm0
	jne	LBB10_3
	jp	LBB10_3
	jmp	LBB10_5
LBB10_3:
	movl	-196(%rbp), %esi
	movsd	64(%rbp), %xmm0                 ## xmm0 = mem[0],zero
	cvtsi2sdl	-196(%rbp), %xmm1
	addsd	%xmm1, %xmm0
	movslq	-196(%rbp), %rax
	movsd	-128(%rbp,%rax,8), %xmm1        ## xmm1 = mem[0],zero
	leaq	L_.str.8(%rip), %rdi
	movb	$2, %al
	callq	_printf
	movq	___stack_chk_guard@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movq	-8(%rbp), %rcx
	cmpq	%rcx, %rax
	jne	LBB10_9
## %bb.4:
	movl	$-1, %edi
	callq	_exit
LBB10_5:                                ##   in Loop: Header=BB10_1 Depth=1
	jmp	LBB10_6
LBB10_6:                                ##   in Loop: Header=BB10_1 Depth=1
	movl	-196(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -196(%rbp)
	jmp	LBB10_1
LBB10_7:
	movq	___stack_chk_guard@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movq	-8(%rbp), %rcx
	cmpq	%rcx, %rax
	jne	LBB10_9
## %bb.8:
	xorl	%eax, %eax
	addq	$208, %rsp
	popq	%rbp
	retq
LBB10_9:
	callq	___stack_chk_fail
	.cfi_endproc
                                        ## -- End function
	.globl	_check_12_vals                  ## -- Begin function check_12_vals
	.p2align	4, 0x90
_check_12_vals:                         ## @check_12_vals
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$80, %rsp
	movl	64(%rbp), %eax
	movq	56(%rbp), %rax
	movq	48(%rbp), %rax
	movl	40(%rbp), %eax
	movl	32(%rbp), %eax
	movl	24(%rbp), %eax
	movl	16(%rbp), %eax
	movq	___stack_chk_guard@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movq	%rax, -8(%rbp)
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
	movl	$0, -76(%rbp)
LBB11_1:                                ## =>This Inner Loop Header: Depth=1
	cmpl	$10, -76(%rbp)
	jge	LBB11_7
## %bb.2:                               ##   in Loop: Header=BB11_1 Depth=1
	movl	64(%rbp), %eax
	addl	-76(%rbp), %eax
	movl	%eax, -80(%rbp)
	movslq	-76(%rbp), %rax
	movl	-48(%rbp,%rax,4), %eax
	cmpl	-80(%rbp), %eax
	je	LBB11_5
## %bb.3:
	movl	-76(%rbp), %esi
	movl	-80(%rbp), %edx
	movslq	-76(%rbp), %rax
	movl	-48(%rbp,%rax,4), %ecx
	leaq	L_.str.1(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movq	___stack_chk_guard@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movq	-8(%rbp), %rcx
	cmpq	%rcx, %rax
	jne	LBB11_15
## %bb.4:
	movl	$-1, %edi
	callq	_exit
LBB11_5:                                ##   in Loop: Header=BB11_1 Depth=1
	jmp	LBB11_6
LBB11_6:                                ##   in Loop: Header=BB11_1 Depth=1
	movl	-76(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -76(%rbp)
	jmp	LBB11_1
LBB11_7:
	movq	48(%rbp), %rax
	movq	(%rax), %rax
	movl	64(%rbp), %ecx
	addl	$10, %ecx
	movslq	%ecx, %rcx
	cmpq	%rcx, %rax
	je	LBB11_10
## %bb.8:
	movl	64(%rbp), %esi
	addl	$10, %esi
	movq	48(%rbp), %rax
	movq	(%rax), %rdx
	leaq	L_.str.9(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movq	___stack_chk_guard@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movq	-8(%rbp), %rcx
	cmpq	%rcx, %rax
	jne	LBB11_15
## %bb.9:
	movl	$-1, %edi
	callq	_exit
LBB11_10:
	movq	56(%rbp), %rax
	movsd	(%rax), %xmm0                   ## xmm0 = mem[0],zero
	movl	64(%rbp), %eax
	addl	$11, %eax
	cvtsi2sd	%eax, %xmm1
	ucomisd	%xmm1, %xmm0
	jne	LBB11_11
	jp	LBB11_11
	jmp	LBB11_13
LBB11_11:
	movl	64(%rbp), %esi
	addl	$11, %esi
	movq	56(%rbp), %rax
	movsd	(%rax), %xmm0                   ## xmm0 = mem[0],zero
	leaq	L_.str.10(%rip), %rdi
	movb	$1, %al
	callq	_printf
	movq	___stack_chk_guard@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movq	-8(%rbp), %rcx
	cmpq	%rcx, %rax
	jne	LBB11_15
## %bb.12:
	movl	$-1, %edi
	callq	_exit
LBB11_13:
	movq	___stack_chk_guard@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movq	-8(%rbp), %rcx
	cmpq	%rcx, %rax
	jne	LBB11_15
## %bb.14:
	xorl	%eax, %eax
	addq	$80, %rsp
	popq	%rbp
	retq
LBB11_15:
	callq	___stack_chk_fail
	.cfi_endproc
                                        ## -- End function
	.globl	_id                             ## -- Begin function id
	.p2align	4, 0x90
_id:                                    ## @id
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_dbl_id                         ## -- Begin function dbl_id
	.p2align	4, 0x90
_dbl_id:                                ## @dbl_id
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movsd	%xmm0, -8(%rbp)
	movsd	-8(%rbp), %xmm0                 ## xmm0 = mem[0],zero
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_long_id                        ## -- Begin function long_id
	.p2align	4, 0x90
_long_id:                               ## @long_id
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_unsigned_id                    ## -- Begin function unsigned_id
	.p2align	4, 0x90
_unsigned_id:                           ## @unsigned_id
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_uchar_id                       ## -- Begin function uchar_id
	.p2align	4, 0x90
_uchar_id:                              ## @uchar_id
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movb	%dil, %al
	movb	%al, -1(%rbp)
	movzbl	-1(%rbp), %eax
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"Expected %d but found %d\n"

L_.str.1:                               ## @.str.1
	.asciz	"Expected argument %d to have value %d, actual value was %d\n"

L_.str.2:                               ## @.str.2
	.asciz	"Expected %c but found %c\n"

L_.str.3:                               ## @.str.3
	.asciz	"Expected %u but found %u\n"

L_.str.4:                               ## @.str.4
	.asciz	"Expected %ld but found %ld\n"

L_.str.5:                               ## @.str.5
	.asciz	"Expected %lu but found %lu\n"

L_.str.6:                               ## @.str.6
	.asciz	"Expected %f but found %f\n"

L_.str.7:                               ## @.str.7
	.asciz	"Expected argument %d to have value %ld, actual value was %ld\n"

L_.str.8:                               ## @.str.8
	.asciz	"Expected argument %d to have value %f, actual value was %f\n"

L_.str.9:                               ## @.str.9
	.asciz	"Expected *k to point to have value %d, actual value was %ld\n"

L_.str.10:                              ## @.str.10
	.asciz	"Expected *l to point to have value %d, actual value was %f\n"

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
	# call test functions, all of which exit early on failure
	callq	_test1
	callq	_test2
	callq	_test3
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
    .text
    .globl _check_alignment
_check_alignment:
    pushq   %rbp
    movq    %rsp, %rbp
    # calculate rsp % 16
    movq    %rsp, %rax
    movq    $0, %rdx
    movq    $16, %rcx
    div     %rcx
    # compare result (in rdx) to 0
    cmpq    $0, %rdx
    je      L_OK
    # it's not zero; exit
    # using exit code already in EDI
    call    _exit
L_OK:
    # success; rsp is aligned correctly
    movl    $0, %eax
    popq    %rbp
    retq


