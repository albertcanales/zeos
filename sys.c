/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>
#include <utils.h>
#include <io.h>
#include <mm.h>
#include <mm_address.h>
#include <sched.h>
#include <errno.h>
#include <interrupt.h>

#define LECTURA 0
#define ESCRIPTURA 1

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

# define BUFFER_SIZE 4096
char buff[BUFFER_SIZE];

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -EBADF;
  if (permissions!=ESCRIPTURA) return -EACCES;
  return 0;
}

int sys_ni_syscall()
{
	return -ENOSYS;
}

int sys_getpid()
{
	return current()->PID;
}

int sys_fork()
{
  int PID=-1;

  // creates the child process
  
  return PID;
}

void sys_exit()
{  
}

int sys_write(int fd, void *buffer, int size) {
	// Check errors
	int ret;
	ret = check_fd(fd, ESCRIPTURA);
	if(ret < 0) return ret;
	if(size < 0) return -EINVAL;
	ret = access_ok(VERIFY_READ, buffer, size);
	if(ret == 0) return -EFAULT;

	// Copy data and implement service
	int written = 0;
	while(written < size) {
		int to_read = MIN(size-written, BUFFER_SIZE);
		copy_from_user(buffer+written, buff, to_read);
		int bytes = sys_write_console(buff, to_read);
		written += bytes;
		if(bytes < to_read) return written;
	}
	return written;
}

int sys_gettime() {
	return zeos_ticks;
}
