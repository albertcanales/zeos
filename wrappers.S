#include <asm.h>

ENTRY(write)
	pushl %ebp
	movl %esp, %ebp

	pushl %ebx
	pushl %edi
	pushl %esi

	movl 8(%ebp), %ebx
	movl 12(%ebp), %ecx
	movl 16(%ebp), %edx

	movl $4, %eax

	pushl %ecx
	pushl %edx

	pushl $write_sysenter_fi

	pushl %ebp
	movl %esp, %ebp

	sysenter

write_sysenter_fi:
	
	popl %ebp
	addl $4, %esp
	popl %edx
	popl %ecx

	popl %esi
	popl %edi
	popl %ebx

	popl %ebp

	cmpl $0, %eax
	jge write_fi
	negl %eax
	movl %eax, errno
	movl $-1, %eax
write_fi:
	ret

ENTRY(gettime)
	pushl %ebp
	movl %esp, %ebp

	pushl %ebx
	pushl %edi
	pushl %esi

	movl $10, %eax

	pushl %ecx
	pushl %edx

	pushl $gettime_sysenter_fi

	pushl %ebp
	movl %esp, %ebp

	sysenter

gettime_sysenter_fi:
	
	popl %ebp
	addl $4, %esp
	popl %edx
	popl %ecx

	popl %esi
	popl %edi
	popl %ebx

	popl %ebp
	
	ret