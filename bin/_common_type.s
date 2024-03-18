	.globl main

    .text
main:
    pushq %rbp
    movq %rsp, %rbp
	subq $32, %rsp
	movl $10, -4(%rbp)
	negl -4(%rbp)
	movb -4(%rbp), %r10b
	movb %r10b, -5(%rbp)
	movb -5(%rbp), %r10b
	movb %r10b, -6(%rbp)
	movl $1, %r11d
	cmpl $0, %r11d
	je .Lconditional_else.4
	movsbl -6(%rbp), %r11d
	movl %r11d, -12(%rbp)
	movl -12(%rbp), %r10d
	movl %r10d, -16(%rbp)
	jmp .Lconditional_end.5
.Lconditional_else.4:
	movl $1, -16(%rbp)
.Lconditional_end.5:
	movl -16(%rbp), %r11d
	movq %r11, -24(%rbp)
	movq $4294967286, %r10
	cmpq %r10, -24(%rbp)
	movl $0, -28(%rbp)
	setne -28(%rbp)
	movl -28(%rbp), %eax

    movq %rbp, %rsp
    popq %rbp
    ret
	movl $0, %eax

    movq %rbp, %rsp
    popq %rbp
    ret
	.section .note.GNU-stack,"",@progbits
