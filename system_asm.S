#include <asm.h>

ENTRY(writeMSR)
      push %ebp
      movl %esp, %ebp
      movl 8(%ebp), %ecx
      movl 12(%ebp), %edx
      movl 16(%ebp), %eax
      wrmsr
      movl %esp, %ebp
      popl %ebp
      ret