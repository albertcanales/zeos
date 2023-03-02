#include <asm.h>

ENTRY(write)
	pushl %edi
	pushl %esi
	pushl %edx
	pushl %ecx
	pushl %ebx
	movl $4, %eax

	sysenter
	
	cmpl $0, %eax
	jge write_fi
	neg %eax
	movl %eax, errno
	movl $-1, %eax
write_fi:
	ret