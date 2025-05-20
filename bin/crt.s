    .globl _start
    .text
_start:
    xorl %ebp, %ebp
    movl (%rsp), %edi
    leaq 8(%rsp), %rsi
    leaq 16(%rsp, %rdi, 8), %rdx
    xorl %eax, %eax
    call main
    movl %eax, %edi
    xorl %eax, %eax
    call exit@PLT
        .section .note.GNU-stack,"",@progbits
