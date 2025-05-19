    .globl _start
    .text
_start:
    xorl %ebp, %ebp
    xorl %eax, %eax
    movl (%rsp), %edi
    leaq 8(%rsp), %rsi
    leaq 16(%rsp, %rdi, 8), %rdx
    pushq %rbp
    movq %rsp, %rbp
        subq $8, %rsp
        pushq %rbx
        call main
        movl %eax, %ebx
        xorl %eax, %eax
        xorl %edi, %edi
        call fflush
        movl %ebx, %edi
        popq %rbx
    movq %rbp, %rsp
    popq %rbp
    xorl %eax, %eax
    call _exit
        .section .note.GNU-stack,"",@progbits
