    .globl _start
    .text
_start:
    xor %ebp, %ebp
    xor %eax, %eax
    movl (%rsp), %edi
    leaq 8(%rsp), %rsi
    leaq 16(%rsp, %rdi, 8), %rdx
    pushq %rbp
    movq %rsp, %rbp
        subq $8, %rsp
        pushq %r15
        call main
        movl %eax, %r15d
        xor %eax, %eax
        xor %edi, %edi
        call fflush
        movl %r15d, %edi
        popq %r15
    movq %rbp, %rsp
    popq %rbp
    xor %eax, %eax
    call _exit
        .section .note.GNU-stack,"",@progbits
