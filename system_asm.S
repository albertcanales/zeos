#include <asm.h>

ENTRY(writeMSR)
      push %ebp
      movl %esp, %ebp
      movl 8(%ebp), %ecx
      movl 12(%ebp), %edx
      movl 16(%ebp), %eax
      wrmsr
      popl %ebp
      ret

ENTRY(task_switch)
      pushl %ebp
      movl %esp, %ebp

      pushl %esi
      pushl %edi
      pushl %ebx
      
      pushl 8(%ebp)

      call inner_task_switch

      addl $4, %esp

      popl %ebx
      popl %edi
      popl %ebx

      popl %ebp
      ret 

ENTRY(inner_task_switch_asm)
      pushl %ebp
      movl %esp, %ebp
      // WIP
      

      popl %ebp
      ret 
