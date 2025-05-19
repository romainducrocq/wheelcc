    .globl _start
    .text
_start:
    xor %ebp, %ebp
    mov (%rsp), %edi
    lea 8(%rsp), %rsi
    lea 16(%rsp, %rdi, 8), %rdx
    xor %eax, %eax
    call main
    mov %eax, %edi
    xor %eax, %eax
    call _exit
        .section .note.GNU-stack,"",@progbits
