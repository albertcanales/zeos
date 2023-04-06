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
      popl %esi //crec q estava mal

      popl %ebp
      ret 

ENTRY(inner_task_switch_asm)
      
      movl 4(%esp), %eax
      movl %ebp, (%eax)       // 4.5.2.3
      movl 8(%esp), %esp      // 4.5.2.4

      popl %ebp
      ret 
