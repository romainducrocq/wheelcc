    .section .rodata
    .align 8
.Ldouble.1:
    .quad 0
    .section .rodata
    .align 8
.Ldouble.2:
    .quad 4621819117588971520
    .globl l
    .data
    .align 8
l:
    .quad 100
    .bss
    .align 8
u_ptr.10:
    .zero 8
    .bss
    .align 4
u.9:
    .zero 4
    .globl return_3
    .text
return_3:
    pushq %rbp
    movq %rsp, %rbp
    movl $3, %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    movl $0, %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    .globl two_pointers
    .text
two_pointers:
    pushq %rbp
    movq %rsp, %rbp
    subq $24, %rsp
    movsd %xmm0, -8(%rbp)
    movq %rdi, -16(%rbp)
    movq -16(%rbp), %r10
    movq %r10, 0(%rax)
    movsd -8(%rbp), %xmm14
    movsd %xmm14, 0(%rax)
    leaq l(%rip), %r11
    movq %r11, -24(%rbp)
    movq -24(%rbp), %rax
    movq %rbp, %rsp
    popq %rbp
    ret
    movl $0, %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    .globl pointers_to_pointers
    .text
pointers_to_pointers:
    pushq %rbp
    movq %rsp, %rbp
    subq $48, %rsp
    movq %rdi, -8(%rbp)
    leaq u.9(%rip), %r11
    movq %r11, -16(%rbp)
    movq -16(%rbp), %r10
    movq %r10, u_ptr.10(%rip)
    movq -8(%rbp), %rax
    movq 0(%rax), %r10
    movq %r10, -24(%rbp)
    movq -24(%rbp), %rax
    movl 0(%rax), %r10d
    movl %r10d, -28(%rbp)
    movl -28(%rbp), %r10d
    movl %r10d, -36(%rbp)
    movl -36(%rbp), %r10d
    movl %r10d, u.9(%rip)
    leaq u_ptr.10(%rip), %r11
    movq %r11, -48(%rbp)
    movq -48(%rbp), %rax
    movq %rbp, %rsp
    popq %rbp
    ret
    movl $0, %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    .globl main
    .text
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $88, %rsp
    movl $0, -4(%rbp)
    leaq -4(%rbp), %r11
    movq %r11, -16(%rbp)
    movq -16(%rbp), %r10
    movq %r10, -24(%rbp)
    leaq -24(%rbp), %r11
    movq %r11, -32(%rbp)
    movq -32(%rbp), %r10
    movq %r10, -40(%rbp)
    movsd .Ldouble.1(%rip), %xmm14
    movsd %xmm14, -48(%rbp)
    movsd .Ldouble.2(%rip), %xmm14
    movsd %xmm14, -56(%rbp)
    leaq -48(%rbp), %r11
    movq %r11, -64(%rbp)
    movq -64(%rbp), %r10
    movq %r10, -72(%rbp)
    call return_3@PLT
    movl %eax, -76(%rbp)
    movl -76(%rbp), %r10d
    movl %r10d, -4(%rbp)
    cmpl $3, -4(%rbp)
    movl $0, -84(%rbp)
    setne -84(%rbp)
    cmpl $0, -84(%rbp)
    je .Lif_false.0
    movl $1, %eax
    movq %rbp, %rsp
    popq %rbp
    ret
.Lif_false.0:
    movl $0, %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    movl $0, %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    .section .note.GNU-stack,"",@progbits
