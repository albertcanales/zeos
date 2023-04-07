#include <asm.h>

#define INIT_WRAPPER \
	pushl %ebp; \
	movl %esp, %ebp; \
	pushl %ebx; \
	pushl %edi; \
	pushl %esi

#define END_WRAPPER \
	popl %ebp; \
	addl $4, %esp; \
	popl %edx; \
	popl %ecx; \
	popl %esi; \
	popl %edi; \
	popl %ebx; \
	popl %ebp; \

ENTRY(write)
	INIT_WRAPPER

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
	END_WRAPPER

	cmpl $0, %eax
	jge write_fi
	negl %eax
	movl %eax, errno
	movl $-1, %eax
write_fi:
	ret


ENTRY(gettime)
	INIT_WRAPPER

	movl $10, %eax

	pushl %ecx
	pushl %edx

	pushl $gettime_sysenter_fi

	pushl %ebp
	movl %esp, %ebp

	sysenter

gettime_sysenter_fi:
	END_WRAPPER
	ret


ENTRY(getpid)
	INIT_WRAPPER

	movl $20, %eax

	pushl %ecx
	pushl %edx

	pushl $getpid_sysenter_fi

	pushl %ebp
	movl %esp, %ebp

	sysenter

getpid_sysenter_fi:
	END_WRAPPER
	ret

ENTRY(fork)
	INIT_WRAPPER

	movl 8(%ebp), %ebx
	movl 12(%ebp), %ecx
	movl 16(%ebp), %edx

	movl $2, %eax

	pushl %ecx
	pushl %edx

	pushl $fork_sysenter_fi

	pushl %ebp
	movl %esp, %ebp

	sysenter

fork_sysenter_fi:
	END_WRAPPER
	cmpl $0, %eax
	jge fork_fi
	negl %eax
	movl %eax, errno
	movl $-1, %eax
fork_fi:
	ret

ENTRY(exit)
	INIT_WRAPPER

	movl $1, %eax

	pushl %ecx
	pushl %edx

	pushl $exit_sysenter_fi

	pushl %ebp
	movl %esp, %ebp

	sysenter

exit_sysenter_fi:
	END_WRAPPER
	ret